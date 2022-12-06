#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma push (1)
struct ID3V2Header {
    char ID3[3];
    char version[2];
    char flags;
    unsigned sizeBytes;
};

struct ID3V2ExtendedHeader {
    unsigned headerSize;
    unsigned short flags;
    unsigned sizeOfPadding;
};

struct FrameHeader {
    char frameID[4];
    unsigned sizeBytes;
    unsigned short flags;
};

struct Frame {
    struct FrameHeader* frameHeader;
    char* frameData;
};

struct MP3File {
    char* fileName;
    struct ID3V2Header* header;
    struct ID3V2ExtendedHeader* extendedHeader;
    unsigned frameAmount;
    struct Frame** frames;
    unsigned dataSize;
    char* data;
};
#pragma pop;

unsigned int getSize(unsigned sizeBytes) {
    return ((sizeBytes >> 24) & 0x000000ff) | ((sizeBytes >> 8) & 0x0000ff00) | ((sizeBytes << 8) & 0x00ff0000) | ((sizeBytes << 24) & 0xff000000);
}

short isFrameCorrect(struct FrameHeader* frameHeader) {
    if (frameHeader->frameID[0] == 0 && frameHeader->frameID[1] == 0 && frameHeader->frameID[2] == 0 && frameHeader->frameID[3] == 0) {
        return 0;
    }
    if (frameHeader->sizeBytes == 0) {
        return 0;
    }
    return 1;
}

struct MP3File* readInf(char* fileName) {
    FILE* file = fopen(fileName, "rb");
    if (file == NULL) {
        return NULL;
    }
    struct MP3File* mp3 = (struct MP3File*) malloc(sizeof(struct MP3File));
    mp3->fileName = fileName;
    mp3->header = (struct ID3V2Header*) malloc(sizeof(struct ID3V2Header));
    fread(mp3->header, sizeof(struct ID3V2Header), 1, file);
    if (mp3->header->flags >> 6 != 0) {
        mp3->extendedHeader = (struct ID3V2ExtendedHeader*) malloc(sizeof(struct ID3V2ExtendedHeader));
        fread(mp3->extendedHeader, sizeof(struct ID3V2ExtendedHeader), 1, file);
    } else {
        mp3->extendedHeader = NULL;
    }
    unsigned headerSize = getSize(mp3->header->sizeBytes);
    mp3->frameAmount = 0;
    mp3->frames = NULL;
    while (ftell(file) < headerSize + sizeof(struct ID3V2Header)) {
        struct Frame* currentFrame = (struct Frame*) malloc(sizeof(struct Frame));
        currentFrame->frameHeader = (struct FrameHeader*) malloc(sizeof(struct FrameHeader));
        fread(currentFrame->frameHeader, sizeof(struct FrameHeader), 1, file);
        if (!isFrameCorrect(currentFrame->frameHeader)) break;
        unsigned frameDataSize = getSize(currentFrame->frameHeader->sizeBytes);
        currentFrame->frameData = (char*) malloc(frameDataSize * sizeof(char));
        mp3->frameAmount++;
        mp3->frames = realloc(mp3->frames, mp3->frameAmount * sizeof(struct Frame));
        mp3->frames[mp3->frameAmount - 1] = currentFrame;
    }
    mp3->dataSize = headerSize + sizeof(struct ID3V2Header) - ftell(file);
    mp3->data = (char*) malloc(mp3->dataSize * sizeof(char));
    fread(mp3->data, sizeof(char), mp3->dataSize, file);
    fclose(file);
    return mp3;
}



int main(int argc, char* argv[]) {
    char commands[10][20] = {};
    char arguments[10][50] = {};
    int ind_in_argv;
    int j;
    for (int i = 1; i < argc; i++) {
        for (j = 0; argv[i][j] != '=' && argv[i][j] != '\0'; j++) {
            commands[i - 1][j] = argv[i][j];
        }
        ind_in_argv = ++j;
        j = 0;
        while (argv[i][ind_in_argv] != '\0') {
            arguments[i - 1][j++] = argv[i][ind_in_argv++];
        }
    }
    struct MP3File* mp3 = readInf(arguments[0]);
    if (mp3 == NULL) {
        printf("Error: incorrect filename");
        return 0;
    }
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(commands[i], "--show") == 0) {
            showFrames(mp3);
        } else if (strcmp(commands[i], "--get") == 0) {
            if (strlen(arguments[i]) == 4) {
                showFrame(mp3, arguments[i]);
            } else {
                printf("Error: incorrect frame name");
            }
        } else if (strcmp(commands[i], "--set") == 0){
            if (strlen(arguments[i]) == 4) {
                setFrame(mp3, arguments[i], arguments[i + 1]);
            } else {
                printf("Error: incorrect frame name");
            }
        }
    }
    return 0;
}
