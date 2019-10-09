/*
 ============================================================================
 Name        : Huffman.c
 Author      : mdmh
 Version     : 1
 Copyright   :
 Description : reads a file and caculate huffman codes.
 ============================================================================
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "huffman.h"

#include "math.h"

void sorting_array(node **symbols, unsigned int numberOfSymbols) {
	//	printf("\nsorting:%i", numberOfSymbols);
	unsigned int current;
	int previous = 0;
	node temporal;

	for (current = numberOfSymbols - 1; current > 0; current--) {
		previous = current - 1;
		
		if (symbols[current]->probability < symbols[previous]->probability) {
			temporal.probability = symbols[current]->probability;
			temporal.symbol = symbols[current]->symbol;
			symbols[current]->probability = symbols[previous]->probability;
			symbols[current]->symbol = symbols[previous]->symbol;
			symbols[previous]->probability = temporal.probability;
			symbols[previous]->symbol = temporal.symbol;
		} else {
			break;
		}
	}
	//printf("sorting done");
}

int compare_huffman_node(const void * a, const void * b) {
	node * ad = (node*) a;
	node * bd = (node*) b;
	
	return ad->probability > bd->probability;
}

double logarithm(double d, double b) {
	double h = log2(d); // (d)/log(b);
	
	return h;
}

double getLn(node **symbols, int length) {
	double Ln = 0;
	int current = 0;
	node *s;
	
	for (current = 0; current < length; current++) {
		s = symbols[current];
		Ln += s->code_length * s->probability;
	}
	
	return Ln;
}

double getHs(node **symbols, int length) {
	double Hs = 0;
	int current = 0;
	node *s;
	
	for (current = 0; current < length; current++) {
		s = symbols[current];
		Hs += s->probability * logarithm(1 / s->probability, 2);
	}
	
	return Hs;
}

/*
 * Print probabilities of an array of nodes
 * for debugging porpuses
 * */
void printProbabilities(node ** symbols, int length) {
	int current;
	printf("\n[");
	
	for (current = 0; current < length; current++) {
		printf("\n%i\t\t\t%i\t%f\t%i\t", symbols[current]->symbol,
				symbols[current]->code, symbols[current]->probability,
				symbols[current]->code_length);

		//printf("%f,",symbols[current]->probability);
	}
	
	printf("]");
}

void save_huffman_codes(node ** symbols, unsigned int length, FILE * file) {
	unsigned int current;

	fwrite(&length, sizeof(int), 1, file);

	for (current = 0; current < length; current++) {

		fwrite(&symbols[current]->symbol, sizeof(int), 1, file);
		//fwrite(&symbols[current]->code_length, sizeof(int), 1, file);
		fwrite(&symbols[current]->code, sizeof(int), 1, file);
	}

}
/*
 * this method creates the node structure,
 * */
void nodify(node ** symbols, int remainingToNodify)

{
	/*
	 * Since it's neccesary take 2 nodes for the binary tree, if just remains one node
	 * it's not possible to nodify it
	 * */

	//printf("%ui\n",x);
	int current;
	if (remainingToNodify <= 1)
		return;
	/*
	 * This node will be the new one, it will point the 2 first nodes in the array
	 * */
	node * newNode = (node*) malloc(sizeof(node));
	newNode->right = symbols[0];
	newNode->left = symbols[1];
	newNode->leaf = 0;
	newNode->symbol = 0;
	newNode->probability = symbols[0]->probability + symbols[1]->probability;
	//	printf ("\nDEBUG:New Node:P= %f,\tl=%p,\tr=%p",newNode->probability,newNode->left,newNode->right);

	/*
	 * More than two nodes: will be possible the left-shifting by 2 steps
	 * */
	if (remainingToNodify > 2) {
		for (current = 2; current < remainingToNodify; current++) {
			symbols[current - 2] = symbols[current];
		}
		symbols[remainingToNodify - 2] = newNode;
	}
	/*
	 * otherwise the newnode is the root node,
	 * arbitrary it's moved to the first position in the array
	 * */
	else {
		symbols[0] = newNode;
		newNode->symbol = 0;
	}

}

//Create the codes recursively
void codify(node * symbol, unsigned int load, unsigned int next, int count) {
	symbol->code_length = count + 1;
	load <<= 1;
	load |= next;
	symbol->code = load;
	
	//printf("%i\t%i\n", next, symbol->code_length);
	if (symbol->leaf == 0) {
		codify(symbol->left, symbol->code, 0, count + 1);
		codify(symbol->right, symbol->code, 1, count + 1);
	}
}

/////////////////////////////////PRINT THE TREE//////////////////
void printTree(FILE *f,node *symbol, int deep) {


	int current = 0;
	fprintf(f,"\n");
	
	for (current = 0; current < deep; current++) {
		fprintf(f,"   ");
	}

	fprintf(f,"%i\t\t\t%i\t%f\t", symbol->symbol, symbol->code,
			symbol->probability);

	if (symbol->leaf != 1) {
		printTree(f,symbol->left, deep + 1);
		printTree(f,symbol->right, deep + 1);
	}
}

/////////////////////////////////PRINT THE SYMBOLS//////////////////
void printLeafs(node * symbol) {
	
	if (symbol->leaf != 1) {
		printLeafs(symbol->left);
		printLeafs(symbol->right);
	} else {
		printf("\n%i\t\t\t%i\t%f\t%i\t", symbol->symbol, symbol->code,
				symbol->probability, symbol->code_length);
	}
}

void huffman_algorithm(node **symbols, int length) {

	int current;
	
	for (current = length; current >= 0; current--) {
		nodify(symbols, current);
	}

	//printf ("\nDEBUG> BEFORE CODING TREE");
	codify(symbols[0], 1, 0, -1);
	//printf ("\nDEBUG> AFTER CODING TREE");

}

void putLeafOnArrayAgain(node * symbol, node **symbols, int &count) {
	
	if (symbol->leaf != 1) {
		putLeafOnArrayAgain(symbol->left, symbols, count);
		putLeafOnArrayAgain(symbol->right, symbols, count);
	} else {
		symbols[count] = symbol;
		count++;
	}
}

void paddify(node **symbols, int length) {

	//	for (int i = 0; i < length; i++) {
	//		int must_be_padded = symbols[i]->code_length % 8;
	//		if (must_be_padded > 0) {
	//			must_be_padded = 8 - must_be_padded;
	//			for (int j = 0; j < must_be_padded; j++) {
	//				strcat(symbols[i]->code, 0);
	//			}
	//			symbols[i]->code_length += must_be_padded;
	//		}
	//	}
}

void charfy(node **symbols, int length) {

	//	int charset_length;
	//	char * charset;
	//	long shifter;
	//	for (int i = 0; i < length; i++) {
	//
	//		charset_length = symbols[i]->code_length / 8;
	//		charset = (char*) (malloc(charset_length));
	//		//lets be tricky! >:D
	//		for (int j = 0; j < charset_length; j++) {
	//
	//			shifter = 1;
	//			for (int k = 0; k < 8; k++) {
	//				int position = (j * 8) + k;
	//				if (symbols[i]->code[position] == '1') {
	//					charset[j] |= shifter;
	//					shifter <<= 1;
	//				}
	//				shifter <<= 1;
	//			}
	//		}
	//		symbols[i]->code = charset;
	//		symbols[i]->code_length = charset_length;
	//}
}

int validate_codes(node ** codes, unsigned int codes_length) {
	for (unsigned int i = 0; i < codes_length; i++) {
		for (unsigned int j = i; j < codes_length; j++) {
			if (codes[i]->code == codes[j]->code && &codes[i] != &codes[j]) {
				return -1;
			}
		}
	}
	return 1;
}

void save_codified(node ** codes, unsigned int codes_length,
		int * input_symbols, unsigned int input_symbols_length, FILE * file) {

	fwrite(&input_symbols_length, sizeof(int), 1, file);

	printf("\nhuffman> :staritng write codes...%i", input_symbols_length);
	unsigned int wrote = 0;

	//	char * octets = (char *) (malloc(4));

	for (unsigned int i = 0; i < input_symbols_length; i++) {

		//	printf ("%i\n",input_symbols[i]);
		for (unsigned int j = 0; j < codes_length; j++) {
			//
			if (input_symbols[i] == codes[j]->symbol) {
				fwrite(&codes[j]->code, sizeof(int), 1, file);
				wrote++;
				break;
				//				unsigned int x = codes[j]->code;
				//
				//				octets[0] = x;
				//				x >>= 8;
				//				octets[1] = x;
				//				x >>= 8;
				//				octets[2] = x;
				//				x >>= 8;
				//				octets[3] = x;
				//
				//				//	printf ("%i=%i,%i,%i,%i\n",codes[j]->code,octets[3],octets[2],octets[1],octets[0]);
				//
				//				for (int k = 0; k < 4; k++) {
				//					if (octets[k] != 0) {
				//						fwrite(&octets[k], sizeof(char), 1, file);
			}
		}
	}

	printf("{\nhuffman> %i codes were wrote.", wrote);
}

node ** get_huffman_nodes_and_save_it(const char * probability_file,
		const char * output_file_name, int * input_symbols,
		unsigned int input_symbols_length) {

	///////////////////////////////VAR DECLARATION/////////////////////////////////

	int numberOfSymbols = 0,

			current; //, j;
	float readProbability = 0.0;
	//node temporal;
	FILE * file;

	///////////////////////////////OPENING FILE////////////////////////////////////

	file = fopen(probability_file, "rb");
	
	if (file == NULL) {
		fprintf(stderr, "ERROR: can't open %s\n", probability_file);
		return NULL;
	}

	///////////////////////////////LOADING SYMBOLS/////////////////////////////////

	printf("\nhuffman> reading: %s", probability_file);
	//printf("\n--------------------------------------------------------------------------------\n");

	fscanf(file, "%i", &numberOfSymbols);
	printf("\nhuffman> symbols:%i", numberOfSymbols);
	//scanf("%c");
	node* symbols[numberOfSymbols];

	float d = 0;
	
	for (current = 0; current < numberOfSymbols; current++) {
		fscanf(file, "%f %f", &d, &readProbability);
		//	printf("\n%i %f %f",current,d,readProbability);

		///////////////////////////////KIND OF CONTRUCTOR//////////////////////////

		symbols[current] = (node*) (malloc(sizeof(node)));
		//strcpy(symbols[current]->symbol, ch);
		symbols[current]->symbol = d;
		symbols[current]->probability = readProbability;
		symbols[current]->leaf = 1;
		symbols[current]->code = 0;
		symbols[current]->code_length = 0;
		
		if (current > 0)
			sorting_array(symbols, current);
		///////////////////////////////////////////////////////////////////////////
	}
	sorting_array(symbols, current);
	//	for (current = 0; current < numberOfSymbols; current++) {
	//		printf("%f\n", symbols[current]->probability);
	//
	//	}

	//printf("\n Printing done.");
	///////////////////////////////ENOUGH WITH THE FILE////////////////////////////
	fclose(file);
	//printf("\n file closed.");
	/////////////////////////////// SORTING BY PROBABILITIES///////////////////////

	/////////////////////////////////NOW THE CALL TO HUFFMAN ALGORITHM//////////////////
	//	return;
	//printf ("\nDEBUG> BEFORE HUFFMAN ALGORITHM");
	printf("\nhuffman> starting algorithm...");
	huffman_algorithm(symbols, numberOfSymbols);
	//printf ("\nDEBUG> AFTHER HUFFMAN ALGORITHM");
	printf("\nhuffman> algorithm finished.");
	/////////////////////////////////DONE! SO LET'S PRINT IT////////////////////////////

	//printf ("\nDEBUG> BEFORE PRINTNG TREE WITH CODES");
	//printf("\n--------------------------------------------------------------------------------");
	//printf("\nCreated Tree\n");
	//printTree(symbols[0], 0);
	//printf ("\nDEBUG> AFTHER PRINTNG TREE WITH CODES");

	//printf ("\nDEBUG> BEFORE PRINTNG LEAFS WITH CODES");

	//printf("\n--------------------------------------------------------------------------------");
	//printf("\nCode of symbols\n");
	//printLeafs(symbols[0]);
	FILE * outp;

	outp = fopen(output_file_name, "a");

	//	outp =fopen("/home/maestria/Dropbox/CINVESTAV/Tesis/Foxes/WhiteFox/CCD WAV Test 2/huffman_code.bin","wb");

	//printf ("\nDEBUG> AFTHER PRINTNG LEAFS WITH CODES");
	int v = 0;
	//printf(
	//		"\n--------------------------------------------------------------------------------");
	//saveCode(symbols[0],outp);
	putLeafOnArrayAgain(symbols[0], symbols, v);

	//paddify(symbols, numberOfSymbols);
	//charfy(symbols, numberOfSymbols);

	//printProbabilities(symbols, numberOfSymbols);



//	const char * file_name= "huffman_tree";
//
//	FILE * f = fopen(file_name,"w");
//	printTree(f,symbols[0],0);
//
//	fclose(f);

	printf("\nhuffman> saving generated codes...");
	save_huffman_codes(symbols, numberOfSymbols, outp);
	printf("\nhuffman> codes saved.");

	printf("\nhuffman>code validation>start...");
	
	if (validate_codes(symbols, numberOfSymbols)) {
		printf("\nhuffman>code validation> Codes were validated =)");
	}

	else {
		printf("\nhuffman>code validation> Bad codes x.x");
	}
	printf("\nhuffman>code validation>finish.");
	//printf(
	//		"\n--------------------------------------------------------------------------------");
	save_codified(symbols, numberOfSymbols, input_symbols, input_symbols_length,
			outp);


	/////////////////////////////////Time to calculate////////////////////////////
	//	double Hs = getHs(symbols, numberOfSymbols);
	//	double Ln = getLn(symbols, numberOfSymbols);
	//	double ef = Hs / Ln;

	//printf("\nHs=%f\tLn=%f\teficciency:%f", Hs, Ln, ef);

	/////////////////////////////////THAT'S ALL FOLKS////////////////////////////

	fclose(outp);

	return symbols;

}

