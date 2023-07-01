#ifndef __PARSE_H_
#define __PARSE_H_

#define MAX_VAR_NUM 10
#define PIPE_MAX_NUM 10
#define FILE_MAX_SIZE 40
#define MAXLINE 81

struct commandType 
{
  char *command;
  char *VarList[MAX_VAR_NUM];
  int VarNum;
};


struct parseInfo
{
  int   boolInfile;		      
  int   boolOutfile;		       
  int   boolBackground;		      
  struct commandType CommArray[PIPE_MAX_NUM];
  int   pipeNum;
  char  inFile[FILE_MAX_SIZE];	      
  char  outFile[FILE_MAX_SIZE];	    
};

struct parseInfo *parse(char *);
void free_info(struct parseInfo *);
void print_info(struct parseInfo *);



#endif







