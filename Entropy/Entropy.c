//============================================================================
// Name        : Second Homework
// Author      : mdmh
// Version     : 1.0.0
// Copyright   :
// Description : A program that calculates the entropy for a given text based on a set of characters
//============================================================================


#include "stdio.h"
#include "stdlib.h"
#include "math.h"
using namespace std;

double logarithm(double d,double b);
int main() {
	////////////////////////////////////////////////////////////////////////////vars
	double
	entropy =0,
	probability;
	int
	characterRead,
	NumberOfcharactersRead=0;
	int charactersSet[37]={

			//  blank (1)
			32,
			// numbers
			//  0  1  2  3  4  5  6  7  8  9 (10)(11)
			48,49,50,51,52,53,54,55,56,57,
			// letters
			//  a   b   c   d  e   f   g   h   i   j   k   l   m   n   o   p	(16)(27)
			97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,112,
			// q   r   s   t   u   v   w   x   y   z							(10)(37)
			113,114,115,116,117,118,119,120,121,122};

	int count [37]={};

	FILE *inputFile;
	inputFile= fopen("meta.txt","rb");
	////////////////////////////////////////////////////////////////////////////lets do it

	while ((characterRead =getc(inputFile))!=-1)
	{
		NumberOfcharactersRead++;
		//a would be A, b would be B etc...
		characterRead+=(characterRead >='A' && characterRead<='Z')? 32 : 0;//

		// is in the set?
		for (int i =0;i<37;i++)
			if (characterRead==charactersSet[i])
				count[i]++;
	}
	printf("Caractér\tApariciones\tProbabilidad\n");
	for (int i =0;i<37;i++)
	{
		probability =((count[i]+0.0)/NumberOfcharactersRead);
		entropy+=(probability>0)? probability * logarithm(1/probability,2):0;
		printf("'%c'\t\t%i\t\t%3.5f\n",charactersSet[i],count[i],probability);
	}
	printf("Número de caracteres:%i\n",NumberOfcharactersRead);
	printf("La entropia es: %f",entropy);

	fclose(inputFile);
	return 0;
}

double logarithm(double d,double b)
{
	double h = log (d)/log(b);
	return h;
}