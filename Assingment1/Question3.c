#include <stdio.h>
#include <stdlib.h>

//To free the memory as I am writing this program for both rose and museum
void clearmem(FILE *fin, FILE *fout, unsigned char* mem_in, unsigned char* mem_out)
{
    if (fin)
        fclose(fin);

    if (fout)
        fclose(fout);

    if (mem_in)
        free(mem_in);

    if (mem_out)
        free(mem_out);

}

// Check the number of arguments in the command line.
int main(int argc, char **argv)
{
    if(argc != 6) {
        fprintf(stderr, "Usage: %s in.img out.img ROWS COLS intensityLevel\n", argv[0]);
        return 1;
    }

    FILE *fin = NULL, *fout = NULL;
    int ret = 0;
    
    int ROWS = atoi(argv[3]); //rows for both input and output
    int COLS = atoi(argv[4]); //columns for both input and output
    int intensityLevel = atoi(argv[5]); //Assigning last argument for intensity

// dynamically freeing the unused or previousy used memory
    unsigned char* mem_in = (unsigned char*) calloc(ROWS*COLS, sizeof(unsigned char));
    if (NULL == mem_in) {
        fprintf(stderr, "ERROR: Failed to allocate memory, resize Row and colume\n");
        return 1;
    }

//dynamically freeing the unused or previousy used memory
    unsigned char* mem_out = (unsigned char*) calloc(ROWS*COLS, sizeof(unsigned char));
    if (NULL == mem_out) {
        fprintf(stderr, "ERROR: Failed to allocate memory, resize Row and colume\n");
        clearmem(NULL, NULL, mem_in, NULL);
        return 1;
    }
    
    // Open the input image file
    if((fin = fopen(argv[1], "rb")) == NULL) {
        fprintf(stderr, "ERROR: Can't open input image file %s\n", argv[1]);
        clearmem(NULL, NULL, mem_in, mem_out);
        return 1;
    }

    // Open the output image file
    if((fout = fopen(argv[2], "wb")) == NULL) {
        fprintf(stderr, "ERROR: Can't open output image file %s\n", argv[2]);
        clearmem(fin, NULL, mem_in, mem_out);
        return 1;
    }

    printf("... Load input image\n");

    ret = fread(mem_in, sizeof(unsigned char), ROWS*COLS, fin);
    if(ret < ROWS*COLS*sizeof(unsigned char)) {
        fprintf(stderr, "ERROR: Read input image file %s error\n", argv[1]);
        clearmem(fin, fout, mem_in, mem_out);
        return 1;
    }

//Logic for 128, 64, 32 and 16 level intensity quantization
    printf("Loading done\n");
    for (int i = 0; i < ROWS*COLS; i++) {
        if(intensityLevel == 128) {
            *(mem_out +i) = *(mem_in+i) & (0xFE);
        } else if(intensityLevel == 64) {
                *(mem_out +i) = *(mem_in+i) & (0xFC);     
        } else if(intensityLevel== 32) {
                *(mem_out +i) = *(mem_in+i) & (0xF8);
        } else if(intensityLevel == 16) {
                *(mem_out +i) = *(mem_in+i) & (0xF0);
        } else {
            printf("Invalid intensity %d \n", intensityLevel);
            clearmem(fin, fout, mem_in, mem_out);
            exit(1);
        }
    }

//save the output image
    printf("...Save the output image\n");
    ret = fwrite(mem_out, sizeof(unsigned char), ROWS*COLS, fout);
    if(ret < ROWS*COLS*sizeof(unsigned char)) {
        fprintf(stderr, "ERROR: Write output image file %s error\n", argv[2]);
        clearmem(fin, fout, mem_in, mem_out);
        return 1;
    }

    clearmem(fin, fout, mem_in, mem_out);
    return 0;
}