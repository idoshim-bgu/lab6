#include "linux/limits.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "LineParser.h"

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0


typedef int pid_t;

typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                  /* next process in chain */
} process;

int debug = 0;
process *processList = NULL;

char* getStatus(int status){
    switch (status)
    {
    case -1:
        return "TERMINATED";
    case 0:
        return "SUSPENDED";
    case 1:
        return "RUNNING";
    default:
        return "invalid status";
    }
}

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
    process *proc = malloc(sizeof(process));

    proc->next = NULL;
    proc->cmd = cmd;
    proc->pid = pid;
    proc->status = RUNNING;

    if (*process_list == NULL)
    {
        *process_list = proc;
        return;
    }

    process *curr = *process_list;
    while (curr->next != NULL)
    {
        curr = curr->next;
    }
    curr->next = proc;
}

void freeProcessList(process* process_list){
    while (process_list != NULL)
    {
        process *curr = process_list;
        process_list = curr->next;
        freeCmdLines(curr->cmd);
        free(curr);
    }
}

void updateProcessStatus(process* process_list, int pid, int status){
    while (process_list != NULL)
    {
        if (process_list->pid == pid){
            process_list->status = status;
            return;
        }
        process_list = process_list->next;
    }
    
}

void updateProcessList(process **process_list){
    process *curr = *process_list;
    while (curr != NULL)
    {
        int status;
        pid_t retPid =  waitpid(curr->pid,&status, WNOHANG | WUNTRACED | WCONTINUED);
            if (retPid == -1 || WIFSIGNALED(status))
                curr->status = TERMINATED;
            else if (WIFCONTINUED(status))
                curr->status = RUNNING;
            else if (WIFSTOPPED(status))
                curr->status = SUSPENDED;
        curr = curr->next;
    }
}

void printProcess(process* proc){
    printf("PID: %d, status: %s, command: %s\n", proc->pid, getStatus(proc->status), proc->cmd->arguments[0]);
}

void printProcessList(process** process_list){
    updateProcessList(process_list);
    process *prev = NULL;
    process *curr = *process_list;
    while (curr != NULL)
    {
       printProcess(curr);
       if (curr->status == TERMINATED)
       {
            if (prev != NULL)
            {
                prev->next = curr->next;
            }else
                *process_list = curr->next;
       }else
        prev = curr;
       
       curr = curr->next;
    }
}

void execute(cmdLine *pCmdLine){
    char* command;
    char* const *args;
    int status;
    
    command = pCmdLine->arguments[0];
    args = &(pCmdLine->arguments[1]);
    if (!strcmp("cd", command)){
        if (chdir(args[0]) == -1)
            fprintf(stderr, "failed to cd into %s", args[0]);
    } else if (!strcmp("procs", command)){
        updateProcessList(&processList);
        printProcessList(&processList);
    }
    else{
        int pid = fork();
        if (debug)
            fprintf(stderr,"PID: %d is executing command: %s\n",pid, command);
        if (pid == 0)
        {
            int retStatus = execvp(command, args);
            if (retStatus == -1)
                perror("Error:");
            _exit(1);
        } else if(pid < 0)
            _exit(1);
        else{
            addProcess(&processList,pCmdLine,pid);
            if (pCmdLine->blocking)
               waitpid(pid, &status, 0);
        }           
    }
}

int main(int argc, char const *argv[])
{
    char path[PATH_MAX];
    char buffer[2048];
    cmdLine* parsedLine;
    int shouldExit = 0;

    for (int i = 1; i < argc; i++)
        if (!strncmp("-d", argv[i], 2))
            debug = 1;
    
    while (!shouldExit)
    {
        getcwd(path, sizeof(path));
        printf("%s: ",path);
        fgets(buffer, 2048, stdin);
        if (strncmp("quit", buffer, 4) != 0){
            parsedLine = parseCmdLines(buffer);
            execute(parsedLine);
        } else
            shouldExit = 1;
    }
    // freeCmdLines(parsedLine);
    freeProcessList(processList);
    
    
    return 0;
}

