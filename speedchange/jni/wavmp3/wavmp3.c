#include <jni.h>
#include <string.h>
#include <android/log.h>
#include        <stdio.h>
#include		"codec.h"
#include    "samplein.h"
#include		"bladesys.h"

#define MAX_NAMELEN 256

#ifdef OS2
	#define INCL_DOSFILEMGR
	#define INCL_DOSERRORS
	#define INCL_DOSPROCESS
	#include <os2.h>
	#include "os2key.h"
#endif

extern	char *mystrupr(char * strng);

#define FALSE 0
#define TRUE 1

/*____ Structure Definitions __________________________________________________*/

typedef struct  JobDef  Job;

struct JobDef
{
        CodecInitIn		sCodec;
        SplIn         sInput;
        Job         * psNext;
        int           fDeleteSource;
        char          outputFilename[MAX_NAMELEN];
        char          sourceFilename[MAX_NAMELEN];
};
int				wantedBitrate = -1;             /* -1 = Unspecified. */
int				wantedCRC = FALSE;
int				wantedPrivate = FALSE;
int				wantedCopyright = FALSE;
int				wantedOriginal = TRUE;
int				wantedChannelSwap = FALSE;
SampleType	wantedInputType = STEREO;

int     wantedDeleteSource = FALSE;

#ifdef	WAIT_KEY
	int     wantedQuit = FALSE;
#else
	int			wantedQuit = TRUE;
#endif
int			wantedQuiet = FALSE;
int			wantedSTDOUT = FALSE;
int			fPreparedSTDIN = FALSE;

char		prioString[256];
char	* pPrioString = NULL;

Job		* psJobQueue = NULL;

char		outputDir[MAX_NAMELEN];

FILE	* textout;

jboolean Java_com_txl_jni_JNI_process2(JNIEnv* env, jclass clazz,
        jstring from, jstring to)
{
	const jbyte *byteFrom = (*env)->GetStringUTFChars(env, from, NULL);
	const jbyte *byteTo = (*env)->GetStringUTFChars(env, to, NULL);
	char mp3FileName[100];
	char wavFileName[100];
	strcpy(mp3FileName, byteTo);
	strcpy(wavFileName, byteFrom);
	
	int           samplesPerFrame;
	int           nSamples;

  short         readBuffer[2304];
  int           x;

  char          mystring[2] = { 13, 0 };
  char          input;
  Job         * psTemp;

  time_t        startTimeBatch, startTimeFile, currTime;
  double        batchSamplesTotal = 0.0, batchSamplesRead = 0.0;

  CodecInitOut *pCodecInfo;
  char        * pBuffer;
  uint          encodedChunkSize;
  FILE        * fp;

	int						cmdOfs;
	char					temp[256];
#ifdef	WILDCARDS
	void				* pWildcardLink;
#endif

	/* First things first... */

 	textout = stdout;	
	outputDir[0] = 0;

#ifdef	WILDCARDS
	pWildcardLink = expandWildcards( &argc, &argv );
#endif

	cmdOfs = readGlobalSwitches( argc-1, argv+1 ) +1;

	/* Check for STDOUT */

	for( x = 1 ; x < argc ; x++ )
	{
		strcpy( temp, argv[x] );
		mystrupr( temp );
		if( strcmp( temp, "STDOUT" ) == 0 )
		{
			prepStdout();
			textout = stderr;
			break;
		}
	}

  /* Print Text */

	if( !wantedQuiet )
	{
		fprintf( textout, "\n" );
		fprintf( textout, "BladeEnc 0.82    (c) Tord Jansson       Homepage: http://www.bladeenc.cjb.net\n" );
		fprintf( textout, "===============================================================================\n" );
		fprintf( textout, "BladeEnc is free software, distributed under the Lesser General Public License.\n" );
		fprintf( textout, "See the file COPYING, BladeEnc's homepage or www.fsf.org for more details.\n" );
    fprintf( textout, "\n" );
	}

	/* Initialise batch */

	while( cmdOfs < argc )
	{
		x = addCommandlineJob( argc - cmdOfs, &argv[cmdOfs] );
		if( x == 0 )
		{
#ifdef	WILDCARDS	
			freeExpandWildcardMem( pWildcardLink );
#endif
			quit( -1 );
		}
		cmdOfs += x;
	}

#ifdef	WILDCARDS
	freeExpandWildcardMem( pWildcardLink );
#endif
	

	/* Validate job settings */

	x = validateJobs( psJobQueue );
  if( x == FALSE )
    quit( -2 );


  /* Set priority */

	if( setPriority( pPrioString ) == FALSE )
	{
		fprintf( textout, "Error: '%s' is not a valid priority setting!\n", pPrioString );
		quit(	-1 );
	};

	/* Procedure if no files found */

  if( psJobQueue == NULL )
  {
    printUsage();                                                           /* No files on the commandline */
    quit( -1 );
  }


  /* Print files to encode */

  for( x = 0, psTemp = psJobQueue ; psTemp != NULL ; x++, psTemp = psTemp->psNext );
	if( !wantedQuiet )
	  fprintf( textout, "Files to encode: %d\n\n", x );


  /* Encode */

  startTimeBatch = time( NULL );

  for( psTemp = psJobQueue ; psTemp != NULL ; batchSamplesTotal += psTemp->sInput.length, psTemp = psTemp->psNext );


  while( psJobQueue != NULL )
  {
    /* Print information */

		if( !wantedQuiet )
		{
			fprintf( textout, "Encoding:  %s\n", psJobQueue->sourceFilename );
			fprintf( textout, "Input:     %.1f kHz, %d bit, ", psJobQueue->sInput.freq/1000.f, psJobQueue->sInput.bits );
			if( psJobQueue->sInput.fReadStereo == TRUE )
				fprintf( textout, "stereo.\n" );
			else
				fprintf( textout, "mono.\n" );
			fprintf( textout, "Output:    %d kBit, ", psJobQueue->sCodec.bitrate );
			if( psJobQueue->sCodec.mode == 0 )
				fprintf( textout, "stereo.\n\n" );
			else
				fprintf( textout, "mono.\n\n" );
		}

    /* Init a new job */

    startTimeFile = time( NULL );
    pCodecInfo = codecInit( &psJobQueue->sCodec );
    samplesPerFrame = pCodecInfo->nSamples;
    pBuffer = (char *) malloc( pCodecInfo->bufferSize );
		if( strcmp( psJobQueue->outputFilename, "STDOUT" ) == 0 )
			fp = stdout;
		else
		{
			fp = fopen( psJobQueue->outputFilename, "wb" );
			if( fp == NULL )
			{
		/*  codecExit(); */
				closeInput( &psJobQueue->sInput );
				fprintf( textout, "ERROR: Couldn't create '%s'!\n", psJobQueue->outputFilename );
				quit( -1 );
			}
		}

    /* Encoding loop */

    while ( (nSamples = readSamples( &psJobQueue->sInput, samplesPerFrame, readBuffer)) > 0 )
    {
      encodedChunkSize = codecEncodeChunk( nSamples, readBuffer, pBuffer );
      if( fwrite( pBuffer, 1, encodedChunkSize, fp ) != encodedChunkSize )
      {
        fprintf( textout, "ERROR: Couldn't write '%s'! Disc probably full.\n", psJobQueue->outputFilename );
        quit( -1 );
      }

      batchSamplesRead += nSamples;

			if( !wantedQuiet )
				updateProgressIndicator( startTimeBatch, batchSamplesTotal, batchSamplesRead,
					                       startTimeFile, psJobQueue->sInput.length,
						                     psJobQueue->sInput.length - psJobQueue->sInput.samplesLeft );
			if( be_kbhit() != 0 )
			{
				input = be_getch();
				if( input == 27 )
        {
          fprintf( textout, "%s                                                                             %s", mystring, mystring );
          fprintf( textout, "Quit, are you sure? (y/n)" );
					fflush( textout );
          input = be_getch();
          if( input == 'y' || input == 'Y' )
          {
            encodedChunkSize = codecExit( pBuffer );
            if( encodedChunkSize != 0 )
              if( fwrite( pBuffer, encodedChunkSize, 1, fp ) != 1 )
              {
                fprintf( textout, "ERROR: Couldn't write '%s'! Disc probably full.\n", psJobQueue->outputFilename );
                quit( -1 );
              }
            free( pBuffer );
            closeInput( &psJobQueue->sInput );
						if( fp != stdout )
							fclose( fp );
            return  0;
          }
          else
            fprintf( textout, "%s                                                                             %s", mystring, mystring );
        }
      }
    }

    /* File done */


    encodedChunkSize = codecExit( pBuffer );
    if( encodedChunkSize != 0 )
      if( fwrite( pBuffer, encodedChunkSize, 1, fp ) != 1 )
      {
        fprintf( textout, "ERROR: Couldn't write '%s'! Disc probably full.\n", psJobQueue->outputFilename );
        quit( -1 );
      }
    if( fp != stdout )
			fclose( fp );
    free( pBuffer );
    if( psJobQueue->fDeleteSource == TRUE )
      remove( psJobQueue->sourceFilename );
    x = time( NULL ) - startTimeFile;
		if( !wantedQuiet )
		{
			fprintf( textout, "%s                                                                             %s", mystring, mystring );
			fprintf( textout, "Completed. Encoding time: %02d:%02d:%02d (%.2fX)\n\n",
               x/3600, (x/60)%60, x%60, ((float)psJobQueue->sInput.length) /
               ((psJobQueue->sInput.fReadStereo+1)*psJobQueue->sInput.freq*x) );
		}
    removeJobQueueEntry( psJobQueue );
  }

  /* Batch done */

  if( !wantedQuiet )
  {
    currTime = time( NULL ) - startTimeBatch;
    fprintf( textout, "All operations completed. Total encoding time: %02d:%02d:%02d\n",
             (int) currTime/3600, (int)(currTime/60)%60, (int) currTime%60 );

    if( !wantedQuit )
		{
			fprintf( textout, "Press ENTER to exit..." );
			be_getch();
		  fprintf( textout, "\n" );
		}
  }
	
	
	return true;
}