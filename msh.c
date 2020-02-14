// The MIT License (MIT)
//
// Copyright (c) 2016, 2017 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n" // We want to split our command line up into tokens \
                           // so we need to define what delimits our tokens.   \
                           // In this case  white space                        \
                           // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10 // Mav shell only supports ten arguments

#define MAX_HIST_COMMAND 15 // The maximum history commands to display

#define MAX_PID_COMMAND 15 // The maximum showpids command tp list the PIDS

char command_history[MAX_HIST_COMMAND][MAX_COMMAND_SIZE];
int command_pid[MAX_PID_COMMAND];
char *history_token[MAX_COMMAND_SIZE];
int command_tracker = 0;
int pids_tracker = 0;

void print_history(int tracker, int command_number)
{
  int i;
  if (tracker < command_number)
  {
    for (i = 0; i < tracker; i++)
    {
      printf("%d: %s", i + 1, command_history[i]);
    }
  }
  else
  {
    for (i = 0; i < command_number; i++)
    {
      printf("%d: %s", i + 1, command_history[i]);
    }
  }
}

void print_pids(int tracker)
{
  int i;
  for (i = 0; i < tracker; i++)
  {
    printf("%d: %d\n", i + 1, command_pid[i]);
  }
}

int main()
{

  char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);

  pid_t pid;

  while (1)
  {
    // Print out the msh prompt
    printf("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin))
      ;

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;

    char *working_str = strdup(cmd_str);

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while (((arg_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
           (token_count < MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup(arg_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0)
      {
        token[token_count] = NULL;
      }
      token_count++;
    }

    /*********************
    *                     *
    *                     *
    * Shell Functionality *
    *                     *
    *                     *
     ********************/

    if (token[0] != NULL)
    {
      if (command_tracker < MAX_HIST_COMMAND)
      {
        strcpy(command_history[command_tracker], cmd_str);
        command_tracker++;
      }
      else
      {
        int row, column;
        for (row = 0; row < MAX_HIST_COMMAND - 1; row++)
        {
          for (column = 0; column < MAX_COMMAND_SIZE; column++)
          {
            command_history[row][column] = command_history[row + 1][column];
          }
        }
        strcpy(command_history[MAX_HIST_COMMAND - 1], cmd_str);
        command_tracker++;
      }
    }
    if (token[0] == NULL)
    {
      continue;
    }
    else if ((strcmp("quit", token[0]) == 0) || (strcmp("exit", token[0]) == 0))
    {
      return 0;
    }
    else if (strcmp("history", token[0]) == 0)
    {
      print_history(command_tracker, MAX_HIST_COMMAND);
    }
    else if (strcmp("showpids", token[0]) == 0)
    {
      print_pids(pids_tracker);
    }
    else if (strcmp("cd", token[0]) == 0)
    {
      if (chdir(token[1]) == -1)
      {
        printf("%s: Directory not found.\n", token[1]);
      }
    }
    else
    {
      pid = fork();
      if (pid == -1)
      {
        perror("Fork Failed.");
        exit(EXIT_FAILURE);
      }

      else if (pid == 0)
      {
        if (cmd_str[0] == '!')
        {
          int index = atoi(&cmd_str[1]);

          if (index > command_tracker - 1)
          {
            printf("Command not in history.\n");
            fflush(NULL);
            _exit(1);
          }

          else
          {
            int i = 0;
            command_history[index - 1][strlen(command_history[index - 1]) - 1] = '\0';
            char *h = strtok(command_history[index - 1], " ");
            while (h != NULL)
            {
              history_token[i++] = h;
              h = strtok(NULL, " ");
            }
            if (strcmp(history_token[0], "history") == 0)
            {
              print_history(command_tracker-1,MAX_HIST_COMMAND-1);
              printf("\n");
            }
            else if (strcmp(history_token[0], "cd") == 0)
            {
              if (chdir(history_token[1]) == -1)
              {
                printf("%s: Directory not found.\n", history_token[1]);
              }
            }
            else if (strcmp(history_token[0], "showpids") == 0)
            {
              print_pids(pids_tracker);
            }
            else
            {
              int ret;
              ret = execvp(history_token[0], history_token);
              if (ret == -1)
              {
                printf("%s: Command not found.\n", history_token[0]);
              }
            }
          }
          fflush(NULL);
          _exit(1);
        }
        else
        {
          int ret = execvp(token[0], token);
          if (ret == -1)
          {
            printf("%s: Command not found.\n", token[0]);
          }
          fflush(NULL);
          _exit(1);
        }
      }
      else
      {
        int i = 0;
        if (pids_tracker == MAX_PID_COMMAND)
        {
          for (i = 0; i < (pids_tracker - 1); i++)
          {
            command_pid[i] = command_pid[i + 1];
          }
          pids_tracker--;
        }
        command_pid[pids_tracker] = pid;
        pids_tracker++;
        int status;
        wait(&status);
      }
    }
    free(working_root);
  }
  return 0;
}
