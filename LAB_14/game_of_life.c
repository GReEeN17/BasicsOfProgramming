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

/*void readBMPRowByRow(FILE* file, struct BMPFile* bmp) {
    int bytesPerPixel = bmp->bitMapInfoHeader->bitCount;
    int rowSize = bytesPerPixel * bmp->bitMapInfoHeader->width;
    int rowPadding = (4 - (rowSize % 4)) % 4;
    int rowsWritten = 0;
    unsigned char* row = (unsigned char*) malloc(rowSize + rowPadding);
    unsigned char* p = &bmp->data[(bmp->bitMapInfoHeader->height - 1) * rowSize];
    fseek(file, bmp->bitMapFileHeader->offsetBits, SEEK_SET);
    while (rowsWritten < bmp->bitMapInfoHeader->height) {
        fread(row, rowSize + rowPadding, 1, file);
        if (bytesPerPixel == 3) {
            for (int i = 0; i < rowSize; i += bytesPerPixel) {
                *p = row[i + 2]; p++;
                *p = row[i + 1]; p++;
                *p = row[i]; p++;
            }
        } else {
            printf("Error: wrong number of bytes per pixel");
            exit(0);
        }
        rowsWritten++;
        p = p - 2 * rowSize;
    }
}*/

struct BMPFile* readBMP (char* fileName) {
    FILE* file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("Can't load file");
        exit(0);
    }
    struct BMPFile* bmp = (struct BMPFile*) malloc(sizeof(struct BMPFile));
    fread(bmp->bitMapFileHeader, sizeof(struct BitMapFileHeader), 1, file);
    fread(bmp->bitMapInfoHeader, sizeof(struct BitMapInfoHeader), 1, file);
    /*int data_size = bmp->bitMapInfoHeader->width * bmp->bitMapInfoHeader->height * bmp->bitMapInfoHeader->bitCount / 8;
    bmp->data = (unsigned char*) malloc(data_size);
    readBMPRowByRow(file, bmp);*/
    bmp->data = (unsigned char*) malloc(bmp->bitMapInfoHeader->size);
    fread(bmp->data, bmp->bitMapInfoHeader->size, 1, file);
    fclose(file);
    return bmp;
}

void writeBMP(struct BMPFile* bmp) {
    FILE* file = fopen("out.bmp", "r+b");
    fwrite(bmp->bitMapFileHeader, sizeof(struct BitMapFileHeader), 1, file);
    fwrite(bmp->bitMapInfoHeader, sizeof(struct BitMapInfoHeader), 1, file);
    fwrite(bmp->data, bmp->bitMapInfoHeader->size, 1, file);
    fclose(file);
}

void freeBMPFile (struct BMPFile* bmp) {
    if (bmp){
        free(bmp);
    }
}



