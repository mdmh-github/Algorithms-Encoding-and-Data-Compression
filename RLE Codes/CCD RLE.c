/*
 ============================================================================
 Name        : CCD RLE.c
 Author      : mdmh
 Version     :
 Copyright   : 
 Description : RLE Implementation
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "math.h"


/*
 * file_name: file path
 * size: how many chars
 * balance: ocurrence probability of the first factor
 * */
void generate_test_file (char * file_name,int size,float balance)
{
	/////////////////VARS
	char * text;
	unsigned int random;
	double factor_a = RAND_MAX*balance;
	
	/////////////////File must exist
	FILE * file = fopen(file_name,"wb");
	if (file == NULL) {
		fprintf(stderr, "ERROR: can't open %s\n", file_name);
		return;
	}
	text = (char*)(malloc(sizeof(char)*size));

	int current;
	
	/////////////////a or b depending on the occuence factor
	for (current=0;current<size;current++)
	{
		random =rand();
		text[current] =(random<=factor_a)?'a':'b';
	}

	/////////////////flush the generated string
	fwrite(text,sizeof(char)*size,1,file);
	fclose(file);

}

//just decode a encoded file
void decode (char * input_file_name, char * output_file_name)
{
	/////////////////VAR
	char character;
	int number;
	int current;
	FILE * input_file = fopen(input_file_name,"rb");

	/////////////////Input file must exist
	if (input_file == NULL) {
		fprintf(stderr, "ERROR: can't open %s\n", input_file_name);
		return;
	}

	/////////////////Output file must exist
	FILE * output_file = fopen(output_file_name,"wb");
	if (output_file == NULL) {
		fprintf(stderr, "ERROR: can't open %s\n", output_file_name);
		return;
	}

	/////////////////reading each char-number pair on the file
	while (fscanf(input_file,"%c%i",&character,&number)==2)
	{
		for (current =0;current<number;current++)
		{
			fprintf(output_file,"%c",character);
		}
	}

	fclose(input_file);
	fclose(output_file);
}

//Just encode a file
void encode (char * input_file_name, char * output_file_name)
{
	/////////////////VARS
	char last_char;
	int count=0;
	char current_char;

	/////////////////Input file must exist
	FILE * input_file = fopen(input_file_name,"rb");
	
	if (input_file == NULL) {
		fprintf(stderr, "ERROR: can't open %s\n", input_file_name);
		return;
	}
	/////////////////Output file must exist
	FILE * output_file = fopen(output_file_name,"wb");
	
	if (output_file == NULL) {
		fprintf(stderr, "ERROR: can't open %s\n", output_file_name);
		return;
	}
	/////////////////First case is handled manually
	current_char=getc(input_file);
	last_char=current_char;
	count++;

	/////////////////for the next cases
	while ((current_char= getc(input_file))!=-1){
		/////////////////same char: increase its count
		if (current_char==last_char)
		{
			count++;
		}
		/////////////////otherwise: flush the char with its count, and sets the new char with count =1
		else
		{
			fprintf(output_file,"%c%i",last_char,count);
			count =1;
			last_char=current_char;
		}
	}
	fprintf(output_file,"%c%i",last_char,count);

	fclose(input_file);
	fclose(output_file);
}


// how much has been reduce a file encoded
void percentageCompression (char * file1, char * file2)
{
	/////////////////Input file must exist
	FILE* f1 = fopen(file1,"rb");	
	if (file1 == NULL) {
		fprintf(stderr, "ERROR: can't open %s\n", file1);
		return;
	}
	
	/////////////////output file must exist
	FILE* f2 = fopen(file2,"rb");
	if (file2 == NULL) {
		fprintf(stderr, "ERROR: can't open %s\n", file2);
		return;
	}
	double l1=0,l2=0;

	/////////////////counts the chars of the encoded file
	while (getc(f2)!=-1)
	{
		l2++;
	}

	/////////////////counts the chars of the decoded/original file
	while (getc(f1)!=-1)
	{
		l1++;
	}

	printf ("%s\n:%.0f \n%s\n:%.0f\n",file1,l1,file2,l2);
	
	/////////////////calculate the compresion percentage
	double comp=100-((l2/l1)*100);
	printf ("%f",comp);
}


int main(int vargs,char * args[]) {

	char c =args [1][0];

	switch(c)
	{
	case '0':
		printf ("Starting the compression of '%s'...\n",args[2]);
		encode(args[2],args[3]);
		printf ("Compression finished. Code generated on: '%s'...\n",args[2]);
		break;

	case '1':
		printf ("Starting the decompression of '%s'...\n",args[2]);
		decode(args[2],args[3]);
		printf ("Decompression finished. Text regenerated on: '%s'...\n",args[2]);
		break;
	default:
		printf("Argument Error");
		break;
	}

	return EXIT_SUCCESS;
}
