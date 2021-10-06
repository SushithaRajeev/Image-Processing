
// Question 3: Low-Pass and High-Pass Filtering
//command line: ./executable input.raw low.raw high.raw ROWS COLS cutoff_frequency

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.1415926

typedef struct { // complex data type
 double r; // real part
 double i; // imaginary part
} COMPLEX;

int *index_map;

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

// Performs Complex Conjugate of a complex number
COMPLEX ComplexConjugate(COMPLEX m1)
{
  COMPLEX m;
  m.r = m1.r;
  m.i = -m1.i;
  return m;
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
    TD_Temp[index_map[i]] = TD[i];
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

// Function:   LowandHighPassFiltering
// Parameters: M, N       – Rows and Columns of the original image
//             F          - Cut-off frequencey
//             img[][]    – Original image
//             C_img[][]  - Complex type image
//             DFT[][]    - Fourier transform image
//             imgNew1[][] – Output image for low pass filtering
//             imgNew2[][] – Output image for high pass filtering
void LowandHighPassFiltering(int M, int N, int F, unsigned char img[M][N], COMPLEX C_img[M][N], COMPLEX DFT[M][N], unsigned char imgNew1[M][N], unsigned char imgNew2[M][N])
{
  int spectrum_max = 0, i, j, n, distance, X = N/2;


  COMPLEX *TDC = (COMPLEX *)malloc(M * sizeof(COMPLEX)); // temp 1D buffer for a column
  COMPLEX *TDR = (COMPLEX *)malloc(N * sizeof(COMPLEX)); // temp buffer for a row

  COMPLEX *TDC_LowpassFilter = (COMPLEX *)malloc(M * sizeof(COMPLEX)); // temp 1D buffer for a column
  COMPLEX *TDR_LowpassFilter = (COMPLEX *)malloc(N * sizeof(COMPLEX)); // temp buffer for a row

  COMPLEX *TDC_HighpassFilter = (COMPLEX *)malloc(M * sizeof(COMPLEX)); // temp 1D buffer for a column
  COMPLEX *TDR_HighpassFilter = (COMPLEX *)malloc(N * sizeof(COMPLEX)); // temp buffer for a row

  int H_HighpassFilter[M][N], H_LowpassFilter[M][N];
  double spectrum[M][N], d;

  COMPLEX Modified_DFT_LowpassFilter[M][N], Modified_DFT_HighpassFilter[M][N], Inverse_DFT_LowpassFilter[M][N], Inverse_DFT_HighpassFilter[M][N];

  index_map = (int *)malloc(M * sizeof(int));
  
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
    index_map[i] = ReverseBits(n, i);
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
  index_map = (int *)malloc(N * sizeof(int));
  
  for (j = 0; j < N; j++){
    index_map[j] = ReverseBits(n, j);
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
  
  // Loop for low pass and high pass filters
  for(int i=0; i<M; i++)
  {
    for(int j=0; j<N; j++)
    {
        d = sqrt((i - X) * (i - X) + (j - X) * (j - X));
        distance = (int) d;

        // If distance is less than or equal to the cut-off frequency 
        if(distance <= F)
        {
          // Lowpass filter value is set to 1
          H_LowpassFilter[i][j]  = 1;
          // Highpass filter value is set to 0
          H_HighpassFilter[i][j] = 0;
        }
        // If distance is greater than the cut-off frequency 
        else
        {
          // Lowpass filter value is set to 0
          H_LowpassFilter[i][j]  = 0;
          // Highpass filter value is set to 1
          H_HighpassFilter[i][j] = 1;
        }
    }    
  }
  
  for(int i=0; i<M; i++) 
  {
    for(int j=0; j<N; j++)
    {
      // ComplexRealMultiplication is performed between DFT and LowpassFilter
      Modified_DFT_LowpassFilter[i][j]  = ComplexRealMultiplication(DFT[i][j], H_LowpassFilter[i][j]);

      // Performs the conjugate of the LowpassFilter
      Modified_DFT_LowpassFilter[i][j]  = ComplexConjugate(Modified_DFT_LowpassFilter[i][j]);

      // ComplexRealMultiplication is performed between DFT and HighpassFilter
      Modified_DFT_HighpassFilter[i][j] = ComplexRealMultiplication(DFT[i][j], H_HighpassFilter[i][j]);

      // Perfomrs the conjugate of the HighpassFilter
      Modified_DFT_HighpassFilter[i][j] = ComplexConjugate(Modified_DFT_HighpassFilter[i][j]);

    }
  }

  // Pass 1: Apply 1D FFT to each column of Modified_DFT_LowpassFilter[][] and Modified_DFT_HighpassFilter[][]
  for(i=0; i<N; i++)
  {
      // Copy the column I from Modified_DFT_LowpassFilter[][] to TDC_LowpassFilter[] and 
      // from Modified_DFT_HighpassFilter[][] to TDC_HighpassFilter[]
      for(j=0; j<M; j++)
      {
        TDC_LowpassFilter[j]  = Modified_DFT_LowpassFilter[j][i];
        TDC_HighpassFilter[j] = Modified_DFT_HighpassFilter[j][i];
      }
      
      // Apply 1D FFT to TDC_LowpassFilter[] and TDC_HighpassFilter[]
      OneDimensionalFFT(M, M, TDC_LowpassFilter);
      OneDimensionalFFT(M, M, TDC_HighpassFilter);

      // Copy TDC_LowpassFilter[] to the column I of Inverse_DFT_LowpassFilter[][] and
      // TDC_HighpassFilter[] to the column I of Inverse_DFT_HighpassFilter[][]
      for(j=0; j<M; j++)
      {
        Inverse_DFT_LowpassFilter[j][i]  = TDC_LowpassFilter[j];
        Inverse_DFT_HighpassFilter[j][i] = TDC_HighpassFilter[j];
      }
  }

  TDR_LowpassFilter = (COMPLEX *)malloc(N * sizeof(COMPLEX));
  TDR_HighpassFilter = (COMPLEX *)malloc(N * sizeof(COMPLEX));
  n = (int)(log((double)N) / log(2.0));
  index_map = (int *)malloc(N * sizeof(int));
  
  for (j = 0; j < N; j++){
    index_map[j] = ReverseBits(n, j);
  }

  // Pass 2: Apply 1D FFT to each row of Inverse_DFT_LowpassFilter[][] and Inverse_DFT_HighpassFilter[][]
  for(i=0; i<M; i++)
  {
      // Copy row I from Inverse_DFT_LowpassFilter[][] to TDR_LowpassFilter[]
      // and from Inverse_DFT_HighpassFilter[][] to TDR_HighpassFilter[];
      for(j=0; j<N; j++)
      {
        TDR_LowpassFilter[j]  = Inverse_DFT_LowpassFilter[i][j];
        TDR_HighpassFilter[j] = Inverse_DFT_HighpassFilter[i][j];
      }

      // Apply 1D FFT to TDR_LowpassFilter[] and return the result in TDR_LowpassFilter[];
      OneDimensionalFFT(M, N, TDR_LowpassFilter);
      
      // Apply 1D FFT to TDR_HighpassFilter[] and return the result in TDR_HighpassFilter[];
      OneDimensionalFFT(M, N, TDR_HighpassFilter);

      // Copy TDR_LowpassFilter[] to the row I of Inverse_DFT_LowpassFilter[][] and
      // TDR_HighpassFilter[] to the row I of Inverse_DFT_HighpassFilter[][] ;
      for(j=0; j<N; j++)
      {
        Inverse_DFT_LowpassFilter[i][j]  = TDR_LowpassFilter[j];
        Inverse_DFT_HighpassFilter[i][j] = TDR_HighpassFilter[j];
      }
  }

// Sets the low pass and high pass filter image values
for(i=0; i<M; i++)
{
  for(j=0; j<N; j++)
  {
    // If real part of Inverse_DFT_LowpassFilter[i][j] is < 0      
    if(Inverse_DFT_LowpassFilter[i][j].r < 0)
    {
      imgNew1[i][j] = -Inverse_DFT_HighpassFilter[i][j].r * M * M;
    }

    // If real part of Inverse_DFT_LowpassFilter[i][j] is >= 0
    if(Inverse_DFT_LowpassFilter[i][j].r >= 0)
    {
      imgNew1[i][j] = Inverse_DFT_LowpassFilter[i][j].r * M * M;
    }
    
    // If real part of Inverse_DFT_HighpassFilter[i][j] is < 0
    if(Inverse_DFT_HighpassFilter[i][j].r < 0)
    {
      imgNew2[i][j] = -Inverse_DFT_HighpassFilter[i][j].r * M * M;
    }

    // If real part of Inverse_DFT_HighpassFilter[i][j] is >= 0
    if(Inverse_DFT_HighpassFilter[i][j].r >= 0)
    {
      
      imgNew2[i][j] = Inverse_DFT_HighpassFilter[i][j].r * M * M;
    }
  }
 }    
}

// Function: main
int main(int argc, char **argv)
{
 FILE *fin, *fout1, *fout2;
 int i, j, n, N, x, y, f;
 
 // args 4 and 5 (x, y) are the rows and columns of the original image
 // arg 6 saves the cut-off frequencey values
 // atoi method is used to convert the string value to int
 x  = atoi(argv[4]);
 y  = atoi(argv[5]);
 f  = atoi(argv[6]);

 unsigned char in_buf[x][y];   // Buffer for input original image
 unsigned char out_buf1[x][y], out_buf2[x][y];  // Buffer for output images 

 COMPLEX C_img[x][y]; // for image of complex type
 COMPLEX DFT[x][y]; // for Fourier Transform 
 
 // Check the number of arguments in the command line.
 if (argc != 7)
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

 // Open the output image file 1
 if ((fout1 = fopen(argv[2], "wb")) == NULL) 
 {
 fprintf(stderr, "ERROR: Can't open output image file 1 %s \n", argv[3]);
 exit(1);
 }

 // Open the output image file 2
 if ((fout2 = fopen(argv[3], "wb")) == NULL) 
 {
 fprintf(stderr, "ERROR: Can't open output image file 2 %s \n", argv[3]);
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


 // Call the LowandHighPassFiltering function by passing the parameters
 LowandHighPassFiltering(x, y, f, in_buf, C_img, DFT, out_buf1, out_buf2);


 // Save the output images
 printf("... Save the output image 1 \n");
 n=fwrite(out_buf1, sizeof(char), x*y, fout1);
 if (n < x*y*sizeof(char))
 {
  fprintf(stderr, "ERROR: Write output image file 1 %s error \n", argv[2]);
  exit(1);
 }

// Save the output images
 printf("... Save the output image 2 \n");
 n=fwrite(out_buf2, sizeof(char), x*y, fout2);
 if (n < x*y*sizeof(char))
 {
  fprintf(stderr, "ERROR: Write output image file 2 %s error \n", argv[3]);
  exit(1);
 }

 fclose(fin);
 fclose(fout1);
 fclose(fout2);
}
