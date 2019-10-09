/*
 * SimbolProbability.cpp
 *
 *  Created on: 02/03/2013
 *      Author: root
 */

#include <iostream>
#include <algorithm>
#include <sndfile.h>
#include <vector>
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
using namespace std;
int compare(const void * a, const void * b) {
	int * ad = (int*) a;
	int * bd = (int*) b;
	return *ad > *bd;
}

int * new_int_array(int length) {
	return (int *) (malloc(length * sizeof(int)));
}
int * create_probability_file(const char * input_file_name,const char * output_file_name) {

	FILE * probability_file = fopen(output_file_name,"wb");

	if (probability_file ==NULL)
	{
		printf("It was not possible create the probability file");
		return NULL;
	}

	SNDFILE *input_file;
	SF_INFO info;
	unsigned int current_in_symbols = 0, i;
	input_file = sf_open(input_file_name, SFM_READ, &info);

	unsigned int frames = info.frames;
	int channels = info.channels;

	unsigned int num_items = frames * channels;
	int * samples = new_int_array(num_items);
	int * samples_copy = new_int_array(num_items);
	sf_read_int(input_file, samples, num_items);

	for (i=0;i<num_items;i++)
	{
		samples_copy[i]=samples[i];
	}

	vector<int> simbols;
	vector<int> findings;

	simbols.push_back(0);
	findings.push_back(0);

	qsort(samples, num_items, sizeof(int), &compare);

	for (i = 0; i < num_items; i++) {
		
		if (simbols[current_in_symbols] == samples[i]) {
			findings[current_in_symbols]++;
		} else {
			simbols.push_back(samples[i]);
			findings.push_back(1);
			current_in_symbols++;
		}
	}
	double total_symbols= simbols.size()-1;
	fprintf(probability_file,"%i",(int)round(total_symbols));

	for (i = 1; i < simbols.size(); i++) {
		fprintf(probability_file,"\n%i\t%f", simbols[i], findings[i]/total_symbols);
	}

	printf("\nProbabilities>%i",simbols.size());

	fclose(probability_file);

	free(samples);

	return samples_copy;
}