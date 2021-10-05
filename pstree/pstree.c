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


/**
 * All CLI options
 */
typedef struct
{
  int show_pids;
  int version;
  int numeric_sort;
} Options;
Options cli;

Process process[MAX_PROC];
int  procpid[MAX_PROC];
int proccnt;

const char *PROC = "/proc";

int _atoi(char * s) {
    int ans = 0;
    int len = strlen(s);
    for (int i = 0; i < len; ++i)
        ans = ans * 10 + s[i] - '0';
    return ans;
}

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
        // if (strncmp(dir->d_name, ".", 1) == 0)
        //     continue;
        char dirname[32]={};

        strncpy(dirname, dir->d_name, strlen(dir->d_name));

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
        else 
            procpid[counter++] = _atoi(dirname);
    }

    closedir(d);

    return counter;
}

int getProcess(int proc_n, int procpid[], Process process[]) {
    
    char buf[1024];
    char path[128];
    int counter = 0;

    for (int i = 0; i < proc_n; ++i) {
        sprintf(path, "%s/%d/stat", PROC, procpid[i]);

        FILE *fp = fopen(path, "r");

        if (fp == NULL) {
            printf("process %d access failed\n", procpid[i]);
            continue;
        }
        
        fgets(buf, sizeof(buf), fp);

        fclose(fp);      

        sscanf(buf, "%d (%s %c %d", &process[counter].pid, process[counter].name, &process[counter].state, &process[counter].ppid);
        process[counter].name[strlen(process[counter].name) - 1] = '\0';

        //printf("%d %s %c %d\n", process[counter].pid, process[counter].name, process[counter].state, process[counter].ppid);
        
        counter++;
    }
    return counter;
}

int cmpbypid(const void *a, const void *b) {
    int _a = *((int *)a), _b =  *((int *)b);
    return process[_a].pid > process[_b].pid;
}

int cmpbyname(const void *a, const void *b) {
    int _a = *((int *)a), _b =  *((int *)b);

    return strcmp(process[_a].name, process[_b].name);
}

int offset;

// print subtree
void print(int root) {

    for (int i = 0; i < offset; ++i)
        printf(" ");
    
    offset += 4;

    if (cli.show_pids == 1) 
        printf("%s(%d)\n", process[root].name, process[root].pid);
    else 
        printf("%s\n", process[root].name);

    int proc[256];
    int counter = 0;

    for (int i = 0; i < proccnt; ++i) {
        if (process[i].ppid == process[root].pid)
            proc[counter++] = i;
    }

    if (cli.numeric_sort == 1)
        qsort(proc, counter, sizeof(int), cmpbypid);
    else qsort(proc, counter, sizeof(int), cmpbyname);

    for (int i = 0; i < counter; ++i)
        print(proc[i]);

    offset -= 4;

}

int main(int argc, char *argv[]) {
    proccnt = getProcessfolder();
    printf("%d\n", proccnt);
    proccnt = getProcess(proccnt, procpid, process);
    printf("%d\n", proccnt);
    
    cli.show_pids = 0;
    cli.version = 0;
    cli.numeric_sort = 0;

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-V") == 0)
            cli.version = 1;
        if (strcmp(argv[i], "-p") == 0)
            cli.show_pids = 1;
        if (strcmp(argv[i], "-n") == 0)
            cli.numeric_sort = 1;
    }


    if (cli.version) 
        printf("pstree 1.0\nCopyright (C) 2021-2021 chagelo\n");
    
    for (int i = 0; i < proccnt; ++i)
        if (process[i].ppid == 0)
            print(i);

}