#include<stdio.h>

#define N 4
#define RIDX(i,j,n) ((i)*(n)+(j))

void naiveRotate(int *src, int *dst){
 	int i;
	int j;
	int dim = N;
	for(i= 0; i < dim; i++)
        	for (j = 0; j < dim; j++)
            		dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}




void displayMatrix(int mat[N][N]){
	int i;
	int j;

	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			printf("%2d",mat[i][j]);
		}
		printf("\n\n");
	}

	printf("\n");
}



void myRotate(int dim, int *src, int *dst) 
{
    int i, j;
    int remainder = dim % 2;
    int n = dim - remainder;


    for (i = 0; i < dim; i++)
    {
	for (j = 0; j < n; j+=2)
	{
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
	    dst[RIDX(dim-1-(j+1), i, dim)] = src[RIDX(i, (j+1), dim)];
	}
    	//remainder
	/*if (remainder == 1) {
	    dst[RIDX(dim-1-(dim-1), i, dim)] = src[RIDX(i,(dim-1), dim)];
	}*/

    }
}





int main()
{
    // Test Case 1
    int mat[N][N] =
    {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };
 	
    int mat2[N][N];
    
    //just display starting matrix
    displayMatrix(mat);

    naiveRotate(mat, mat2);
    displayMatrix(mat2);
	
    int mat3[N][N];
     
    myRotate(N, mat, mat3);
    displayMatrix(mat3);

 
    return 0;
}
