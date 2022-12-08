#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Для этой программы важно, чтобы bmp был с альфа каналом

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

void writeBMP(struct BMPFile* bmp, char* filepath, int fileNumber) {
    char fileName[100];
    strcpy(fileName, filepath);
    strcat(fileName, "/");
    char fileNumberStr[11];
    sprintf(fileNumberStr, "%d", fileNumber);
    strcat(fileName, fileNumberStr);
    strcat(fileName, ".bmp");
    FILE* file = fopen(fileName, "wb");
    if (file == NULL) {
        return;
    }
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

/*void printImage(struct BMPFile* bmp) {
    int countB = 0;
    for (int i = 0; i < -bmp->bitMapInfoHeader->height; i++) {
        for (int j = 0; j < bmp->bitMapInfoHeader->width * 4; j++) {
            printf("%02x ", bmp->image[countB]);
            countB++;
        }
        printf("\n");
    }
    printf("\n");
}*/

void setByte (struct BMPFile* bmp_in, struct BMPFile* bmp_out, int i, int neigh) {
    if (neigh == 3 && bmp_in->image[i] == 255) {
        bmp_out->image[i] = 0; bmp_out->image[i + 1] = 0; bmp_out->image[i + 2] = 0;
    } else if (neigh >= 2 && neigh <= 3 && bmp_in->image[i] == 0) {
        bmp_out->image[i] = 0; bmp_out->image[i + 1] = 0; bmp_out->image[i + 2] = 0;
    } else {
        bmp_out->image[i] = 255; bmp_out->image[i + 1] = 255; bmp_out->image[i + 2] = 255;
    }
    bmp_out->image[i + 3] = bmp_in->image[i + 3];
}

void threeNeighLT (struct BMPFile* bmp_in, struct BMPFile* bmp_out, int i) {
    int countNei = (bmp_in->image[i + 4] + 1) % 2 +
            (bmp_in->image[i + bmp_in->bitMapInfoHeader->width] + 1) % 2 +
            (bmp_in->image[i + bmp_in->bitMapInfoHeader->width + 4] + 1) % 2;
    setByte(bmp_in, bmp_out, i, countNei);
}

void threeNeighRT (struct BMPFile* bmp_in, struct BMPFile* bmp_out, int i) {
    int countNei = (bmp_in->image[i - 4] + 1) % 2 +
            (bmp_in->image[i + bmp_in->bitMapInfoHeader->width * 4] + 1) % 2 +
            (bmp_in->image[i + bmp_in->bitMapInfoHeader->width * 4 - 4] + 1) % 2;
    setByte(bmp_in, bmp_out, i, countNei);
}

void threeNeighLB (struct BMPFile* bmp_in, struct BMPFile* bmp_out, int i) {
    int countNei = (bmp_in->image[i + 4] + 1) % 2 +
            (bmp_in->image[i - bmp_in->bitMapInfoHeader->width * 4] + 1) % 2 +
            (bmp_in->image[i - bmp_in->bitMapInfoHeader->width * 4 + 4] + 1) % 2;
    setByte(bmp_in, bmp_out, i, countNei);
}

void threeNeighRB (struct BMPFile* bmp_in, struct BMPFile* bmp_out, int i) {
    int countNei = (bmp_in->image[i - 4] + 1) % 2 +
            (bmp_in->image[i - bmp_in->bitMapInfoHeader->width * 4] + 1) % 2 +
            (bmp_in->image[i - bmp_in->bitMapInfoHeader->width * 4 - 4] + 1) % 2;
    setByte(bmp_in, bmp_out, i, countNei);
}

void fiveNeighL (struct BMPFile* bmp_in, struct BMPFile* bmp_out, int i) {
    int countNei = (bmp_in->image[i + 4] + 1) % 2 +
            (bmp_in->image[i + bmp_in->bitMapInfoHeader->width * 4] + 1) % 2 +
            (bmp_in->image[i - bmp_in->bitMapInfoHeader->width * 4] + 1) % 2 +
            (bmp_in->image[i + bmp_in->bitMapInfoHeader->width * 4 + 4] + 1) % 2 +
            (bmp_in->image[i - bmp_in->bitMapInfoHeader->width * 4 + 4] + 1) % 2;
    setByte(bmp_in, bmp_out, i, countNei);
}

void fiveNeighR (struct BMPFile* bmp_in, struct BMPFile* bmp_out, int i) {
    int countNei = (bmp_in->image[i - 4] + 1) % 2 +
            (bmp_in->image[i + bmp_in->bitMapInfoHeader->width * 4] + 1) % 2 +
            (bmp_in->image[i - bmp_in->bitMapInfoHeader->width * 4] + 1) % 2 +
            (bmp_in->image[i + bmp_in->bitMapInfoHeader->width * 4 - 4] + 1) % 2 +
            (bmp_in->image[i - bmp_in->bitMapInfoHeader->width * 4 - 4] + 1) % 2;
    setByte(bmp_in, bmp_out, i, countNei);
}

void fiveNeighT (struct BMPFile* bmp_in, struct BMPFile* bmp_out, int i) {
    int countNei = (bmp_in->image[i - 4] + 1) % 2 +
            (bmp_in->image[i + 4] + 1) % 2 +
            (bmp_in->image[i + bmp_in->bitMapInfoHeader->width * 4] + 1) % 2 +
            (bmp_in->image[i + bmp_in->bitMapInfoHeader->width * 4 - 4] + 1) % 2 +
            (bmp_in->image[i + bmp_in->bitMapInfoHeader->width * 4 + 4] + 1) % 2;
    setByte(bmp_in, bmp_out, i, countNei);
}

void fiveNeighB (struct BMPFile* bmp_in, struct BMPFile* bmp_out, int i) {
    int countNei = (bmp_in->image[i - 4] + 1) % 2 +
                   (bmp_in->image[i + 4] + 1) % 2 +
                   (bmp_in->image[i - bmp_in->bitMapInfoHeader->width * 4] + 1) % 2 +
                   (bmp_in->image[i - bmp_in->bitMapInfoHeader->width * 4 - 4] + 1) % 2 +
                   (bmp_in->image[i - bmp_in->bitMapInfoHeader->width * 4 + 4] + 1) % 2;
    setByte(bmp_in, bmp_out, i, countNei);
}

void eightNeigh (struct BMPFile* bmp_in, struct BMPFile* bmp_out, int i) {
    int countNei = (bmp_in->image[i - 4] + 1) % 2 +
                   (bmp_in->image[i + 4] + 1) % 2 +
                   (bmp_in->image[i - bmp_in->bitMapInfoHeader->width * 4] + 1) % 2 +
                   (bmp_in->image[i + bmp_in->bitMapInfoHeader->width * 4] + 1) % 2 +
                   (bmp_in->image[i + bmp_in->bitMapInfoHeader->width * 4 - 4] + 1) % 2 +
                   (bmp_in->image[i + bmp_in->bitMapInfoHeader->width * 4 + 4] + 1) % 2 +
                   (bmp_in->image[i - bmp_in->bitMapInfoHeader->width * 4 - 4] + 1) % 2 +
                   (bmp_in->image[i - bmp_in->bitMapInfoHeader->width * 4  + 4] + 1) % 2;
    setByte(bmp_in, bmp_out, i, countNei);
}

struct BMPFile* makeNewBmp (struct BMPFile* bmp) {
    struct BMPFile* bmp_out = (struct BMPFile*) malloc(sizeof(struct BMPFile));
    bmp_out->bitMapFileHeader = bmp->bitMapFileHeader;
    bmp_out->bitMapInfoHeader = bmp->bitMapInfoHeader;
    bmp_out->image = (unsigned char*) malloc(bmp_out->bitMapFileHeader->size - bmp_out->bitMapFileHeader->offsetBits);
    if (bmp_out->bitMapFileHeader->size - bmp_out->bitMapFileHeader->offsetBits == 4) {
        bmp_out->image[0] = 255; bmp_out->image[1] = 255; bmp_out->image[2] = 255; bmp_out->image[3] = bmp->image[3];
        return bmp_out;
    }
    if (bmp_out->bitMapFileHeader->size - bmp_out->bitMapFileHeader->offsetBits == 8) {
        bmp_out->image[0] = 255; bmp_out->image[1] = 255; bmp_out->image[2] = 255; bmp_out->image[3] = bmp->image[3];
        bmp_out->image[4] = 255; bmp_out->image[5] = 255; bmp_out->image[6] = 255; bmp_out->image[7] = bmp->image[7];
        return bmp_out;
    }
    int count_h = 0, count_w = 0;
    for (int i = 0; i < bmp->bitMapFileHeader->size - bmp->bitMapFileHeader->offsetBits; i += 4) {
        if (count_h == 0 && count_w == 0) {
            threeNeighLT(bmp, bmp_out, i);
        } else if (count_h == -bmp->bitMapInfoHeader->height - 1 && count_w == bmp->bitMapInfoHeader->width - 1) {
            threeNeighRB(bmp, bmp_out, i);
        } else if (count_w == 0 && count_h == -bmp->bitMapInfoHeader->height - 1) {
            threeNeighLB(bmp, bmp_out, i);
        } else if (count_h == 0 && count_w == bmp->bitMapInfoHeader->width - 1) {
            threeNeighRT(bmp, bmp_out, i);
        } else if (count_w == 0) {
            fiveNeighL(bmp, bmp_out, i);
        } else if (count_h == 0) {
            fiveNeighT(bmp, bmp_out, i);
        } else if (count_w == bmp->bitMapInfoHeader->width - 1) {
            fiveNeighR(bmp, bmp_out, i);
        } else if (count_h == -bmp->bitMapInfoHeader->height - 1) {
            fiveNeighB(bmp, bmp_out, i);
        } else {
            eightNeigh(bmp, bmp_out, i);
        }
        if (count_w == bmp->bitMapInfoHeader->width - 1) {
            count_w = 0;
            count_h++;
        } else {
            count_w++;
        }
    }
    return bmp_out;
}

int main(int argc, char* argv[]) {
    char* inputFile;
    char* outputDir;
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
                outputDir = argv[i + 1];
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
    if (inputFile == NULL || outputDir == NULL) {
        printf("Error: please incert input and output files");
        return -1;
    }
    struct BMPFile* bmp = readBMP(inputFile);
    for (int i = 1; i <= maxIter; i++) {
        bmp = makeNewBmp(bmp);
        if (dumpFreq == 1) {
            writeBMP(bmp, outputDir, i);
        } else if (i % dumpFreq == 1) {
            writeBMP(bmp, outputDir, i);
        }
    }
    return 0;
}



