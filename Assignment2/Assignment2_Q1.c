/* This program illustrate how to compute power law transformation function using gamma correction 
   Use the command line argument in the format
   ./Executable "Path to input image" "Path to store transformed image" Rows Columns gamma_value

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Resizing the image using power law transformation
void correlation(int N1, int M1, int N2, int M2, unsigned char img1[M1][N1], unsigned char img2[M2][N2], unsigned char imgNew1[M1][N1], unsigned char imgNew2[M1][N1])
{   
    float normalize;
    unsigned char new[M1][N1];
    int normalize2, normalize1;
    for (int i = 0; i <= 345; i++)
    {
        for(int j = 0; j <= 376; j++)
        {   
            normalize1=0;
            normalize2=0;
            for(int x = 0; x < 10; x++){
                for(int y = 0; y < 14; y++){
                    imgNew1[i][j] += img1[i+x][j+y]*img2[x][y];
                    normalize2 += img2[x][y] * img2[x][y];
                    normalize1 += img1[i+x][j+y] * img1[i+x][j+y];
                    normalize = sqrt(normalize1) * sqrt(normalize2);

                }
            }
            
        }
    imgNew2[i][j] = imgNew1[i][j]/normalize;
    }
}

int main(int argc, char **argv)
{
    printf("argc-%d", argc);
    int ROWS1 = atoi(argv[5]);
    int COLS1 = atoi(argv[6]);
    int ROWS2 = atoi(argv[7]); /* rows for input image */
    int COLS2 = atoi(argv[8]); /* columns for input image */
    //double gamma = atof(argv[5]); /* Gamma value */
    unsigned char in_buf1[ROWS1][COLS1]; 
    unsigned char in_buf2[ROWS2][COLS2];   // Buffer for input image
    unsigned char out_buf[ROWS1][COLS1];    // Buffer for output image
    FILE *fin1, *fin2, *fout1, *fout2;
    int i, j, n, N, n1;   

    printf("argc-%d", argc);
    // Check the number of arguments in the command line.
    if(argc != 9)
    {
        printf("argc-%d", argc);
        fprintf(stderr, "T___Usage: %s in.img out.img\n", argv[0]);
        exit(1);
    }

    // Open the input image file
    if((fin1 = fopen(argv[1], "rb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open input image1 file %s\n", argv[1]);
        exit(1);
    }

    // Open the input image file
    if((fin2 = fopen(argv[2], "rb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open input image2 file %s\n", argv[2]);
        exit(1);
    }
    // Open the output image file1
    if((fout1 = fopen(argv[3], "wb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open output image file %s\n", argv[3]);
        exit(1);
    }

    // Open the output image file2
    if((fout2 = fopen(argv[4], "wb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open output image file %s\n", argv[4]);
        exit(1);
    }
    
    // Load the input image1
    printf("... Load input image\n");
    n=fread(in_buf1, sizeof(char), ROWS1*COLS1, fin1);
    printf("%d", n);
    if(n < ROWS1*COLS1*sizeof(char))
    {
        fprintf(stderr, "ERROR: Read input image file %s error\n", argv[1]);
        exit(1);
    }

     // Load the input image2
    printf("... Load input image\n");
    n=fread(in_buf2, sizeof(char), ROWS2*COLS2, fin2);
    printf("%d", n);
    if(n < ROWS2*COLS2*sizeof(char))
    {
        fprintf(stderr, "ERROR: Read input image file %s error\n", argv[2]);
        exit(1);
    }

    correlation(COLS1, ROWS1, COLS2, ROWS2, in_buf1, in_buf2, out_buf1, out_buf2);
   
    // Save the output image1
    printf("...Save the output image\n");
    n = fwrite(out_buf1, sizeof(char), ROWS1*COLS1, fout1);
    if(n < ROWS1*COLS1*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[3]);
        exit(1);
    }

    // Save the output image2
    printf("...Save the output image\n");
    n = fwrite(out_buf2, sizeof(char), ROWS1*COLS1, fout2);
    if(n < ROWS1*COLS1*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[4]);
        exit(1);
    }

    fclose(fin);
    fclose(fout);
}