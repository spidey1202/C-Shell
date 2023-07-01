#include"Headers.h"
#include"historyQueue.h"

struct history_queue* init_queue(int size)
{
    struct history_queue* hq= (struct history_queue*)malloc(sizeof(struct history_queue));
    if(hq==NULL)
    {
        fprintf(stderr,"%s","Memory allocation failed\n");
        return NULL;
    }
    hq->size=size;
    hq->front=-1;
    hq->rear=0;
    hq->queue=(char**)malloc(sizeof(char*)*size);
    if(hq->queue==NULL)
    {
        fprintf(stderr,"%s","Memory allocation failed\n");
        free(hq);
        return NULL;
    }
    for(int i=0;i<hq->size;i++)
    hq->queue[i]=NULL;
    hq->count=0;
    return hq;
}
int push_queue(struct history_queue* hq,char *cmd)
{
    if(hq->front==hq->rear)
    {
        fprintf(stderr,"History queue full\n");
        return 0;
    }
    hq->queue[hq->rear]=(char *)malloc(strlen(cmd)+1);
    strcpy(hq->queue[hq->rear],cmd);
    hq->queue[hq->rear][strlen(cmd)]='\0';
    hq->rear=(hq->rear+1)%hq->size;
    if(hq->front==-1)
    hq->front++;
    hq->count++;
    return 1;
}
int pop_queue(struct history_queue*hq)
{
    if(hq->front==-1)
    {
        fprintf(stderr,"History queue empty\n");
        return 0;
    }
    free(hq->queue[hq->front]);
    hq->queue[hq->front]=NULL;
    hq->front=(hq->front+1)%hq->size;
    if(hq->front==hq->rear)
    {
        hq->front=-1;
        hq->rear=0;
    }
    hq->count--;
    return 1;
}
int queue_size(struct history_queue*hq)
{
    return hq->count;
}
char* history_cmd_from_index(struct history_queue* hq,int index)
{
    if(index>hq->count || index<1)
    {
        fprintf(stderr,"No command in history with this %d index",index);
        return NULL;
    }
    index--;
    return hq->queue[(index+hq->front)%hq->size];
}
void print_queue(struct history_queue*hq)
{
    if(hq->front==-1)
    {
        printf("Empty history queue\n");
        return ;
    }
    int index =hq->front,temp=0;
    while(temp<hq->count)
    {
        temp++;
        printf("%d %s\n",temp,hq->queue[index]);
        index=(index+1)%hq->size;
    }
}
void free_queue(struct history_queue* hq)
{
    if(hq->front==-1)
    {
        free(hq->queue);
        free(hq);
        return ;
    }
    for(int index=0;index<hq->size;index++)
    {
        if(hq->queue[index]!=NULL)
        free(hq->queue[index]);
    }
    free(hq->queue);
    free(hq);
}