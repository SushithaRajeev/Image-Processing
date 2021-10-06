#include <stdio.h>
#include <stdlib.h>

//Resampling of image
unsigned char resampling(int N, int M, unsigned char img[M][N], double x, double y)
{
    //printf("\n Resampling Start:");
    double s,t ;
    int i, j;
    double result;
    i = (int)x;
    j = (int)y;
    s = x - i;
    t = y - j;
    result =  img[j][i]*(1.0 - s)*(1.0 - t) + img[j][i+1]*s*(1.0 - t) + img[j+1][i]*(1.0 - s)*t + img[j+1][i+1]*s*t;
    return((unsigned char)((int)result));
}

//Resizing of image 
void resizeImage(int N, int M, unsigned char img[M][N], int N1, int M1, unsigned char imgNew[M1][N1])
{
    double x, y;
    double Dx, Dy;
    x = 0.0;
    y = 0.0; 
    Dx = (double) (N - 1)/(double)(N1 - 1);
    Dy = (double) (M - 1)/(double)(M1 - 1);
    double k = 1.2;
    double d = 0.7;
   
    for (int j=0;j<M1; j++)
    {
        for(int i=0; i<N1; i++)

        {
            int k2 = (int)(k/2);
            double sum = 0.0;
            for(int s = -k2; s <= k2; s++){
                for(int t = -k2; t <= k2; t++){
                    sum += resampling(N, M, img, x+k2*d, y+k2*d);
                }
            }
            imgNew[j][i] = (unsigned char)((int)(sum/k*k));
            x += Dx;
        }
        
        y += Dy;
        x = 0.0;
    }
}

int main(int argc, char **argv)
{
    int OLD_ROWS = atoi(argv[3]);    /* rows for input image */
    int OLD_COLS = atoi(argv[4]);    /* columns for input image */
    int NEW_ROWS = atoi(argv[5]);    /* rows for output image */
    int NEW_COLS = atoi(argv[6]);    /* columns for output image */

    unsigned char in_buf[OLD_ROWS][OLD_COLS];    // Buffer for input image
    unsigned char out_buf[NEW_ROWS][NEW_COLS];    // Buffer for output image
    FILE *fin, *fout;
    int i, j, n, N, n1;    

    // Check the number of arguments in the command line.
    if(argc != 7)
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
    printf("%d", n);

    if(n < OLD_ROWS*OLD_COLS*sizeof(char))
    {

        fprintf(stderr, "ERROR: Read input image file %s error\n", argv[1]);
        exit(1);

    }

    resizeImage(OLD_COLS, OLD_ROWS, in_buf, NEW_COLS, NEW_ROWS, out_buf);
   
   //save the output image
    printf("...Save the output image\n");
    n = fwrite(out_buf, sizeof(char), NEW_ROWS*NEW_COLS, fout);

    if(n < NEW_ROWS*NEW_COLS*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[2]);
        exit(1);
    }

    fclose(fin);
    fclose(fout);
}