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

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

#define MAX_HIST_COMMAND 15    // The maximum history commands to display

#define MAX_PID COMMAND 15    // The maximum showpids command tp list the PIDS

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );


  pid_t pid;
  int command_tracker = 0;

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;

    char *working_str  = strdup( cmd_str );

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    char command_history[MAX_HIST_COMMAND][MAX_COMMAND_SIZE];

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
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

    if(token[0]!=NULL)
    {
      if(command_tracker<MAX_HIST_COMMAND)
      {
        strcpy(command_history[command_tracker],cmd_str);
        command_tracker++;
      }
      else
      {
        int row,column;
        for(row = 0; row<MAX_HIST_COMMAND; row++)
        {
          for(column=0;column<MAX_COMMAND_SIZE;column++)
          {
            command_history[row][column]=command_history[row+1][column];
          }
        }
        strcpy(command_history[MAX_HIST_COMMAND-1],cmd_str);
        command_tracker++;
      }
      if((strcmp("quit",token[0])==0) || (strcmp("exit",token[0])==0))
      {
        return 0;
      }
      else if(strcmp("history",token[0])==0)
      {
        int i;
        if(command_tracker<=MAX_HIST_COMMAND)
        {
          for(i= 0; i<command_tracker; i++)
          {
            printf("%d: %s",i+1,command_history[i]);
          }
        }
        else
        {
          for(i= 0; i<MAX_HIST_COMMAND; i++)
          {
            printf("%d: %s",i+1,command_history[i]);
          }
        }
      }
      else if(strcmp("listpids",token[0])==0)
      {

      }

      else
      {
        int token_index  = 0;
        for( token_index = 0; token_index < token_count-1; token_index ++ )
        {
          printf("%s ",token[token_index]);
        }
        printf(": Command not found.");
        printf("\n");
      }
    }
    //pid = fork();

    if(pid == 0)
    {

    }
    free( working_root );

  }
  return 0;
}
