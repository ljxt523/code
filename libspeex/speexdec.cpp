#include <stdio.h>
#include "include/speex/speex.h"

#define FRAME_SIZE 320

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		fprintf(stdout, "Usage:%s input.speex output.pcm\n", argv[0]);
		fprintf(stdout, "\t input.speex is audio data encoded by speex, output.pcm is audio raw data.\n");
		return -1;
	}

	char *infile;
	char *outfile;
	FILE *fin;
	FILE *fout;
	/*Speex handle samples as float, so we need an array of floats*/
	short output[FRAME_SIZE];
	char cbits[200];
	int nbBytes;
	/*Holds the state of the decoder*/
	void *state;
	/*Holds bits so they can be read and written to by the Speex routines*/
	SpeexBits bits;
	int tmp;
	int ret = 0;

	/*Create a new decoder state in narrowband mode*/
	state = speex_decoder_init(&speex_wb_mode);

	/*Set the perceptual enhancement on*/
	tmp = 1;
	speex_decoder_ctl(state, SPEEX_SET_ENH, &tmp);

	infile = argv[1];
	fin = fopen(infile, "r");

	outfile = argv[2];
	fout = fopen(outfile, "w+");

	/*Initialization of the structure that holds the bits*/
	speex_bits_init(&bits);
	while(1)
	{
		/*Read the size encoded by sampleenc, this part will likely be
		  different in your application*/
		fread(&nbBytes, sizeof(int), 1, fin);
		//fprintf(stderr, "nbBytes: %d\n", nbBytes);
		if (feof(fin))
			break;

		/*Read the "packet" encoded by sampleenc*/
		fread(cbits, 1, nbBytes, fin);
		/*Copy the data into the bit-stream struct*/
		speex_bits_read_from(&bits, cbits, nbBytes);

		while (true)
		{
			/*Decode the data*/
			ret = speex_decode_int(state, &bits, output);

			if (!ret)
			{
				/*Write the decoded audio to file*/
				fwrite(output, sizeof(short), FRAME_SIZE, fout);
			}
			else
			{
				break;
			}
		}
	}

	/*Destroy the decoder state*/
	speex_decoder_destroy(state);
	/*Destroy the bit-stream struct*/
	speex_bits_destroy(&bits);
	fclose(fin);
	fclose(fout);
	return 0;
}
