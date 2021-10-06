/* This program illustrate how to compute program for template    matching using cross-correlation
   Use the command line argument in the format
   ./Executable "Path to input image" "path to template image" "Path to store output1 correlation and template matching" "path to store output2 normalized image" cols1 rows1 cols2 rows2
 
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// correlation and template matching
void correlation(int N1, int M1, int N2, int M2, unsigned char img1[M1][N1], unsigned char img2[M2][N2], unsigned char imgNew1[M1][N1])
{   
    int i, j;
    float normalize[M1][N1], normalize1[M1][N1], normalize2[M1][N1], new[M1][N1], another_new[M1][N1];
    //initiliaze the temporary values taken in this function to 0
    for(i=0;i<M1;i++){
        for(j=0;j<N1;j++){
            imgNew1[i][j]=img1[i][j];
            normalize1[i][j] = 0;
            normalize2[i][j] = 0;
            new[i][j] = 0;
        }
    }
    //calculating the correletion and template matching
    for (i = 0; i < M1 - M2; i++)
    {
        for(j = 0; j < N1 - N2; j++)
        {  
            for(int x = 0; x < M2; x++){
                for(int y = 0; y < N2; y++){
                    new[i][j] += img1[i+x][j+y]*img2[x][y];
                    normalize1[i][j] += img1[i+x][j+y] * img1[i+x][j+y];
                    normalize2[i][j] += img2[x][y] * img2[x][y];
                }
            }
        }        
    }
    //function to nomalize the output image
    for(i=0;i<M1-M2;i++)
    {
        for(j=0;j<N1-N2;j++){
            another_new[i][j] = new[i][j]/(sqrt(normalize1[i][j]) * sqrt(normalize2[i][j])); 
        }
    }
    //function to draw line around letter R in the text_image
    for(i=0;i<M1;i++)
    {
        for(j=0;j<N1;j++)
        {
            if(another_new[i][j] > 0.99)
            {
                int temp, temp1, temp2, temp3;
                temp1=i+M2;
                temp=j+N2;
                
                while(j <= temp){
                    imgNew1[i][j]=0;
                    j++;

                }
                while(i <= temp1){
                    imgNew1[i][j]=0;
                    i++;
                }
                temp2=j-N2;
                while(j >= temp2){
                    imgNew1[i][j]=0;
                    j--;
                }
                temp3=i-M2;
                while(i >= temp3){
                    imgNew1[i][j]=0;
                    i--;
                }
            }

        }
    }
    
}

int main(int argc, char **argv)
{
    int ROWS1 = atoi(argv[4]); /* rows for input image1 */
    int COLS1 = atoi(argv[5]); /* columns for input image1 */
    int ROWS2 = atoi(argv[6]); /* rows for input image2 */
    int COLS2 = atoi(argv[7]); /* columns for input image2 */

    unsigned char in_buf1[ROWS1][COLS1];  // Buffer for input image1
    unsigned char in_buf2[ROWS2][COLS2];  // Buffer for input image2
    unsigned char out_buf1[ROWS1][COLS1]; // Buffer for output image1
    unsigned char out_buf2[ROWS1][COLS1]; // Buffer for output image2
    FILE *fin1, *fin2, *fout1;
    int i, j, n, N, n1;   

    // Check the number of arguments in the command line.
    if(argc != 8)
    {
        fprintf(stderr, "T___Usage: %s in.img out.img\n", argv[0]);
        exit(1);
    }

    // Open the input image1 file
    if((fin1 = fopen(argv[1], "rb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open input image1 file %s\n", argv[1]);
        exit(1);
    }

    // Open the input image2 file
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
    
    // Load the input image1
    printf("... Load input image1\n");
    n=fread(in_buf1, sizeof(char), ROWS1*COLS1, fin1);
    if(n < ROWS1*COLS1*sizeof(char))
    {
        fprintf(stderr, "ERROR: Read input image file %s error\n", argv[1]);
        exit(1);
    }

     // Load the input image2
    printf("... Load input image2\n");
    n=fread(in_buf2, sizeof(char), ROWS2*COLS2, fin2);
    if(n < ROWS2*COLS2*sizeof(char))
    {
        fprintf(stderr, "ERROR: Read input image file %s error\n", argv[2]);
        exit(1);
    }

    correlation(COLS1, ROWS1, COLS2, ROWS2, in_buf1, in_buf2, out_buf1);
   
    // Save the output image1
    printf("...Save the output image1\n");
    n = fwrite(out_buf1, sizeof(char), ROWS1*COLS1, fout1);
    if(n < ROWS1*COLS1*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[3]);
        exit(1);
    }

    fclose(fin1);
    fclose(fin2);
    fclose(fout1);
}