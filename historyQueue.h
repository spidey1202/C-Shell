#ifndef __HISTORY_QUEUE_H_
#define __HISTORY_QUEUE_H_


struct history_queue
{
    char **queue;
    int front;
    int rear;
    int size;
    int count;
};

struct history_queue* init_queue(int);
int push_queue(struct history_queue*,char *);
int pop_queue(struct history_queue*);
char* history_cmd_from_index(struct history_queue*,int);
void print_queue(struct history_queue*);
void free_queue(struct history_queue*);
int queue_size(struct history_queue*);

#endif