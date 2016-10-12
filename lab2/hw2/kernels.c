/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
    "0xdeadbeef",              /* Team name */

    "Shafaaf Khaled Hossain",     /* First member full name */
    "shafaaf.hossain@mail.utoronto.ca",  /* First member email address */

    "Sai Kiran Varikooty",        /* Second member full name (leave blank if none) */
    "s.varikooty@mail.utoronto.ca" /* Second member email addr (leave blank if none) */
};

//---------------------------------------------------------------------------------------------------------------------------------------------

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}

/*
 * ECE 454 Students: Write your rotate functions here:
 */ 

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst) 
{
    //Old code
    //naive_rotate(dim, src, dst);
    
    // copied Combine 2: Unroll3 and tiling 2 with loop invariant + more unrolling inside: 3s on avg
     int i, j;
     
     //loop unroll 3
     if(dim<=1024)
     {
     	int loopInvariant = dim - 1;
     	int loopInvariant2;
    	for (i = 0; i < dim; i+=16)
    	{
		for (j = 0; j < dim; j++)
		{
	    		loopInvariant2 = loopInvariant -j;
	    		dst[RIDX(loopInvariant2, i, dim)] = src[RIDX(i, j, dim)];
	    		dst[RIDX(loopInvariant2, (i+1), dim)] = src[RIDX((i+1), j, dim)];
			dst[RIDX(loopInvariant2, (i+2), dim)] = src[RIDX((i+2), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+3), dim)] = src[RIDX((i+3), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+4), dim)] = src[RIDX((i+4), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+5), dim)] = src[RIDX((i+5), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+6), dim)] = src[RIDX((i+6), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+7), dim)] = src[RIDX((i+7), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+8), dim)] = src[RIDX((i+8), j, dim)];
	 		dst[RIDX(loopInvariant2, (i+9), dim)] = src[RIDX((i+9), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+10), dim)] = src[RIDX((i+10), j, dim)];
	   	        dst[RIDX(loopInvariant2, (i+11), dim)] = src[RIDX((i+11), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+12), dim)] = src[RIDX((i+12), j, dim)];
	    	        dst[RIDX(loopInvariant2, (i+13), dim)] = src[RIDX((i+13), j, dim)];
	    	        dst[RIDX(loopInvariant2, (i+14), dim)] = src[RIDX((i+14), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+15), dim)] = src[RIDX((i+15), j, dim)];
      		 }

    	}
    }

    //tiling 2
    else
    {	
    	int blockSize = 16;
	int x,y;
     	int loopInvariant = dim - 1;
     	int loopInvariant2;

	for (i = 0; i < dim; i+=blockSize)
   	 {
		for (j = 0; j < dim; j+=blockSize)
		{
	    		for(y = j; y < j + blockSize; y++)
	    		
			{
				loopInvariant2 = loopInvariant - y;
				for(x = i; x < i + blockSize; x+=8)
				{
				dst[RIDX(loopInvariant2, x, dim)] = src[RIDX(x, y, dim)];
				dst[RIDX(loopInvariant2, x+1, dim)] = src[RIDX(x+1, y, dim)];
				dst[RIDX(loopInvariant2, x+2, dim)] = src[RIDX(x+2, y, dim)];
				dst[RIDX(loopInvariant2, x+3, dim)] = src[RIDX(x+3, y, dim)];
				dst[RIDX(loopInvariant2, x+4, dim)] = src[RIDX(x+4, y, dim)];
				dst[RIDX(loopInvariant2, x+5, dim)] = src[RIDX(x+5, y, dim)];
				dst[RIDX(loopInvariant2, x+6, dim)] = src[RIDX(x+6, y, dim)];
				dst[RIDX(loopInvariant2, x+7, dim)] = src[RIDX(x+7, y, dim)];
				}
	    		}	
       		}

    	}
    }

}

//---------------------------------------------------------------------------------------------------------------------------------------------
 
// 1st loop unroll attempt 
char rotate_unroll_1_descr[] = "Unrolling 1 using inner loop j. Doesnt improve";
void attempt_unroll_1(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    for (i = 0; i < dim; i++)
    {	
	for (j = 0; j < dim; j+=32)
	{
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
	    dst[RIDX(dim-1-(j+1), i, dim)] = src[RIDX(i, j+1, dim)];
	    dst[RIDX(dim-1-(j+2), i, dim)] = src[RIDX(i, j+2, dim)];
	    dst[RIDX(dim-1-(j+3), i, dim)] = src[RIDX(i, j+3, dim)];
	    dst[RIDX(dim-1-(j+4), i, dim)] = src[RIDX(i, j+4, dim)];
	    dst[RIDX(dim-1-(j+5), i, dim)] = src[RIDX(i, j+5, dim)];
	    dst[RIDX(dim-1-(j+6), i, dim)] = src[RIDX(i, j+6, dim)];
	    dst[RIDX(dim-1-(j+7), i, dim)] = src[RIDX(i, j+7, dim)];
	    dst[RIDX(dim-1-(j+8), i, dim)] = src[RIDX(i, j+8, dim)];
	    dst[RIDX(dim-1-(j+9), i, dim)] = src[RIDX(i, j+9, dim)];
	    dst[RIDX(dim-1-(j+10), i, dim)] = src[RIDX(i, j+10, dim)];
	    dst[RIDX(dim-1-(j+11), i, dim)] = src[RIDX(i, j+11, dim)];
	    dst[RIDX(dim-1-(j+12), i, dim)] = src[RIDX(i, j+12, dim)];
	    dst[RIDX(dim-1-(j+13), i, dim)] = src[RIDX(i, j+13, dim)];
	    dst[RIDX(dim-1-(j+14), i, dim)] = src[RIDX(i, j+14, dim)];
	    dst[RIDX(dim-1-(j+15), i, dim)] = src[RIDX(i, j+15, dim)];
	    dst[RIDX(dim-1-(j+16), i, dim)] = src[RIDX(i, j+16, dim)];
	    dst[RIDX(dim-1-(j+17), i, dim)] = src[RIDX(i, j+17, dim)];
	    dst[RIDX(dim-1-(j+18), i, dim)] = src[RIDX(i, j+18, dim)];
	    dst[RIDX(dim-1-(j+19), i, dim)] = src[RIDX(i, j+19, dim)];
	    dst[RIDX(dim-1-(j+20), i, dim)] = src[RIDX(i, j+20, dim)];
	    dst[RIDX(dim-1-(j+21), i, dim)] = src[RIDX(i, j+21, dim)];
	    dst[RIDX(dim-1-(j+22), i, dim)] = src[RIDX(i, j+22, dim)];
	    dst[RIDX(dim-1-(j+23), i, dim)] = src[RIDX(i, j+23, dim)];
	    dst[RIDX(dim-1-(j+24), i, dim)] = src[RIDX(i, j+24, dim)];
	    dst[RIDX(dim-1-(j+25), i, dim)] = src[RIDX(i, j+25, dim)];
	    dst[RIDX(dim-1-(j+26), i, dim)] = src[RIDX(i, j+26, dim)];
	    dst[RIDX(dim-1-(j+27), i, dim)] = src[RIDX(i, j+27, dim)];
	    dst[RIDX(dim-1-(j+28), i, dim)] = src[RIDX(i, j+28, dim)];
	    dst[RIDX(dim-1-(j+29), i, dim)] = src[RIDX(i, j+29, dim)];
	    dst[RIDX(dim-1-(j+30), i, dim)] = src[RIDX(i, j+30, dim)];
	    dst[RIDX(dim-1-(j+31), i, dim)] = src[RIDX(i, j+31, dim)];
	}
	
    }

}

//------------------------------------------------------------------------

// 2nd loop unroll attempt
char rotate_unroll_2_descr[] = "Unrolling 2 using outer loop i and striding by 32. Speedup upto 2.9";
void attempt_unroll_2(int dim, pixel *src, pixel *dst) 
{
     int i, j;

    for (i = 0; i < dim; i+=32)
    {
	for (j = 0; j < dim; j++)
	{
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
	    dst[RIDX(dim-1-j, (i+1), dim)] = src[RIDX((i+1), j, dim)];
	    dst[RIDX(dim-1-j, (i+2), dim)] = src[RIDX((i+2), j, dim)];
	    dst[RIDX(dim-1-j, (i+3), dim)] = src[RIDX((i+3), j, dim)];
	    dst[RIDX(dim-1-j, (i+4), dim)] = src[RIDX((i+4), j, dim)];
	    dst[RIDX(dim-1-j, (i+5), dim)] = src[RIDX((i+5), j, dim)];
	    dst[RIDX(dim-1-j, (i+6), dim)] = src[RIDX((i+6), j, dim)];
	    dst[RIDX(dim-1-j, (i+7), dim)] = src[RIDX((i+7), j, dim)];
	    dst[RIDX(dim-1-j, (i+8), dim)] = src[RIDX((i+8), j, dim)];
	    dst[RIDX(dim-1-j, (i+9), dim)] = src[RIDX((i+9), j, dim)];
	    dst[RIDX(dim-1-j, (i+10), dim)] = src[RIDX((i+10), j, dim)];
	    dst[RIDX(dim-1-j, (i+11), dim)] = src[RIDX((i+11), j, dim)];
	    dst[RIDX(dim-1-j, (i+12), dim)] = src[RIDX((i+12), j, dim)];
	    dst[RIDX(dim-1-j, (i+13), dim)] = src[RIDX((i+13), j, dim)];
	    dst[RIDX(dim-1-j, (i+14), dim)] = src[RIDX((i+14), j, dim)];
	    dst[RIDX(dim-1-j, (i+15), dim)] = src[RIDX((i+15), j, dim)];
	    dst[RIDX(dim-1-j, (i+16), dim)] = src[RIDX((i+16), j, dim)];
	    dst[RIDX(dim-1-j, (i+17), dim)] = src[RIDX((i+17), j, dim)];
	    dst[RIDX(dim-1-j, (i+18), dim)] = src[RIDX((i+18), j, dim)];
	    dst[RIDX(dim-1-j, (i+19), dim)] = src[RIDX((i+19), j, dim)];
	    dst[RIDX(dim-1-j, (i+20), dim)] = src[RIDX((i+20), j, dim)];
	    dst[RIDX(dim-1-j, (i+21), dim)] = src[RIDX((i+21), j, dim)];
	    dst[RIDX(dim-1-j, (i+22), dim)] = src[RIDX((i+22), j, dim)];
	    dst[RIDX(dim-1-j, (i+23), dim)] = src[RIDX((i+23), j, dim)];
	    dst[RIDX(dim-1-j, (i+24), dim)] = src[RIDX((i+24), j, dim)];
	    dst[RIDX(dim-1-j, (i+25), dim)] = src[RIDX((i+25), j, dim)];
	    dst[RIDX(dim-1-j, (i+26), dim)] = src[RIDX((i+26), j, dim)];
	    dst[RIDX(dim-1-j, (i+27), dim)] = src[RIDX((i+27), j, dim)];
	    dst[RIDX(dim-1-j, (i+28), dim)] = src[RIDX((i+28), j, dim)];
	    dst[RIDX(dim-1-j, (i+29), dim)] = src[RIDX((i+29), j, dim)];
	    dst[RIDX(dim-1-j, (i+30), dim)] = src[RIDX((i+30), j, dim)];
	    dst[RIDX(dim-1-j, (i+31), dim)] = src[RIDX((i+31), j, dim)];


       }

    }

}

//------------------------------------------------------------------------

//3rd loop unroll attempt - Max 3.2, good for smaller values
char rotate_unroll_3_descr[] = "Unrolling 3 using outer loop i and striding by 16. Also loop invariants. Good for values <= 1024 as compared to tiling 2.  Max 3.2";

void attempt_unroll_3(int dim, pixel *src, pixel *dst) 
{
     int i, j;
     int loopInvariant = dim - 1;
     int loopInvariant2;

    for (i = 0; i < dim; i+=16)
    {
	for (j = 0; j < dim; j++)
	{
	    loopInvariant2 = loopInvariant -j;
	    dst[RIDX(loopInvariant2, i, dim)] = src[RIDX(i, j, dim)];
	    dst[RIDX(loopInvariant2, (i+1), dim)] = src[RIDX((i+1), j, dim)];
	    dst[RIDX(loopInvariant2, (i+2), dim)] = src[RIDX((i+2), j, dim)];
	    dst[RIDX(loopInvariant2, (i+3), dim)] = src[RIDX((i+3), j, dim)];
	    dst[RIDX(loopInvariant2, (i+4), dim)] = src[RIDX((i+4), j, dim)];
	    dst[RIDX(loopInvariant2, (i+5), dim)] = src[RIDX((i+5), j, dim)];
	    dst[RIDX(loopInvariant2, (i+6), dim)] = src[RIDX((i+6), j, dim)];
	    dst[RIDX(loopInvariant2, (i+7), dim)] = src[RIDX((i+7), j, dim)];
	    dst[RIDX(loopInvariant2, (i+8), dim)] = src[RIDX((i+8), j, dim)];
	    dst[RIDX(loopInvariant2, (i+9), dim)] = src[RIDX((i+9), j, dim)];
	    dst[RIDX(loopInvariant2, (i+10), dim)] = src[RIDX((i+10), j, dim)];
	    dst[RIDX(loopInvariant2, (i+11), dim)] = src[RIDX((i+11), j, dim)];
	    dst[RIDX(loopInvariant2, (i+12), dim)] = src[RIDX((i+12), j, dim)];
	    dst[RIDX(loopInvariant2, (i+13), dim)] = src[RIDX((i+13), j, dim)];
	    dst[RIDX(loopInvariant2, (i+14), dim)] = src[RIDX((i+14), j, dim)];
	    dst[RIDX(loopInvariant2, (i+15), dim)] = src[RIDX((i+15), j, dim)];
       }

    }

}

//---------------------------------------------------------------------------------------------------------------------------------------------

// loop reorder on naive rotate attempt - 1.3
char rotate_reorder_1_descr[] = "Loop reordering by itself is 1.3";
void attempt_reorder_1(int dim, pixel *src, pixel *dst) 
{
     int i, j;
     int loopInvariant = dim - 1;
    for (j = 0; j < dim; j+=1)
    {
	for (i = 0; i < dim; i++)
	{
	    dst[RIDX(loopInvariant-j, i, dim)] = src[RIDX(i, j, dim)];
       }

    }

}

//---------------------------------------------------------------------------------------------------------------------------------------------

// Generic tiling - Tiling 1 - 1.8
// Block - right, down
// Within block src - right, down
// Within block dst - up, right

char rotate_tiling_1_descr[] = "Tiling 1 - Generic on naive rotate with block size 32 - 1.8";
void attempt_tiling_1(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    int blockSize = 32;
    int x, y; 

    for (i = 0; i < dim; i+=blockSize)
    {
	for (j = 0; j < dim; j+=blockSize)
	{
	    for(x = i; x < i + blockSize; x++)
	    {
		for(y = j; y < j + blockSize; y++)
		{
			dst[RIDX(dim-1-y, x, dim)] = src[RIDX(x, y, dim)];
		}
	    }	
       }

    }

}

//------------------------------------------------------------------------

// Tiling 2 - Mean 3.0 but good for big values
// Block - right, down
// Within block src - down, right
// Within block dst - right, up

char rotate_tiling_2_descr[] = "Tiling 2 with concentration on dst by interchanging inner 2 loops - Mean 3.0 but good for larger values";

void attempt_tiling_2(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    int blockSize = 16;	//8, 32 makes it worse, 16 good
    int x, y; 

    for (i = 0; i < dim; i+=blockSize)
    {
	for (j = 0; j < dim; j+=blockSize)
	{
	    for(y = j; y < j + blockSize; y++)
	    {
		for(x = i; x < i + blockSize; x++)
		{
			dst[RIDX(dim-1-y, x, dim)] = src[RIDX(x, y, dim)];
		}
	    }	
       }

    }

}

//------------------------------------------------------------------------

// Tiling 3 gives 1.8 - bad
// Block moves down, right
// Within block src - down, right
// Within block dst - right, up

char rotate_tiling_3_descr[] = "Tiling 3 with concentration on dst and other tricks";
void attempt_tiling_3(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    int blockSize = 32; //32 makes it good
    int x, y; 

    for (j = 0; j < dim; j+=blockSize)
    {
	for (i = 0; i < dim; i+=blockSize)
	{
	    for(y = j; y < j + blockSize; y++)
	    {
		for(x = i; x < i + blockSize; x++)
		{
			dst[RIDX(dim-1-y, x, dim)] = src[RIDX(x, y, dim)];
		}
	    }	
       }

    }

}


//------------------------------------------------------------------------

// Tiling 4
// Block moves down, left to make dst move right,down
// Within block src - down, left
// Within block dst - right, down

char rotate_tiling_4_descr[] = "Tiling 4 using diagram logic attempt. Get around 2.4 with 16, 2.7 with 32";
void attempt_tiling_4(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    int blockSize = 32;
    int x, y; 

    for (j = dim-1; j >= 0; j-=blockSize)
    {
	for (i = 0; i < dim; i+=blockSize)
	{
	    for(y = j; y > j - blockSize; y--)
	    {
		for(x = i; x < i + blockSize; x++)
		{
			dst[RIDX(dim-1-y, x, dim)] = src[RIDX(x, y, dim)];
		}
	    }	
       }

    }

}

//------------------------------------------------------------------------

// Tiling 5
// Block moves
// Within block src - 
// Within block dst - 

char rotate_tiling_5_descr[] = "Tiling 5 using flip. Get around 2.5 with 16, 2.8 with 32 ";
void attempt_tiling_5(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    int blockSize = 32;
    int x, y; 

    for (j = 0; j < dim; j+=blockSize)
    {
	for (i = 0; i < dim; i+=blockSize)
	{
	    for(y = j; y < j + blockSize; y++)
	    {
		for(x = i; x < i + blockSize; x++)
		{
			dst[RIDX(dim-1-y, x, dim)] = src[RIDX(x, y, dim)];
		}
	    }	
       }

    }

}


//---------------------------------------------------------------------------------------------------------------------------------------------

// Combine 1: Just combine unroll 3 and tiling 2 
char rotate_combine_1_descr[] = "Combine 1: Combination of just loopUnroll3 and tiling";
void attempt_combine_1(int dim, pixel *src, pixel *dst) 
{	
     int i, j;
     
     //loop unroll 3
     if(dim<=1024)
     {
     	int loopInvariant = dim - 1;
     	int loopInvariant2;
    	for (i = 0; i < dim; i+=16)
    	{
		for (j = 0; j < dim; j++)
		{
	    		loopInvariant2 = loopInvariant -j;
	    		dst[RIDX(loopInvariant2, i, dim)] = src[RIDX(i, j, dim)];
	    		dst[RIDX(loopInvariant2, (i+1), dim)] = src[RIDX((i+1), j, dim)];
			dst[RIDX(loopInvariant2, (i+2), dim)] = src[RIDX((i+2), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+3), dim)] = src[RIDX((i+3), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+4), dim)] = src[RIDX((i+4), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+5), dim)] = src[RIDX((i+5), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+6), dim)] = src[RIDX((i+6), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+7), dim)] = src[RIDX((i+7), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+8), dim)] = src[RIDX((i+8), j, dim)];
	 		dst[RIDX(loopInvariant2, (i+9), dim)] = src[RIDX((i+9), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+10), dim)] = src[RIDX((i+10), j, dim)];
	   	        dst[RIDX(loopInvariant2, (i+11), dim)] = src[RIDX((i+11), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+12), dim)] = src[RIDX((i+12), j, dim)];
	    	        dst[RIDX(loopInvariant2, (i+13), dim)] = src[RIDX((i+13), j, dim)];
	    	        dst[RIDX(loopInvariant2, (i+14), dim)] = src[RIDX((i+14), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+15), dim)] = src[RIDX((i+15), j, dim)];
      		 }

    	}
    }

    //tiling 2
    else
    {	
    	int blockSize = 16;
	int x,y;	
	for (i = 0; i < dim; i+=blockSize)
   	 {
		for (j = 0; j < dim; j+=blockSize)
		{
	    		for(y = j; y < j + blockSize; y++)
	    		{
				for(x = i; x < i + blockSize; x++)
				{
					dst[RIDX(dim-1-y, x, dim)] = src[RIDX(x, y, dim)];
				}
	    		}	
       		}

    	}
    }

}


//------------------------------------------------------------------------

// Combine 2: combine unroll 3 and tiling 2 with more unrolling inside
char rotate_combine_2_descr[] = "Combine 2 of unroll3 and tiling 2 with loop invariant + more unrolling inside: 3s on avg";
void attempt_combine_2(int dim, pixel *src, pixel *dst) 
{	
     int i, j;
     
     //loop unroll 3
     if(dim<=1024)
     {
     	int loopInvariant = dim - 1;
     	int loopInvariant2;
    	for (i = 0; i < dim; i+=16)
    	{
		for (j = 0; j < dim; j++)
		{
	    		loopInvariant2 = loopInvariant -j;
	    		dst[RIDX(loopInvariant2, i, dim)] = src[RIDX(i, j, dim)];
	    		dst[RIDX(loopInvariant2, (i+1), dim)] = src[RIDX((i+1), j, dim)];
			dst[RIDX(loopInvariant2, (i+2), dim)] = src[RIDX((i+2), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+3), dim)] = src[RIDX((i+3), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+4), dim)] = src[RIDX((i+4), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+5), dim)] = src[RIDX((i+5), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+6), dim)] = src[RIDX((i+6), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+7), dim)] = src[RIDX((i+7), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+8), dim)] = src[RIDX((i+8), j, dim)];
	 		dst[RIDX(loopInvariant2, (i+9), dim)] = src[RIDX((i+9), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+10), dim)] = src[RIDX((i+10), j, dim)];
	   	        dst[RIDX(loopInvariant2, (i+11), dim)] = src[RIDX((i+11), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+12), dim)] = src[RIDX((i+12), j, dim)];
	    	        dst[RIDX(loopInvariant2, (i+13), dim)] = src[RIDX((i+13), j, dim)];
	    	        dst[RIDX(loopInvariant2, (i+14), dim)] = src[RIDX((i+14), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+15), dim)] = src[RIDX((i+15), j, dim)];
      		 }

    	}
    }

    //tiling 2
    else
    {	
    	int blockSize = 16;
	int x,y;
     	int loopInvariant = dim - 1;
     	int loopInvariant2;

	for (i = 0; i < dim; i+=blockSize)
   	 {
		for (j = 0; j < dim; j+=blockSize)
		{
	    		for(y = j; y < j + blockSize; y++)
	    		
			{
				loopInvariant2 = loopInvariant - y;
				for(x = i; x < i + blockSize; x+=8)
				{
				dst[RIDX(loopInvariant2, x, dim)] = src[RIDX(x, y, dim)];
				dst[RIDX(loopInvariant2, x+1, dim)] = src[RIDX(x+1, y, dim)];
				dst[RIDX(loopInvariant2, x+2, dim)] = src[RIDX(x+2, y, dim)];
				dst[RIDX(loopInvariant2, x+3, dim)] = src[RIDX(x+3, y, dim)];
				dst[RIDX(loopInvariant2, x+4, dim)] = src[RIDX(x+4, y, dim)];
				dst[RIDX(loopInvariant2, x+5, dim)] = src[RIDX(x+5, y, dim)];
				dst[RIDX(loopInvariant2, x+6, dim)] = src[RIDX(x+6, y, dim)];
				dst[RIDX(loopInvariant2, x+7, dim)] = src[RIDX(x+7, y, dim)];
				}
	    		}	
       		}

    	}
    }

}


//------------------------------------------------------------------------

// Combine 3: combine unroll 3 and tiling 2 with more unrolling outside 
char rotate_combine_3_descr[] = "Combine 3 of unroll3 and tiling 2 with loop invariant + more unrolling outside: Comes to 2s baad";
void attempt_combine_3(int dim, pixel *src, pixel *dst) 
{	
     int i, j;
     
     //loop unroll 3
     if(dim<=1024)
     {
     	int loopInvariant = dim - 1;
     	int loopInvariant2;
    	for (i = 0; i < dim; i+=16)
    	{
		for (j = 0; j < dim; j++)
		{
	    		loopInvariant2 = loopInvariant -j;
	    		dst[RIDX(loopInvariant2, i, dim)] = src[RIDX(i, j, dim)];
	    		dst[RIDX(loopInvariant2, (i+1), dim)] = src[RIDX((i+1), j, dim)];
			dst[RIDX(loopInvariant2, (i+2), dim)] = src[RIDX((i+2), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+3), dim)] = src[RIDX((i+3), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+4), dim)] = src[RIDX((i+4), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+5), dim)] = src[RIDX((i+5), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+6), dim)] = src[RIDX((i+6), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+7), dim)] = src[RIDX((i+7), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+8), dim)] = src[RIDX((i+8), j, dim)];
	 		dst[RIDX(loopInvariant2, (i+9), dim)] = src[RIDX((i+9), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+10), dim)] = src[RIDX((i+10), j, dim)];
	   	        dst[RIDX(loopInvariant2, (i+11), dim)] = src[RIDX((i+11), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+12), dim)] = src[RIDX((i+12), j, dim)];
	    	        dst[RIDX(loopInvariant2, (i+13), dim)] = src[RIDX((i+13), j, dim)];
	    	        dst[RIDX(loopInvariant2, (i+14), dim)] = src[RIDX((i+14), j, dim)];
	    		dst[RIDX(loopInvariant2, (i+15), dim)] = src[RIDX((i+15), j, dim)];
      		 }

    	}
    }

    //tiling 2
    else
    {	
    	int blockSize = 16;
	int x,y;
     	int loopInvariant = dim - 1;
	for (i = 0; i < dim; i+=blockSize)
   	 {
		for (j = 0; j < dim; j+=blockSize)
		{
	    		for(y = j; y < j + blockSize; y+=16)
	    		{
				for(x = i; x < i + blockSize; x++)
				{
				dst[RIDX((loopInvariant)-y, x, dim)] = src[RIDX(x, y, dim)];
				dst[RIDX((loopInvariant)-(y+1), x, dim)] = src[RIDX(x, y+1, dim)];
				dst[RIDX((loopInvariant)-(y+2), x, dim)] = src[RIDX(x, y+2, dim)];
				dst[RIDX((loopInvariant)-(y+3), x, dim)] = src[RIDX(x, y+3, dim)];
				dst[RIDX((loopInvariant)-(y+4), x, dim)] = src[RIDX(x, y+4, dim)];
				dst[RIDX((loopInvariant)-(y+5), x, dim)] = src[RIDX(x, y+5, dim)];
				dst[RIDX((loopInvariant)-(y+6), x, dim)] = src[RIDX(x, y+6, dim)];
				dst[RIDX((loopInvariant)-(y+7), x, dim)] = src[RIDX(x, y+7, dim)];
				dst[RIDX((loopInvariant)-(y+8), x, dim)] = src[RIDX(x, y+8, dim)];
				dst[RIDX((loopInvariant)-(y+9), x, dim)] = src[RIDX(x, y+9, dim)];
			dst[RIDX((loopInvariant)-(y+10), x, dim)] = src[RIDX(x, y+10, dim)];
			dst[RIDX((loopInvariant)-(y+11), x, dim)] = src[RIDX(x, y+11, dim)];
			dst[RIDX((loopInvariant)-(y+12), x, dim)] = src[RIDX(x, y+12, dim)];
			dst[RIDX((loopInvariant)-(y+13), x, dim)] = src[RIDX(x, y+13, dim)];
			dst[RIDX((loopInvariant)-(y+14), x, dim)] = src[RIDX(x, y+14, dim)];
			dst[RIDX((loopInvariant)-(y+15), x, dim)] = src[RIDX(x, y+15, dim)];
				}
	    		}	
       		}

    	}
    }

}


//------------------------------------------------------------------------

//not done fully - try also outer unroll
// Combine 4 - tiling 2 with unroll

char rotate_combine_4_descr[] = "Combine 4: Tiling 2 with unroll.";
void attempt_combine_4(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    int blockSize = 16;	//8, 32 makes it worse, 16 good
    int x, y; 
    int loopInvariant = dim - 1;
    int loopInvariant2;
    for (i = 0; i < dim; i+=blockSize)
    {
	for (j = 0; j < dim; j+=blockSize)
	{
	    for(y = j; y < j + blockSize; y++)
	    {
		for(x = i; x < i + blockSize; x+=8)
		{
			loopInvariant2 = loopInvariant - y;
			dst[RIDX(loopInvariant2, x, dim)] = src[RIDX(x, y, dim)];
			dst[RIDX(loopInvariant2, x+1, dim)] = src[RIDX(x+1, y, dim)];
			dst[RIDX(loopInvariant2, x+2, dim)] = src[RIDX(x+2, y, dim)];
			dst[RIDX(loopInvariant2, x+3, dim)] = src[RIDX(x+3, y, dim)];
			dst[RIDX(loopInvariant2, x+4, dim)] = src[RIDX(x+4, y, dim)];
			dst[RIDX(loopInvariant2, x+5, dim)] = src[RIDX(x+5, y, dim)];
			dst[RIDX(loopInvariant2, x+6, dim)] = src[RIDX(x+6, y, dim)];
			dst[RIDX(loopInvariant2, x+7, dim)] = src[RIDX(x+7, y, dim)];
		}
	    }	
       }

    }

}




//---------------------------------------------------------------------------------------------------------------------------------------------



/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{   
    add_rotate_function(&naive_rotate, naive_rotate_descr);   
    add_rotate_function(&rotate, rotate_descr); 
 	
    /* ... Register additional rotate functions here */
    
    //unroll attempts
    add_rotate_function(&attempt_unroll_1, rotate_unroll_1_descr);	
    add_rotate_function(&attempt_unroll_2, rotate_unroll_2_descr);
    add_rotate_function(&attempt_unroll_3, rotate_unroll_3_descr);	//quite good for smaller values 
    
    //loop reordering attempts
    add_rotate_function(&attempt_reorder_1, rotate_reorder_1_descr);   
	   
 
    //tiling attempts
    add_rotate_function(&attempt_tiling_1, rotate_tiling_1_descr);   
    add_rotate_function(&attempt_tiling_2, rotate_tiling_2_descr);  //quite good for larger values
    add_rotate_function(&attempt_tiling_3, rotate_tiling_3_descr);   
    add_rotate_function(&attempt_tiling_4, rotate_tiling_4_descr);   
    add_rotate_function(&attempt_tiling_5, rotate_tiling_5_descr);   
    add_rotate_function(&attempt_combine_1, rotate_combine_1_descr); 
    add_rotate_function(&attempt_combine_2, rotate_combine_2_descr);  
    add_rotate_function(&attempt_combine_3, rotate_combine_3_descr);  
    add_rotate_function(&attempt_combine_4, rotate_combine_4_descr);  
}






