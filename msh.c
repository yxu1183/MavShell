/*
  Name: Yunika Upadhayaya
  ID: 1001631183
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n" /*                                                  \
                              We want to split our command line up into tokens \
                              so we need to define what delimits our tokens.   \
                              In this case  white space                        \
                              will separate the tokens on our command line     \
                            */

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10 // Mav shell only supports ten arguments

#define MAX_HIST_COMMAND 15 // The maximum history commands to display

#define MAX_PID_COMMAND 15 // The maximum showpids command tp list the PIDS

//Declaring global variable for history and showpids functionality
char command_history[MAX_HIST_COMMAND][MAX_COMMAND_SIZE];
int command_pid[MAX_PID_COMMAND];
char *history_token[MAX_COMMAND_SIZE];
int command_tracker = 0;
int pids_tracker = 0;

/*
  print_history prints latest 15 commands from the history.
  Takes in the total number of commands entered and max number of
  history commands that needs to be printed as parameters.
  Returns nothing.
*/
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

/*
  print_pids prints latest 15 process IDs after
  child process is created using fork().
  Takes in total number of commands entered in the
  child process as a parameter.
  Returns nothing.
*/
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

    /*
       Read the command from the commandline.  The
       maximum command that will be read is MAX_COMMAND_SIZE
       This while command will wait here until the user
       inputs something since fgets returns NULL when there
       is no input
    */
    while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin))
      ;

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;

    /*
       Pointer to point to the token
       parsed by strsep
    */
    char *arg_ptr;

    char *working_str = strdup(cmd_str);

    /*
       we are going to move the working_str pointer so
       keep track of its original value so we can deallocate
       the correct amount at the end
    */
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

    /*
      Copy the input commands into the history array except for NULL command.
      Copy the first 15 commands as it is.
      After the commands hit 15, shift one position in array with new value.
      Get rid of the old command and add new command into the array.
    */
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

    /*
      Avoid sgementation fault.
      Keep on prompting "msh", accepting new line of input.
    */
    if (token[0] == NULL)
    {
      continue;
    }

    /*
      Peacefully exit the program with zero status,
      if user enters "quit" or "exit".
    */
    else if ((strcmp("quit", token[0]) == 0) || (strcmp("exit", token[0]) == 0))
    {
      return 0;
    }

    /*
      Print the latest 15 history commands,
      if user enters "history" using print_history function.
    */
    else if (strcmp("history", token[0]) == 0)
    {
      print_history(command_tracker, MAX_HIST_COMMAND);
    }

    /*
      Print the latest 15 process IDs in the child process,
      if user enters "showpids".
    */
    else if (strcmp("showpids", token[0]) == 0)
    {
      print_pids(pids_tracker);
    }

    /*
      Execute "cd" command in linux.
      "cd ~" does not execute.
      If chdir() fails, print otherwise.
    */
    else if (strcmp("cd", token[0]) == 0)
    {
      if (chdir(token[1]) == -1)
      {
        printf("%s: Directory not found.\n", token[1]);
      }
    }

    //Create a child process using fork.
    else
    {
      pid = fork();

      //Process was not created. fork() didn't work.
      if (pid == -1)
      {
        perror("Fork Failed.");
        exit(EXIT_FAILURE);
      }

      //We are in the child process.
      else if (pid == 0)
      {
        /*
          Execute the "!n" history functionality.
          Execute nth command in the history when entered !n.
        */
        if (cmd_str[0] == '!')
        {
          //Change the string "n" into integer n using atoi()
          int index = atoi(&cmd_str[1]);

          //Only account for the history commands entered upto 15th index
          if (index > command_tracker - 1)
          {
            printf("Command not in history.\n");
            fflush(NULL);
            _exit(1);
          }

          /*
            Tokenize the command in the nth index of the command history.
            So, that we can pass it onto to the execv() to execute the command.
          */
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
          }

          //Print the history commands, till nth index.
          if (strcmp(history_token[0], "history") == 0)
          {
            command_history[index - 1][strlen(command_history[index - 1])] = '\n';
            print_history(command_tracker - 1, MAX_HIST_COMMAND - 1);
          }

          /*
            Execute chdir(), to change the directory (cd) for nth index.
            Does not execute "cd ~".
            If no directory found, print otherwise.
          */
          else if (strcmp(history_token[0], "cd") == 0)
          {
            if (chdir(history_token[1]) == -1)
            {
              printf("%s: Directory not found.\n", history_token[1]);
            }
          }

          //Print the latest process IDs, till nth index.
          else if (strcmp(history_token[0], "showpids") == 0)
          {
            print_pids(pids_tracker);
          }

          /*
            Supports and executes commands entered in nth index.
            Searches in the following path order:
            Current working directory
            /usr/local/bin
            /usr/bin
            /bin
          */
          else
          {
            int ret;
            ret = execvp(history_token[0], history_token);
            if (ret == -1)
            {
              printf("%s: Command not found.\n", history_token[0]);
            }
          }
          fflush(NULL);
          _exit(1);
        }
        /*
          Executes shell supporting commands entered.
          Searches in the following path order:
          Current working directory
          /usr/local/bin
          /usr/bin
          /bin
        */
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
      //We are in the parent process.
      else
      {
        /*
          Update the pids array with the latest 15 process IDs
          after fork() is called and child process is created.
        */
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

        //Wait and terminate only after child process terminates.
        int status;
        wait(&status);
      }
    }
    /*
      Free the dynamically allocated memory that was reserved for us.
      Avoid memory leak.
    */
    free(working_root);
  }
  return 0;
}

