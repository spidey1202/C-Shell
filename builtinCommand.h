#ifndef __BUITLINCOMMAND_H_
#define __BUITLINCOMMAND_H_

enum BUILTIN_COMMANDS { NO_SUCH_BUILTIN=0, EXIT,CD,JOBS,KILL,HISTORY,RERUN,HELP};
void handle_builtInCommand(struct commandType *,struct bg_jobs_info*,struct history_queue*,int,int*);
void handle_cd(struct commandType *);
void handle_jobs(int,struct bg_jobs_info*);
int isBuiltInCommand(char *);
void handle_kill(struct commandType *,struct bg_jobs_info*);
void handle_exit(struct bg_jobs_info*);
void handle_rerun(struct commandType *,struct bg_jobs_info*,struct history_queue*,pid_t*);
void handle_help(struct commandType *);
#endif
