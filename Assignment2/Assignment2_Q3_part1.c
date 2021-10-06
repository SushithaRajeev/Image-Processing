/*  ==================================================================================

    The below C program illustrates the process of calculating the
    histogram and cumulative histogram and transformed image.

    Use the below command line to run the program

    ./executable "Path to input image" "Path to store histogram image" "Path to store cummulative histogram image"
    "Path to store tranformed image" rows columns k

    
    ==================================================================================  */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/*  This function is for computing the histogram.
 
    Parameters:
    img[][]     : The original image
    N, M        : Columns and rows of the original image
    k           : Indicates the intensity level of grayscale image
    imgNew[][]  : Histogram of the original image
    hist[]      : Histogram                                      */

void computeHistogram(int N, int M, unsigned char img[M][N], int k, unsigned char imgNew[k][k], int hist[k])
{

    int i, j, z, q, max_hist=0, hist_normal[k];

    // Initialize histogram to zero
    for (i = 0; i < k; i++)
    {
        hist[i] = 0;
    }

    // Compute the histogram. Assume that an image has been loaded into img[ ][ ]
    for (i = 0; i < M; i++)
    {
        for (j = 0; j < N; j++)
        {
            hist[img[i][j]]++;
        }
    }

    // Initialize output image with 255
    for(i=0;i<k;i++)
    {
       for(int j=0;j<k;j++)
       {

         imgNew[i][j] = 255;
       }

    }

    // find maximum histogram value for scaling
    for(i=0;i<k;i++)
    {
       if(hist[i] > max_hist)
       {
           max_hist = hist[i];
       }

    }

    // Normalize the histogram values
    for(int i=0;i<k;i++)
    {

        hist_normal[i] = (int) ((hist[i] * 255) / (max_hist));

    }

    // Set the appropriate pixel values to zero
    for(int i=0;i<k;i++)
    {
       for(int j=0;j<hist_normal[i];j++)
       {
           imgNew[255 - j][i] = 0;
       }

    }
}


/*  This function is for computing the cummulative histogram.
 
    Parameters:
    img[][]     : The original image
    N, M        : Columns and rows of the original image
    k           : Indicates the intensity level of grayscale image
    imgNew1[][] : Cummulative histogram of the original image
    hist[]      : Histogram
    cumHist[k]  : Cummulative histogram
    cumHistN[k] : Normalized Cummulative histogram                         */

void computeCummulativeHistogram(int N, int M, unsigned char img[M][N], int k, unsigned char imgNew1[k][k],  int hist[k], int cum_Hist[k], int cumhist_normalized[k])
{

    int i, j, max_cumhist=0;

    // Calculate Scale factor
    double S = (double)(k - 1)/(M * N);

    // Initialize cumulative histogram to zero
    for (i = 0; i < k; i++)
    {
        cum_Hist[i] = 0;
    }

    // Compute the cumulative histogram. Assume that an image has been loaded into img[ ][ ]
    cum_Hist[0] = hist[0];
    for(i=1; i < k; i++)
    {
        cum_Hist[i] = cum_Hist[i-1] + hist[i];
    }

    // Initialize output image with 255
    for(int i=0;i<k;i++)
    {
       for(int j=0;j<k;j++)
       {

         imgNew1[i][j] = 255;
       }

    }

    // find maximum cummulative histogram value
    for(i=0;i<k;i++)
    {
       if(cum_Hist[i] > max_cumhist)
       {
           max_cumhist = cum_Hist[i];
       }

    }

    // Compute the normalized histogram
    cumhist_normalized[0] = cum_Hist[0] * S;
    for(i = 1; i < k; i++)
    {
        cumhist_normalized[i] = cum_Hist[i] * S;
    }

    // Set the appropriate pixel values to zero
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < (cumhist_normalized[i]); j++)
        {
          imgNew1[i][255 - j] =  0;
        }
    }
}


/*  This function is for transforming the image.
 
    Parameters:
    img[][]     : The original image
    N, M        : Columns and rows of the original image
    k           : Indicates the intensity level of grayscale image
    imgNew2[][] : Cummulative histogram of the original image
    hist[]      : Histogram
    cumHistN[k] : Normalised Cummulative histogram                         */

void transformation(int N, int M, unsigned char img[M][N], unsigned char imgNew2[M][N], int k, int cumhist_normalized[k])
{
    int i, j;

    for(i = 0; i < M; i++)
    {
        for(j = 0; j < N; j++)
        {
            imgNew2[i][j] = cumhist_normalized[img[i][j]];
        }
    }
}


int main(int argc, char **argv)
{
    FILE *fin, *fout, *fout1, *fout2;
    int i, j, n, N, n1;

    // Check the number of arguments in the command line.
    if(argc != 8)
    {
        fprintf(stderr, "Usage: %s in.img out.img out1.img out2.img rows cols k\n", argv[0]);
        exit(1);
    }

    //converting arguments to integer
    int ROWS = atoi(argv[5]);
    int COLS = atoi(argv[6]);
    int k    = atoi(argv[7]);

    int hist[k];
    int cum_Hist[k];
    int cumhist_normalized[k];

    unsigned char in_buf[ROWS][COLS];    // Buffer for input image
    unsigned char out_buf[k][k];         // Buffer for histogram image
    unsigned char out_buf1[k][k];        // Buffer for cumulative histogram image
    unsigned char out_buf2[ROWS][COLS];  // Buffer for output image
    

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

    // Open the output image file
    if((fout1 = fopen(argv[3], "wb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open output image file 1%s\n", argv[3]);
        exit(1);
    }

    // Open the output image file
    if((fout2 = fopen(argv[4], "wb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open output image file 2%s\n", argv[4]);
        exit(1);
    }

    // Loading the input image
    printf("... Loading the input image\n");
    n=fread(in_buf, sizeof(char), ROWS*COLS, fin);
    if(n < ROWS*COLS*sizeof(char))
    {
        fprintf(stderr, "ERROR: Read input image file %s error\n", argv[1]);
        exit(1);
    }

    //Calling the computeHistogram function
    computeHistogram(COLS, ROWS, in_buf, k, out_buf, hist);

    // Calling the computeCummulativeHistogram function
    computeCummulativeHistogram(COLS, ROWS, in_buf, k, out_buf1, hist, cum_Hist, cumhist_normalized);

    // Calling the image transformation function
    transformation(COLS, ROWS, in_buf, out_buf2, k, cumhist_normalized);

    // Saving the output image i.e., histogram
    printf("...Saving the output image\n");
    n = fwrite(out_buf, sizeof(char), (ROWS*COLS), fout);
    if(n < ROWS*COLS*sizeof(char))
    {

        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[2]);
        exit(1);
    }

    // Saving the output image 1 i.e., cummulative histogram
    printf("...Saving the output image1\n");
    n = fwrite(out_buf1, sizeof(char), (ROWS*COLS), fout1);
    if(n < ROWS*COLS*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file 1%s error\n", argv[3]);
        exit(1);
    }

    // Saving the output image 2 i.e., transformed image
    printf("...Saving the output image2\n");
    n = fwrite(out_buf2, sizeof(char), (ROWS*COLS), fout2);
    if(n < ROWS*COLS*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file 2 %s error\n", argv[4]);
        exit(1);
    }
    
    fclose(fin);
    fclose(fout);
    fclose(fout1);
    fclose(fout2);

}