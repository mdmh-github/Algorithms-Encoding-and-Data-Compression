/*
 ============================================================================
 Name        : CCD.c
 Author      : mdmh
 Version     :
 Copyright   :
 Description : lz77 implementation
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
typedef struct node_l node_lz77;
//typedef struct  linked_char linked_char;
//DEBUG int number=0;

//template used for write and read data
const char * code_template = "%i\t%i\t%i ";
//char representation, in oder to avoid create and destoy char arrays
struct node_l
{
	int character;
	node_lz77 * previous;
	node_lz77 * next;
	//DEBUG	int number;
};
///////////////////////////////////////////headers
void write_forward (int length, node_lz77 * current_char, node_lz77 * last);
node_lz77 * get_next_node (FILE* inputFile, node_lz77 * current_node);
void encode (char * input_file_name, char * output_file_name);
void backtrack (int offset,int length, node_lz77 * current_char, node_lz77 * last);
void write_forward (int length, node_lz77 * current_char, node_lz77 * last);
void decode (char * input_file_name, char * output_file_name);


//a function that provides nodes
node_lz77 * new_node(){
	return (node_lz77*)(malloc(sizeof (node_lz77)));
}

/*
 * A function that get the next node of a current node,
 * even if it has not been read.
 */

node_lz77 * get_next_node (FILE* inputFile, node_lz77 * current_node){

	int	characterRead;
	node_lz77 * next_node;

	if (current_node==NULL|| current_node->next==NULL){

		characterRead =getc(inputFile);
		next_node = new_node();
		next_node->character=characterRead;
		next_node->previous=current_node;
		next_node->next=NULL;
		//DEBUG	next_node->number=number;
		//DEBUG number++;
		return next_node;
	}
	else{
		return current_node->next;
	}
}


//Function that encodes
void encode (char * input_file_name, char * output_file_name)
{
	/// vars
	int end=0;
	node_lz77 * begin_word =NULL;
	node_lz77 *current_cursor=NULL;
	node_lz77 * buffer =NULL;
	node_lz77  *buffer_cursor =NULL;
	FILE * inputFile;
	FILE * outputFile;
	int
	current_offset,
	current_size,
	best_offset,
	best_size;


	//file must exist
	inputFile = fopen(input_file_name,"rb");
	
	if (inputFile == NULL) {
		fprintf(stderr, "ERROR: can't open %s\n", input_file_name);
		return;
	}

	outputFile=fopen(output_file_name,"a");
	//initializing vars
	best_size=0;
	best_offset=0;
	current_offset=0;
	current_size=0;
	//first case hardcoded
	//////////////////////////////////////////////////////////////////////////////////////
	current_cursor = get_next_node(inputFile,current_cursor);


	//fprintf(outputFile,code_template,best_offset,best_size,current_cursor->character);

	fwrite(&best_offset,sizeof(int),1,outputFile);
	fwrite(&best_size,sizeof(int),1,outputFile);
	fwrite(&current_cursor->character,sizeof(char),1,outputFile);


	current_cursor->next= get_next_node(inputFile,current_cursor);
	current_cursor= current_cursor->next;
	begin_word=current_cursor;
	buffer =begin_word->previous;
	buffer_cursor =buffer;
	//////////////////////////////////////////////////////////////////////////////////////

	do{
		do{
			current_offset++;

			do{
				
				//There is a match
				if (current_cursor->character == buffer_cursor->character){
					//increace the size of the equalness
					current_size++;
					//the next node is retrived
					current_cursor->next =get_next_node(inputFile,current_cursor);
					
					//but if there is no next node the end flag is set
					if (current_cursor->next->character==-1){
						end=1;
						break;
					}
					//here is the step
					current_cursor = current_cursor->next;
					//an the step of the buffer cursor
					buffer_cursor = buffer_cursor->next;
					
					//a better case found?
					if (current_size>best_size){
						best_size=current_size;
						best_offset=current_offset;
					}
				}
				//if there is no more match lets try with the revious buffer cursor
				else{
					break;
				}

			}while(1);

			if (end==1)
			{
				break;
			}
			/*
			 * the buffer back one step and the current cursor returns
			 * to the begining of the current word in order to allow a new searcg
			 * */
			buffer=buffer->previous;
			buffer_cursor=buffer;
			current_cursor=begin_word;
			current_size=0;
			
			//but if there's no more buffer finish this loop
			if (buffer==NULL)
				break;

		}while(1);
		
		//current cursor is moved to the last known char
		while(current_cursor!=NULL&& current_cursor->next!=NULL){
			current_cursor=current_cursor->next;
		}

		//if is the end the last output is wrote and finish this loop
		if (end==1)
		{
		//	fprintf(outputFile,code_template,best_offset,best_size,current_cursor->previous->character);


			fwrite(&best_offset,sizeof(int),1,outputFile);
			fwrite(&best_size,sizeof(int),1,outputFile);
			fwrite(&current_cursor->character,sizeof(char),1,outputFile);

			//DEBUG	printf(code_template,best_offset,best_size,current_cursor->previous->character);
			break;
		}

		//if not the end, the output is wrote
		//fprintf(outputFile,code_template,best_offset,best_size,current_cursor->character);
	//	printf("\n");
	//	printf(code_template,best_offset,best_size,current_cursor->character);


		fwrite(&best_offset,sizeof(int),1,outputFile);
		fwrite(&best_size,sizeof(int),1,outputFile);
		fwrite(&current_cursor->character,sizeof(char),1,outputFile);


		//DEBUG printf(code_template,best_offset,best_size,current_cursor->character);

		current_cursor->next = get_next_node(inputFile,current_cursor);
		
		if (current_cursor->next->character==-1)
		{
			break;
		}
		//preparing the pointer for the next loop
		current_cursor=current_cursor->next;
		begin_word=current_cursor;
		buffer = begin_word->previous;
		buffer_cursor = begin_word->previous;
		best_offset=0;
		best_size=0;
		current_offset=0;

	}while(1);


	fclose(inputFile);
	fclose(outputFile);
}


//recursively it goes to search a position
void backtrack (int offset,int length, node_lz77 * current_char, node_lz77 * last)
{
	if (offset!=0 && current_char!=NULL)
	{
		backtrack(offset-1,length,current_char->previous,last);
	}
	else{
		write_forward(length,current_char,last);
	}
}

//add to the buffer recursively has many elements has requested
void write_forward (int length, node_lz77 * current_char, node_lz77 * last)
{
	if (length==0)
	{
		return;
	}

	last->next = new_node();
	last->next->character =current_char->character;
	last->next->previous=last;
	last= last->next;
	current_char=current_char->next;
	write_forward(length-1,current_char,last);

}

void decode (char * input_file_name, char * output_file_name){

	//Vars
	FILE * inputFile;
	FILE * outputFile;

	int offset, size;
	int scape;
	node_lz77 * last,* current_char,*first, * on_scrren;
	//input file must exist
	inputFile = fopen(input_file_name,"rb");
	
	if (inputFile == NULL) {
		fprintf(stderr, "ERROR: can't open %s\n", input_file_name);
		return;
	}

	//first case is handled manually
	fscanf(inputFile,code_template,&offset,&size,&scape);

	first = new_node();
	first->character=scape;
	current_char=first;
	last=first;

	outputFile=fopen(output_file_name,"wb");


	while(fscanf(inputFile,code_template,&offset,&size,&scape)==3)
	{
		//if there is nothing to search, the value is added
		if (offset==0 && size==0){

			last->next = new_node();
			last->next->character =scape;
			last->next->previous=last;
			last= last->next;
			continue;
		}

		current_char=last;
		//add to the last pointer the requested elements
		backtrack(offset-1,size,current_char,last);
		
		//last value is updated, because new values has been added
		while(last->next!=NULL)
		{
			last=last->next;
		}
		//adding the char that was scaped
		last->next = new_node();
		last->next->character =scape;
		last->next->previous=last;
		last= last->next;
	}

	//print the file
	on_scrren=first;
	
	while(on_scrren!=NULL)
	{
		fprintf(outputFile,"%c",on_scrren->character);
		on_scrren=on_scrren->next;
	}

	fclose(inputFile);
	fclose(outputFile);
}


void percentageCompression (char * file1, char * file2)
{
	FILE* f1 = fopen(file1,"rb");
	if (file1 == NULL) {
		fprintf(stderr, "ERROR: can't open %s\n", file1);
		return;
	}

	FILE* f2 = fopen(file2,"rb");
	if (file2 == NULL) {
		fprintf(stderr, "ERROR: can't open %s\n", file2);
		return;
	}
	double l1=0,l2=0;

	while (getc(f2)!=-1)
	{
		l2++;
	}

	int input;

	while (fscanf(f1,"%i ",&input)==1)
	{
		l1++;
	}

	printf ("%s\n:%.0f \n%s\n:%.0f\n",file1,l1,file2,l2);
	double comp=100-((l1/l2)*100);
	printf ("\nlz77 %f",comp);
}


/*
 *
 * firt arg:
 * 		0: coding mode
 * 		1: decoding mode
 *
 * second arg: input file
 *
 * third arg:output file
 *
 *
 *
 * */
int lz77(int argc,char * args []) {
	char c =args [1][0];
	switch (c)
	{
	case '0':
		printf ("\nlz77> starting the compression of '%s'...",args[2]);
		encode(args[2],args[3]);

		printf ("\nlz77> compression finished. Code generated on: '%s'...",args[2]);

		break;

	case '1':
		printf ("\nlz77> starting the decompression of '%s'...",args[2]);
		decode(args[2],args[3]);
		printf ("\nlz77> Decompression finished. Text regenerated on: '%s'...",args[2]);
		break;

	case '2':
		percentageCompression(args[2],args[3]);

		break;
	default:
		printf ("\nlz77> error on decoding argument: use 0 for encode and 1 for decode.");
		return 0;
		break;
	}

	return EXIT_SUCCESS;
}
