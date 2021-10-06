
/* Question 2: Feature Vectors in Frequency Domain
   a. Dividing the spectrum into set of rings
   command line arguments: ./executable input.raw output.text ROWS COLS
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.1415926

typedef struct { // complex data type
 double r; // real part
 double i; // imaginary part
} COMPLEX;

int *index_mapping;

// Reverses the lower n bits of an Integer
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

// Function:   OneDimensionalFFT
// Parameters: R          – Rows of the original image
//             N          – Rows/Columns of the original image
//             TD[]       - 1D Column/Row buffer
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

// Function:   FourierSpectrumAnalysis
// Parameters: N          – Rows/Columns of the original image
//             DFT[][]    – Fourier transform image
//             fout       - FILE pointer that points to output text file
void FourierSpectrumAnalysis(int N, COMPLEX DFT[N][N], FILE *fout)
{
  int M = N/2, distance;
  double FH[M], d;
  
  // Initialize the frequency histogram array
  for(int i=0; i<M; i++)
  {
    FH[i] = 0.0;
  }

  for(int i=0; i<N; i++)
  {
    for(int j=0; j<N; j++)
    {
        // Calculates the distance from current pixel (i, j) to the center pixel (M, M)
        d = sqrt((i - M) * (i - M) + (j - M) * (j - M));

        // Converts distance 'd' to an integer value
        distance = (int) d;

        // Sets the distance to 'M-1' if the distance is greater than 'M-1'
        if(distance > M - 1)
        {
          distance = M - 1;
        }

        // Spectra value of corresponding pixel is calculated and is added to Frequency Histogram
        // array of corresponding index (distance)
        FH[distance] += sqrt(DFT[i][j].r * DFT[i][j].r + DFT[i][j].i * DFT[i][j].i);
    }    
  }

  // Writes the content to output text file
  fprintf(fout,"index,value \n");
  for(int i=0; i<M; i++)
  {
    // Writes the Frequency Histogram index and array values to output text file
    fprintf(fout, "%d,%f \n", i, FH[i]);
  } 

}


// Function:   TwoDimensionalFFT
// Parameters: M, N       – Rows and Columns of the original image
//             img[][]    – Original image
//             C_img[][]  - Complex type image
//             DFT[][]    - Fourier transform image
//             fout       – FILE pointer that points to the output tect file
void TwoDimensionsalFFT(int M, int N, unsigned char img[M][N], COMPLEX C_img[M][N], COMPLEX DFT[M][N], FILE *fout)
{
  int spect_max = 0, i, j, n;


  COMPLEX *TDC = (COMPLEX *)malloc(M * sizeof(COMPLEX));; // temp 1D buffer for a column
  COMPLEX *TDR = (COMPLEX *)malloc(N * sizeof(COMPLEX));; // temp buffer for a row
  double spect[M][N];

  index_mapping = (int *)malloc(M * sizeof(int));
  
  // Binary bits count that are to be reversed
  n = (int)(log((double)M) / log(2.0));
      
  // Loop for intializing the arrays
  for(i=0; i<M; i++)
  {
    for(j=0; j<N; j++)
    {
       C_img[i][j].r = pow(-1, i + j) * (double)img[i][j];
       C_img[i][j].i = 0.0;
    }
  }

  // Call ReverseBits function to reverse the bits
  for(i=0; i<M; i++)
  {
    index_mapping[i] = ReverseBits(n, i);
  }
  
 // Pass 1: Apply 1D FFT to each column of Cimg[][]
  for(i=0; i<N; i++)
  {
      // Copy the column I from Cimg[][] to TDC[]
      for(j=0; j<M; j++)
      {
        TDC[j] = C_img[j][i];
      }
      
      // Apply 1D FFT to TDC[] and return the result in TDC[]
      OneDimensionalFFT(M, M, TDC);

      // Copy TDC[] to the column I of DFT[][]
      for(j=0; j<M; j++)
      {
        DFT[j][i] = TDC[j];
      }
  }

  TDR = (COMPLEX *)malloc(N * sizeof(COMPLEX));
  n = (int)(log((double)N) / log(2.0));
  index_mapping = (int *)malloc(N * sizeof(int));
  
  for (j = 0; j < N; j++){
    index_mapping[j] = ReverseBits(n, j);
  }

  // Pass 2: Apply 1D FFT to each row of DFT[][]
  for(i=0; i<M; i++)
  {
      // Copy row I from DFT[][] to TDR[];
      for(j=0; j<N; j++)
      {
        TDR[j] = DFT[i][j];
      }

      // Apply 1D FFT to TDR[] and return the result in TDR[];
      OneDimensionalFFT(M, N, TDR);

      // Copy TDR[] to the row I of DFT[][];
      for(j=0; j<N; j++)
      {
        DFT[i][j] = TDR[j];
      }
  }    
  
 // Call FourierSpectrumAnalysis function to calculate the Frequency histogram
 FourierSpectrumAnalysis(N, DFT, fout);
}

// Function: main
int main(int argc, char **argv)
{
 FILE *fin, *fout;
 int i, j, n, N, x, y;
 
 // args 3 and 4 (x, y) are the rows and columns of the original image
 // atoi method is used to convert the string value to int
 x  = atoi(argv[3]);
 y  = atoi(argv[4]);

 unsigned char in_buf[x][y];   // Buffer for input original image
 unsigned char out_buf[x][1000];  // Buffer for output text file 

 COMPLEX C_img[x][y]; // for image of complex type
 COMPLEX DFT[x][y]; // for Fourier Transform 
 
 // Check the number of arguments in the command line.
 if (argc != 5)
 {
 fprintf(stderr, "Usage: %s in.img out.img \n", argv[0]);
 exit(1);
 }

 // Open the input original image file
 if ((fin = fopen(argv[1], "rb")) == NULL) 
 {
 fprintf(stderr, "ERROR: Cann't open input image file %s \n", argv[1]);
 exit(1);
 }

 // Open the output text file
 if ((fout = fopen(argv[2], "w")) == NULL) 
 {
 fprintf(stderr, "ERROR: Can't open output text file %s \n", argv[3]);
 exit(1);
 }

 // Load the input original image
 printf("... Load input original image \n");
 n = fread(in_buf, sizeof(char), x*y, fin);
 if (n < x*y*sizeof(char))
 {
  fprintf(stderr, "ERROR: Read input original image file %s error \n", argv[1]);
  exit(1);
 }


 // Call the TwoDimensionsalFFT function by passing the parameters
 TwoDimensionsalFFT(x, y, in_buf, C_img, DFT, fout);


 // Save the output text file
 printf("... Save the output text file \n");
 n=fwrite(out_buf, sizeof(char), x*y, fout);
 if (n < x*y*sizeof(char))
 {
  fprintf(stderr, "ERROR: Write output text file %s error \n", argv[2]);
  exit(1);
 }

 fclose(fin);
 fclose(fout);
}