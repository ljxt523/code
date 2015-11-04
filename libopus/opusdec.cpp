/*
 * 本程序实现读入opus音频流，转换输出为pcm数据流。
 *
 * 因为opus编码的packet是没有长度字段的。所以我们的输入要求是连续的short+opus packet的形式。short的值代表后面opus packet的长度。我们
 * 分析的时候先读入长度，然后分析后面的opus的packet，再读入一个长度，再分析一个packet。。。
 * 注：short在此为little-endian
 */

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "include/opus/opus.h"
#include <stdio.h>

/*The frame size is hardcoded for this sample code but it doesn't have to be*/
#define FRAME_SIZE 960
#define SAMPLE_RATE 48000
#define CHANNELS 1
#define APPLICATION OPUS_APPLICATION_VOIP
#define BITRATE 64000

#define MAX_FRAME_SIZE 6*960
#define MAX_PACKET_SIZE (3*1276)


int opus2pcm(const char* infile, const char* outfile)
{
   FILE *fin;
   FILE *fout;
   opus_int16 out[MAX_FRAME_SIZE*CHANNELS];
   unsigned char cbits[MAX_PACKET_SIZE];
   OpusDecoder *decoder;
   int err;

   fin = fopen(infile, "r");
   if (fin==NULL)
   {
      fprintf(stderr, "failed to open file: %s\n", strerror(errno));
      return EXIT_FAILURE;
   }


    //Create a new decoder state.
   decoder = opus_decoder_create(SAMPLE_RATE, CHANNELS, &err);
   if (err<0)
   {
      fprintf(stderr, "failed to create decoder: %s\n", opus_strerror(err));
      return EXIT_FAILURE;
   }
   fout = fopen(outfile, "w");
   if (fout==NULL)
   {
      fprintf(stderr, "failed to open file: %s\n", strerror(errno));
      return EXIT_FAILURE;
   }

   while (1)
   {
      int i;
      unsigned char pcm_bytes[MAX_FRAME_SIZE*CHANNELS*2];
      int frame_size;
      char ch[2] = {0};

       //Read a 16 bits/sample audio frame.
      fread(ch, 1, 2, fin);
      unsigned short plen = *(unsigned char*)ch;
      fread(cbits, plen, 1, fin);
      if (feof(fin))
         break;


       /*Decode the data. In this example, frame_size will be constant because
         the encoder is using a constant frame size. However, that may not
         be the case for all encoders, so the decoder must always check
         the frame size returned.*/
      frame_size = opus_decode(decoder, cbits, plen, out, MAX_FRAME_SIZE, 0);
      if (frame_size < 0)
      {
         fprintf(stderr, "decoder failed: %s\n", opus_strerror(err));
         return EXIT_FAILURE;
      }

       //Convert to little-endian ordering.
      for(i=0;i<CHANNELS*frame_size;i++)
      {
         pcm_bytes[2*i]=out[i]&0xFF;
         pcm_bytes[2*i+1]=(out[i]>>8)&0xFF;
      }
       //Write the decoded audio to file.
      fwrite(pcm_bytes, sizeof(short), frame_size*CHANNELS, fout);
   }
   opus_decoder_destroy(decoder);
   fclose(fin);
   fclose(fout);
   return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
	  fprintf(stderr, "usage: opus_decode input.opus output.pcm\n");
	  return EXIT_FAILURE;
	}
	opus2pcm(argv[1], argv[2]);
	return 0;
}
