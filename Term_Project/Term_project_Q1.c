/*
This program implements the FFT algorithm. 
Command line input: ./executable input.raw output.raw ROWS COLS
*/
#include <stdio.h>
#include <math.h>
#define PI 3.1415926

//Global Image Buffers:

#define ROWS 256 // number of rows
#define COLS 256 // number of columns
typedef struct 
{ 
  // complex data type
double r; // real part
double i; // imaginary part
} COMPLEX;
COMPLEX C_img[ROWS][COLS]; // for image of complex type
COMPLEX DFT[ROWS][COLS]; // for Fourier Transform
COMPLEX Cimg[ROWS][COLS];
unsigned char out_img[ROWS][COLS]; 
int *index_mapping;


//decimal to binary
int ReverseBits(int n, int index)
{
    unsigned int p, q;
    int r;
    p = index;
    q = 0;
    for(r=0; r<n; r++)
    {
        q = q * 2;
        q += p % 2;
        p = p/2;
    }
    return((int)q);
}
// Performs Addition between complex numbers
COMPLEX ComplexAddition(COMPLEX a1, COMPLEX a2)
{
  COMPLEX a;
  a.r = a1.r + a2.r;
  a.i = a1.i + a2.i;
  return a;
}

// Performs Subtraction between complex numbers
COMPLEX ComplexSubtraction(COMPLEX s1, COMPLEX s2)
{
  COMPLEX s;
  s.r = s1.r - s2.r;
  s.i = s1.i - s2.i;
  return s;
}

// Performs Multiplication between complex numbers
COMPLEX ComplexMultiplication(COMPLEX m1, COMPLEX m2)
{
  COMPLEX m;
  m.r = (m1.r * m2.r) - (m1.i * m2.i);
  m.i = (m1.r * m2.i) + (m2.r * m1.i);
  return m;
}

// Performs Multiplication between a complex number and a decimal number (double)
COMPLEX ComplexRealMultiplication(COMPLEX m, double x)
{
  COMPLEX n;
  n.r = m.r * x;
  n.i = m.i * x;
  return n;
}
//1dimension FFT
void OneDimensionalFFT(int R, int N, COMPLEX TD[])
{
  int i, j, z, M, p, q, x, n;
  COMPLEX *TD_Temp = (COMPLEX *)malloc(N * sizeof(COMPLEX));
  COMPLEX c1, c2;

  for(i=0; i<N; i++)
  {
    TD_Temp[index_mapping[i]] = TD[i];
  }

  // The initial length of subgroups
  M = 1;

  // The number of pairs of subgroups
  j = N/2;
  n = (int)(log((double)R) / log(2.0));

  // Successive merging for n levels
  for(i=0; i<n; i++)
  {
    // Merge pairs at the current level
    for(z=0; z<j; z++)
    {
      // the start of the first group
      p = z*2*M;

      // the start of the second group
      q = (z*2 + 1) * M;
      for(x=0; x< M; x++)
      {
          c1.r           = cos(PI * x  / M);
          c1.i           = sin(-PI * x / M);
          c2             = ComplexMultiplication(TD_Temp[q + x], c1);
          TD_Temp[q + x] = ComplexRealMultiplication(ComplexSubtraction(TD_Temp[p + x], c2), 0.5);
          TD_Temp[p + x] = ComplexRealMultiplication(ComplexAddition(TD_Temp[p + x], c2), 0.5);

      }
    }

    // Double the subgroup length
    M = 2*M;

    // The number of groups is reduced by a half
    j = j/2;
  }
  
  for (i = 0; i < N; i++)
  {
    TD[i] = TD_Temp[i];
  }
  return; 
}


void twodFFT(unsigned char in_buf[ROWS][COLS], unsigned char out_buf[ROWS][COLS], COMPLEX Cimg[ROWS][COLS], COMPLEX DFT[ROWS][COLS]){
  int i,j,n,spect_max = 0;
  double spect[ROWS][COLS];
  COMPLEX *TDC = (COMPLEX *)malloc(ROWS * sizeof(COMPLEX)); // temp 1D buffer for a column
  COMPLEX *TDR = (COMPLEX *)malloc(COLS * sizeof(COMPLEX)); // temp buffer for a row

  index_mapping = (int *)malloc(ROWS * sizeof(int));
  // Binary bits count that are to be reversed
  n = (int)(log((double)ROWS) / log(2.0));
 
  //INITIALIZING THE ARRAY
  for(i=0; i<ROWS; i++){
    for(j=0; j<COLS; j++){
      Cimg[i][j].r = pow(-1, i + j) * (double)in_buf[i][j];
      Cimg[i][j].i = 0.0;
    }
  }
  //calling reversebit function
  for(i = 0; i < ROWS; i++){
    index_mapping[i] = ReverseBits(n, i);
  }
  // Pass 1: Apply 1D FFT to each column of Cimg[][]
  for(i=0; i<COLS; i++)
  {
      // Copy the column I from Cimg[][] to TDC[]
      for(j=0; j<ROWS; j++)
      {
        TDC[j] = Cimg[j][i];
      }
      
      // Apply 1D FFT to TDC[] and return the result in TDC[]
      OneDimensionalFFT(ROWS, ROWS, TDC);

      // Copy TDC[] to the column I of DFT[][]
      for(j=0; j<ROWS; j++)
      {
        DFT[j][i] = TDC[j];
      }
  }

  TDR = (COMPLEX *)malloc(COLS * sizeof(COMPLEX));
  n = (int)(log((double)COLS) / log(2.0));
  index_mapping = (int *)malloc(COLS * sizeof(int));
  
  for (j = 0; j < COLS; j++){
    index_mapping[j] = ReverseBits(n, j);
  }

  // Pass 2: Apply 1D FFT to each row of DFT[][]
  for(i=0; i<ROWS; i++)
  {
      // Copy row I from DFT[][] to TDR[];
      for(j=0; j<COLS; j++)
      {
        TDR[j] = DFT[i][j];
      }

      // Apply 1D FFT to TDR[] and return the result in TDR[];
      OneDimensionalFFT(ROWS, COLS, TDR);

      // Copy TDR[] to the row I of DFT[][];
      for(j=0; j<COLS; j++)
      {
        DFT[i][j] = TDR[j];
      }
  }    
  for(i=0; i<ROWS; i++) 
  {
    for(j=0; j<COLS; j++) 
    {
      // Compute the magnitude of DFT[i][j] and store the value in spectrum[i][j]
      spect[i][j] = sqrt(DFT[i][j].r * DFT[i][j].r + DFT[i][j].i * DFT[i][j].i);

      // Calculatest the maximum value in spectrum[i][j]
      if(spect_max < spect[i][j])
      {
        spect_max = spect[i][j];
      }
    }
  }
  for(i=0; i<ROWS; i++) 
  {
    for(j=0; j<COLS; j++) 
    {
        // Map the value in spectrum[i][j] to a new value in the range [0, 255] using the scaling factor
        spect[i][j] = (float)(255.0 *(1.0*log(1.0 + spect[i][j])) / log(1.0 + spect_max));
        
        // Convert the scaled spectrum value from double type to unsigned char type and save it in out_img[i][j]
        out_buf[i][j]  = (unsigned char) spect[i][j];
    }
  }

}


int main(int argc, char **argv)
{
    //printf("argc-%d", argc);
    //int ROWS = atoi(argv[3]);
    //int COLS = atoi(argv[4]);
    unsigned char in_buf[ROWS][COLS]; 
    unsigned char out_buf[ROWS][COLS]; 
    unsigned char img[ROWS][COLS];// buffer of real image
    COMPLEX Cimg[ROWS][COLS]; // buffer of complex image
    COMPLEX DFT[ROWS][COLS];
    
    FILE *fin, *fout;
    int i, j, n, N, n1;   

    printf("argc-%d", argc);
    // Check the number of arguments in the command line.
    if(argc != 5)
    {
        printf("argc-%d", argc);
        fprintf(stderr, "T___Usage: %s in.img out.img\n", argv[0]);
        exit(1);
    }

    // Open the input image file
    if((fin = fopen(argv[1], "rb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open input image1 file %s\n", argv[1]);
        exit(1);
    }

    // Open the output image file1
    if((fout = fopen(argv[2], "wb")) == NULL)
    {
        fprintf(stderr, "ERROR: Can't open output image file %s\n", argv[2]);
        exit(1);
    }
    
    // Load the input image1
    printf("... Load input image\n");
    n=fread(in_buf, sizeof(char), ROWS*COLS, fin);
    printf("%d", n);
    if(n < ROWS*COLS*sizeof(char))
    {
        fprintf(stderr, "ERROR: Read input image file %s error\n", argv[1]);
        exit(1);
    }

    printf("Here/n");
    twodFFT(in_buf, out_buf, Cimg, DFT); 
   
    // Save the output image1
    printf("...Save the output image\n");
    n = fwrite(out_buf, sizeof(char), ROWS*COLS, fout);
    if(n < ROWS*COLS*sizeof(char))
    {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[2]);
        exit(1);
    }

    fclose(fin);
    fclose(fout);
}