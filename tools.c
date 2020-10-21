#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "tools.h"


double tools_byte1_0_1(uint8_t x){

	return (double)x/(256-1);

}

double tools_byte2_0_1(uint16_t x){

	return (double)x/(256*256-1);

}
double tools_byte2_plus_minus_20(uint16_t x){

	return (double)x/((256*256-1))*40-20;

}

double tools_byte2_plus_minus_5(uint16_t x){

	return (double)x/((256*256-1))*10-5;

}

uint8_t tools_0_1_byte1(double x){

	return (uint8_t)((x)*256.0);

}
/*
double tools_sigmoid(double x){
	return x / (double)(1 + abs((int)x));
}
*/
float tools_sigmoid(float x)
{
     float exp_value;
     float return_value;

     /*** Exponential calculation ***/
     exp_value = exp((double) -x);

     /*** Final sigmoid value ***/
     return_value = 1 / (1 + exp_value);

     return return_value;
}
float tools_sigmoid_derivative(float x)
{
     float y = tools_sigmoid(x);
     return y * (1.0-y);

}