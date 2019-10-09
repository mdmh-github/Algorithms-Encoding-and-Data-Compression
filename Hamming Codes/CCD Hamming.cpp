//============================================================================
// Name        : CCD.cpp
// Author      : mdmh
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#include <vector>
using namespace std;

int c1 = 0;
int c2 = 1;
int c3 = 2;
int c4 = 3;
int c5 = 4;
int c6 = 5;
int c7 = 6;
int pos0 = pow(2, 0);
int pos1 = pow(2, 1);
int pos2 = pow(2, 2);
int pos3 = pow(2, 3);
int pos4 = pow(2, 4);
int pos5 = pow(2, 5);
int pos6 = pow(2, 6);
int pos7 = pow(2, 7);

void print_array(int * array, int length) {
	for (int j = 0; j < length; j++) {
		printf("%i", array[j]);
	}
}

int ** getPermutations() {
	int ** permutations = (int**) (malloc(sizeof(int*) * 16));
	int * current_permutation;
	int current = 0;
	for (int i3 = 0; i3 < 2; i3++) {
		for (int i2 = 0; i2 < 2; i2++) {
			for (int i1 = 0; i1 < 2; i1++) {
				for (int i0 = 0; i0 < 2; i0++) {
					current_permutation = (int*) (malloc(sizeof(int) * 4));
					current_permutation[0] = i0;
					current_permutation[1] = i1;
					current_permutation[2] = i2;
					current_permutation[3] = i3;
					permutations[current] = current_permutation;
					current++;
				}
			}
		}
	}

	return permutations;
}

int getP1(int * code) {
	int r = code[c7] ^ code[c5] ^ code[c3];
	return r;
}

int getP2(int * code) {
	int r = code[c7] ^ code[c6] ^ code[c3];
	return r;
}

int getP3(int * code) {
	int r = code[c7] ^ code[c6] ^ code[c5];
	return r;
}

int * generate_code(int* word) {
	int * code = (int*) (malloc(sizeof(int) * 7));
	code[c7] = word[0];
	code[c6] = word[1];
	code[c5] = word[2];
	code[c3] = word[3];
	code[c4] = getP3(code);
	code[c2] = getP2(code);
	code[c1] = getP1(code);
	return code;
}

int getT1(int * code) {
	int r = code[c7] ^ code[c5] ^ code[c3] ^ code[c1];
	return r;
}

int getT2(int * code) {
	int r = code[c7] ^ code[c6] ^ code[c3] ^ code[c2];
	return r;
}

int getT3(int * code) {
	int r = code[c7] ^ code[c6] ^ code[c5] ^ code[c4];
	return r;
}

int * get_error(int * code) {
	int * error = (int*) (malloc(sizeof(int) * 3));
	error[0] = getT1(code);
	error[1] = getT2(code);
	error[2] = getT3(code);
	return error;
}

int get_error_position(int * error) {
	int position = 0;
	position |= (int) (error[0] * pow(2, 0));
	position |= (int) (error[1] * pow(2, 1));
	position |= (int) (error[2] * pow(2, 2));
	return position;
}

int fix_error(int * code) {
	int * error = get_error(code);
	int error_positon = get_error_position(error);

	if (error_positon != 0) {
		code[error_positon - 1] ^= 1;
	}

	free(error);
	return error_positon;
}

void code(char * file_name_in, char * file_name_out) {
	//	int ** words = getPermutations();
	//	int ** codes = (int**)(malloc(sizeof(int*)*16));

	char c;
	FILE * file_in = fopen(file_name_in, "rb");
	FILE * file_out = fopen(file_name_out, "wb");
	int * first = (int *) (malloc(sizeof(int) * 4));
	int * second = (int *) (malloc(sizeof(int) * 4));

	printf("\n*Text:\n");

	vector<int> redundance_bites;
	int error_position;
	while (fread(&c, sizeof(char), 1, file_in) == 1) {
		printf("%c", c);
		first[0] = (c & pos0) == pos0;
		first[1] = (c & pos1) == pos1;
		first[2] = (c & pos2) == pos2;
		first[3] = (c & pos3) == pos3;

		second[0] = (c & pos4) == pos4;
		second[1] = (c & pos5) == pos5;
		second[2] = (c & pos6) == pos6;
		second[3] = (c & pos7) == pos7;

		int * r1 = generate_code(first);
		int * r2 = generate_code(second);


		//Adding errors
		error_position = rand()%7;
		r1[error_position] ^= 1;

		//Adding errors
		error_position = rand()%7;
		r2[error_position] ^= 1;

		redundance_bites.push_back(r1[0]);
		redundance_bites.push_back(r1[1]);
		redundance_bites.push_back(r1[2]);
		redundance_bites.push_back(r1[3]);
		redundance_bites.push_back(r1[4]);
		redundance_bites.push_back(r1[5]);
		redundance_bites.push_back(r1[6]);

		redundance_bites.push_back(r2[0]);
		redundance_bites.push_back(r2[1]);
		redundance_bites.push_back(r2[2]);
		redundance_bites.push_back(r2[3]);
		redundance_bites.push_back(r2[4]);
		redundance_bites.push_back(r2[5]);
		redundance_bites.push_back(r2[6]);

	}
	int padd = (8 - (redundance_bites.size() % 8));
	padd = (padd == 8) ? 0 : padd;

	for (int i = 0; i < padd; i++) {
		redundance_bites.push_back(0);
	}

	int x = 0;
	for (unsigned int i = 0; i < redundance_bites.size(); i += 8) {
		x++;
		int byte1 = 0;
		byte1 |= redundance_bites[i] * pos0;
		byte1 |= redundance_bites[i + 1] * pos1;
		byte1 |= redundance_bites[i + 2] * pos2;
		byte1 |= redundance_bites[i + 3] * pos3;
		byte1 |= redundance_bites[i + 4] * pos4;
		byte1 |= redundance_bites[i + 5] * pos5;
		byte1 |= redundance_bites[i + 6] * pos6;
		byte1 |= redundance_bites[i + 7] * pos7;
		fprintf(file_out, "%c", byte1);
	}
	fclose(file_out);
	fclose(file_in);

}

void decode(char * file_name_in, char * file_name_out) {

	FILE * file_in;
	FILE * file_out;

	int * coded1 = (int *) (malloc(sizeof(int) * 7));
	int * coded2 = (int *) (malloc(sizeof(int) * 7));

	file_in = fopen(file_name_in, "rb");

	vector<int> get;

	file_out = fopen(file_name_out, "wb");
	printf("*Decoded Text with errors:\n\n");

	char c;

	while (fread(&c, sizeof(char), 1, file_in) == 1) {

		get.push_back((c & pos0) == pos0);
		get.push_back((c & pos1) == pos1);
		get.push_back((c & pos2) == pos2);
		get.push_back((c & pos3) == pos3);
		get.push_back((c & pos4) == pos4);
		get.push_back((c & pos5) == pos5);
		get.push_back((c & pos6) == pos6);
		get.push_back((c & pos7) == pos7);
	}

	for (unsigned int i = 0; i < get.size() && i + 14 <= get.size(); i += 14) {
		int decoded_char = 0;
		coded1[0] = get[i];
		coded1[1] = get[i + 1];
		coded1[2] = get[i + 2];
		coded1[3] = get[i + 3];
		coded1[4] = get[i + 4];
		coded1[5] = get[i + 5];
		coded1[6] = get[i + 6];

		coded2[0] = get[i + 7];
		coded2[1] = get[i + 8];
		coded2[2] = get[i + 9];
		coded2[3] = get[i + 10];
		coded2[4] = get[i + 11];
		coded2[5] = get[i + 12];
		coded2[6] = get[i + 13];

		decoded_char |= coded1[c7] * pos0;
		decoded_char |= coded1[c6] * pos1;
		decoded_char |= coded1[c5] * pos2;
		decoded_char |= coded1[c3] * pos3;

		decoded_char |= coded2[c7] * pos4;
		decoded_char |= coded2[c6] * pos5;
		decoded_char |= coded2[c5] * pos6;
		decoded_char |= coded2[c3] * pos7;
		printf("%c", decoded_char);

	}

	printf("\n\n");
	printf("\n*Decoded Text without errors:\n\n");
	for (unsigned int i = 0; i < get.size() && i + 14 <= get.size(); i += 14) {
		int decoded_char = 0;
		coded1[0] = get[i];
		coded1[1] = get[i + 1];
		coded1[2] = get[i + 2];
		coded1[3] = get[i + 3];
		coded1[4] = get[i + 4];
		coded1[5] = get[i + 5];
		coded1[6] = get[i + 6];
		fix_error(coded1);

		coded2[0] = get[i + 7];
		coded2[1] = get[i + 8];
		coded2[2] = get[i + 9];
		coded2[3] = get[i + 10];
		coded2[4] = get[i + 11];
		coded2[5] = get[i + 12];
		coded2[6] = get[i + 13];
		fix_error(coded2);

		decoded_char |= coded1[c7] * pos0;
		decoded_char |= coded1[c6] * pos1;
		decoded_char |= coded1[c5] * pos2;
		decoded_char |= coded1[c3] * pos3;

		decoded_char |= coded2[c7] * pos4;
		decoded_char |= coded2[c6] * pos5;
		decoded_char |= coded2[c5] * pos6;
		decoded_char |= coded2[c3] * pos7;

		fprintf(file_out, "%c", decoded_char);
		printf("%c", decoded_char);
	}

}

void bit_test() {
	printf("Error Test:\n\n");
	int ** words = getPermutations();
	int ** codes = (int**) (malloc(sizeof(int*) * 16));
	printf("number\tword\tcode\n");
	for (int i = 0; i < 16; i++) {

		printf("%i\t",i);
		codes[i] = generate_code(words[i]);
		print_array(words[i], 4);
		printf("\t");
		print_array(codes[i], 7);
		printf("\n");
	}
	printf("\n");
	printf("\n");
	for (int i = 0; i < 16; i++) {
		int herror = rand() % 7;
		codes[i][herror] ^= 1;
		printf("Set error on: %i >",herror);
		print_array(codes[i], 7);
		printf("\n");
	}

	printf("\n");
	printf("\n");
	for (int i = 0; i < 16; i++) {

		int fix=fix_error(codes[i]);
		printf("fixed error on %i >",fix-1);
		print_array(codes[i], 7);
		printf("\n");
	}
}

int main(int num_args, char * args[]) {

	switch (args[1][0]) {

	//test bit
	case '0':
		bit_test();
		break;

		//code file
	case '1':
		code(args[2], args[3]);
		break;

		//decode file
	case '2':
		decode(args[2], args[3]);
		break;

	}

	return 0;

}
