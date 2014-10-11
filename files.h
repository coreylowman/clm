#ifndef FILES_H
#define FILES_H

char *getFileContents(char *fileName);
long getFileLength(char *fileName);
int getFileLines(char *fileName);
int writeLine(char *fileName,const char *line);
int clearFile(char *fileName);

#endif