/*  ==================================================================================

    The below C program illustrates the process of calculating the histogram and transformed image.

    Use the below command line to run the program

    ./q3 "path to input image" "path to store transformed image" rows columns
    ==================================================================================  */

#include <stdio.h>
#include <stdlib.h>
#define k 256 //Intensity level of grayscale image

/*  This function is for computing the histogram.
 
    Parameters:
    img[][]     : The original image
    N, M        : Columns and rows of the original image
    imgNew[][]  : Histogram of the original image
    hist[]      : Histogram                                      */

void Histogram(int N, int M, unsigned char img[M][N], int hist[k]){
    int maxi_hist_val = 0, min_hist_val = 65536;
    int q, hist_normal[k];
    // Initialize histogram to zero
    for(int z=0;z<k;z++)
    {
        hist[z]=0;
    }
    // Compute the histogram. Assume that an image has been loaded into img
    for(int i=0;i<M;i++){
        for(int j=0;j<N;j++){
            hist[img[i][j]]++;
        }
    }
   
    //find maximum histogram value for scaling
    for(q=0;q<k;q++)
    {
        if(hist[q] > maxi_hist_val)
        {
            maxi_hist_val = hist[q];
        }
    }
    //Normalize the histogram values
    for(int y=0;y<k;y++)
    {
        hist_normal[y] = (int) ((hist[y] * 255) / (maxi_hist_val));
    }
}
//Divide function to calculate divide in nodel algorithm
int divide(int hist[k], int l, int h)
{
   
    int difference;
    int i, j;
    int min_value = 65536;
    int min_index = 0;
    
   for(i=l; i<=h; i++)
   {
       int sum_temp_01 = 0;
       //for left side
       for(j=l; j<=i; j++)
       {
           sum_temp_01 = sum_temp_01 + hist[j];
       }

       int sum_temp_02 = 0;
       //for right side
       for(j=i+1; j<=h; j++)
       {
           sum_temp_02 = sum_temp_02 + hist[j];
       }
        // difference between right and left side
       difference = abs(sum_temp_01 - sum_temp_02);
       if (difference < min_value)
        {
            min_value = difference;
            min_index = i;
        }
   }
   return min_index;
}

/* This function is to build novel algorithm logic of build 
Parameters:
hist[] : Histogram of the image
l      : low value initially assigned
h      : high value initially assigned
l2     : low2 value initially assigned
h2     : high2 value initially assigned
T[]    : Transform array

*/
void build(int hist[k], int l, int h, int l2, int h2, int T[k]){
    int m;
    if(l < h){
        
        m = divide(hist, l, h);
        T[m] = (l2+h2)/2;
        build(hist, l, m, l2, T[m], T);//left build
        build(hist, m+1, h, T[m]+1, h2, T);//right build
    }
}

/* Transformation function
   parameters:
   N, M:  column and row for the image
   img[][] : input image
   NewImg[][]:output image

*/
void Transformation(int N, int M, unsigned char img[M][N], int T[k], unsigned char NewImg[M][N])
{
	int i, j;
	for (i = 0; i < M; i++){
        for (j = 0; j < N; j++){
            NewImg[i][j] = T[img[i][j]];
        }
    }
}

int main(int argc, char **argv)
{
    int OLD_ROWS = atoi(argv[3]);    /* rows for input image */
    int OLD_COLS = atoi(argv[4]);    /* columns for input image */
    int NEW_ROWS = atoi(argv[3]);    /* rows for output image */
    int NEW_COLS = atoi(argv[4]);    /* columns for output image */

    unsigned char in_buf[OLD_ROWS][OLD_COLS];    // Buffer for input image
    unsigned char out_buf[OLD_ROWS][OLD_COLS];    // Buffer for output image
    FILE *fin, *fout;
    int i, j, n, N, n1;
    int low, high, lowN, highN;
    int hist[k];
    int T[k];

    // Check the number of arguments in the command line.
    if(argc != 5)
    {
        fprintf(stderr, "Usage: %s in.img out.img\n", argv[0]);
        exit(1);

    }

    // Open the input image file
    if((fin = fopen(argv[1], "rb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open input image file %s\n", argv[1]);
        exit(1);

    }

    // Open the output image file
    if((fout = fopen(argv[2], "wb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open output image file %s\n", argv[2]);
        exit(1);
    }
    
    // Load the input image
    printf("... Load input image\n");
    n=fread(in_buf, sizeof(char), OLD_ROWS*OLD_COLS, fin);
    if(n < OLD_ROWS*OLD_COLS*sizeof(char))
    {
        fprintf(stderr, "ERROR: Read input image file %s error\n", argv[1]);
        exit(1);
    }
    //Calling the histogram compute program but im not displaying the histogram just the normalized image
    Histogram(OLD_COLS, OLD_ROWS, in_buf, hist);

    //Initialize T[k] to -1
    for (i = 0; i < k; i++){
        T[i] = -1;
    }

    low = 0;
    high = k-1;
    lowN = 0;
    highN = k-1;

    build(hist, low, high, lowN, highN, T);
    //filling left over values to nearest neighbors
    for(i = 0; i < k; i++){
        if(T[k-1] == -1){
            T[k-1] = T[k-2];
        }
        else if(T[i] == -1){
            T[i] = T[i+1];
        }
    }
    //calling the transformation function for novel algorithm
    Transformation(OLD_COLS, OLD_ROWS, in_buf, T, out_buf);

    // Save the output image
    printf("...Save the output image\n");
    n = fwrite(out_buf, sizeof(char), (OLD_ROWS*OLD_COLS), fout);
    if(n < OLD_ROWS*OLD_COLS*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[2]);
        exit(1);
    }

    fclose(fin);
    fclose(fout);
}