#include <bits/stdc++.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;
const int len = 20; // Can be Changed appropriately

int main()
{
        char PIPE_OPERATOR[len] = "|\0";
        int p[len][2];
        printf("\n");
        while (1)
        {
                printf("my terminal > ");
                int is_more = 1;
                int cur = 0;
                int first = 1, last = 0;
                for (int i = 0; i < len; i++)
                {
                        if (pipe(p[i]) == -1)
                        {
                                printf("pipe failed\n"); // Notifying on Fail
                        }
                }
                int now = 0;
                string str;
                getline(cin, str);

                if ((int)(str.length()) == 0)
                        continue;
                stringstream in(str);
                for (; is_more == 1;)
                {
                        char *args[len];
                        for (int i = 0; i < len; i++)
                                args[i] = NULL;
                        int index = 0;
                        char temp[len];
                        last = 1;
                        is_more = 0;
                        for (; in >> temp;)
                        {
                                if (strcmp(temp, PIPE_OPERATOR) == 0)
                                {
                                        last = 0, is_more = 1;
                                        break;
                                }

                                args[index] = (char *)malloc(len * sizeof(char)); // Getting first command by %s
                                strcpy(args[index], temp);
                                index += 1;
                        }
                        args[index] = NULL;
                        int flag;
                        if (args[index - 1][0] == '&')
                                flag = 0, args[index - 1] = NULL, index--;
                        else
                                flag = 1;
                        int process_id = fork(); // Returns 0 in child  process , Process Id in Parent process
                        if (process_id < 0)
                        {
                                printf("fork failed\n");
                        }
                        if (process_id == 0)
                        {

                                if (last && first)
                                {
                                        char *file;
                                        int i = 0;
                                        while (i < index - 1)
                                        {
                                                if (args[i] != NULL && args[i][0] == '<')
                                                {
                                                        file = args[i + 1];
                                                        args[i] = NULL;
                                                        close(0);
                                                        int fd = open(file, O_RDONLY); // Opening with Read Only Mode
                                                        dup2(fd, 0);
                                                        break;
                                                }
                                                i++;
                                        }
                                        i = 0;
                                        while (i < index - 1)
                                        {
                                                if (args[i] != NULL && args[i][0] == '>')
                                                {
                                                        file = args[i + 1];
                                                        args[i] = NULL;
                                                        close(1);
                                                        int fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR); // Opening with most modes on
                                                        dup2(fd, 1);                                                                              // the file descriptor fd will be copied into the FDT entry corresponding to stdin
                                                        break;
                                                }
                                                i++;
                                        }
                                        execvp(args[0], args); // Exec the command
                                        return 0;
                                }
                                else if (first)
                                {
                                        if (dup2(p[now][1], STDOUT_FILENO) == -1)
                                                perror("dup2 in first");
                                        char *file;
                                        int i = 0;
                                        while (i < index - 1)
                                        {
                                                if (args[i] != NULL && args[i][0] == '<')
                                                {
                                                        file = args[i + 1];
                                                        args[i] = NULL;
                                                        close(0);
                                                        int fd = open(file, O_RDONLY);
                                                        dup2(fd, 0);
                                                        break;
                                                }
                                                i++;
                                        }
                                        i = 0;
                                        while (i < index - 1)
                                        {
                                                if (args[i] != NULL && args[i][0] == '>')
                                                {
                                                        file = args[i + 1];
                                                        args[i] = NULL;
                                                        close(1);
                                                        int fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                                                        dup2(fd, 1);
                                                        break;
                                                }
                                                i++;
                                        }
                                        execvp(args[0], args);
                                        return 0;
                                }
                                else if (last)
                                {

                                        if (dup2(p[now - 1][0], 0) == -1)
                                                perror("dup2 in last");
                                        char *file;
                                        int i = 0;
                                        while (i < index - 1)
                                        {
                                                if (args[i] != NULL && args[i][0] == '<')
                                                {
                                                        file = args[i + 1];
                                                        args[i] = NULL;
                                                        close(0);
                                                        int fd = open(file, O_RDONLY);
                                                        dup2(fd, 0);
                                                        break;
                                                }
                                                i++;
                                        }
                                        i = 0;
                                        while (i < index - 1)
                                        {
                                                if (args[i] != NULL && args[i][0] == '>')
                                                {
                                                        file = args[i + 1];
                                                        args[i] = NULL;
                                                        close(1);
                                                        int fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                                                        dup2(fd, 1);
                                                        break;
                                                }
                                                i++;
                                        }
                                        execvp(args[0], args);
                                        return 0;
                                }
                                else
                                {

                                        if (dup2(p[now][1], 1) == -1)
                                                perror("dup2 in immidiate");

                                        if (dup2(p[now - 1][0], 0) == -1)
                                                perror("dup2 in immidiate");
                                        char *file;
                                        int i = 0;
                                        while (i < index - 1)
                                        {
                                                if (args[i] != NULL && args[i][0] == '<')
                                                {
                                                        file = args[i + 1];
                                                        args[i] = NULL;
                                                        close(0);
                                                        int fd = open(file, O_RDONLY);
                                                        dup2(fd, 0);
                                                        break;
                                                }
                                                i++;
                                        }
                                        i = 0;
                                        while (i < index - 1)
                                        {
                                                if (args[i] != NULL && args[i][0] == '>')
                                                {
                                                        file = args[i + 1];
                                                        args[i] = NULL;
                                                        close(1);
                                                        int fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                                                        dup2(fd, 1);
                                                        break;
                                                }
                                                i++;
                                        }
                                        execvp(args[0], args);
                                        return 0;
                                }
                        }
                        else
                        { // wait only if no & else parent goes on.
                                int status;
                                if (flag) // if no &.
                                {
                                        waitpid(process_id, &status, 0);
                                }
                        }
                        first = 0;
                        close(p[now][1]);
                        now++;
                }
        }
        return 0;
}