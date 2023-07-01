#include"Headers.h"
#include "parse.h"



void init_info(struct parseInfo *p) 
{
	p->boolInfile=0;
	p->boolOutfile=0;
	p->boolBackground=0;
	p->pipeNum=0;
	p->inFile[0]='\0';
	p->outFile[0]='\0';
}

void parse_command(char * command, struct commandType *comm) 
{
	comm->VarNum=0;
	int command_index=0,n=strlen(command);
	while(command_index<n && command[command_index]!='\0')
	{
		while(command_index<n && command[command_index]==' ')
		{
			command_index++;
		}
		if(command_index>=n)
		break;
		comm->VarList[comm->VarNum]=(char *)malloc(sizeof(char)*81);
		int varlist_index=0;

		while(command[command_index]!='\0' && command[command_index]!=' ')
		{
			comm->VarList[comm->VarNum][varlist_index++]=command[command_index++];
		}

		comm->VarList[comm->VarNum][varlist_index]='\0';
		if(varlist_index)
		comm->VarNum++;
	}
	comm->VarList[comm->VarNum]=NULL;
	comm->command=comm->VarList[0];
}

struct parseInfo *parse(char *cmdline) 
{
	if (strlen(cmdline)==0 || (strlen(cmdline)==1 && (cmdline[0]=='\n' || cmdline[0]=='\r')))
		return NULL;

	struct parseInfo *Result;
	int cmdline_index=0;

	Result = (struct parseInfo *)malloc(sizeof(struct parseInfo));
	if(Result==NULL)
	{
		fprintf(stderr,"Memory allocation failed\n");
		return NULL;
	}
	init_info(Result);
	
	while(cmdline[cmdline_index]!='\0' && cmdline[cmdline_index]!='\n')
	{
		if(cmdline[cmdline_index]=='|')
		{
			Result->pipeNum++;
		}		

		cmdline_index++;
	}
	
	cmdline_index=0;

	if(Result->pipeNum==0)
	{
		char command[MAXLINE];
		int command_index=0;

		while(cmdline[cmdline_index]!='\0' && cmdline[cmdline_index]!='\n')
		{
			if(cmdline[cmdline_index]=='>')
			{
				Result->boolOutfile=1;
				cmdline_index++;
				int outfile_index=0;

				while(cmdline[cmdline_index]==' ')
				{
					cmdline_index++;
				}


				while(cmdline[cmdline_index]!='\0' && cmdline[cmdline_index]!=' ' && cmdline[cmdline_index]!='\n')
				{
					Result->outFile[outfile_index++]=cmdline[cmdline_index++];
				}

				Result->outFile[outfile_index]='\0';
				command[command_index++]=' ';
			}
			else if(cmdline[cmdline_index]=='<')
			{
				Result->boolInfile=1;
				cmdline_index++;
				int infile_index=0;

				while(cmdline[cmdline_index]==' ')
				{
					cmdline_index++;
				}

				while(cmdline[cmdline_index]!='\0' && cmdline[cmdline_index]!=' ' && cmdline[cmdline_index]!='\n')
				{
					Result->inFile[infile_index++]=cmdline[cmdline_index++];
				}

				Result->inFile[infile_index]='\0';
				command[command_index++]=' ';
			}
			else if(cmdline[cmdline_index]=='&')
			{
				Result->boolBackground=1;
				break;
			}
			else
			{
				command[command_index++]=cmdline[cmdline_index++];
			}
		}

		command[command_index]='\0';
		parse_command(command,&Result->CommArray[Result->pipeNum]);
	}
	else
	{
		char command[MAXLINE];
		int command_index=0;
		int count_pipeNum=0;

		/* For the first command before pipe */
		while(cmdline[cmdline_index]!='|')
		{
			if(cmdline[cmdline_index]=='<')
			{
				Result->boolInfile=1;
				cmdline_index++;
				int infile_index=0;

				while(cmdline[cmdline_index]==' ')
				{
					cmdline_index++;
				}

				while(cmdline[cmdline_index]!='|' && cmdline[cmdline_index]!=' ')
				{
					Result->inFile[infile_index++]=cmdline[cmdline_index++];
				}

				Result->inFile[infile_index]='\0';
				command[command_index++]=' ';
			}
			else
			{
				command[command_index++]=cmdline[cmdline_index++];
			}
		}

		command[command_index]='\0';
		parse_command(command,&Result->CommArray[0]);
		count_pipeNum++;
		cmdline_index++;
		
		/* For all the commands before first and last pipe*/
		while(count_pipeNum<Result->pipeNum)
		{
			command_index=0;
			while(cmdline[cmdline_index]!='|')
			{
				command[command_index++]=cmdline[cmdline_index++];
			}

			command[command_index]='\0';
			parse_command(command,&Result->CommArray[count_pipeNum]);
			count_pipeNum++;
			cmdline_index++;
		}

		command_index=0;

		/* For the last command */
		while(cmdline[cmdline_index]!='\0' && cmdline[cmdline_index]!='\n')
		{
			if(cmdline[cmdline_index]=='>')
			{
				Result->boolOutfile=1;
				cmdline_index++;
				int outfile_index=0;

				while(cmdline[cmdline_index]==' ')
				{
					cmdline_index++;
				}


				while(cmdline[cmdline_index]!='\0' && cmdline[cmdline_index]!=' ' && cmdline[cmdline_index]!='\n')
				{
					Result->outFile[outfile_index++]=cmdline[cmdline_index++];
				}

				Result->outFile[outfile_index]='\0';
				command[command_index++]=' ';
			}
			else if(cmdline[cmdline_index]=='&')
			{
				Result->boolBackground=1;
				break;
			}
			else
			{
				command[command_index++]=cmdline[cmdline_index++];
			}
		}

		command[command_index]='\0';
		parse_command(command,&Result->CommArray[Result->pipeNum]);
	}

	return Result;
}

void print_info(struct parseInfo *info) 
{
	printf("Input file: %s\nOutput file: %s\nNo of pipe: %d\nIs background: %d\n",info->inFile,info->outFile,info->pipeNum,info->boolBackground);
	printf("No of commands: %d\n",info->pipeNum+1);
	for(int i=0;i<=info->pipeNum;i++)
	{
		printf("Command number %d\n",i+1);
		for(int j=0;j<info->CommArray[i].VarNum;j++)
		{
			printf("Arg%d: %s\n",j,info->CommArray[i].VarList[j]);
		}
	}
}
 
void free_info (struct parseInfo *info) 
{
	if(!info)
	return ;
	for(int i=0;i<=info->pipeNum;i++)
	{
		for(int j=0;j<info->CommArray[i].VarNum;j++)
		{
			free(info->CommArray[i].VarList[j]);
		}
	}
	free(info);
}
