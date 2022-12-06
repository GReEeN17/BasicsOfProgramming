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
    unsigned char* data;
};

#pragma pack(pop)

void readBMPRowByRow(FILE* file, struct BMPFile* bmp) {

}

struct BMPFile* readBMP (char* fileName) {
    FILE* file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("Can't load file");
        exit(0);
    }
    struct BMPFile* bmp = (struct BMPFile*) malloc(sizeof(struct BMPFile));
    fread(bmp->bitMapFileHeader, sizeof(struct BitMapFileHeader), 1, file);
    fread(bmp->bitMapInfoHeader, sizeof(struct BitMapInfoHeader), 1, file);
    int data_size = bmp->bitMapInfoHeader->width * bmp->bitMapInfoHeader->height * bmp->bitMapInfoHeader->bitCount / 8;
    bmp->data = (unsigned char*) malloc(data_size);
    readBMPRowByRow(file, bmp);
    fclose(file);
    return bmp;
}

void freeBMPFile (struct BMPFile* bmp) {
    if (bmp){
        free(bmp);
    }
}



