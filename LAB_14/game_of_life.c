#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#pragma pack(push, 1)

struct BitMapFileHeader {
    unsigned short type;
    unsigned size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned offsetBits;
};

struct BitMapInfoHeader {
    unsigned size;
    int width;
    int height;
    unsigned short planes;
    unsigned short bitCount;
    unsigned compression;
    unsigned sizeImage;
    int xPelsPerMeter;
    int yPelsPerMeter;
    unsigned colorsUsed;
    unsigned colorsImportant;
};

struct BMPFile {
    struct BitMapFileHeader* bitMapFileHeader;
    struct BitMapInfoHeader* bitMapInfoHeader;
    unsigned char* image;
};

#pragma pack(pop)

struct BMPFile* readBMP (char* fileName) {
    FILE* file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("Can't load file");
        exit(0);
    }
    struct BMPFile* bmp = (struct BMPFile*) malloc(sizeof(struct BMPFile));
    bmp->bitMapFileHeader = (struct BitMapFileHeader*) malloc(sizeof(struct BitMapFileHeader));
    fread(bmp->bitMapFileHeader, sizeof(struct BitMapFileHeader), 1, file);
    bmp->bitMapInfoHeader = (struct BitMapInfoHeader*) malloc(sizeof(struct BitMapInfoHeader));
    fread(bmp->bitMapInfoHeader, sizeof(struct BitMapInfoHeader), 1, file);
    fseek(file, bmp->bitMapFileHeader->offsetBits, SEEK_SET);
    bmp->image = (unsigned char*) malloc(bmp->bitMapFileHeader->size - bmp->bitMapFileHeader->offsetBits);
    fread(bmp->image, bmp->bitMapFileHeader->size - bmp->bitMapFileHeader->offsetBits, 1, file);
    fclose(file);
    return bmp;
}

void writeBMP(struct BMPFile* bmp) {
    FILE* file = fopen("out.bmp", "r+b");
    fwrite(bmp->bitMapFileHeader, sizeof(struct BitMapFileHeader), 1, file);
    fwrite(bmp->bitMapInfoHeader, sizeof(struct BitMapInfoHeader), 1, file);
    fwrite(bmp->image, bmp->bitMapFileHeader->size - bmp->bitMapFileHeader->offsetBits, 1, file);
    fclose(file);
}

void freeBMPFile (struct BMPFile* bmp) {
    if (bmp){
        free(bmp->bitMapInfoHeader);
        free(bmp->bitMapFileHeader);
        free(bmp->image);
        free(bmp);
    }
}

void printImage(struct BMPFile* bmp) {
    for (int i = 0; i < bmp->bitMapFileHeader->size - bmp->bitMapFileHeader->offsetBits; i++) {
        if (i % 16 == 0) {
            printf("\n%04x: ", i);
        }
        printf("%02x ", bmp->image[i]);
    }
}

int threeNeighLT (struct BMPFile* bmp, unsigned char* image, int i) {
    return 0;
}

int threeNeighRT (struct BMPFile* bmp, unsigned char* image, int i) {
    return 0;
}

int threeNeighLB (struct BMPFile* bmp, unsigned char* image, int i) {
    return 0;
}

int threeNeighRB (struct BMPFile* bmp, unsigned char* image, int i) {
    return 0;
}

int fiveNeighL (struct BMPFile* bmp, unsigned char* image, int i) {
    return 0;
}

int fiveNeighR (struct BMPFile* bmp, unsigned char* image, int i) {
    return 0;
}

int fiveNeighT (struct BMPFile* bmp, unsigned char* image, int i) {
    return 0;
}

int fiveNeighB (struct BMPFile* bmp, unsigned char* image, int i) {
    return 0;
}

int eightNeigh (struct BMPFile* bmp, unsigned char* image, int i) {
    return 0;
}

struct BMPFile* makeNewBmp (struct BMPFile* bmp, char* inputFile, char* outputFile) {
    struct BMPFile* bmp_out = (struct BMPFile*) malloc(sizeof(struct BMPFile));
    bmp_out->bitMapFileHeader = bmp->bitMapFileHeader;
    bmp_out->bitMapInfoHeader = bmp->bitMapInfoHeader;
    bmp_out->image = (unsigned char*) malloc(bmp_out->bitMapFileHeader->size - bmp_out->bitMapFileHeader->offsetBits);
    int count_h = 0, count_w = 0;
    for (int i = 0; i < bmp->bitMapFileHeader->size - bmp->bitMapFileHeader->offsetBits; i++) {
        if (count_h == 0 && count_w == 0) {
            threeNeighLT(bmp_out, bmp->image, i);
        } else if (count_h == bmp->bitMapInfoHeader->height - 1 && count_w == bmp->bitMapInfoHeader->width - 1) {
            threeNeighRB(bmp_out, bmp->image, i);
        } else if (count_w == 0 && count_h == bmp->bitMapInfoHeader->height - 1) {
            threeNeighLB(bmp_out, bmp->image, i);
        } else if (count_h == 0 && count_w == bmp->bitMapInfoHeader->width - 1) {
            threeNeighRT(bmp_out, bmp->image, i);
        } else if (count_w == 0) {
            fiveNeighL(bmp_out, bmp->image, i);
        } else if (count_h == 0) {
            fiveNeighT(bmp_out, bmp->image, i);
        } else if (count_w == bmp->bitMapInfoHeader->width - 1) {
            fiveNeighR(bmp_out, bmp->image, i);
        } else if (count_h == bmp->bitMapInfoHeader->height - 1) {
            fiveNeighB(bmp_out, bmp->image, i);
        } else {
            eightNeigh(bmp_out, bmp->image, i);
        }
        if (count_w == bmp->bitMapInfoHeader->width - 1) {
            count_w = 0;
            count_h++;
        } else {
            count_w++;
        }
    }
}

int main(int argc, char* argv[]) {
    char* inputFile;
    char* outputFile;
    int maxIter = 10000000;
    int dumpFreq = 1;
    if (argc < 5) {
        printf("Error: invalid amount of arguments");
        return -1;
    }
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "--input") == 0) {
            if (i + 1 < argc) {
                inputFile = argv[i + 1];
            } else {
                printf("Error: invalid parameter --input");
            }
        } else if (strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                outputFile = argv[i + 1];
            } else {
                printf("Error: invalid parameter --output");
            }
        } else if (strcmp(argv[i], "--max_iter") == 0) {
            if (i + 1 < argc) {
                maxIter = atoi(argv[i + 1]);
            } else {
                printf("Error: invalid parameter --max_iter");
            }
        } else if (strcmp(argv[i], "--dump_freq") == 0) {
            if (i + 1 < argc) {
                dumpFreq = atoi(argv[i + 1]);
            } else {
                printf("Error: invalid parameter --dump_freq");
            }
        }
    }
    if (inputFile == NULL || outputFile == NULL) {
        printf("Error: please incert input and output files");
        return -1;
    }
    struct BMPFile* bmp = readBMP(inputFile);
    printImage(bmp);
    return 0;
}



