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

#include "math.h"


///////////////////////////////node struct/////////////////////////////////
typedef struct huffman_node node;
struct huffman_node
{
	char symbol [20];
	double probability;
	char *code;
	int code_length;
	node * left;
	node * right;
	int leaf;
};

double logarithm(double d,double b)
{
	double h = log2(d);// (d)/log(b);
	return h;
}

double getLn(node **symbols,int length)
{
	double Ln=0;
	int	current =0;
	node *s;
	for (current=0;current<length;current++)
	{
		s = symbols[current];
		Ln += s->code_length * s-> probability;
	}
	return Ln;
}

double getHs(node **symbols,int length)
{
	double Hs=0;
	int	current =0;
	node *s;
	for (current=0;current<length;current++)
	{
		s = symbols[current];
		Hs += s->probability * logarithm(1/s->probability,2);
	}
	return Hs;
}

/*
 * Print probabilities of an array of nodes
 * for debugging porpuses
 * */
void printProbabilities (node ** symbols,int length)
{
	int current;
	printf("\n[");
	for (current =0;current<length;current++)
	{
		printf ("\n%s\t\t\t%s\t%f\t%i\t",
				symbols[current]->symbol,
				symbols[current]->code,
				symbols[current]->probability,
				symbols[current]->code_length);

		//printf("%f,",symbols[current]->probability);
	}
	printf("]");
}
/*
 * this method creates the node structure,
 * */
void nodify(node ** symbols,int remainingToNodify)

{
	/*
	 * Since it's neccesary take 2 nodes for the binary tree, if just remains one node
	 * it's not possible to nodify it
	 * */
	int current;
	if (remainingToNodify<=1)
		return;
	/*
	 * This node will be the new one, it will point the 2 first nodes in the array
	 * */
	node * newNode = (node*) malloc (sizeof (node));
	newNode->right= symbols[0];
	newNode->left= symbols[1];
	newNode->leaf=0;
	strcpy( newNode->symbol,"Branch");
	newNode->probability = symbols[0]->probability+ symbols[1]->probability;
	//	printf ("\nDEBUG:New Node:P= %f,\tl=%p,\tr=%p",newNode->probability,newNode->left,newNode->right);

	/*
	 * More than two nodes: will be possible the left-shifting by 2 steps
	 * */
	if (remainingToNodify>2){
		for (current = 2; current<remainingToNodify;current++)
		{
			symbols[current-2] = symbols[current];
		}
		symbols[remainingToNodify-2]= newNode;
	}
	/*
	 * otherwise the newnode is the root node,
	 * arbitrary it's moved to the first position in the array
	 * */
	else
	{
		symbols[0]= newNode;
		strcpy( newNode->symbol,"Root");
	}

}

/*
 * Create the codes recursively
 * */
void codify(node * symbol,const char * load,const char * next,int count)
{
	char * code = (char*)(malloc(sizeof(char)*100));
	strcpy(code,load);
	strcat(code,next);
	symbol->code = code;
	symbol->code_length= count+1;
	if (symbol->leaf==0)
	{
		codify(symbol->left,code,"0",count+1);
		codify(symbol->right,code,"1",count+1);
	}
}


/////////////////////////////////PRINT THE TREE//////////////////

void printTree (node *symbol,int deep)
{
	int current =0;
	printf("\n");
	for (current =0;current <deep;current++)
	{
		printf("   ");
	}

	printf ("%s\t\t\t%s\t%f\t",symbol->symbol, symbol->code,symbol->probability);

	if (symbol->leaf!=1)
	{
		printTree(symbol->left,deep+1);
		printTree(symbol->right,deep+1);
	}
}
/////////////////////////////////PRINT THE SYMBOLS//////////////////
void printLeafs(node * symbol)
{
	if (symbol->leaf!=1)
	{
		printLeafs(symbol->left);
		printLeafs(symbol->right);
	}
	else{
		printf ("\n%s\t\t\t%s\t%f\t%i\t",symbol->symbol, symbol->code,symbol->probability,symbol->code_length);
	}
}

void huffman_algorithm(node **symbols,int length) {

	int current;
	for (current =length;current>=0;current--)
	{
		nodify(symbols,current);
	}

	//printf ("\nDEBUG> BEFORE CODING TREE");
	codify(symbols[0],"","",-1);
	//printf ("\nDEBUG> AFTER CODING TREE");

}

void putLeafOnArrayAgain (node * symbol,node **symbols,int &count)
{
	if (symbol->leaf!=1)
	{
		putLeafOnArrayAgain(symbol->left,symbols,count);
		putLeafOnArrayAgain(symbol->right,symbols,count);
	}else
	{
		symbols[count] =symbol;
		count++;
	}
}


int main(int argc,char * args []) {

	///////////////////////////////VAR DECLARATION/////////////////////////////////

	int numberOfSymbols=0, current, j;
	float readProbability;
	char ch[10];
	node temporal;
	FILE * file;

	///////////////////////////////OPENING FILE////////////////////////////////////

	file = fopen(args[1],"rb");
	if (file == NULL) {
		fprintf(stderr, "ERROR: can't open %s\n", args[1]);
		return 0;
	}

	///////////////////////////////LOADING SYMBOLS/////////////////////////////////

	printf ("leyendo :%s\n",args[1]);
	printf ("\n--------------------------------------------------------------------------------\n");

	fscanf(file,"%i",&numberOfSymbols);
	printf ("simbolos:%i",numberOfSymbols);

	node* symbols [numberOfSymbols];

	for (current=0;current<numberOfSymbols;current++)
	{
		fscanf(file,"%s %f",ch,&readProbability);
		printf("\n%s %f",ch,readProbability);

		///////////////////////////////KIND OF CONTRUCTOR//////////////////////////

		symbols[current] = (node*)(malloc(sizeof(node)));
		strcpy(symbols[current]->symbol, ch);
		symbols[current]->probability = readProbability;
		symbols[current]->leaf=1;
		symbols[current]->code_length=0;
		///////////////////////////////////////////////////////////////////////////
	}

	///////////////////////////////ENOUGH WITH THE FILE////////////////////////////
	fclose(file);
	/////////////////////////////// SORTING BY PROBABILITIES///////////////////////

	int next =0;

	/////////////////////////////////JUST SORTING//////////////////

	for (j = 1; j <= numberOfSymbols - 1; j++) {
		for (current = 0; current < numberOfSymbols - 1; current++) {
			next=current+1;
			if ((symbols[current]->probability) > (symbols[next]->probability)) {
				temporal.probability = symbols[current]->probability;
				strcpy(temporal.symbol, symbols[current]->symbol);
				symbols[current]->probability = symbols[next]->probability;
				strcpy(symbols[current]->symbol, symbols[next]->symbol);
				symbols[next]->probability = temporal.probability;
				strcpy(symbols[next]->symbol, temporal.symbol);
			}
		}
	}


	/////////////////////////////////NOW THE CALL TO HUFFMAN ALGORITHM//////////////////

	//printf ("\nDEBUG> BEFORE HUFFMAN ALGORITHM");
	huffman_algorithm(symbols, numberOfSymbols);
	//printf ("\nDEBUG> AFTHER HUFFMAN ALGORITHM");

	/////////////////////////////////DONE! SO LET'S PRINT IT////////////////////////////


	//printf ("\nDEBUG> BEFORE PRINTNG TREE WITH CODES");
	printf ("\n--------------------------------------------------------------------------------");
	printf("\nCreated Tree\n");
		printTree(symbols[0],0);
	//printf ("\nDEBUG> AFTHER PRINTNG TREE WITH CODES");


	//printf ("\nDEBUG> BEFORE PRINTNG LEAFS WITH CODES");

	printf ("\n--------------------------------------------------------------------------------");
	printf("\nCode of symbols\n");
	printLeafs(symbols[0]);
	//printf ("\nDEBUG> AFTHER PRINTNG LEAFS WITH CODES");
	int v =0;
	printf ("\n--------------------------------------------------------------------------------");


	putLeafOnArrayAgain (symbols[0],symbols, v);
//	printProbabilities(symbols,numberOfSymbols);
	printf ("\n--------------------------------------------------------------------------------");

	/////////////////////////////////Time to calculate////////////////////////////
	double Hs = getHs(symbols,numberOfSymbols);
	double Ln = getLn (symbols,numberOfSymbols);
	double ef = Hs/Ln;

	printf ("\nHs=%f\tLn=%f\teficciency:%f",Hs,Ln,ef);

	/////////////////////////////////THAT'S ALL FOLKS////////////////////////////


	return 0;

}

















