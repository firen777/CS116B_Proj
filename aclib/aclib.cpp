#include "aclib.h"

float aclib::Q_rsqrt(float number){
    // //type punning w/ union
    // union {
    //     float f;
    //     long i;
    // } conv;
    
    // float x2;
    // const float threehalfs = 1.5F;

    // x2 = number * 0.5F;
    // conv.f  = number;
    // // conv.i  = 0x5f3759df - ( conv.i >> 1 );	// what the fuck? 
    // conv.i  = 0x5F375A86 - ( conv.i >> 1 ); //trying out new constant.
    // conv.f  = conv.f * ( threehalfs - ( x2 * conv.f * conv.f ) );
    // return conv.f;

    long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                          // evil floating point bit level hacking
	// i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
    i  = 0x5F375A86 - ( i >> 1 );                  //trying out new constant.
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
    //	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}