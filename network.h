#define INPUT_LAYER_SIZE 784
#define LAYER_1_SIZE 16
#define LAYER_2_SIZE 16
#define OUTPUT_LAYER_SIZE 10
#define TOTAL_WEIGHTS INPUT_LAYER_SIZE*LAYER_1_SIZE + LAYER_1_SIZE*LAYER_2_SIZE + LAYER_2_SIZE*OUTPUT_LAYER_SIZE
#define TOTAL_BIASES LAYER_1_SIZE + LAYER_2_SIZE + OUTPUT_LAYER_SIZE
#define TOTAL_ACTIVATIONS LAYER_1_SIZE + LAYER_2_SIZE + OUTPUT_LAYER_SIZE
#define GENERATE_RANDOM 0
extern uint16_t index_weight(int layer, int a, int b);
extern uint16_t index_bias(int layer, int a);
extern void network_print_output_layer();
extern void network_initialize_network();
extern void network_compute_first_layer(int x);
extern void network_compute_second_layer();
extern void network_compute_output_layer();
extern double network_cost(int image_index);
extern void network_compute_gradient(int image_index);



extern double *weight;
extern double *output_layer; // feeling cute, might remove later idk
extern double *bias;
extern double *activation;
extern double *z;
extern double *gradient;