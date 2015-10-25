#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util/clm_file.h"

const char *clm_file_get_contents(char *fileName){
    FILE *file = fopen(fileName,"r");
    if(!file) return NULL;
    char * buffer;
    long size;    
    fseek(file,0L,SEEK_END);
    size = ftell(file);
    rewind(file);
    buffer = calloc(1, size*sizeof(*buffer) + 1);
    if(!buffer) printf("unable to allocate memory"),exit(1);
    fread(buffer,sizeof(char),size,file);
    fclose(file);
    return buffer;
}

long clm_file_get_length(char *fileName){
    FILE *file = fopen(fileName,"r");
    if(!file) return -1;
    fseek(file,0L,SEEK_END);
    return ftell(file);
}

int clm_file_get_num_lines(char *fileName){
    FILE *file = fopen(fileName,"r");
    char c;
    long size;
    int count = 0;
    if(!file) return -1;
    fseek(file,0L,SEEK_END);
    size = ftell(file);
    rewind(file);
    do{
        c = fgetc(file);
        if(c == '\n') count++;
    }while(c != EOF);
    fclose(file);
    return count;
}

int clm_file_append_line(char *fileName,const char *line){
    FILE *file = fopen(fileName,"a");
    if(!file) return -1;
    fputs(line,file);
    fclose(file);
    return 0;
}

int clm_file_clear(char *fileName){
    FILE *file = fopen(fileName,"w");
    fclose(file);
    return 0;
}