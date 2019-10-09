//============================================================================
// Name        : CCDSindrome.cpp
// Author      : mdmh
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "stdlib.h"

#include "math.h"
#include <stdio.h>

using namespace std;


struct mapp
{
	int code_length;
	int wredundance_length;
	int * code;
	int * wredundance;
};

int H[]={
		1,0,0,0,0,1,1,
		0,1,0,0,1,0,1,
		0,0,1,0,1,1,0,
		0,0,0,1,1,1,1
};
typedef mapp map;

int H_width=7;
int H_height=4;



int H_t[]={
		0,1,1,
		1,0,1,
		1,1,0,
		1,1,1,
		1,0,0,
		0,1,0,
		0,0,1,
};

int H_t_width=3;
int H_t_height=7;

void print_permutation (int * permutation,int length)
{
	for (int j=0;j<length;j++)
	{
		printf ("%i,",permutation[j]);
	}
}


int ** getPermutations()
{
	int ** permutations = (int**)(malloc(sizeof(int*)*16));
	int * current_permutation;
	int current=0;
	for (int i3=0;i3<2;i3++)
	{
		for (int i2=0;i2<2;i2++)
		{
			for (int i1=0;i1<2;i1++)
			{
				for (int i0=0;i0<2;i0++)
				{
					current_permutation= (int*)(malloc(sizeof(int)*4));
					current_permutation[0]=i0;
					current_permutation[1]=i1;
					current_permutation[2]=i2;
					current_permutation[3]=i3;
					permutations[current]=current_permutation;
					current++;
				}
			}
		}
	}

	return permutations;
}


void print_permutations (int ** permutations)
{
	for (int i=0;i<16;i++)
	{
		print_permutation(permutations[i],4);
		printf ("\n");
	}
}


int * get_redundance (int * permutation,int *h)
{
	//printf ("\ncreating redundance\n");
	int * permutation_with_redundance = (int *)(malloc (sizeof (int)*7));
	int * support_array=(int *)(malloc (sizeof (int)*4));
	int position;
	int current_value;


	permutation_with_redundance[0]=permutation[0];
	permutation_with_redundance[1]=permutation[1];
	permutation_with_redundance[2]=permutation[2];
	permutation_with_redundance[3]=permutation[3];


	for (int width =0;width<7;width++)
	{

		//filling support array with & operation
		for (int height =0;height<4;height++)
		{
			position =(7*height)+width;
			support_array[height]=h[position]&permutation[height];
		}

		current_value=support_array[0];
		for (int height =1;height<4;height++)
		{
			current_value^=support_array[height];
		}

		permutation_with_redundance[width]=current_value;
	}
	free(support_array);
	return permutation_with_redundance;
}


mapp ** get_map ()
{
	int ** permutations =getPermutations();
	mapp ** map= (mapp**)(malloc(sizeof(mapp*)*16));

	for (int current=0;current<16;current++)
	{
		map[current] = (mapp*)(malloc(sizeof(mapp)));
		map[current]->code_length=4;
		map[current]->wredundance_length=7;
		map[current]->code=permutations[current];
		map[current]->wredundance=get_redundance(permutations[current],H);
		//		print_permutation(map[current]->code,map[current]->code_length);
		//		//		printf("\n");
		//		printf ("%i: ",current);
		//		print_permutation(map[current]->wredundance,map[current]->wredundance_length);
		//		printf("\n");
	}
	return map;
}

void flip (int * array, int position)
{
	array[position]=(array[position]==1)?0:1;
}

int ** get_test_sequenses (mapp** map,int map_length, int test_quantity)
{
	int ** tests= (int **)(malloc(sizeof(int*)*16));


	int selected_error;

	for (int current =0;current<16;current++)
	{

		int length=map[current]->wredundance_length;


		;
		tests[current]= (int*)(malloc(sizeof(int)*length));

		for (int j=0;j<length;j++)
		{
			tests[current][j]=map[current]->wredundance[j];
		}

		selected_error= rand()%length;

		flip (tests[current],selected_error);
		printf("\nA copy of the sequence in %i was created with the bit %i flipped",current,selected_error);
	}
	return tests;
}




int to_position_error(int * detected_error_in, int length)
{
	int position=0;

	//for (int current =length-1;current>=0;current--)
	for (int current =0;current<length;current++)

	{
		//	printf ("\nposition:%i\n",position);
		//	printf ("%i:%i\n",current,detected_error_in[current]);
		if (detected_error_in[current]==1)
		{

			position|= (int)pow(2,current);

			//position|= (int)pow(detected_error_in[2-current],2);
			//	printf("%f\n",pow(current,2));

		}
	}

	//	printf ("\nRETURNED position:%i\n",position);
	return position;
}


void seek_and_repair (int ** tests,int test_quantity,int test_length,int * H_t)
{
	printf ("\nSeeking and repairing...\n");
	int * support_array = (int *)(malloc(sizeof(int*)*test_length));
	int * detected_error_in =(int *)(malloc(sizeof(int*)*3));
	int current_value;
	int error_postion;
	int position;
	int current_array_value;
	for (int current =0;current<test_quantity;current++)
	{


		for (int width =0;width<3;width++)
		{

			//	printf ("\n-------------------");
			for (int height =0;height<7;height++)
			{
				current_array_value=tests[current][height];
				position = (3*(height))+width;
				support_array[height]=H_t[position]&current_array_value;
				printf("\n%i =%i & %i",
						support_array[height],
						H_t[position],
						current_array_value);

			}
			//				printf("\n");

			current_value=support_array[0];
			for (int height =1;height<7;height++)
			{
				current_value^=support_array[height];
			}

			printf ("\ncurrent_value:%i",current_value);

			detected_error_in[2-width]=current_value;
		}

		printf("\n");
		print_permutation(detected_error_in,3);
		printf("\n");
		error_postion= to_position_error(detected_error_in,3);
		flip(tests[current],error_postion);

		printf("\n%i: An error was found on the current sequence on the bit %i, it was flipped",current,error_postion);
		printf ("\n");
		print_permutation(detected_error_in,3);
		printf ("\n");
		print_permutation(tests[current],7);
		printf ("\n");
	}
}


int main() {
	int number_of_test =16;
	printf ("\nbegining\n\n");
	mapp ** map =get_map();
	printf ("\n Creating permutations\n\n");
	for(int i=0;i<16;i++)
	{
		printf("\n");
		print_permutation(map[i]->code,4);

	}

	printf ("\n Creating redundance\n\n");
	for(int i=0;i<16;i++)
	{
		printf("\n");
		print_permutation(map[i]->wredundance,7);

	}


	printf("\n\nCreeating tests\n");

	//int ** tests= get_test_sequenses(map,16,number_of_test);
			int ** tests= (int**)(malloc(sizeof(int*)));

			tests[0]=(int*)(malloc(sizeof(int)*7));
			tests[0][0]=1;
			tests[0][1]=1;
			tests[0][2]=0;
			tests[0][3]=1;
			tests[0][4]=1;
			tests[0][5]=0;
			tests[0][6]=1;
	//	printf("Printing tests\n");
	//	for(int i=0;i<number_of_test;i++)
	//	{
	//		printf("\n");
	//		print_permutation(tests[i],7);
	//
	//	}

	seek_and_repair(tests,16,7,H_t);

	printf ("\nend");


	return 0;
}






