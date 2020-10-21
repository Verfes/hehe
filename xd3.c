#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "tools.h"
#include "bmp.h"
#include "xd3.h"
#include "network.h"


uint8_t *img;
uint8_t *lab;
uint32_t magic_number, nr_images, rows, cols;
FILE *file_train_images;
FILE *file_train_labels;
FILE *weights_biases;


uint32_t makeLowEndian(uint32_t x){

	uint32_t ret = 0;

	for (int i = 0; i < 4; i++){

		ret <<= 8;
		ret += x&255;
		x >>= 8;

	}

	return ret;

}


void xd3_load_images_into_RAM(){

	img = (uint8_t*)malloc(sizeof(uint8_t)*rows*cols*nr_images);
	if (img == NULL){
		printf("failure to allocate memory to store all images\n");
		exit(0);
	}

	fread(img, rows*cols, nr_images, file_train_images);
	lab = (uint8_t*)malloc(sizeof(uint8_t)*rows*cols*nr_images);
	if (lab == NULL){
		printf("fked up here\n");
		exit(0);
	}
	fread(lab, rows*cols, nr_images, file_train_labels);

}

int main(int argc, char *argv[]){

	file_train_images = fopen("Train_data/train-images.idx3-ubyte", "rb");  // r for read, b for binary
	file_train_labels = fopen("Train_data/train-labels.idx1-ubyte", "rb");  // r for read, b for binary
	
	if (file_train_images == NULL || file_train_labels == NULL){
		printf("error opening training files\n");
		exit(0);
	}	

	fread(&magic_number, sizeof(magic_number), 1, file_train_images);
	fread(&nr_images, sizeof(nr_images), 1, file_train_images);
	fread(&rows, sizeof(rows), 1, file_train_images);
	fread(&cols, sizeof(cols), 1, file_train_images);

	nr_images = makeLowEndian(nr_images);
	rows = makeLowEndian(rows);
	cols = makeLowEndian(cols);
	magic_number = makeLowEndian(magic_number);

	if (magic_number != 2051){
		printf("not good\n");
		exit(0);		
	}

	fread(&magic_number, sizeof(magic_number), 1, file_train_labels);
	magic_number = makeLowEndian(magic_number);
	if (magic_number != 2049){
		printf("not good\n");
		exit(0);	
	}
	fread(&nr_images, sizeof(nr_images), 1, file_train_labels);
	nr_images = makeLowEndian(nr_images);


	printf("nr of images: %d\n", nr_images);
	printf("rows: %d\ncols: %d\n", rows, cols);

	xd3_load_images_into_RAM();
	fclose(file_train_images);
	fclose(file_train_labels);
	network_initialize_network();

	for (int i = 0; i < 10; i++){
		printf("lab[%d] = %d\n", i, lab[i]);
	}

	double min = 5.0, max = -5.0;
	for (int i = 0; i < TOTAL_WEIGHTS; i++){
		//printf("%f\n", byte2_0_1(weight[i]));
		if (weight[i] > max) max = weight[i];
		if (weight[i] < min) min = weight[i];
	}

	printf("min = %f, max = %f\n", min, max);
	for (int i = 0; i < 1; i++){
		//printf("\n\n    IMAGE NUMBER    -    %d\n\n", i+1);
		network_compute_first_layer(i);
		network_compute_second_layer();
		network_compute_output_layer();
		printf("cost [%d] %f\r", i+1, network_cost(i));
		for (int j = 0; j < 10000; j++){
			network_compute_gradient(i);
			network_compute_first_layer(i);
			network_compute_second_layer();
			network_compute_output_layer();
		}



		//for (int j = 0; j < TOTAL_WEIGHTS+TOTAL_BIASES; j++)
			//printf("gradient[%d] = %f\n", j+1, gradient[j]);
		//printf("\b\b\b\b\b\b\b\b\b\b\b\b\b");
		//for (int i = 0; i < OUTPUT_LAYER_SIZE; i++)
			//printf("act[%d] = %d \n", i+1, activation[i + LAYER_1_SIZE + LAYER_2_SIZE]);
	}		
	//printf("cost [%d] %f\n", 1, network_cost(0));
	printf("cost [%d] %f\n", 1, network_cost(0));

	weights_biases = fopen("weights_biases/wb", "wb");

	if (weights_biases == NULL){
		printf("error opening weights biases file\n");
		exit(0);
	}
	fwrite(weight, sizeof(*weight), TOTAL_WEIGHTS, weights_biases);
	fwrite(bias, sizeof(*bias), TOTAL_BIASES, weights_biases);
	fclose(weights_biases);
	for (int i = 0; i < OUTPUT_LAYER_SIZE; i++){
		printf("act[%d] = %f\n", i+1, activation[LAYER_1_SIZE+LAYER_2_SIZE+i]);
	}

	//system("clear");
	return 0;
}


/*
COMMENTS: 

	for (int i = 0; i < nr_images; i++){
		fread(img, rows*cols, 1, file_train_images);
		bmp_create_bitmap(rows, cols, img, i+1);
	}
	
	for (int i = 0; i < rows; i++){
		for (int j = 0; j < cols; j++){

			if (img[i*cols+j] > 0)
				printf("11");
			else printf("00");

		}
		printf("\n");
	}
*/
