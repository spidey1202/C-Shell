#include"Headers.h"
#include"bgJobs.h"

struct  bg_jobs_info* init_bg_jobs(int size)
{
    struct  bg_jobs_info* bg_jobs = (struct bg_jobs_info*)malloc(sizeof(struct bg_jobs_info));
    if(bg_jobs==NULL)
    {
        fprintf(stderr,"%s","Memory allocation failed\n");
        return NULL;
    }
    bg_jobs->jobs=(int*)malloc(sizeof(int)*size);
    if(bg_jobs->jobs==NULL)
    {
        fprintf(stderr,"%s","Memory allocation failed\n");
        free(bg_jobs);
        return NULL;
    }
    bg_jobs->commands=(char**)malloc(sizeof(char*)*size);
    if(bg_jobs->commands==NULL)
    {
        fprintf(stderr,"%s","Memory allocation failed\n");
        free(bg_jobs->jobs);
        free(bg_jobs);
        return NULL;
    }
    bg_jobs->size=size;
    for(int i=0;i<bg_jobs->size;i++)
    bg_jobs->jobs[i]=-1;
    return bg_jobs;
}

int count_bg_jobs(struct bg_jobs_info* bg_jobs)
{
    int count=0;
    for(int i=0;i<bg_jobs->size;i++)
    {
        if(bg_jobs->jobs[i]!=-1)
        count++;
    }
    return count;
}

int get_pid_of_bg_jobs(struct bg_jobs_info* bg_jobs,int index)
{
    if(index<1 || index>bg_jobs->size)
        return -1;
    return bg_jobs->jobs[index-1];
}
int add_bg_job(struct bg_jobs_info* bg_jobs,int pid,char *cmd)
{
    int index=-1;
    for(int i=bg_jobs->size-1;i>=0;i--)
    {
        if(bg_jobs->jobs[i]==-1)
        {
            index=i;
        }
    }
    if(index==-1)
    {
        fprintf(stderr,"%s","No slot available for background jobs\n");
        return -1;
    }
    bg_jobs->jobs[index]=pid;
    bg_jobs->commands[index]=(char*)malloc(sizeof(char)*(strlen(cmd)+1));
    strcpy(bg_jobs->commands[index],cmd);
    bg_jobs->commands[strlen(cmd)]='\0';
    return index;
}

int remove_bg_job(struct bg_jobs_info* bg_jobs,int pid)
{
    int index=-1;
    for(int i=0;i<bg_jobs->size;i++)
    {
        if(bg_jobs->jobs[i]==pid)
        {
            index=i;
            break;
        }
    }
    if(index==-1)
    {
        fprintf(stderr,"%s %d","No background job with this",pid);
        return -1;
    }
    free(bg_jobs->commands[index]);
    bg_jobs->jobs[index]=-1;
    return index;
}

int bg_jobs_status(struct bg_jobs_info* bg_jobs,int show_running)
{
    int flag=0;
    for(int i=0;i<bg_jobs->size;i++)
    {
        if(bg_jobs->jobs[i]!=-1)
        {
            int status;
            pid_t pid=bg_jobs->jobs[i];
            if((pid = waitpid(pid, &status,WNOHANG))==-1)
            {
                bg_jobs->jobs[i]=-1;
            }
            else if(pid==0)
            {
                if(show_running)
                printf("Background job [%d] \t Running \t Command: %s\n",i+1,bg_jobs->commands[i]);
            }
            else if(pid != 0) 
            {
                flag=1;
                if (WIFEXITED(status))
                {
                    printf("Background job [%d] \t Done \t Command: %s\n",i+1,bg_jobs->commands[i]);
                }
                else 
                fprintf(stderr,"Background job [%d] \t Terminated \t Command: %s\n",i+1,bg_jobs->commands[i]);
                bg_jobs->jobs[i]=-1;
                free(bg_jobs->commands[i]);
            }
        }
    }
    fflush(stdout);
    return flag;
}

void free_bg_jobs_info(struct bg_jobs_info* bg_jobs)
{
    for(int i=0;i<bg_jobs->size;i++)
    {
        if(bg_jobs->jobs[i]!=-1)
        free(bg_jobs->commands[i]);
    }
    free(bg_jobs->commands);
    free(bg_jobs->jobs);
    free(bg_jobs);
}

