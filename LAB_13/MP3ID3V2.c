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
