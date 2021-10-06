/* This program illustrate how to compute histogram, cumulative histogram 
    Use the command line argument in the format
    ./Executable "Path to input image" "Path to store histogram image" "Path to store cumulative histogram image" Rows Columns k

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//To computer Histogram
void tocomputeHistogram(int N, int M, unsigned char img[M][N], int k, unsigned char imgNew[k][k], int hist[k])
{
    int i, j, alpha, beta, z, q, maximum_val=0, hist_normal[k];
    //initialize histogram to 0
    for (i = 0; i < k; i++)
    {
        hist[i] = 0;
    }

    //computing histogram
    for (i = 0; i < M; i++)
    {
        for (j = 0; j < N; j++)
        {
            hist[img[i][j]]++;
        }
    }

    //Initialize output image with 255
    for(alpha=0;alpha<k;alpha++)
       {
           for(beta=0;beta<k;beta++){
               imgNew[alpha][beta] = 255;
           }
       }
       
    //// Find the maximum histogram value
    for(q=0;q<k;q++)
       {
           if(hist[q] > maximum_val)
           {
               maximum_val = hist[q];
           }
       }
    //scaling the histogram
    for(int y=0;y<k;y++)
       {
            hist_normal[y] = (int) ((hist[y] * 255) / (maximum_val));
       }
    //Set the appropriate pixel values to zero
    for(int p=0;p<k;p++){
        for(int q=0;q<hist_normal[p];q++){
               
            imgNew[255 -q][p] = 0;
              
        }
    }
      
}

//To compute cumulative histogram    
void tocomputeCummulativeHistogram(int N, int M, unsigned char img[M][N], int k, unsigned char imgNew[k][k],  int hist[k], int cum_Hist[k])
{

    int cumhist_normalized[k], i, j, max_cum_hist=0;

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
         imgNew[i][j] = 255;
       }

    }

    // Find the maximum cummulative histogram value
    for(i=0;i<k;i++)
    {
       if(cum_Hist[i] > max_cum_hist)
       {
           max_cum_hist = cum_Hist[i];
       }

    }

    // Compute the normalized histogram
    cumhist_normalized[0] = cum_Hist[0] * S;
    for(i=1; i < k; i++)
    {
        cumhist_normalized[i] = (int) ((cum_Hist[i] * 255) / (max_cum_hist));
    }

    // Set the appropriate pixel values to zero
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < (cumhist_normalized[i]); j++)
        {
          imgNew[i][255 - j] =  0;
        }
    }
}

int main(int argc, char **argv)
{
    FILE *fin, *fout, *histfout;
    int i, j, n, N;
    // Check the number of arguments in the command line.
    if(argc != 7)
    {
        fprintf(stderr, "Usage: %s in.img out.img rows cols k\n", argv[0]);
        exit(1);
    }
    //converting arguments to integer
    int ROWS = atoi(argv[4]); 
    int COLS = atoi(argv[5]);
    int k    = atoi(argv[6]);
    int hist[k];
    int cum_Hist[k];
    unsigned char in_buf[ROWS][COLS];    // Buffer for input image
    unsigned char out_buf[k][k];   // Buffer for output image
    unsigned char cumout_buf[k][k];   // Buffer for output image

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
    if((histfout = fopen(argv[3], "wb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open output image file %s\n", argv[3]);
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
    //to compute histogram
    tocomputeHistogram(COLS, ROWS, in_buf, k, out_buf, hist);

    //to compute cumulative histogram
    tocomputeCummulativeHistogram(COLS, ROWS, in_buf, k, cumout_buf, hist, cum_Hist);  

    //saving histogram output image
    printf("...Saving the output image\n");
    n = fwrite(out_buf, sizeof(char), (ROWS*COLS), fout);
    if(n < ROWS*COLS*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[2]);
        exit(1);
    }
    
    // Saving the cumulative output image
    printf("...Saving the output image 1\n");
    n = fwrite(cumout_buf, sizeof(char), (ROWS*COLS), histfout);
    if(n < ROWS*COLS*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[3]);
        exit(1);
    }

    fclose(fin);
    fclose(fout);
    fclose(histfout);
}

