#include"Headers.h"
#include"parse.h"
#include"bgJobs.h"
#include"historyQueue.h"
#include"commandExecution.h"
#include"builtinCommand.h"

void change_stdin(char *inFile)
{
    int infileFd = open(inFile,O_RDONLY);
    if(infileFd==-1)
    {
        fprintf(stderr, "%s", "Error in opening file\n");
    }
    dup2(infileFd,STDIN_FILENO);   
    close(infileFd);
}

void change_stdout(char *outFile)
{
    int outfileFd = open(outFile,O_WRONLY|O_CREAT,0777);
    if(outfileFd==-1)
    {
        fprintf(stderr, "%s", "Error in opening file\n");
    }
    dup2(outfileFd,STDOUT_FILENO);   
    close(outfileFd);
}

void execute_single_command(struct commandType *com)
{
    if((com==NULL) || (com->command==NULL)) 
    {
        return ;
    }

    int status = execvp(com->command,com->VarList);
    
    if(status==-1)
    {
        fprintf(stderr, "%s [command(%s)]\n", "Error in executing child process",com->command);
    }
    exit(-1);    
    
}

void execute_complete_command(struct parseInfo *info,struct bg_jobs_info *bg_jobs,struct history_queue* hq,pid_t *current_process_id)
{
    struct commandType *com=&info->CommArray[0];
    
    if(info->pipeNum==0)
    {
        int type=isBuiltInCommand(com->command);
        if(type)
        {
            char command[100]="";
            strncat(command,info->CommArray[0].command,strlen(info->CommArray[0].command));
            for(int j=1;j<info->CommArray[0].VarNum;j++)
            {
                strncat(command," ",2);
                strncat(command,info->CommArray[0].VarList[j],strlen(info->CommArray[0].VarList[j]));
            }
            if((hq->count==0 || (strcmp(hq->queue[(hq->rear+hq->size-1)%hq->size],command)!=0)) && (type!=RERUN))
            {
                if(queue_size(hq)<hq->size)
                {
                    push_queue(hq,command);
                }
                else
                {
                    pop_queue(hq);
                    push_queue(hq,command);
                }
            }
            handle_builtInCommand(com,bg_jobs,hq,type,current_process_id);

            return ;
        }

        pid_t pid = fork();

        if(pid<0)
        {
            fprintf(stderr, "%s", "Error in creating child process\n");
            return ;
        }
        if(pid==0)
        {
            if(info->boolInfile)
            {
                change_stdin(info->inFile);
            }
            if(info->boolOutfile)
            {
                change_stdout(info->outFile);
            }
            execute_single_command(com);
        }
        else
        {
            char command[100]="";
            strncat(command,info->CommArray[0].command,strlen(info->CommArray[0].command));
            for(int j=1;j<info->CommArray[0].VarNum;j++)
            {
                strncat(command," ",2);
                strncat(command,info->CommArray[0].VarList[j],strlen(info->CommArray[0].VarList[j]));
            }
            if(!(hq->count && (strcmp(hq->queue[(hq->rear+hq->size-1)%hq->size],command))==0))
            {
                if(queue_size(hq)<hq->size)
                {
                    push_queue(hq,command);
                }
                else
                {
                    pop_queue(hq);
                    push_queue(hq,command);
                }
            }
            if(info->boolBackground)
            {
                int pos=add_bg_job(bg_jobs,pid,command);
                printf("[%d] %d\n",pos+1,pid);
            }
            else
            {
                *current_process_id=pid;
                int status=0;
                wait(&status);
            }
        }
    }
    else
    {
        pid_t topLevelPid=fork();
        if(topLevelPid<0)
        {
            fprintf(stderr, "%s", "Error in creating child process\n");
            return ;
        } 
        if(topLevelPid==0)
        {

            int noOfCommands = info->pipeNum+1;
            int stdin=dup(STDIN_FILENO);
            int stdout=dup(STDOUT_FILENO);

            int fd[info->pipeNum][2];
            for(int i=0;i<info->pipeNum;i++)
            {
                if(pipe(fd[i])==-1)
                {
                    fprintf(stderr, "%s", "Error in creating pipe\n");
                }
            }

            // For first command
            pid_t pid = fork();
            if(pid<0)
            {
                fprintf(stderr, "%s", "Error in creating child process\n");
                return ;
            }   
            if(pid==0)
            {
                if(info->boolInfile)
                {
                    change_stdin(info->inFile);
                }
                dup2(fd[0][1],STDOUT_FILENO);
                if(isBuiltInCommand(info->CommArray[0].command))
                {
                    handle_builtInCommand(&info->CommArray[0],bg_jobs,hq,isBuiltInCommand(info->CommArray[0].command),current_process_id);
                    exit(1);
                }
                execute_single_command(&info->CommArray[0]);
            }
            else
            {
                int status=0;
                wait(&status);  
                close(fd[0][1]);
            }

            // For command between first and last
            for(int currentCommand=1;currentCommand<noOfCommands-1;currentCommand++)
            {
                int pipeNo=currentCommand-(currentCommand%2);
                struct commandType *com=&info->CommArray[currentCommand];

                pid_t pid = fork();
                if(pid<0)
                {
                    fprintf(stderr, "%s", "Error in creating child process\n");
                    return ;
                }
                
                if(pid==0)
                {
                    dup2(fd[pipeNo][0],STDIN_FILENO);
                    dup2(fd[pipeNo+1][1],STDOUT_FILENO);
                    if(isBuiltInCommand(com->command))
                    {
                        handle_builtInCommand(com,bg_jobs,hq,isBuiltInCommand(com->command),current_process_id);
                        return ;
                    }
                    execute_single_command(com);
                }
                else
                {
                    int status=0;
                    wait(&status);
                    close(fd[pipeNo+1][1]);
                    close(fd[pipeNo][0]);

                }
            }

            // For last command
            if(info->boolOutfile)
            {
                change_stdout(info->outFile);
            }
            dup2(fd[info->pipeNum-1][0],STDIN_FILENO);
            if(isBuiltInCommand(info->CommArray[info->pipeNum].command))
            {
                handle_builtInCommand(&info->CommArray[info->pipeNum],bg_jobs,hq,isBuiltInCommand(info->CommArray[info->pipeNum].command),current_process_id);
                exit(1);
            }
            execute_single_command(&info->CommArray[info->pipeNum]);
        }
        else
        {
            char command[100]="";
            for(int i=0;i<=info->pipeNum;i++)
            {
                strncat(command,info->CommArray[i].command,strlen(info->CommArray[i].command));
                for(int j=1;j<info->CommArray[i].VarNum;j++)
                {
                    strncat(command," ",2);
                    strncat(command,info->CommArray[i].VarList[j],strlen(info->CommArray[i].VarList[j]));
                }
                if(i!=info->pipeNum)
                strncat(command," | ",4);
            }
            if(!(hq->count && (strcmp(hq->queue[(hq->rear+hq->size-1)%hq->size],command))==0))
            {
                if(queue_size(hq)<hq->size)
                {
                    push_queue(hq,command);
                }
                else
                {
                    pop_queue(hq);
                    push_queue(hq,command);
                }
            }
            if(info->boolBackground)
            {
                int pos=add_bg_job(bg_jobs,topLevelPid,command);
                printf("[%d] %d\n",pos+1,topLevelPid);
            }
            else
            {
                *current_process_id=topLevelPid;
                int status=0;
                wait(&status);
            }
        }
    }
}