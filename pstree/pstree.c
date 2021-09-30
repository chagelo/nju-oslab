#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// #define DT_UNKNOWN       0
// #define DT_FIFO          1
// #define DT_CHR           2
// #define DT_DIR           4
// #define DT_BLK           6
// #define DT_REG           8
// #define DT_LNK          10
// #define DT_SOCK         12
// #define DT_WHT          14

#define MAX_PROC 1024


typedef struct {
    int pid;
    char name[256];
    char state;
    int ppid;
}Process;

Process process[MAX_PROC];
int propid[MAX_PROC];

const char *PROC = "/proc";

int getProcessfolder() {

    DIR *d = opendir(PROC);
    struct dirent *dir;
    
    //RPOC cannot be opened
    if (d == NULL)
      return -1;

    int counter = 0;
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_type != DT_DIR)
            continue;
        
        char dirname[32];
        strcpy(dirname, dir->d_name);
        
        int len = strlen(dirname);
        // check whether is a process file
        int flag = 1;
        for (int i = 0; i < len; ++i) {
            if (!isdigit(dirname[i])) {
                flag = 0;
                break;
            }
        }

        if (!flag)
            continue;
        else {
            propid[counter++] = atoi(dirname);
        }
    }

    closedir(d);

    return 0;
}

void test() {
    int state = getProcessfolder();
}

int main() {
    // getProcessfolder();
    printf("hello");
}