#include "include/speex/speex.h"
#include <stdio.h>

#define FRAME_SIZE 160

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		fprintf(stdout, "Usage:%s input.pcm output.speex\n", argv[0]);
		fprintf(stdout, "\tinput.pcm is pcm raw audio data, output.speex is audio data codeced by speex.\n");
		return -1;
	}

	char *infile;
	char *outfile;
	FILE *fin;
	FILE *fout;
	short in[FRAME_SIZE];
	char cbits[2000];
	int nbBytes;
	/*Holds the state of the encoder*/
	void *state;
	/*Holds bits so they can be read and written to by the Speex routines*/
	SpeexBits bits;
	int tmp;

	/*Create a new encoder state in narrowband mode*/
	state = speex_encoder_init(&speex_nb_mode);

	//int fz;
	//speex_encoder_ctl(state, SPEEX_GET_FRAME_SIZE, &fz);
	//fprintf(stdout, "frame_size of wideband = %d\n", fz);
	//return 0;

	/*Set the quality to 8 (15kbps)*/
	tmp = 8;
	speex_encoder_ctl(state, SPEEX_SET_QUALITY, &tmp);

	infile = argv[1];
	fin = fopen(infile, "r");

	outfile = argv[2];
	fout = fopen(outfile, "w+");

	/*Initialization of the structure that holds the bits*/
	speex_bits_init(&bits);
	while(1)
	{
		/*Read a 16 bits/sample audio frame*/
		fread(in, sizeof(short), FRAME_SIZE, fin);
		if (feof(fin))
			break;

		/*Flush all the bits in the struct so we can encode a new frame*/
		speex_bits_reset(&bits);

		/*Encode the frame*/
		speex_encode_int(state, in, &bits);
		/*Copy the bits to an array of char that can be written*/
		nbBytes = speex_bits_write(&bits, cbits, 2000);

		/*Write the size of the frame first. This is what sampledec expects but
		 it's likely to be different in your own application*/
		fwrite(&nbBytes, sizeof(int), 1, fout);
		/*Write the compressed data*/
		fwrite(cbits, 1, nbBytes, fout);
	}

	/*Destroy the encoder state*/
	speex_encoder_destroy(state);
	/*Destroy the bit-packing struct*/
	speex_bits_destroy(&bits);
	fclose(fin);
	fclose(fout);
	return 0;
}
