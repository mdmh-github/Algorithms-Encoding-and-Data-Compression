//============================================================================
// Name        : ShanonFano.cpp
// Author      : mdmh
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "stdio.h"
#include "string.h"
#include "math.h"
struct SYMBOL {
	char symbol[10]; //input simbol
	float probability;
	int code[20];
	int count; //next byte position
};

typedef struct SYMBOL node;


void permute(int k,int size,int *array, int * best,node *simbols, node * temp_simbols, double bestReached);
void order(node simbols[], int length);
double difference(node simbols[],double length);


void prints(int from, int to, node simbols[]) {
	int i;
	for ( i = from; i <= to; i++) {
		printf("\n%s\t%f", simbols[i].symbol, simbols[i].probability);
	}
}

//////////////////// Recursive function that generates the codes for all the simbols
void shannon_algorithm(int from, int to, node simbols[]) {

	//////////////////// if the end has been reached
	if (from == to || from > to)
		return;

	//////////////////// vars
	float first_part = 0;
	float second_part = 0;
	float best_difference = 0;
	float current_difference = 0;
	int cut_here, j,i;

	//////////////////// for the last element in the first part
	if ((from + 1) == to) {
		simbols[to].count++;
		simbols[from].count++;
		simbols[to].code[simbols[to].count] = 0;
		simbols[from].code[simbols[from].count] = 1;
		return;
	}

	//////////////////// calculates the probability of both parts
	for ( i = from; i <= to - 1; i++)
		first_part += simbols[i].probability;
	second_part += simbols[to].probability;

	//////////////////// calculates the initial difference
	best_difference = first_part - second_part;
	if (best_difference < 0)
		best_difference *= -1;
	/////////////////////////////////////////////////////////////////////////////
	j = 2;
	while (j != to - from + 1) {
		cut_here = to - j;
		first_part = 0;
		second_part = 0;

		//////////////////////sums the CURRENT(in the recursive instance) probabilities of the first part
		for ( i = to; i <= cut_here; i++)
			first_part += simbols[i].probability;

		//////////////////////sums the CURRENT(in the recursive instance) probabilities of the second part
		for ( i = to; i > cut_here; i--)
			second_part += simbols[i].probability;

		////////////////////	//calculates the current difference
		current_difference = first_part - second_part;
		if (current_difference < 0)
			current_difference *= -1;

		//////////////////////if the current difference is bigger than the best difference
		if (current_difference >= best_difference)
			break;
		//otherwise update the best diffetence
		best_difference = current_difference;
		j++;
	}
	//////////////////////define the plase of the division of simbols
	cut_here++;

	//////////////////////1 is added to all the codes in construction of the first part
	for ( i = from; i <= cut_here; i++) {
		simbols[i].count++;
		simbols[i].code[simbols[i].count] = 1;
	}
	//////////////////////0 is added to all the codes in construction of the second part
	for ( i = cut_here + 1; i <= to; i++) {
		simbols[i].count++;
		simbols[i].code[simbols[i].count] = 0;

	}

	//////////////////////shanon algorith is applied on the first part recursively
	shannon_algorithm(from, cut_here, simbols);

	//////////////////////shanon algorith is applied on the second part recursively
	shannon_algorithm(cut_here + 1, to, simbols);
}


//////////////////////this function orders the simbols to get the most balance distribution on its probabilities taking in account
//////////////////////the array will be splited in two parts
void order(node * simbols, int length)
{
	int best[length];
	int index[length];
	node temp_simbols[length];
	int i;
	////////////////////// init array of index
	for ( i =0;i<length;i++)
		index[i]=i;

	//////////////////////Check for all the posible combinations
	//////////////////////the last parameter is the worst case on balance (diffence of both is 1, that means 100%)
	permute(length,length,index,best,simbols,temp_simbols,1);

	//////////////////////a copy of the original simbols will be needed
	for ( i =0;i<length;i++)
	{
		temp_simbols[i]=simbols[i];
	}
	////////////////////// at this point the best array has the index order of the best permutation
	////////////////////// so in the next for simbols are moved to that positios

	for ( i =0;i<length;i++)
	{
		simbols[i]=temp_simbols[best[i]];
	}

}

//A to, B from
void copyState(int A[],int B[], int length)
{
	int i;
	for ( i = 0; i < length; ++i) {
		A[i]=B[i];
	}
}


void permute(int k,int size,int *array, int * best,node *simbols, node * temp_simbols, double bestReached){

	int i,j;
	//////////////////////if the best permutation has been reached it has no sense continue searching
	if (bestReached ==0)
		return;

	//////////////////////one complete permutation has been reached
	//////////////////////now its time to check his balace
	if (k==0)
	{
		//////////////////////temp simbols are fixed to its current permutated index
		for ( j=0 ; j < size; j++)
			temp_simbols[j]=simbols[array[j]];


		double e = difference(temp_simbols,size);
		//////////////////////if this permutation is more balanced
		//////////////////////the bestReached is updated and the best state are saved
		if (e<=bestReached)
		{
			bestReached = e;
			copyState(best,array,size);
		}
	}
	else{
		int temp;
		for ( i=k-1;i>=0;i--){
			//////////////////////permutation
			temp = array[i];
			array[i]=array[k-1];
			array[k-1]=temp;
			//////////////////// no next permutation
			permute(k-1,size,array,best,simbols,temp_simbols,bestReached);
			////////////////////return to the original position
			temp = array[i];
			array[i]=array[k-1];
			array[k-1]=temp;
		}
	}

	return;
}

//gets the difference of both parts
double difference(node simbols[],double length)
{
	int i;
	int split_value =floor(length/2.0); ////////////////////// in odd case ceil
	double part1=0;
	double part2=0;

	//////////////////////sum the first part probabilities
	for ( i =0;i<split_value;i++)
	{
		part1+= simbols[i].probability;
	}

	//////////////////////sum the second part probabilities
	for ( i =split_value;i<length;i++)
	{
		part2+= simbols[i].probability;
	}

	//////////////////////abs() just for ints so...
	if (part1>part2)
	{
		return part1-part2;
	}
	else
	{
		return part2-part1;
	}

}


int main(int argc,char * args []) {
	int numberOfSimbols=0, i, j;
	float readProbability;//,total = 0;
	char ch[10];
	node temporal;

	FILE * file;
	file = fopen(args[1],"rb");
	if (file == NULL) {
		fprintf(stderr, "ERROR: can't open %s\n", args[1]);
		return 0;		}
	printf ("leyendo :%s\n",args[1]);
	fscanf(file,"%i",&numberOfSimbols);
	printf ("simbolos:%i",numberOfSimbols);
	node simbols [numberOfSimbols];
	for (i=0;i<numberOfSimbols;i++)
	{
		fscanf(file,"%s %f",ch,&readProbability);
		printf("\n%s %f",ch,readProbability);
		strcpy(simbols[i].symbol, ch);
		simbols[i].probability = readProbability;
		simbols[i].count=0;
		for (j=0;j<20;j++)
		{
			simbols[i].code[j]=0;
		}
	}
	fclose(file);

	//order by probabilities desc
	for (j = 1; j <= numberOfSimbols - 1; j++) {
		for (i = 0; i < numberOfSimbols - 1; i++) {
			if ((simbols[i].probability) > (simbols[i + 1].probability)) {
				temporal.probability = simbols[i].probability;
				strcpy(temporal.symbol, simbols[i].symbol);
				simbols[i].probability = simbols[i + 1].probability;
				strcpy(simbols[i].symbol, simbols[i + 1].symbol);
				simbols[i + 1].probability = temporal.probability;
				strcpy(simbols[i + 1].symbol, temporal.symbol);
			}
		}
	}
	for (i = 0; i < numberOfSimbols; i++)
		simbols[i].count = -1;

	//
	//	printf ("\nAntes de ordenar");
	//	printf ("\n-------------------------------------------------------------");
	//	prints(0,numberOfSimbols-1,s);
	//	printf ("\n-------------------------------------------------------------");
	//
	//	order(s,numberOfSimbols-1);
	//
	//	printf("\n\nDespues de ordenar");
	//	printf ("\n-------------------------------------------------------------");
	//
	//	prints(0,numberOfSimbols-1,s);
	//	printf ("\n-------------------------------------------------------------");
	//	printf ("\nGenerando código con ShanonFano");

	shannon_algorithm(0, numberOfSimbols - 1, simbols);

	printf("\n---------------------------------------------------------------");
	printf("\n\n\nSímbolo\tProbabilidad\tCódigo");
	for (i = numberOfSimbols - 1; i >= 0; i--) {
		printf("\n%s\t%f\t", simbols[i].symbol,simbols[i].probability);
		for (j = 0; j <= simbols[i].count; j++)
			printf("%d", simbols[i].code[j]);
	}
	printf("\n------------------------------------------------------------");


	return 0;
}




































