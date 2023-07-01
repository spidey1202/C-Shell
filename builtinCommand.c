#include"Headers.h"
#include"parse.h"
#include"bgJobs.h"
#include"historyQueue.h"
#include"builtinCommand.h"
#include"commandExecution.h"

int isBuiltInCommand(char * cmd)
{
  
    if(strncmp(cmd,"exit",strlen("exit"))==0)
    {
        return EXIT;
    }
    if(strncmp(cmd,"cd",strlen("cd"))==0)
    {
        return CD;
    }
    if(strncmp(cmd,"jobs",strlen("jobs"))==0)
    {
        return JOBS;
    }
    if(strncmp(cmd,"kill",strlen("kill"))==0)
    {
        return KILL;
    }
    if(strncmp(cmd,"history",strlen("history"))==0)
    {
        return HISTORY;
    }
    if(strncmp(cmd,"help",strlen("help"))==0)
    {
        return HELP;
    }
    if(cmd[0]=='!')
    {
        return RERUN;
    }
    return NO_SUCH_BUILTIN;
}

void handle_cd(struct commandType *com)
{
    if(com->VarNum>2)
    {
        fprintf(stderr, "%s", "Too many arguments\n");
        return ;
    }
    
    char path[PATH_MAX];
    if(getcwd(path,PATH_MAX)==NULL)
    {
        fprintf(stderr, "%s", "Error in getting current directory\n");
        return ;
    }

    if(com->VarNum==1)
    {
        if(chdir(getenv("HOME"))==-1)
        {
            fprintf(stderr, "%s", "Cannot change directory\n");
        }
    }
    else
    {
        if(chdir(com->VarList[1])==-1)
        {
            fprintf(stderr, "%s", "Cannot change directory\n");
        }
    }
}
void handle_jobs(int argc,struct bg_jobs_info *bg_jobs)
{
    if(argc>1)
    {
        fprintf(stderr, "%s", "Too many arguments\n");
        return ;
    }
    bg_jobs_status(bg_jobs,1);
}

void handle_kill(struct commandType *com,struct bg_jobs_info* bg_jobs)
{  
    if(com->VarNum==1)
    {   
        fprintf(stderr, "%s", "Too few arguments\n");
        return ;
    }
    if(com->VarNum>2)
    {   
        fprintf(stderr, "%s", "Too many arguments\n");
        return ;
    }
    int pid;
    if(com->VarList[1][0]=='%')
    {
        int index=atoi(com->VarList[1]+1);
        pid=get_pid_of_bg_jobs(bg_jobs,index);
        if(pid==-1)
        {
            fprintf(stderr,"No background job corresponding to [%d]\n",index);
            return ;
        }
    }
    else
    {
        pid=atoi(com->VarList[1]);
    }
    int status=kill(pid,SIGKILL);
    if(status==-1)
    {
        fprintf(stderr, "Not able to kill process with pid: %d\n",pid);
    }
}
void handle_exit(struct bg_jobs_info* bg_jobs)
{
    if(count_bg_jobs(bg_jobs))
    {
        printf("Unfinished background jobs\n");
        handle_jobs(1,bg_jobs);
    }
    else
        exit(1);
}
void handle_history(struct history_queue* hq)
{
    print_queue(hq);
}

void handle_rerun(struct commandType *com,struct bg_jobs_info*bg_jobs,struct history_queue*hq,pid_t* current_process_id)
{
    if(com->VarNum>1)
    {
        fprintf(stderr,"Too many arguments for rerun\n");
        return ;
    }
    int index=atoi(com->command+1);
    if(index==0)
    {
        fprintf(stderr,"No command in history with this %d index\n",index);
        return ;
    }
    if(index<0)
    {
        index=(hq->count-abs(index)+1);
    }
    char* command=history_cmd_from_index(hq,index);
    if(command==NULL)
    {
        return ;
    }
    char nwcommand[100]="";
    strcpy(nwcommand,command);
    struct parseInfo* nwInfo= parse(nwcommand);
    if(nwInfo==NULL)
    {
        return ;
    }
    execute_complete_command(nwInfo,bg_jobs,hq,current_process_id);
    free_info(nwInfo);
}

void handle_help(struct commandType *com)
{
    if(com->VarNum>1)
    {
        fprintf(stderr,"Too many arguments for help\n");
        return ;
    }
    printf("\nBuiltin commands\n");
    printf("\tcd [absolute or relative path]\n");
    printf("\tjobs\n");
    printf("\tkill [ %%num | pid ] (num refers to process number in jobs, pid refers to process id )\n");
    printf("\thistory\n");
    printf("\t![num] ( num refers to the command numbered num in the list of command returned by history )\n");
    printf("\texit\n");
    printf("Command format\n");
    printf("\tcommand1 [< infile] [| command]* [> outfile] [&]\n\n");
}

void handle_builtInCommand(struct commandType *com,struct bg_jobs_info* bg_jobs,struct history_queue* hq,int type,int *pid) 
{
    switch(type)
    {
        case EXIT:
            handle_exit(bg_jobs);
            break;
        case CD:
            handle_cd(com);
            break;
        case JOBS:
            handle_jobs(com->VarNum,bg_jobs);
            break;
        case KILL:
            handle_kill(com,bg_jobs);
            break;
        case HISTORY:
            handle_history(hq);
            break;
        case RERUN:
            handle_rerun(com,bg_jobs,hq,pid);
            break;
        case HELP:
            handle_help(com);
            break;
    }

}