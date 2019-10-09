#include "stdlib.h"
#include "stdio.h"
#include <fftw3.h>
#include "SymbolProbability.h"
#include <sndfile.h>
#include "huffman.h"
//#include "lz77.h"

#include <math.h>

float small_factor = 2;
float medium_factor = 5;
float large_factor = 20;

const char * prob_file_name = "Probability_file";
const char * coded_file_name = "audio_coded";
const char *input_file_name = "untitled.wav";
const char *output_file_name = "first_output.wav";
const char *decoded_file_name = "audio_decoded.wav";

unsigned int frames;
int channels;
int samplerate;
int num_items;

void save_headers(const char * coded_file_name, SF_INFO * info) {
	FILE * codedfile = fopen(coded_file_name, "wb");

	fwrite(&info->frames, sizeof(int), 1, codedfile);
	fwrite(&info->samplerate, sizeof(int), 1, codedfile);
	fwrite(&info->format, sizeof(int), 1, codedfile);

	fclose(codedfile);
}

void print_wav_headers(SF_INFO * info) {
	printf("\nframes=%i", (int) info->frames);
	printf("\nsample rate=%i", info->samplerate);
	printf("\nchannels=%i", info->channels);
}

void decompress(const char *infilename, const char *outfilename) {
	printf("\n\ndecompess----------------------------------");
	unsigned int i=0;
	int frames;
	int samplerate;
	int format;


	int symbol;
	unsigned int code;
	unsigned int number_of_codes;

	unsigned int number_of_samples;

	unsigned int current_int=0;

	FILE * file_in = fopen(infilename, "rb");
	fread(&frames, sizeof(int), 1, file_in);
	fread(&samplerate, sizeof(int), 1, file_in);
	fread(&format, sizeof(int), 1, file_in);
	printf("\nframes:%i\nsample rate:%i\nformat:%i", frames, samplerate,
			format);
	fread(&number_of_codes, sizeof(int), 1, file_in);
	printf("\nHuffman-------------------------------------");

	int * symbols = (int*)(malloc(sizeof(int)*number_of_codes));
	unsigned int * codes = (unsigned int*)(malloc(sizeof(unsigned int)*number_of_codes));


	for (i=0;i<number_of_codes;i++)
	{
		fread(&symbol, sizeof(int), 1, file_in);
		fread(&code, sizeof(int), 1, file_in);
		//	printf("\n%i \t %i",symbol,code);
		symbols[i]=symbol;
		codes[i]=code;
	}

	fread(&number_of_samples, sizeof(int), 1, file_in);
	printf ("\nnumber of samples:%i",number_of_samples);
	//scanf("%c");
	printf("\ndecoding-------------------------------------");


	int * samples = (int *)(malloc(sizeof(int)*number_of_samples));
	unsigned int count=0;
	unsigned int var =0;

	while(fread(&current_int, sizeof(unsigned int), 1, file_in)==1)
	{
		for (i=0;i<number_of_codes;i++)
		{
			if (codes[i]==current_int)
			{
				samples[count]=symbols[i];
				var ++;
				count++;
				//printf ("\n%i found",current_int);
			}
		}

		//		current_int|=(unsigned int)current_char;
		//		printf("\ncurrent_int:%i",current_int);
		//		for (i=0;i<number_of_codes;i++)
		//		{
		//			//printf ("\n%i with %i",current_int,codes[i]);
		//
		//			//if (i==2);
		//			//scanf("%c");
		//			if (codes[i]==current_int)
		//			{
		//				printf("\n!!!  %i \t %i",symbols[i],codes[i]);
		//				current_int=0;
		//				break;
		//			}
		//		}
		//		//	printf ("\nnot found %i",current_int);
		//		current_int<<=8;
	}

	printf("\ntotal:%i",var);

	SNDFILE *output_file;
	SF_INFO info;

	info.samplerate=samplerate;
	info.channels=1;
	info.format= format;
	info.frames=frames;
	output_file = sf_open(outfilename, SFM_WRITE, &info);

	sf_write_int(output_file, samples, frames);

	sf_close(output_file);

	fclose(file_in);

	//	double symbol;
	//	int code_legth;
	//	char * code;
	//
	//	fwrite(&symbols[current]->symbol,sizeof(double),1,file);
	//	fwrite(&symbols[current]->code_length,sizeof(int),1,file);
	//	fwrite(symbols[current]->code,sizeof(char),symbols[current]->code_length,file);
	//	fwrite(&symbols[current]->probability,sizeof(double),1,file);
	//

	fclose(file_in);
}

double * new_double_array(int length) {
	return (double *) (malloc(length * sizeof(double)));
}

double * stero_to_mono(double * samples, int frames, int channels) {

	printf("\nlossy>starting stereo to mono...");

	double * mono_sample = new_double_array(frames);

	for (int i = 0; i < frames; i++) {
		mono_sample[i] = 0;
		for (int j = 0; j < channels; j++)
			mono_sample[i] += samples[i * channels + j];
		mono_sample[i] /= channels;
	}

	printf("\nlossy>stereo to mono done.");

	return mono_sample;
}

double * decimate(double * mono_sample, int frames, int decimate_factor)

{
	printf("\nlossy>starting decimate with factor %i...", decimate_factor);

	double * decimate_sample = new_double_array(frames / decimate_factor);
	
	//	printf("\nd%i",frames / decimate_factor);
	for (int i = 0; i < frames; i++) {
		//printf("%i :",i);
		if (i % decimate_factor == 0 &&i<frames / decimate_factor) {
			//		printf ("\n%iK",(i / decimate_factor));
			decimate_sample[i / decimate_factor] = mono_sample[i];

		}
	}

	printf("\nlossy>decimated.");
	
	return decimate_sample;
}

double * apply_lossy_compression(const char *file_input_name,
		const char * file_output_name, int factor) {
	
	SNDFILE *input_file;
	SNDFILE *output_file;
	SF_INFO info;
	
	printf("\nLossy> starting lossy compression...");
	
	input_file = sf_open(file_input_name, SFM_READ, &info);

	frames = info.frames;
	channels = info.channels;
	samplerate = info.samplerate;
	num_items = frames * channels;
	
	print_wav_headers(&info);
	
	/////////////////////////////////////////////////processing
	
	double * samples = new_double_array(num_items);
	sf_read_double(input_file, samples, num_items);

	double * mono_sample = stero_to_mono(samples, frames, channels);
	free(samples);

	double * decimate_sample = decimate(mono_sample, frames, factor);
	free(mono_sample);
	/////////////////////////////////////////////////processing done
	info.channels = 1;
	info.frames = frames / factor;
	info.samplerate = samplerate / factor;
	output_file = sf_open(file_output_name, SFM_WRITE, &info);

	info.frames = frames / factor;

	print_wav_headers(&info);
	sf_write_double(output_file, decimate_sample, frames / factor);

	//	fftw_complex * out = (fftw_complex *)(malloc(sizeof(fftw_complex )*transform_length));
	//	fftw_plan p;
	//	p = fftw_plan_dft_r2c_1d(num_items, samples, out, FFTW_ESTIMATE);
	//	fftw_execute(p);
	//	fftw_destroy_plan(p);

	//	save_probabilities_(probability_file_name, decimate_sample,frames / decimate_factor);
	sf_close(input_file);
	sf_close(output_file);
	printf("\nLossy> lossy compression finished");
	save_headers(coded_file_name,&info);
	return decimate_sample;
}

int main(int argc, char * argv[]) {

	int factor=0;
	input_file_name = argv[1];
	char ch = argv[2][0];

	switch (ch) {
	case 's':
		factor=small_factor;
		break;
	case 'm':
		factor = medium_factor;
		break;

	case 'l':
		factor = large_factor;
		break;

	default:
		printf ("\nerror: unknown size.");
		return 0;
		break;
	}
	//output_file_name = argv[2];


//actor =10;

	double * sample = apply_lossy_compression(input_file_name, output_file_name,
			factor);
	free(sample);
	int * decimate_sample = create_probability_file(output_file_name,
			prob_file_name);

	get_huffman_nodes_and_save_it(
			prob_file_name,
			coded_file_name,
			decimate_sample,
			(int)(frames / factor));


	free(decimate_sample);

	decompress(coded_file_name,decoded_file_name);

	//		char ** inputs = (char**) (malloc(sizeof(char*) * 4));

	//
	//
	//		inputs[1][0] = '0';
	//		inputs[2] = (char*) temp_file_name2;
	//		inputs[3] = (char*) coded_file_name;
	//
	//		printf("\nstarting LZ77");
	//		lz77(0, inputs);
	//		printf("\ndone LZ77");





	//	inputs[1][0] = '1';
	//	inputs[2] = (char*) coded_file_name;
	//	inputs[3] = (char*) decoded_file_name;
	//	lz77(0, inputs);


	//decompress(output_file_name, recoverfilename);


	//	//

	//
	//
	//	decompress(coded_file_name,coded_file_name);
	return 0;
}
