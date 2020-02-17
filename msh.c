/*
  Name: Yunika Upadhayaya
  ID: 1001631183
  Assignment 1-(Mav Shell)
*/

#define _GNU_SOURCE

//Required Header files
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#define WHITESPACE " \t\n" /*                                                  
                              We want to split our command line up into tokens  
                              so we need to define what delimits our tokens.    
                              In this case  white space                         
                              will separate the tokens on our command line.    
                            */

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10 // Mav shell only supports ten arguments

#define MAX_HIST_COMMAND 15 // The maximum history commands to display

#define MAX_PID_COMMAND 15 // The maximum showpids command tp list the PIDS

//2D array to store the input commands to print the history
char command_history[MAX_HIST_COMMAND][MAX_COMMAND_SIZE];

//Array to store the process IDs of the child process created
int command_pid[MAX_PID_COMMAND];

//Counter variable to track the total input commands in history array
int command_tracker = 0;

//Counter variable to track the process IDs in pid array
int pids_tracker = 0;

//Boolean to check in prompting "msh>" to take the input
bool allow_input = true;

/*
  print_history to print latest 15 commands from the history.
  Takes in the total number of commands entered as a parameter.
  Returns nothing.
*/
void print_history(int tracker)
{
  int i;
  for (i = 0; i < tracker; i++)
  {
    printf("%d: %s", i + 1, command_history[i]);
  }
}

/*
  print_pids to print latest 15 process IDs after
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
    printf("%d: %d\n", i+1, command_pid[i]);
  }
}

int main()
{
  //Varible declared to store in the input provided by the user
  char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);

  //Run the loop until it is true
  while (1)
  {
    /*
      Prompt to take the input until the boolean value is true.
      Allow_input becomes false when we change the cmd_str later while executing
      n!(history functionality), since we copy the command in nth index to cmd_str.
    */
    if (allow_input)
    {
      // Print out the msh prompt
      printf("msh> ");

      /*
        Read the command from the commandline.  The
        maximum command that will be read is MAX_COMMAND_SIZE
        This while command will wait here until the user
        inputs something since fgets returns NULL when there
        is no input.
      */
      while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin))
        ;
    }

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
      We are going to move the working_str pointer so
      keep track of its original value so we can deallocate
      the correct amount at the end.
    */
    char *working_root = working_str;

    //Reset allow_input to true to keep on prompting "msh>".
    allow_input = true;

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
      Copy the input commands into the history array except for NULL command
      and command which starts with '!', because when user enters !n, we 
      copy the command in nth index later directly into cmd_str.
      Copy the first 15 commands as it is.
      After the commands hit 15, shift one position in array with new value.
      Get rid of the old command and add new command into the array.
    */
    if (token[0] != NULL && cmd_str[0] != '!')
    {
      if (command_tracker < MAX_HIST_COMMAND)
      {
        strcpy(command_history[command_tracker], cmd_str);
        command_tracker++;
      }
      else
      {
        int row, column;
        for (row = 0; row < MAX_HIST_COMMAND; row++)
        {
          for (column = 0; column < MAX_COMMAND_SIZE; column++)
          {
            command_history[row][column] = command_history[row + 1][column];
          }
        }
        strcpy(command_history[MAX_HIST_COMMAND - 1], cmd_str);
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
      _exit(1);
    }

    /*
      Print the latest 15 history commands,
      if user enters "history" using print_history function.
    */
    else if (strcmp("history", token[0]) == 0)
    {
      print_history(command_tracker);
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

    /*
      Execute the "!n" history functionality.
      Execute nth command in the history when entered !n.
    */
    else if (cmd_str[0] == '!')
    {
      //Change the string "n" into integer n using atoi()
      int index = atoi(&cmd_str[1]);

      //Only account for the history commands entered upto 15th index
      if (index > command_tracker)
      {
        printf("Command not in history.\n");
      }

      /*
        Copy the command in nth index into the original cmd_str 
        and execute the command in same nth index.
      */
      strcpy(cmd_str, command_history[index - 1]);

      /*
        Set the value of allow_input to false, to stop prompting
        msh, after we copy the command into cmd_str.
      */
      allow_input = false;
      continue;
    }

    else
    {
      //Create a child process using fork.
      pid_t pid;
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
          Executes shell supporting commands entered.
          Searches in the following path order:
          Current working directory
          /usr/local/bin
          /usr/bin
          /bin
        */
        int ret = execvp(token[0], token);
        if (ret == -1)
        {
          printf("%s: Command not found.\n", token[0]);
        }
        //Exit child process before the parent process.
        exit(1);
      }

      //We are in the parent process.
      else
      {
        /*
          Update the pids array with the latest 15 process IDs
          after child process is created using fork(). 
          After the commands hit 15,shift one position in array with new process ID.
          Get rid of the old pid and add new pid into the array.
        */
        if (pids_tracker < MAX_PID_COMMAND)
        {
          command_pid[pids_tracker] = pid;
          pids_tracker++;
        }
        else
        {
          int i = 0;
          for (i = 0; i < pids_tracker - 1; i++)
          {
            command_pid[i] = command_pid[i + 1];
          }
          command_pid[MAX_PID_COMMAND - 1] = pid;
        }

        //Wait and terminate only after child process terminates.
        int status;
        waitpid(pid, &status, 0);
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

