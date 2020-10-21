#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "tools.h"
#include "network.h"
#include "xd3.h"

double *weight;
double *output_layer; // feeling cute, might remove later idk
double *bias;
double *activation;
double *z;
double *gradient;

uint16_t index_activation(int layer, int a){

	switch(layer){ // FIRST LAYER
		case 0: return a; break;
		case 1: return LAYER_1_SIZE + a; break;
		case 2: return LAYER_1_SIZE + LAYER_2_SIZE + a; break;
		default: return 0;
	}


}

uint16_t index_weight(int layer, int a, int b){

	switch (layer){ // INPUT LAYER -> FIRST LAYER
		case 0: return a*LAYER_1_SIZE + b; break;
		case 1: return INPUT_LAYER_SIZE*LAYER_1_SIZE + a*LAYER_2_SIZE + b; break;
		case 2: return LAYER_1_SIZE*LAYER_2_SIZE + INPUT_LAYER_SIZE*LAYER_1_SIZE + a*OUTPUT_LAYER_SIZE + b; break;
		default: return 0;
	}
}

uint16_t index_bias(int layer, int a){

	switch(layer){ // FIRST LAYER
		case 0: return a; break;
		case 1: return LAYER_1_SIZE + a; break;
		case 2: return LAYER_1_SIZE + LAYER_2_SIZE + a; break;
		default: return 0;
	}

}

/*
void network_print_output_layer(){

	for (int i = 0; i < OUTPUT_LAYER_SIZE; i++){

		printf("%d -> %d\n", i, output_layer[i]);

	}

}*/

	

void network_initialize_network(){


	weight = (double*)malloc(sizeof(double)*TOTAL_WEIGHTS);
	bias = (double*)malloc(sizeof(double)*TOTAL_BIASES);
	activation = (double*)malloc(sizeof(double)*TOTAL_ACTIVATIONS);
	z = (double*)malloc(sizeof(double)*TOTAL_ACTIVATIONS);
	gradient = (double*)malloc(sizeof(double)*(TOTAL_WEIGHTS+TOTAL_BIASES));

	if (weight == NULL || bias == NULL || activation == NULL || z == NULL || gradient == NULL){
		printf("error allocating memory");
		exit(0);
	}

	if (GENERATE_RANDOM == 0){
		weights_biases = fopen("weights_biases/wb", "rb");
		if (weights_biases == NULL){
			printf("error opening weights biases file\n");
			exit(0);
		}
		fread(weight, TOTAL_WEIGHTS, sizeof(*weight), weights_biases);
		fread(bias, TOTAL_BIASES, sizeof(*bias), weights_biases);
		fclose(weights_biases);
	}else{
		srand( time(0) );
		int i;

		for (i = 0; i < TOTAL_WEIGHTS; i++){
			weight[i] = (double)rand()/RAND_MAX*10.0-5.0; //float in range -5 to 5;
		}

		for (i = 0; i < TOTAL_BIASES; i++){
			bias[i] = (double)rand()/RAND_MAX*40.0-20.0; // float in range -20 to 20;
		}

		weights_biases = fopen("weights_biases/wb", "wb");

		if (weights_biases == NULL){
			printf("error opening weights biases file\n");
			exit(0);
		}
		fwrite(weight, sizeof(*weight), TOTAL_WEIGHTS, weights_biases);
		fwrite(bias, sizeof(*bias), TOTAL_BIASES, weights_biases);
		fclose(weights_biases);
	}


}

/*

I first want to compute the gradient for the weights and biases in the 3rd last layer,
											   	  then the ones in the 2nd layer,
												  then the ones in the 1st layer.

*/
void network_compute_gradient(int image_index){

	double p1, p2, p3;
	double y[OUTPUT_LAYER_SIZE];
	for (int i = 0; i < OUTPUT_LAYER_SIZE; i++){
		if (i+1 == lab[image_index])
			y[i] = 1.0;
		else
			y[i] = 0.0;
	}

	for (int i = 0; i < OUTPUT_LAYER_SIZE; i++){
		for (int j = 0; j < LAYER_2_SIZE; j++){

			p3 = 2*(activation[LAYER_1_SIZE+LAYER_2_SIZE+i] - y[i]);
			p2 = tools_sigmoid_derivative(z[LAYER_1_SIZE+LAYER_2_SIZE+i]);
			p1 = activation[LAYER_1_SIZE+j];
			p2*= p2 * p3;
			gradient[TOTAL_WEIGHTS+index_bias(2, i)] = p2;
			gradient[index_weight(2, j, i)] = p1*p2;

		}
	}

	////////////////////////////////////////////////////////////////////////////////////////


	for (int i = 0; i < LAYER_2_SIZE; i++){
		for (int j = 0; j < LAYER_1_SIZE; j++){

			p3 = 2*(activation[LAYER_1_SIZE+LAYER_2_SIZE+i] - y[i]);
			p2 = tools_sigmoid_derivative(z[LAYER_1_SIZE+LAYER_2_SIZE+i]);
			p1 = activation[LAYER_1_SIZE+j];
			p2*= p2 * p3;
			gradient[TOTAL_WEIGHTS+index_bias(1, i)] = p2;
			gradient[index_weight(1, j, i)] = p1*p2;

		}
	}

	////////////////////////////////////////////////////////////////////////////////////////


	for (int i = 0; i < TOTAL_WEIGHTS; i++){

		weight[i] -= gradient[i];

	} 
	for (int i = 0; i < TOTAL_BIASES; i++){

		bias[i] -= gradient[i+TOTAL_WEIGHTS];

	}



}

double network_cost(int image_index){

	double cost = 0.0;

	for (int i = 0; i < OUTPUT_LAYER_SIZE; i++){

		cost += (activation[LAYER_1_SIZE+LAYER_2_SIZE+i]-(double)(lab[image_index]==i+1) )*
		(activation[LAYER_1_SIZE+LAYER_2_SIZE+i]-(double)(lab[image_index]==i+1) );

	}
	return cost;

}

void network_compute_first_layer(int image_index){

	double aux;

	for (int i = 0; i < LAYER_1_SIZE; i++){
		aux = 0.0;
		for (int j = 0; j < INPUT_LAYER_SIZE; j++){
			aux += (double)(img[j+image_index*INPUT_LAYER_SIZE]/255.0)*weight[index_weight(0, j, i)];
		}
		aux += bias[index_bias(0, i)];
		z[i] = aux;
		//printf("%f -> %f    ", aux, tools_sigmoid(aux));
		activation[i] = tools_sigmoid(aux);

		//activation[index_activation(0, i)] =  

	}
	//printf("\n");

}

void network_compute_second_layer(){

	double aux;

	for (int i = 0; i < LAYER_2_SIZE; i++){
		aux = 0.0;
		for (int j = 0; j < LAYER_1_SIZE; j++){
			aux += activation[j]*weight[index_weight(1, j, i)];
		}
		aux += bias[index_bias(1, i)];
		z[LAYER_1_SIZE + i] = aux;
		//printf("%f -> %f    ", aux, tools_sigmoid(aux));
		activation[LAYER_1_SIZE + i] = tools_sigmoid(aux);
		//activation[index_activation(0, i)] =  

	}
	//printf("\n");

}

void network_compute_output_layer(){

	double aux;

	for (int i = 0; i < OUTPUT_LAYER_SIZE; i++){
		aux = 0.0;
		for (int j = 0; j < LAYER_2_SIZE; j++){
			aux += activation[j+LAYER_1_SIZE]*weight[index_weight(2, j, i)];
		}
		aux += bias[index_bias(2, i)];
		z[LAYER_2_SIZE + LAYER_1_SIZE + i] = aux;
		//printf("%f -> %f    ", aux, tools_sigmoid(aux));
		activation[LAYER_2_SIZE + LAYER_1_SIZE + i] = tools_sigmoid(aux);
		//activation[index_activation(0, i)] =  

	}
	//printf("\n");

}
/*
void compute_output_layer(){	



}

void network_load_into_RAM(){



}
*/