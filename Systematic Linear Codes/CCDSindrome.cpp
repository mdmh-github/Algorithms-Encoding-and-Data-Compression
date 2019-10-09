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
	int error_length;
	int * code;
	int * wredundance;
	int * error;

};

int G[]={
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

void print_array (int * array,int length)
{
	for (int j=0;j<length;j++)
	{
		printf ("%i",array[j]);
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



int * get_redundance (int * permutation,int *H_matrix)
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

	printf("\n-----------------------------");
	for (int width =0;width<7;width++)
	{
		//filling support array with & operation
		for (int height =0;height<4;height++)
		{
			position =(7*height)+width;
			support_array[height]=H_matrix[position]&permutation[height];

			printf("\n%i = %i & %i",support_array[height],permutation[height],H_matrix[position]);
		}

		current_value=support_array[0];

		for (int height =1;height<4;height++)
		{
			current_value^=support_array[height];
		}
		printf("\nxor %i",current_value);

		printf("\n");
		permutation_with_redundance[width]=current_value;

	}
	printf("\n");
	free(support_array);
	return permutation_with_redundance;
}

void print_map (mapp** map, int length)
{
	for(int i =0;i<length;i++)
	{
		print_array(map[i]->code, map[i]->code_length);
		printf (" - ");
		print_array(map[i]->wredundance, map[i]->wredundance_length);
		printf (" - ");
		print_array(map[i]->error, map[i]->error_length);
		printf("\n");
	}
}



int *get_Error (int * array,int array_length)
{
	int selected_error;

	int * error= (int*)(malloc(sizeof(int)*array_length));

	for (int current=0;current<array_length;current++)
	{
		error[current]=array[current];
	}

	print_array(error,array_length);
	//	int r=(rand()%array_length);
	//////////////////////////////////////////////////////////////////////////////////////
	selected_error=3;
	//	printf (">%i",selected_error);
	//selected_error= ceil (selected_error);
	//(((rand()+10e-10)%array_length));
	//selected_error=2;
	error[selected_error]^=1;
	printf("\ncreate error on:%i\n",selected_error);
	//flip (error,selected_error);

	return error;
}

mapp ** get_map (int c)
{
	int ** permutations =getPermutations();

	mapp ** map= (mapp**)(malloc(sizeof(mapp*)*c));

	for (int current=0;current<c;current++)
	{
		map[current] = (mapp*)(malloc(sizeof(mapp)));
		map[current]->code_length=4;
		map[current]->wredundance_length=7;
		map[current]->code=permutations[current];
		int * redundance =get_redundance(permutations[current],G);
		map[current]->wredundance=redundance;
		map[current]->error_length=7;
		map[current]->error= get_Error(redundance,7);

	}
	return map;
}


int to_position_error(int * detected_error_in, int length)
{
	int position=0;

	for (int current =0;current<length;current++)
		//for (int current =length-1;current>=0;current--)
	{
		//	printf ("\nposition:%i\n",position);
		//	printf ("%i:%i\n",current,detected_error_in[current]);
		//	detected_error_in[current]^=1;
		if (detected_error_in[current]==1)
		{

			position|= (int)pow(2,current);
			//		position|= (int)pow(2,detected_error_in[current]*current);

			//position|= (int)pow(detected_error_in[2-current],2);
			//	printf("%f\n",pow(current,2));

		}

	}
	//position^=1;

	printf("\n");
	print_array(detected_error_in,length);
	//	printf ("\nRETURNED position:%i\n",position);
	return position;
}


void seek_and_repair (mapp ** map,int length,int * H_t)
{
	printf ("\nSeeking and repairing...\n");
	int * support_array = (int *)(malloc(sizeof(int*)*7));
	int * detected_error_in =(int *)(malloc(sizeof(int*)*3));

	int xor_value;
	for (int current=0;current<length;current++)
	{

		for (int width =0;width<3;width++)
		{
			for (int height =0;height<7;height++)
			{
				int position = (3*height)+width;
				support_array[height]=map[current]->error[6-height]&H_t[position];
				printf("\n(%i,%i)\t%i & %i = %i",height,position,map[current]->error[6-height],H_t[position],support_array[height]);
			}
			printf("\n");
			print_array(support_array,7);
			printf("\n");

			xor_value= support_array[0];

			for (int i=1;i<7;i++)
			{
				xor_value^=support_array[i];
			}

			printf("\nxor: %i",xor_value);
			detected_error_in[2-width]=xor_value;
			//////////////////////////////////////////////////////////////////////////////////////
			//	detected_error_in[width]^=1;
		}
		int position_error = to_position_error(detected_error_in,3);
		printf("\nerror on : %i",position_error);
		printf("\n--------------------------------\n");
	}

}


int main() {
	//printf ("\nbegining\n\n");
	int elements =16;
	mapp ** map =get_map(elements);
	//printf ("\n Creating permutations\n\n");
	//for(int i=0;i<16;i++)
	//{
	//printf("\n");
	//	print_array(map[i]->code,4);

	//}
	print_map(map,elements);
	//	printf ("\n Creating redundance\n\n");
	//	for(int i=0;i<16;i++)
	//	{
	//		printf("\n");
	//		print_array(map[i]->wredundance,7);
	//
	//	}
	//seek_and_repair(map,elements,H_t);

	//printf("\n\nCreeating tests\n");

	//int ** tests= get_test_sequenses(map,16,number_of_test);


	//seek_and_repair(tests,16,7,H_t);

	//printf ("\nend");

	int pos0=pow(2,0);
	int pos1=pow(2,1);
	int pos2=pow(2,2);
	int pos3=pow(2,3);
	int pos4=pow(2,4);
	int pos5=pow(2,5);
	int pos6=pow(2,6);
	int pos7=pow(2,7);


	int * first = (int *) (malloc(sizeof(int)*4));
	int * second = (int *) (malloc(sizeof(int)*4));
	FILE * file = fopen ("file.txt","rb");
	unsigned char c;
	printf("\n-----------------------------------\n");
	while ((c=getc(file))!=-1)
	{

		first[0]=(c&pos0)==pos0;
		first[1]=(c&pos1)==pos1;
		first[2]=(c&pos2)==pos2;
		first[3]=(c&pos3)==pos3;

		second[0]=(c&pos4)==pos4;
		second[1]=(c&pos5)==pos5;
		second[2]=(c&pos6)==pos6;
		second[3]=(c&pos7)==pos7;

		int* r1= get_redundance(first,G);
		int * r2 = get_redundance(second,G);

		print_array(first,4);
		printf("\n");
		print_array(r1,7);
		printf("\n");
		printf("\n");

		print_array(second,4);
		printf("\n");
		print_array(r2,7);
		printf("\n");
		printf("\n");
	}

	return 0;
}