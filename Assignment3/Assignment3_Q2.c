/* This program illustrate how to compute sobel, Gx, Gy, Gradient and            threshold
   Use the command line argument in the format
   ./Executable "Path to input image" "path to store Gx out_img1" "path to store Gy out_img2" "path to store gradient out_img3" "path to store threshold out_img4"

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

  /* calculating sobel operator
   parameters:
   M1, N1 - Rows and columns on input image
   in_img[][] - input buiding image
   out_img1[][] - Gx output image
   out_img2[][] - Gy output image
   out_img3[][] - Gradient output image
   out_img4[][] - Threshold output image
   Gx[][] - Gx matrix
   Gy[][] - Gy matrix
   temp_x[][] - to save Gx image
   temp_y[][] - to save Gy image
  */
void sobel(int N1, int M1, unsigned char in_img[M1][N1], unsigned char out_img1[M1][N1], unsigned char out_img2[M1][N1], unsigned char out_img3[M1][N1], unsigned char out_img4[M1][N1], int Gx[3][3], int Gy[3][3])
{   
    int i, j, x, y;
    int temp_x[M1][N1], temp_y[M1][N1];
    //Initialize both temp_x and temp_y to 0
    for(i=0;i<M1;i++){
        for(j=0;j<N1;j++){
            temp_x[i][j] = 0;
            temp_y[i][j] = 0;
        }
    }
    //calculating gradx and grady
    for(i=1;i<M1-1;i++){
        for(j=1;j<N1-1;j++){
            for(x=0;x<3;x++){
                for(y=0;y<3;y++){
                    temp_x[i][j] += in_img[i+x-1][j+y-1] * Gx[x][y];
                    temp_y[i][j] += in_img[i+x-1][j+y-1] * Gy[x][y]; 
                }
            }
        }
    }
  
    for(i=0;i<M1;i++){
        for(j=0;j<N1;j++){
            //calculating Gx
            out_img1[i][j] = abs(temp_x[i][j]);
            //calculating Gy
            out_img2[i][j] = abs(temp_y[i][j]);
            //calculating gradient
            out_img3[i][j] = sqrt((out_img1[i][j] * out_img1[i][j]) + (out_img2[i][j] * out_img2[i][j]));
            //calculating threshold
            if(out_img3[i][j] >= 128){
                out_img4[i][j] = 255;
            }
            else{
                out_img4[i][j] = 0;
            }
        }
    }
}

int main(int argc, char **argv)
{
    FILE *fin1, *fout1, *fout2, *fout3, *fout4;
    int i, j, n, N, n1;

    int ROWS1 = atoi(argv[6]);              // Rows of input image
    int COLS1 = atoi(argv[7]);              //  Columns of input image
    
    unsigned char in_buf1[ROWS1][COLS1];    // Buffer for input image1
    unsigned char out_buf1[ROWS1][COLS1];   // Buffer for output image1
    unsigned char out_buf2[ROWS1][COLS1];   // Buffer for output image2 
    unsigned char out_buf3[ROWS1][COLS1];   // Buffer for output image3
    unsigned char out_buf4[ROWS1][COLS1];   // Buffer for output image4
     
    // initializing Gx and Gy matrix
    int Gx[3][3] = { {-1,0,1}, {-2,0,2}, {-1,0,1}}; 
    int Gy[3][3] = { {-1,-2,-1}, {0,0,0}, {1,2,1}};  

    // Check the number of arguments in the command line.
    if(argc != 8)
    {
        fprintf(stderr, "T___Usage: %s in.img out.img\n", argv[0]);
        exit(1);
    }

    // Open the input image file
    if((fin1 = fopen(argv[1], "rb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open input image1 file %s\n", argv[1]);
        exit(1);
    }
    
    // Open the output image file1
    if((fout1 = fopen(argv[2], "wb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open output image file %s\n", argv[2]);
        exit(1);
    }

    // Open the output image file2
    if((fout2 = fopen(argv[3], "wb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open output image file %s\n", argv[3]);
        exit(1);
    }

    // Open the output image file3
    if((fout3 = fopen(argv[4], "wb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open output image file %s\n", argv[4]);
        exit(1);
    }

    // Open the output image file4
    if((fout4 = fopen(argv[5], "wb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open output image file %s\n", argv[5]);
        exit(1);
    }
    
    // Load the input image1
    printf("... Load input image\n");
    n=fread(in_buf1, sizeof(char), ROWS1*COLS1, fin1);
    if(n < ROWS1*COLS1*sizeof(char))
    {
        fprintf(stderr, "ERROR: Read input image file %s error\n", argv[1]);
        exit(1);
    }

    //sobel operator
    sobel(COLS1, ROWS1, in_buf1, out_buf1, out_buf2, out_buf3, out_buf4, Gx, Gy);
   
    // Save the output image1
    printf("...Save the output image1\n");
    n = fwrite(out_buf1, sizeof(char), ROWS1*COLS1, fout1);
    if(n < ROWS1*COLS1*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[2]);
        exit(1);
    }

    // Save the output image2
    printf("...Save the output image2\n");
    n = fwrite(out_buf2, sizeof(char), ROWS1*COLS1, fout2);
    if(n < ROWS1*COLS1*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[3]);
        exit(1);
    }

    // Save the output image3
    printf("...Save the output image3\n");
    n = fwrite(out_buf3, sizeof(char), ROWS1*COLS1, fout3);
    if(n < ROWS1*COLS1*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[4]);
        exit(1);
    }

    // Save the output image4
    printf("...Save the output image4\n");
    n = fwrite(out_buf4, sizeof(char), ROWS1*COLS1, fout4);
    if(n < ROWS1*COLS1*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[5]);
        exit(1);
    }

    fclose(fin1);
    fclose(fout1);
    fclose(fout2);
    fclose(fout3);
    fclose(fout4);
}