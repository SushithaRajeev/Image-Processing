/* This program illustrate how to implement the 3 × 3 Laplacian sharpening filter
   Use the command line argument in the format
   ./Executable "Path to input image" "Path to output image" Rows Columns

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// laplacian sharpening filter function
void zero_crossing(int N1, int M1, unsigned char in_img[M1][N1], int lap_matrix[3][3], unsigned char out_img[M1][N1], int w)
{   
    int i, j, x, y, lap_zero[M1][N1], img_sharp[M1][N1], maximum_value=0;
    //Initialize out_img, lap_zero and img_sharp to 0
    for(i=0;i<M1;i++){
        for(j=0;j<N1;j++){
            out_img[i][j] = 0;
            lap_zero[i][j] = 0;
            img_sharp[i][j] = 0;
        }
    }
    //function for laplacian operator
    for(i=1;i<M1-1;i++){
        for(j=1;j<N1-1;j++){
            for(x=0;x<3;x++){
                for(y=0;y<3;y++){
                    lap_zero[i][j] += in_img[i+x-1][j+y-1] * lap_matrix[x][y];
                }
            }
        }
    }
   
    //compute positive zero crossing points
    for(i=0;i<M1;i++){
        for(j=0;j<N1;j++){
          img_sharp[i][j] = in_img[i][j] - (w * lap_zero[i][j]);
          if(img_sharp[i][j] > 255){
            img_sharp[i][j] = 255;
          }
          else if(img_sharp[i][j] < 0){
            img_sharp[i][j] = 0;
          }
          out_img[i][j] = img_sharp[i][j];
        }
    }
}

int main(int argc, char **argv)
{
    FILE *fin, *fout;
    int i, j, n, N, n1;
    int ROWS1 = atoi(argv[3]);
    int COLS1 = atoi(argv[4]);
    int w = atoi(argv[5]);
    unsigned char in_buf[ROWS1][COLS1];    // Buffer for input image
    unsigned char out_buf[ROWS1][COLS1];   // Buffer for output image
  
    int lap_matrix[3][3] = { {0,1,0}, {1,-4,1}, {0,1,0}}; 
    
    // Check the number of arguments in the command line.
    if(argc != 6)
    {
        fprintf(stderr, "T___Usage: %s in.img out.img\n", argv[0]);
        exit(1);
    }

    // Open the input image file
    if((fin = fopen(argv[1], "rb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open input image1 file %s\n", argv[1]);
        exit(1);
    }

    
    // Open the output image file
    if((fout = fopen(argv[2], "wb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open output image file %s\n", argv[2]);
        exit(1);
    }

    // Load the input image1
    printf("... Load input image\n");
    n=fread(in_buf, sizeof(char), ROWS1*COLS1, fin);
    if(n < ROWS1*COLS1*sizeof(char))
    {
        fprintf(stderr, "ERROR: Read input image file %s error\n", argv[1]);
        exit(1);
    }
    //computing zero crossing points
    zero_crossing(COLS1, ROWS1, in_buf, lap_matrix, out_buf, w);
   
    // Save the output image1
    printf("...Save the output image1\n");
    n = fwrite(out_buf, sizeof(char), ROWS1*COLS1, fout);
    if(n < ROWS1*COLS1*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[2]);
        exit(1);
    }

    fclose(fin);
    fclose(fout);
}