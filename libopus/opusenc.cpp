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
#define BITRATE 35000

#define MAX_FRAME_SIZE 6*960
#define MAX_PACKET_SIZE (3*1276)

int pcm2opus(const char *infile, const char *outfile)
{
   FILE *fin;
   FILE *fout;
   opus_int16 in[FRAME_SIZE*CHANNELS];
   unsigned char cbits[MAX_PACKET_SIZE];
   int nbBytes;
   /*Holds the state of the encoder and decoder */
   OpusEncoder *encoder;
   int err;

   /*Create a new encoder state */
   encoder = opus_encoder_create(SAMPLE_RATE, CHANNELS, APPLICATION, &err);
   if (err<0)
   {
      fprintf(stderr, "failed to create an encoder: %s\n", opus_strerror(err));
      return EXIT_FAILURE;
   }
   /* Set the desired bit-rate. You can also set other parameters if needed.
      The Opus library is designed to have good defaults, so only set
      parameters you know you need. Doing otherwise is likely to result
      in worse quality, but better. */
   err = opus_encoder_ctl(encoder, OPUS_SET_BITRATE(BITRATE));
   if (err<0)
   {
      fprintf(stderr, "failed to set bitrate: %s\n", opus_strerror(err));
      return EXIT_FAILURE;
   }
   fin = fopen(infile, "r");
   if (fin==NULL)
   {
      fprintf(stderr, "failed to open file: %s\n", strerror(errno));
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

      /* Read a 16 bits/sample audio frame. */
      fread(pcm_bytes, sizeof(short)*CHANNELS, FRAME_SIZE, fin);
      if (feof(fin))
         break;
      /* Convert from little-endian ordering. */
      for (i=0;i<CHANNELS*FRAME_SIZE;i++)
         in[i]=pcm_bytes[2*i+1]<<8|pcm_bytes[2*i];

      /* Encode the frame. */
      nbBytes = opus_encode(encoder, in, FRAME_SIZE, cbits, MAX_PACKET_SIZE);
      if (nbBytes<0)
      {
         fprintf(stderr, "encode failed: %s\n", opus_strerror(nbBytes));
         return EXIT_FAILURE;
      }

      /* Write the encoded audio to file. */
      unsigned short len = (unsigned short)nbBytes;
      fwrite(&len, 1, sizeof(len), fout);
      fwrite(cbits, 1, nbBytes, fout);
   }
   /*Destroy the encoder state*/
   opus_encoder_destroy(encoder);
   fclose(fin);
   fclose(fout);
   return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
	  fprintf(stderr, "usage:%s input.pcm output.opus\n", argv[0]);
	  return EXIT_FAILURE;
	}
	pcm2opus(argv[1], argv[2]);
}
