#include <jni.h>
#include <string.h>
#include <android/log.h>
#include "audio.h"
#include "mpglib\mpg123.h"

#define  LOG_TAG    "txl"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

extern long freqs[9]; // wfz_added

jboolean Java_com_txl_jni_JNI_process(JNIEnv* env, jclass clazz,
        jstring from, jstring to)
{
	int size;
	char out[8192];
	int len,ret;
	FILE *pFileRead;
 	audio_file *aufile;

	char mp3FileName[100];
	char wavFileName[100];
	int samplesperframe;
	int bytespersample;

	int samplerate   = 44100;
	int channels     = 2;
	int outputFormat = FAAD_FMT_16BIT;  //only support 16bit now, otherwise error
	int fileType     =  OUTPUT_WAV;
	int channelMask  = 0;
	bool firstbuf = true;
	
	char buf[16384];
	struct mpstr mp;
	
	const jbyte *byteFrom = (*env)->GetStringUTFChars(env, from, NULL);
	const jbyte *byteTo = (*env)->GetStringUTFChars(env, to, NULL);
	LOGI("process from=%s,to=%s",byteFrom,byteTo);
	//------print log info-----
//	FILE *pLogFile = fopen("d:\\Mp3toWav.txt","wa");
	char log[150];


//	switch(outputFormat)
//	{
//	case FAAD_FMT_16BIT:
//		bytespersample = 2;
//		break;
//	case FAAD_FMT_24BIT:
//		bytespersample = 3;
//		break;
//	case FAAD_FMT_32BIT:
//		bytespersample = 4;
//		break;
//	default:
//		bytespersample = 2;
//	}
	bytespersample = 2;
	strcpy(mp3FileName, byteFrom);
	strcpy(wavFileName, byteTo);
	LOGI("process1 mp3FileName=%s wavFileName=%s",mp3FileName,wavFileName);
//------
	// Open file for input: 
	if( (pFileRead = fopen( mp3FileName, "rb" )) == NULL )
	{
		perror( "open jsb.mp3 failed on input file" );
		exit( 1 );
	}

	// Open file for output: 
	aufile = open_audio_file(wavFileName, samplerate, channels,
		outputFormat, fileType, channelMask);
	
	if(aufile->sndfile  == NULL)
	{
		perror( "Open failed on output file" );
		exit( 1 );
	}
//-------
	LOGI("process2");

	InitMP3(&mp);
	LOGI("process3");
	while( !feof(pFileRead) )
	{
		len = fread(buf, sizeof(char), 10000, pFileRead);
		
		if(len <= 0)
			break;
		ret = decodeMP3(&mp,buf,len,out,8192,&size);
		if(firstbuf)
		{
			aufile->samplerate = freqs[mp.fr.sampling_frequency]; 
			firstbuf = false;
		}
		while(ret == MP3_OK)
		{
			samplesperframe = size/bytespersample;
      write_audio_file(aufile, out, samplesperframe, 0);
			if(samplesperframe != 2304) // 1152 * 2 channels = 2304
			{
				sprintf(log, " samples of current frame is %d \n", samplesperframe);
//				fwrite(log, 1, strlen(log), pLogFile); 
			}
			ret = decodeMP3(&mp,NULL,0,out,8192,&size);
		}
	}
	LOGI("process5");
	//aufile->samplerate = mp.fr.sampling_frequency; //save the real samplerate
	ExitMP3(&mp);
	
	fclose(pFileRead);
//	fclose(pLogFile);
  close_audio_file(aufile);
	LOGI("process6");
	return true;
}
        
        
        