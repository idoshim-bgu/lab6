#include "linux/limits.h"
#include <stdio.h>
#include <unistd.h>
#include "LineParser.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char path[PATH_MAX];
    char buffer[2048];
    cmdLine* parsedLine;
    char* command;
    char* const *args;
    int retStatus;
    int shouldExit = 0;

    while (!shouldExit)
    {
        getcwd(path, sizeof(path));
        printf("%s: ",path);
        fgets(buffer, 2048, stdin);
        if (strncmp("quit", buffer, 4) != 0){
            parsedLine = parseCmdLines(buffer);
            command = parsedLine->arguments[0];
            args = &(parsedLine->arguments[1]);
            retStatus = execvp(command, args);
            freeCmdLines(parsedLine);
            free(command);
            if (retStatus == -1){
                perror("Error:");
                exit(1);
            }
        } else
            shouldExit = 1;

    }
    
    return 0;
}

void execute(cmdLine *pCmdLine){

}