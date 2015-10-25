#ifndef CLM_FILE_H_
#define CLM_FILE_H_

const char *clm_file_get_contents(char *fileName);
long clm_file_get_length(char *fileName);
int clm_file_get_num_lines(char *fileName);
int clm_file_append_line(char *fileName,const char *line);
int clm_file_clear(char *fileName);

#endif