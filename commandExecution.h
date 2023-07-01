#ifndef __COMMANDEXECUTION_H_
#define __COMMANDEXECUTION_H_

void change_stdin(char *);
void change_stdout(char *);
void execute_single_command(struct commandType *);
void execute_complete_command(struct parseInfo *,struct bg_jobs_info*,struct history_queue*,pid_t*);

#endif