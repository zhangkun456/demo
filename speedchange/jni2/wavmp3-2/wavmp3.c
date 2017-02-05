#include <jni.h>
#include <string.h>
#include <android/log.h>
#include  <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#include "libavcodec\\avcodec.h"

#pragma comment (lib,".\\FFMPEG_lib\\avformat.lib")
#pragma comment (lib,".\\FFMPEG_lib\\avutil.lib")
#pragma comment (lib,".\\FFMPEG_lib\\swscale.lib")
#pragma comment (lib,".\\FFMPEG_lib\\avcodec.lib")
#pragma comment (lib,".\\FFMPEG_lib\\avdevice.lib")
#pragma comment (lib,".\\FFMPEG_lib\\avfilter.lib")

jboolean Java_com_txl_jni_JNI_process0(JNIEnv* env, jclass clazz,
        jstring from, jstring to)
{
	avcodec_init(); //���ȣ�main������һ��ʼ��ȥ����avcodec_init()�������ú����������ǳ�ʼ��libavcodec����������ʹ��avcodec������ʱ���ú������뱻���á�
  avcodec_register_all();//ע�����еı��������codecs������������parsers���Լ�������������bitstream filters������Ȼ����Ҳ����ʹ�ø����ע�ắ����ע��������Ҫ֧�ֵĸ�ʽ��
	
	return true;
}

jboolean Java_com_txl_jni_JNI_process2(JNIEnv* env, jclass clazz,
        jstring from, jstring to)
{
	const jbyte *byteFrom = (*env)->GetStringUTFChars(env, from, NULL);
	const jbyte *byteTo = (*env)->GetStringUTFChars(env, to, NULL);
	char mp3FileName[100];
	char wavFileName[100];
	strcpy(mp3FileName, byteTo);
	strcpy(wavFileName, byteFrom);
	
	AVCodec *codec;
    AVCodecContext *c= NULL;
    int frame_size,  out_size, outbuf_size;
    FILE * fin,*fout;
    short *samples;
    uint8_t *outbuf;
	int numberframe = 0;
	int size = 0;
	int  FRAME_READ= 0;

    printf("Audio encoding\n");

    /* find the MP2 encoder */
    codec = avcodec_find_encoder(CODEC_ID_MP3);
    if (!codec) 
	{
        fprintf(stderr, "codec not found\n");
        exit(1);
    }

    c= avcodec_alloc_context();

    /* put sample parameters */
    c->bit_rate = 64000;
    c->sample_rate = 44100;
    c->channels = 2;

    /* open it */
    if (avcodec_open(c, codec) < 0)
	{
        fprintf(stderr, "could not open codec\n");
        exit(1);
    }

    /* the codec gives us the frame size, in samples */
    frame_size = c->frame_size;                                    
	samples = malloc(frame_size * 2 * c->channels);  //* 2 ����Ϊ һ��PCM���ݶ���16bit�� ,c->channels ��������

    FRAME_READ  = frame_size * 2 * c->channels;

    outbuf_size = 10000;
    outbuf = malloc(outbuf_size);

	fin = fopen(wavFileName, "rb+");
	if (!fin)
	{
		fprintf(stderr, "could not open %s\n", wavFileName);
		exit(1);
	}

    fout = fopen(mp3FileName, "wb");
    if (!fout) 
	{
        fprintf(stderr, "could not open %s\n", mp3FileName);
        exit(1);
    }
    for(;;)
	{
		size = fread(samples, 1,FRAME_READ , fin);
		if (size == 0)
		{
			break;
		}
        /* encode the samples */
        out_size = avcodec_encode_audio(c, outbuf, outbuf_size, samples);
        fwrite(outbuf, 1, out_size, fout);
		numberframe ++ ;
		printf("save frame %d\n",numberframe);

    }
    fclose(fout);
    free(outbuf);
    free(samples);

    avcodec_close(c);
    av_free(c);
	
	
	return true;
}