#ifndef __BGJOBS_H_
#define __BGJOBS_H_

struct bg_jobs_info
{
    int size;
    int* jobs;
    char **commands;
};

struct bg_jobs_info* init_bg_jobs(int);
int count_bg_jobs(struct bg_jobs_info*);
int add_bg_job(struct bg_jobs_info*,int,char *);
int get_pid_of_bg_jobs(struct bg_jobs_info*,int);
int remove_bg_job(struct bg_jobs_info*,int);
int bg_jobs_status(struct bg_jobs_info*,int);
void free_bg_jobs_info(struct bg_jobs_info*);
#endif