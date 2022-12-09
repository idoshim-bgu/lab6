#include "linux/limits.h"
#include <stdio.h>
#include <unistd.h>
#include "LineParser.h"
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int debug = 0;

void execute(cmdLine *pCmdLine){
    char* command;
    char* const *args;
    int status;
    
    command = pCmdLine->arguments[0];
    args = &(pCmdLine->arguments[1]);
    if (!strcmp("cd", command)){
        if (chdir(args[0]) == -1)
            fprintf(stderr, "failed to cd into %s", args[0]);
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
        else
            if (pCmdLine->blocking)
                waitpid(pid, &status, 0);
    }
    freeCmdLines(pCmdLine);
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
    
    
    return 0;
}

