//============================================================================
// Name        : CCD_JPG_Imp.cpp
// Author      : mdmh
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "stdlib.h"

#include "stdio.h"
#include <math.h>
#include <fftw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>
#include "huffman.h"
#include <vector>
using namespace std;

void decompress(const char *infilename, const char *outfilename);
void dct_forward(double * in, double * imOut, int width, int height);
void dct_backward(double* in, double *out, int width, int height);
void print_as_2d_array(double*array, int width, int height);
double * get_array8x8(double * image, int width, int height, int inX, int inY);
void write_array8x8(double * image, int width, int height, int inX, int inY,
		double *array8x8);
void divide_arrays(double * array_1, double * array_2, int image_width,
		int image_height, double *output_array);
void multiply_arrays(double * array_1, double * array_2, int image_width,
		int image_height, double *output_array);
jpeg_decompress_struct * crete_decompress(FILE* file);
void destroy(jpeg_decompress_struct * cinfo, FILE *file);
void destroy(jpeg_compress_struct * cinfo, FILE *file);
double* get_pixel_matrix(FILE *frile, jpeg_decompress_struct * cinfo);
jpeg_compress_struct * crete_compress(FILE *file, int width, int height,
		int n_components, J_COLOR_SPACE colorspace);
void sum_to_array(double * array_1, double val, int length,
		double *output_array);
void save_image(const char *nombre, double * image, unsigned int width,
		unsigned int height, int n_components, J_COLOR_SPACE color_space);
double equalness(double * array1, double * array2, double length);
void print_as_2d_array(int*array, int width, int height);
vector<int> get_lossy_compressed_array(double *image, int width, int height);
double *get_unlossy_compressed_array(int *array, int length);
double * create_probability_file(double * array, int width, int height,
		const char * output_file_name);
int squares_slice_length = 8;

double normalization_matrix[] = { 16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14,
		19, 26, 58, 60, 55, 14, 13, 16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51,
		87, 80, 62, 18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104,
		113, 92, 49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100,
		103, 99 };
const char * file_name ="mc.jpg";
const char * output1 ="output_original.jpg";
const char * output2 ="output_final.jpg";
const char * probability_file ="probability_file.txt";
const char * coded_file ="coded";
int main() {

	FILE * file = fopen(file_name, "rb");

	if (file == NULL) {
		printf("The file has not been found");
		return 0;
	}
	jpeg_decompress_struct * decompress_data = crete_decompress(file);
	int n_components = decompress_data->num_components;
	int width = decompress_data->image_width;
	int real_width = width * n_components;
	int height = decompress_data->image_height;
	double * slice;
	double * array = get_pixel_matrix(file, decompress_data);

	double * copy = (double *) (malloc(sizeof(double) * height * real_width));

	for (int position_y = 0; position_y < height; position_y +=
			squares_slice_length) {
		for (int position_x = 0; position_x < real_width; position_x +=
				squares_slice_length) {

			slice = get_array8x8(array, real_width, height, position_x,
					position_y);

			sum_to_array(slice, -128,
					squares_slice_length * squares_slice_length, slice);

			dct_forward(slice, slice, squares_slice_length,
					squares_slice_length);

			divide_arrays(slice, normalization_matrix, squares_slice_length,
					squares_slice_length, slice);
//
//			multiply_arrays(slice, normalization_matrix, squares_slice_length,
//					squares_slice_length, slice);
//
//			dct_backward(slice, slice, squares_slice_length,
//					squares_slice_length);
//
//		sum_to_array(slice, 128,
//					squares_slice_length * squares_slice_length, slice);
//
			write_array8x8(copy, real_width, height, position_x, position_y,
					slice);
		}
	}

	FILE *f = fopen(coded_file, "wb");
	fwrite(&width, sizeof(int), 1, f);
	fwrite(&height, sizeof(int), 1, f);
	fwrite(&n_components, sizeof(int), 1, f);
	fwrite(&decompress_data->out_color_space, sizeof(J_COLOR_SPACE), 1, f);
	fclose(f);

	create_probability_file(copy, real_width, height, probability_file);

	get_huffman_nodes_and_save_it(probability_file, coded_file, copy,
			real_width * height);

	//int window = 8;

	//print_as_2d_array(copy, window, window);
	//	vector<int> compressed = get_lossy_compressed_array(copy, real_width,height);
	//	printf("\n%i",real_width*height);
	//	int * compressed_array = &compressed[0];
	//	//print_as_2d_array(compressed_array, window*4, window*4);
	//	double *decompressed_array = get_unlossy_compressed_array(compressed_array,
	//			compressed.size());
	//
	//	//	printf("\nv1:%i\tv2:%i", real_width * height, decompressed.size());
	//
	//	print_as_2d_array(decompressed_array, width, height);

	//

	//printf("\nequalness:%f", equalness(copy, decompressed_array, window * window));
	//
	//	printf("\nequalness:%f",
	//			equalness(copy, decompressed_array, window * window));
	//
	//	print_as_2d_array(copy, window, window);
	//	print_as_2d_array(decompressed_array, window, window);
	//

	save_image(output1, copy, width, height, n_components,
			decompress_data->out_color_space);
	//
//	save_image(output2, copy, width, height, n_components,
//			decompress->out_color_space);
	destroy(decompress_data, file);
	printf("\nend");
//return 0;
	decompress(coded_file, output2);

	return 0;
}

vector<int> get_lossy_compressed_array(double *image, int width, int height) {

	int zerocount;
	vector<int> v;
	int significative_coeficients;
	int length = width * height;
	
	for (int i = 0; i < length; i += 8) {

		int max = i + 7;
		int min = i;

		//int in_reverse = i + 7;

		if (max > length) {
			max = length - 1;
		}

		//	printf ("\n*");
		significative_coeficients = 8;
		zerocount = 0;
		
		for (int j = max; j > i; j--) {
			if (image[j] <= 0) {
				zerocount++;
			} else {
				break;
			}
		}
		significative_coeficients -= zerocount;
		//printf("\nsc:%i\tzeros:%i\twrote %i/%i", significative_coeficients,
		//		zerocount, i, width * height);
		v.push_back(significative_coeficients);
		
		for (int j = min; j < min + significative_coeficients; j++) {
			//	printf ("\n*");
			v.push_back(image[j]);
		}
	}
	printf("\nend w");

	//	for (int i = 0; i < 8 * 8; i++) {
	//		printf("%i,", v[i]);
	//	}
	return v;
	//	for (int y = 0; y < height; y += squares_slice_length) {
	//
	//		for (int x = 0; x < width; x += squares_slice_length) {
	//			significative_coeficients = 8;
	//			zerocount = 0;
	//			 int position=(width * y) + x+8;
	//
	//
	//			 //slice = get_array8x8(image, width, height, x, y);
	//			for (int i = 8; i >= 0; i--) {
	//				if (slice[position-i] == 0) {
	//					zerocount++;
	//				} else {
	//					break;
	//				}
	//			}
	//			ww += 8;
	//			significative_coeficients -= zerocount;
	//			//	printf("\nsc:%i\tzeros:%i\twrote %i/%i",significative_coeficients,zerocount,ww,width*height);
	//			v.push_back(significative_coeficients);
	//			for (int i = 0; i < significative_coeficients; i++) {
	//				v.push_back(slice[i]);
	//			}
	//		}
	//}
	//printf("\nv: ");

}

double *get_unlossy_compressed_array(int *array, int length) {

	vector<double> v;
	int descriptor;
	int remaining;
	int i = 0;
	int j;

	while (i + 1 < length) {
		descriptor = array[i];
		//	printf("\ndescriptor:%i",descriptor);
		i++;
		
		for (j = 0; j < descriptor; j++) {
			v.push_back(array[i]);
			i++;
			//		printf("\n*");
		}
		remaining = 8 - descriptor;
		for (j = 0; j < remaining; j++) {
			v.push_back(0);
		}
	}
	printf("\nd%i", v.size());
	//print_as_2d_array(&v[0],100,100);

	return &v[0];

}

double equalness(double * array1, double * array2, double length) {
	double same_pixels = 0;
	double not_same_pixels = 0;
	
	for (unsigned int i = 0; i < length; i++) {
		if (fabs(array1[i] - array2[i]) < 0.0001) {
			//		printf ("\n(%f == %f) = %i",array1[i],array2[i],fabs(array1[i]-array2[i])<0.001);
			same_pixels++;
		} else {
			//

			//		printf ("\n%i:(%f == %f) = %i",i,array1[i],array2[i],fabs(array1[i]-array2[i])<0.001);

			not_same_pixels++;
		}
	}
	
	return same_pixels / length; // (same_pixels+0.0)/(length+0.0)
}

void save_image(const char *file_output_long_name, double * image,
		unsigned int width, unsigned int height, int n_components,
		J_COLOR_SPACE color_space) {

	FILE *file_output = fopen(file_output_long_name, "wb");

	if (!file_output) {
		printf("Error opening output jpeg file %s\n!", file_output_long_name);
		return;
	}

	JSAMPROW row_pointer[1];
	row_pointer[0] = (unsigned char *) (malloc(
			sizeof(unsigned char) * width * n_components));
	jpeg_compress_struct * compress = crete_compress(file_output, width, height,
			n_components, color_space);

	unsigned int l = width * height * n_components;

	unsigned char *raw_image = (unsigned char *) (malloc(
			sizeof(unsigned char) * l));

	for (unsigned int i = 0; i < l; i++) {
		raw_image[i] = image[i] + 1e-9;
		//	printf ("\nDEBUG (%i == %i == %f) = %i", int_array[i], raw_image[i], raw_image_num[i], fabs(int_array[i]-raw_image_num[i])<0.001);
	}

	while (compress->next_scanline < height) {
		row_pointer[0] = &raw_image[compress->next_scanline * width
				* n_components];
		jpeg_write_scanlines(compress, row_pointer, 1);
	}
	destroy(compress, file_output);

}
jpeg_compress_struct * crete_compress(FILE *file, int width, int height,
		int n_components, J_COLOR_SPACE colorspace) {
	//printf ("\nCreating Compress struct....");

	struct jpeg_compress_struct * cinfo = (jpeg_compress_struct*) (malloc(
			sizeof(jpeg_compress_struct)));
	struct jpeg_error_mgr jerr;
	cinfo->err = jpeg_std_error(&jerr);
	jpeg_create_compress( cinfo);
	jpeg_stdio_dest(cinfo, file);
	cinfo->image_width = width;
	cinfo->image_height = height;
	cinfo->input_components = n_components;
	cinfo->in_color_space = colorspace;
	jpeg_set_defaults(cinfo);
	cinfo->block_size = 1;
	jpeg_set_quality(cinfo, 100, FALSE);
	jpeg_set_linear_quality(cinfo, 100, FALSE);
	//jpeg_quality_scaling(100);
	jpeg_start_compress(cinfo, TRUE);
	//printf ("\nCompress struct created.");

	return cinfo;
}

jpeg_decompress_struct * crete_decompress(FILE* file) {
	struct jpeg_decompress_struct * cinfo = (jpeg_decompress_struct*) (malloc(
			sizeof(jpeg_decompress_struct)));
	struct jpeg_error_mgr jerr;
	cinfo->err = jpeg_std_error(&jerr);
	jpeg_create_decompress( cinfo);
	jpeg_stdio_src(cinfo, file);
	jpeg_read_header(cinfo, TRUE);
	jpeg_start_decompress(cinfo);
	return cinfo;
}

void destroy(jpeg_decompress_struct * cinfo, FILE *file) {
	jpeg_destroy_decompress(cinfo);
	fclose(file);
}

void destroy(jpeg_compress_struct * cinfo, FILE *file) {
	jpeg_finish_compress(cinfo);
	jpeg_destroy_compress(cinfo);
	fclose(file);
}

double* get_pixel_matrix(FILE *frile, jpeg_decompress_struct * cinfo) {

	unsigned int width = cinfo->image_width * cinfo->num_components;
	unsigned int height = cinfo->image_height;
	unsigned int length = height * width * cinfo->num_components;
	unsigned int position = 0;
	double * matrix = (double*) malloc(sizeof(double) * length);
	JSAMPROW row_pointer[1];
	row_pointer[0] = (unsigned char*) malloc(sizeof(unsigned char) * width);
	
	while (cinfo->output_scanline < height) {
		jpeg_read_scanlines(cinfo, row_pointer, 1);
		for (unsigned int i = 0; i < width; i++) {
			matrix[position] = row_pointer[0][i];
			position++;
			//		printf ("\n%i",position);
		}
	}
	
	return matrix;
}

void sum_to_array(double * array_1, double val, int length,
		double *output_array) {
	int current;
	for (current = 0; current < length; current++) {
		output_array[current] = array_1[current] + val;
	}
}

void multiply_arrays(double * array_1, double * array_2, int image_width,
		int image_height, double *output_array) {

	int length = image_width * image_height;
	int x;
	int y;
	int current;
	x = -1;
	y = 1;
	int UP = 0, DOWN = 1, STATE;
	STATE = UP;
	int position;

	for (current = 0; current < length; current++) {

		//DOWN
		if (STATE == DOWN) {
			//(D,L)
			if (y + 1 < image_height && x - 1 >= 0) {
				y++;
				x--;
			}
			//(X,X)
			else if (y + 1 >= image_height && x - 1 < 0) {
				x++;
				STATE = UP;
			}
			//(D,X)
			else if (y + 1 < image_height && x - 1 < 0) {
				y++;
				STATE = UP;
			}
			//(X,L)
			else if (y + 1 >= image_height && x - 1 >= 0) {
				x++;
				STATE = UP;
			}
		}
		//UP
		else {
			//(U,R)
			if (x + 1 < image_width && y - 1 >= 0) {
				x++;
				y--;
			}
			//(X,X)
			else if (x + 1 >= image_width && y - 1 < 0) {
				y++;
				STATE = DOWN;
			}
			//(X,R)
			else if (x + 1 < image_width && y - 1 < 0) {
				x++;
				STATE = DOWN;
			}
			//(U,X)
			else if (x + 1 >= image_width && y - 1 >= 0) {
				y++;
				STATE = DOWN;
			}
		}

		position = (image_width * y) + x;
		output_array[position] = array_1[position] * array_2[position];

	}

}
void divide_arrays(double * array_1, double * array_2, int image_width,
		int image_height, double *output_array) {

	int length = image_width * image_height;
	int x;
	int y;
	int current;
	x = -1;
	y = 1;
	int UP = 0, DOWN = 1, STATE;
	STATE = UP;
	int position;

	for (current = 0; current < length; current++) {

		//DOWN
		if (STATE == DOWN) {
			//(D,L)
			if (y + 1 < image_height && x - 1 >= 0) {
				y++;
				x--;
			}
			//(X,X)
			else if (y + 1 >= image_height && x - 1 < 0) {
				x++;
				STATE = UP;
			}
			//(D,X)
			else if (y + 1 < image_height && x - 1 < 0) {
				y++;
				STATE = UP;
			}
			//(X,L)
			else if (y + 1 >= image_height && x - 1 >= 0) {
				x++;
				STATE = UP;
			}
		}
		//UP
		else {
			//(U,R)
			if (x + 1 < image_width && y - 1 >= 0) {
				x++;
				y--;
			}
			//(X,X)
			else if (x + 1 >= image_width && y - 1 < 0) {
				y++;
				STATE = DOWN;
			}
			//(X,R)
			else if (x + 1 < image_width && y - 1 < 0) {
				x++;
				STATE = DOWN;
			}
			//(U,X)
			else if (x + 1 >= image_width && y - 1 >= 0) {
				y++;
				STATE = DOWN;
			}
		}

		position = (image_width * y) + x;
		output_array[position] = int(array_1[position] / array_2[position]);

	}

}

void print_as_2d_array(double*array, int width, int height) {
	printf("\n");
	for (int heightActual = 0; heightActual < height; heightActual++) {
		for (int widthActual = 0; widthActual < width; widthActual++) {
			int posicion = (width * heightActual) + widthActual;
			printf("%3.2f\t", array[posicion]);
		}
		printf("\n");
	}
}

void write_array8x8(double * image, int width, int height, int inX, int inY,
		double *array8x8) {

	int limitX = inX + squares_slice_length;
	int limitY = inY + squares_slice_length;

	if (limitX > width) {
		limitX = width;
	}

	if (limitY > height) {
		limitY = height;
	}
	int countx = 0;
	int county = 0;

	for (int heightActual = inY; heightActual < limitY; heightActual++) {
		countx = 0;
		for (int widthActual = inX; widthActual < limitX; widthActual++) {

			int image_position = (width * heightActual) + widthActual;
			int window_position = (squares_slice_length * county) + countx;

			image[image_position] = array8x8[window_position];
			countx++;

		}
		county += 1;
	}

}

double * get_array8x8(double * image, int width, int height, int inX, int inY) {

	int length = squares_slice_length * squares_slice_length;

	double *array8x8 = (double*) (malloc(sizeof(double) * length));

	for (int i = 0; i < length; i++) {
		array8x8[i] = 0;
	}

	int limitX = inX + squares_slice_length;
	int limitY = inY + squares_slice_length;

	if (limitX > width) {
		limitX = width;
	}

	if (limitY > height) {
		limitY = height;
	}

	int countx = 0;
	int county = 0;

	for (int heightActual = inY; heightActual < limitY; heightActual++) {
		countx = 0;
		for (int widthActual = inX; widthActual < limitX; widthActual++) {

			int image_position = (width * heightActual) + widthActual;
			int window_position = (squares_slice_length * county) + countx;
			//printf ("p:%i\n",window_position);
			array8x8[window_position] = image[image_position];
			countx++;
		}
		county++;
	}
	return array8x8;
}

void print_as_2d_array(int*array, int width, int height) {
	printf("\n");
	for (int heightActual = 0; heightActual < height; heightActual++) {
		for (int widthActual = 0; widthActual < width; widthActual++) {
			int posicion = (width * heightActual) + widthActual;
			printf("%i,", array[posicion]);
		}
		printf("\n");
	}
}
double dct_normalizationFactor(int current, int length) {
	if (current == 0) {
		return 1 / sqrt(length);
	} else {
		return sqrt(2.0 / length);
	}
}

void dct_normalization(double*array, int width, int height) {
	for (int heightActual = 0; heightActual < height; heightActual++) {
		for (int widthActual = 0; widthActual < width; widthActual++) {
			int posicion = (width * heightActual) + widthActual;
			array[posicion] *= dct_normalizationFactor(heightActual, height)
					* dct_normalizationFactor(widthActual, width);
			array[posicion] /= 4;
		}
	}
}

void dct_denormalization(double*array, int width, int height) {
	for (int heightActual = 0; heightActual < height; heightActual++) {
		for (int widthActual = 0; widthActual < width; widthActual++) {
			int posicion = (width * heightActual) + widthActual;
			array[posicion] /= dct_normalizationFactor(heightActual, height)
					* dct_normalizationFactor(widthActual, width);
			array[posicion] *= 4;
		}
	}
}

void dct_forward(double* in, double *out, int width, int height) {

	fftw_plan plan = fftw_plan_r2r_2d(width, height, in, out, FFTW_REDFT10,
			FFTW_REDFT10, FFTW_ESTIMATE);
	fftw_execute(plan);
	fftw_destroy_plan(plan);
	dct_normalization(out, width, height);
}

void dct_backward(double * in, double * out, int width, int height) {
	dct_denormalization(in, width, height);
	fftw_plan plan = fftw_plan_r2r_2d(width, height, in, out, FFTW_REDFT01,
			FFTW_REDFT01, FFTW_ESTIMATE);
	fftw_execute(plan);
	fftw_destroy_plan(plan);

	for (int heightActual = 0; heightActual < height; heightActual++) {
		for (int widthActual = 0; widthActual < width; widthActual++) {
			int posicion = (width * heightActual) + widthActual;
			out[posicion] /= (width * height * 4);
			out[posicion] =
					(out[posicion] == -0.0) ?
							abs(out[posicion]) : out[posicion];
		}
	}
}

int * new_int_array(int length) {
	return (int *) (malloc(length * sizeof(int)));
}

int compare(const void * a, const void * b) {
	int * ad = (int*) a;
	int * bd = (int*) b;
	return *ad > *bd;
}

double * create_probability_file(double * array, int width, int height,
		const char * output_file_name) {

	FILE * probability_file = fopen(output_file_name, "wb");

	if (probability_file == NULL) {
		printf("It was not possible create the probability file");
		return NULL;
	}
	unsigned int length = width * height;
	double * array_copy = (double *) (malloc(sizeof(double) * length));
	//	SNDFILE *input_file;
	//	SF_INFO info;
	unsigned int current_in_symbols = 0, i;
	//	input_file = sf_open(input_file_name, SFM_READ, &info);
	//
	//	unsigned int frames = info.frames;
	//	int channels = info.channels;
	//
	//	unsigned int num_items = frames * channels;
	//	int * samples = new_int_array(num_items);
	//	int * samples_copy = new_int_array(num_items);
	//	sf_read_int(input_file, samples, num_items);

	for (i = 0; i < length; i++) {
		array_copy[i] = array[i];
	}

	vector<double> simbols;
	vector<int> findings;

	simbols.push_back(0);
	findings.push_back(0);

	qsort(array_copy, length, sizeof(int), &compare);

	for (i = 0; i < length; i++) {
		//if (simbols[current_in_symbols] == array_copy[i]) {

		if (fabs(simbols[current_in_symbols] - array_copy[i]) < 0.0001) {
			findings[current_in_symbols]++;
		} else {
			simbols.push_back(array_copy[i]);
			findings.push_back(1);
			current_in_symbols++;
		}
	}
	double total_symbols = simbols.size() - 1;
	fprintf(probability_file, "%i", (int) round(total_symbols));

	for (i = 1; i < simbols.size(); i++) {
		fprintf(probability_file, "\n%f\t%f", simbols[i],
				findings[i] / total_symbols);
	}
	printf("\nProbabilities>%i", simbols.size());

	fclose(probability_file);

	return array_copy;
}

void decompress(const char *infilename, const char *outfilename) {
	printf("\n\ndecompess----------------------------------");
	unsigned int i = 0;
	int width;
	int height;
	int n_components;
	J_COLOR_SPACE out_color_space;
	unsigned int num_dct;

	double symbol;
	unsigned int code;
	unsigned int number_of_codes;

//	unsigned int number_of_samples;

	unsigned int current_int = 0;

	FILE * file_in = fopen(infilename, "rb");
	fread(&width, sizeof(int), 1, file_in);
	fread(&height, sizeof(int), 1, file_in);
	fread(&n_components, sizeof(int), 1, file_in);
	fread(&out_color_space, sizeof(J_COLOR_SPACE), 1, file_in);
	//fread(&num_dct, sizeof(unsigned int), 1, file_in);
	fread(&number_of_codes, sizeof(int), 1, file_in);
	printf("\nHuffman-------------------------------------");
	printf("\nwidth:%i", width);
	printf("\nheight:%i", height);
	printf("\nn_components:%i", n_components);
	printf("\nout_color_space:%i", out_color_space);
	printf("\nnumber of codes:%i", number_of_codes);

	double * symbols = (double*) (malloc(sizeof(double) * number_of_codes));
	unsigned int * codes = (unsigned int*) (malloc(
			sizeof(unsigned int) * number_of_codes));

	for (i = 0; i < number_of_codes; i++) {
		fread(&symbol, sizeof(double), 1, file_in);
		fread(&code, sizeof(int), 1, file_in);
//			printf("\n%i \t %i",symbol,code);
		symbols[i] = symbol;
		codes[i] = code;
	}

	fread(&num_dct, sizeof(int), 1, file_in);
	printf("\ndct:%i", num_dct);
	//scanf("%c");
	printf("\ndecoding-------------------------------------");

	double * samples = (double *) (malloc(sizeof(double) * num_dct));
	unsigned int count = 0;
	unsigned int var = 0;
	
	while (fread(&current_int, sizeof(int), 1, file_in) == 1) {
		for (i = 0; i < number_of_codes; i++) {
			if (codes[i] == current_int) {
				samples[count] = symbols[i];
				var++;
				count++;
				//	printf ("\n%i found",current_int);
			}
		}

		//		current_double|=(unsigned int)current_char;
		//		printf("\ncurrent_int:%i",current_int);
		//		for (i=0;i<number_of_codes;i++)
		//		{
		//			//printf ("\n%i with %i",current_int,codes[i]);
		//
		//			//if (i==2);
		//			//scanf("%c");
		//			if (codes[i]==current_int)
		//			{
		//				printf("\n!!!  %i \t %i",symbols[i],codes[i]);
		//				current_int=0;
		//				break;
		//			}
		//		}
		//		//	printf ("\nnot found %i",current_int);
		//		current_double<<=8;
	}

	printf("\ntotal:%i", var);

	int real_width = width * n_components;

	double * slice;
	
	for (int position_y = 0; position_y < height; position_y +=
			squares_slice_length) {
		for (int position_x = 0; position_x < real_width; position_x +=
				squares_slice_length) {

			slice = get_array8x8(samples, real_width, height, position_x,
					position_y);
			multiply_arrays(slice, normalization_matrix, squares_slice_length,
					squares_slice_length, slice);

			dct_backward(slice, slice, squares_slice_length,
					squares_slice_length);

			sum_to_array(slice, 128,
					squares_slice_length * squares_slice_length, slice);

			write_array8x8(samples, real_width, height, position_x, position_y,
					slice);
		}
	}

	save_image(output2, samples, width, height, n_components, out_color_space);

	//sf_write_int(output_file, samples, frames);

	//sf_close(output_file);

	fclose(file_in);
}
//	double symbol;
//	int code_legth;
//	char * code;
//
//	fwrite(&symbols[current]->symbol,sizeof(double),1,file);
//	fwrite(&symbols[current]->code_length,sizeof(int),1,file);
//	fwrite(symbols[current]->code,sizeof(char),symbols[current]->code_length,file);
//	fwrite(&symbols[current]->probability,sizeof(double),1,file);
//
//fclose(file_in);
