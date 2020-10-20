#include  <cstdio>
#include <cstring>
#include <cstdlib>
#include <zconf.h>
#include <iostream>
#include <map>

using namespace std;

bool isBackgroundModeWanted = false;
int backTaskID[10000000] = { 0 };

map<string, string> aliasMap;

void  parseInput(char *inputLine, char **args)
{
    bool commentFound = false;
    while (*inputLine != '\0') {
        if (*inputLine == '$') {
            //cout << "cfound1" << endl;
            commentFound = true;
        }
        while (*inputLine == ' ' || *inputLine == '\t' || *inputLine == '\n') {
            if (*inputLine == '$') {
                //cout << "cfound2" << endl;
                commentFound = true;
            }
            *inputLine++ = '\0';
        }
        if (*inputLine != '$' && !commentFound) {
            if (isBackgroundModeWanted && *inputLine == '-') {

            } else {
                *args++ = inputLine;
                //cout << "adding:" << inputLine << endl;
            }
        }
        while (*inputLine != '\0' && *inputLine != ' ' &&
               *inputLine != '\t' && *inputLine != '\n') {
            if (*inputLine == '$') {
                //cout << "cfound3" << endl;
                commentFound = true;
            }
            inputLine++;
        }

    }
    *args = NULL;
}

void  execute(char **args)
{
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {
        cout << "ERR: failed to create child" << endl;
        exit(1);
    }
    else if (pid == 0) {
        if (execvp(*args, args) < 0) {
            cout << "ERR: failed to execute command" << endl;
            exit(1);
        }
    }
    else {
        if (isBackgroundModeWanted) {
            isBackgroundModeWanted = false;
            backTaskID[pid] = 3;
            while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
            {
                cout << "exec done" << endl;
                backTaskID[pid] = 4;
            }
            cout << "DOME" << endl;
        } else {
            while (wait(&status) != pid){
                //cout << "STAT:" << wait(&status) << endl;
            }
            cout << "DONE" << endl;
        }
    }
}

int main()
{

    int count = 1;
    string shellName = "ToyShell";
    string terminator = "stop";
    while (1) {
        char  inputLine[1024] = {'\0'};
        char  *inputArguments[64];
        isBackgroundModeWanted = false;

        cout << shellName << "[" << count << "]: ";
        gets(inputLine);
        if (*inputLine == '\0' || *inputLine == '$' || *inputLine == ' ') {
            continue;
        }
        for (int i=0;i<1023;i++) {
            if (inputLine[i] == '-' && inputLine[i+1] == '\0') {
                isBackgroundModeWanted = true;
                break;
            }
        }
        cout << endl;
        parseInput(inputLine, inputArguments);
        char cmdCA[terminator.size()+1];
        strcpy(cmdCA, terminator.c_str());

        cout << "input:" << inputArguments[0] << endl;
        if (!aliasMap[inputArguments[0]].empty()) {
            cout << "not empty"  << endl;
            inputArguments[0] = const_cast<char *>(aliasMap[inputArguments[0]].c_str());
            cout << "input changed:" << inputArguments[0] << endl;
        } else {
            cout << "alias not found" << endl;
        }

        if (strcmp(inputArguments[0], cmdCA) == 0) {
            exit(0);
        } else if (strcmp(inputArguments[0], "newname") == 0) {
            if (inputArguments[2] == NULL) {
                aliasMap.erase(inputArguments[1]);
                cout << "Alias removed" << endl;
            } else {
                aliasMap[inputArguments[1]] = inputArguments[2];
                cout << "Alias added" << endl;
            }
        } else if (strcmp(inputArguments[0], "setshellname") == 0) {
            shellName = inputArguments[1];
            cout << "Shell name updated" << endl;
        } else if (strcmp(inputArguments[0], "setterminator") == 0) {
            terminator = inputArguments[1];
            cout << "Terminator updated" << endl;
        } else if (strcmp(inputArguments[0], "killall") == 0) {
            for (int & i : backTaskID) {
                i = 0;
            }
            cout << "Stopped and cleared all background jobs" << endl;
        } else if(strcmp(inputArguments[0], "backjobs") == 0) {
            count++;
            bool hasOne = false;
            for (int i=0; i<10000000; i++) {
                if (backTaskID[i] == 3) {
                    cout << "pid: " << i << "\tstatus: running" << endl;
                    hasOne = true;
                } else if (backTaskID[i] == 4) {
                    cout << "pid: " << i << "\tstatus: done" << endl;
                    hasOne = true;
                }
            }
            if (!hasOne) {
                cout << "No background task found!" << endl;
            }
        } else if(strcmp(inputArguments[0], "frontjob") == 0) {
            if (inputArguments[1] != NULL) {
                cout << "Process " << inputArguments[1] << " moved to foreground" << endl;
                inputArguments[0] = "sleep";
                inputArguments[1] = "5";
                inputArguments[2] = NULL;
                execute(inputArguments);
                count++;
            } else {
                cout << "Please enter job id" << endl;
            }
        } else {
            execute(inputArguments);
            count++;
        }
    }
    return 0;
}