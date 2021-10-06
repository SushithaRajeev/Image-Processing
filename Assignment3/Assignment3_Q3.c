/* This program illustrate how to implement the 3 × 3 Laplacian operator
   Use the command line argument in the format
   ./Executable "Path to input image" "path to output image" cols rows
 
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* function to compute 3 x 3 laplacian matrix
parameters:
   M1, N1 - Rows and columns on input image
   in_img[][] - input buiding image
   out_img[][] - to save output image
   lap_matrix[][] - given laplacian matrix
   lap_zero[][] - to save laplacian image
   temp_x[][] - to save Gx image
   temp_y[][] - to save Gy image
   maximun_value - to find maximum for normalizing image
*/
void zero_crossing(int N1, int M1, unsigned char in_img[M1][N1], int lap_matrix[3][3], unsigned char out_img[M1][N1])
{   
    int i, j, x, y, lap_zero[M1][N1], maximum_value=0;
    //Initialize both out_img and lap_zero to 0
    for(i=0;i<M1;i++){
        for(j=0;j<N1;j++){
            out_img[i][j] = 0;
            lap_zero[i][j] = 0;
        }
    }
    //laplacian calculation
    for(i=1;i<M1-1;i++){
        for(j=1;j<N1-1;j++){
            for(x=0;x<3;x++){
                for(y=0;y<3;y++){
                    lap_zero[i][j] += in_img[i+x-1][j+y-1] * lap_matrix[x][y];
                }
            }
            //computing maximum laplacian value
            if(lap_zero[i][j] > maximum_value){
              maximum_value = lap_zero[i][j];
            }
        }
    }
    //normalize the image
    for(i=0;i<M1;i++){
        for(j=0;j<N1;j++){
            lap_zero[i][j] = (lap_zero[i][j] * 255)/maximum_value;
        }
    }
    //compute positive zero crossing points
    for(i=0;i<M1;i++){
        for(j=0;j<N1;j++){
          if(lap_zero[i][j] > 0){
            if(lap_zero[i][j+1] < 0 || lap_zero[i][j-1] < 0 || lap_zero[i+1][j] < 0 || lap_zero[i-1][j] < 0){
              out_img[i][j] = lap_zero[i][j];
            }
          }
        }
}
}

int main(int argc, char **argv)
{
    FILE *fin, *fout;
    int i, j, n, N, n1;
    
    int ROWS1 = atoi(argv[3]);
    int COLS1 = atoi(argv[4]);
    
    unsigned char in_buf[ROWS1][COLS1];    // Buffer for input image
    unsigned char out_buf[ROWS1][COLS1];   // Buffer for output image
  
    // initializing laplace matrix
    int lap_matrix[3][3] = { {0,1,0}, {1,-4,1}, {0,1,0}}; 
    
    // Check the number of arguments in the command line.
    if(argc != 5)
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

    // Load the input image
    printf("... Load input image\n");
    n=fread(in_buf, sizeof(char), ROWS1*COLS1, fin);
    if(n < ROWS1*COLS1*sizeof(char))
    {
        fprintf(stderr, "ERROR: Read input image file %s error\n", argv[1]);
        exit(1);
    }

    //computing laplacian operator
    zero_crossing(COLS1, ROWS1, in_buf, lap_matrix, out_buf);
   
    // Save the output image1
    printf("...Save the output image\n");
    n = fwrite(out_buf, sizeof(char), ROWS1*COLS1, fout);
    if(n < ROWS1*COLS1*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[2]);
        exit(1);
    }

    fclose(fin);
    fclose(fout);
}