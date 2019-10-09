/*
 * huffman.h
 *
 *  Created on: 02/03/2013
 *      Author: root
 */

#ifndef HUFFMAN_H_
#define HUFFMAN_H_
///////////////////////////////nodes struct/////////////////////////////////
typedef struct huffman_node node;
struct huffman_node {
	double symbol;
	double probability;
	//char *code;
	unsigned int code;
	int code_length;
	node * left;
	node * right;
	int leaf;
};

node ** get_huffman_nodes_and_save_it(const char * probability_file,
		const char * output_file_name, double * input_symbols,
		unsigned int input_symbols_length);
#endif /* HUFFMAN_H_ */
