#include"Headers.h"
#include"parse.h"
#include"bgJobs.h"
#include"historyQueue.h"
#include"commandExecution.h"
#include"builtinCommand.h"

#define SHELL_PREFIX "Shell:"
#define MAX_BG_PROCESS 1000
#define HISTORY_QUEUE_SIZE 10

struct bg_jobs_info* bg_jobs=NULL;
pid_t current_process_id=-1;
char * cmdLine=NULL;
struct history_queue* hq=NULL;

void build_prompt()
{
    char path[PATH_MAX+8];
    strcpy(path,SHELL_PREFIX);
    if(getcwd(path+strlen(SHELL_PREFIX),PATH_MAX)==NULL)
    {
        fprintf(stderr, "%s", "Error in getting current directory\n");
        return ;
    }
    strcat(path,"$ ");
    printf("%s",path);
}

void handle_current_process_kill()
{
    printf("\n");
    if(current_process_id!=-1)
    {
        kill(current_process_id,SIGINT);
        current_process_id=-1;
    }
    else
    {
        build_prompt();
        fflush(stdout);
    }
}
int handle_safe_exit()
{
    if(strncmp(cmdLine,"exit",strlen("exit"))==0)
    {
        if(count_bg_jobs(bg_jobs))
        {
            printf("Unfinished background jobs\n");
            handle_jobs(1,bg_jobs);
            free(cmdLine);
            return 1;
        }
        free(cmdLine);
        free_bg_jobs_info(bg_jobs); 
        free_queue(hq);
        exit(1);
    }
    return 0;
}

void run_shell()
{
    while(1)
    {
        fflush(stdout);
        current_process_id=-1;
        cmdLine=(char *)malloc(sizeof(char)*MAXLINE);
        if(cmdLine==NULL)
        {
            fprintf(stderr,"%s","Memory allocation failed\n");
            continue;
        }

        build_prompt();
        size_t cmdLineSize=81;
        int cmdsize=getline(&cmdLine,&cmdLineSize,stdin);
        
        // Handles ctrl+D
        if (cmdLine == NULL || cmdsize==-1) 
        {
            printf("\n");
            free(cmdLine);
            free_bg_jobs_info(bg_jobs); 
            free_queue(hq);
            exit(1);
        }
        if(cmdsize>MAXLINE)
        {
            fprintf(stderr,"%s","Command length exceeds 80 character\n");
            free(cmdLine);
            bg_jobs_status(bg_jobs,0);
            continue;
        }
        
        if(handle_safe_exit())
            continue;

        struct parseInfo *info = parse(cmdLine);
        if (info == NULL)
        {
            free(cmdLine);
            bg_jobs_status(bg_jobs,0);
            continue;
        }
        
        execute_complete_command(info,bg_jobs,hq,&current_process_id);

        free(cmdLine);
        free_info(info);
        bg_jobs_status(bg_jobs,0);
        fflush(stdout);
    }
    free_bg_jobs_info(bg_jobs); 
    free_queue(hq);
}


int main ()
{
    bg_jobs=init_bg_jobs(MAX_BG_PROCESS);
    if(bg_jobs==NULL)
    {
        exit(-1);
    }
    hq=init_queue(HISTORY_QUEUE_SIZE);
    if(hq==NULL)
    {
        exit(-1);
    }
    signal(SIGINT,handle_current_process_kill);

    run_shell();
}
  





