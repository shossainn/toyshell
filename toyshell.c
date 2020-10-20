#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>

char **get_input(char *);
int cd(char *);

int main() {
    char **command;
    char *input;
    pid_t child_pid;
    int stat_loc;
    int num_of_commands = 1;
    char command_promt[50] = "toyshell";
    char shell_name[50];
    strcpy(shell_name, command_promt);
    char terminator[5] = ">";
    char command_history[10][50];
    for (int i = 0; i < 10; ++i) {
        strcpy(command_history[i], "");
    }


    while (1) {
        strcpy(command_promt, shell_name);
        strcat(command_promt, "[");
        char str[20];
        sprintf(str, "%d", num_of_commands);
        strcat(str, "]");
        strcat(str, terminator);
        strcat(str, " ");
        strcat(command_promt, str);
        
        input = readline(command_promt);
        command = get_input(input);
        strcpy(command_history[num_of_commands % 10], command[0]);

        if (strcmp(command[0], "STOP") == 0) {
            return 0;
        }
        if (strcmp(command[0], "SETSHELLNAME") == 0) {
            strcpy(shell_name, command[1]);
            
            ++num_of_commands;
            continue;
        }
        if (strcmp(command[0], "SETTERMINATOR") == 0) {
            strcpy(terminator, command[1]);
            
            ++num_of_commands;
            continue;
        }

        if (strcmp(command[0], "HISTORY") == 0) {
            for(int i = 0; i < 10; ++i) {
                if (strcmp(command_history[i], "") != 0) {
                    printf("%s\n", command_history[i]);
                }
                
            }
            ++num_of_commands;
            continue;
        }

        
        if (!command[0]) {      
            free(input);
            free(command);
            ++num_of_commands;
            continue;
        }

        if (strcmp(command[0], "cd") == 0) {
            if (cd(command[1]) < 0) {
                perror(command[1]);
            }

            /* Skip the fork */
            
            ++num_of_commands;
            continue;
        }

        child_pid = fork();
        if (child_pid < 0) {
            perror("Fork failed");
            exit(1);
        }

        if (child_pid == 0) {
            if (execvp(command[0], command) < 0) {
                perror(command[0]);
                exit(1);
            }
        } else {
            waitpid(child_pid, &stat_loc, WUNTRACED);
        }

        free(input);
        free(command);
        ++num_of_commands;
    }

    return 0;
}

char **get_input(char *input) {
    char **command = malloc(8 * sizeof(char *));
    if (command == NULL) {
        perror("malloc failed");
        exit(1);
    }

    char *separator = " ";
    char *parsed;
    int index = 0;

    parsed = strtok(input, separator);
    while (parsed != NULL) {
        command[index] = parsed;
        index++;

        parsed = strtok(NULL, separator);
    }

    command[index] = NULL;
    return command;
}

int cd(char *path) {
    return chdir(path);
}
