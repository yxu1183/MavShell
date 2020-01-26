#include <sys/types.h> //used for pid_t
#include <sys/wait.h> //used for waitpid
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> //used for miscellaneous functions - fork() in this case

/*fork a chid and print the messages from the parent and child*/

int main(void)
{
  pid_t pid = fork(); //fork returns two values

  if(pid == -1) // if fork returns -1, an error will occur
  {
    perror("fork failed: "); //function that produces a message on standard error describing the last error encountered during the process
    exit(EXIT_FAILURE);
  }

  else if(pid == 0)
  {
    printf("Hello from the child process.\n"); //if fork returns 0, then we are in child process
    fflush(NULL);
    exit(EXIT_SUCCESS);

    /*fflush() is typically used for output stream only.
    Its purpose is to clear (or flush) the output buffer and move the buffered data
    to console (in case of stdout) or disk (in case of file output stream).
    Here, we used fflush, becuase I/o is buffered.*/
  }

  else
  {
    /*if fork returns a positive number, we are in the parent process,
    and its return value is the PID of the newly created child process.*/
    int status;
    (void)waitpid(pid, &status, 0); //We force the parent process to wait until the child process exits
    //waitpid () - waitpid(pid_t pid, int *status,int options)
    printf("Hello from the parent process\n");
    fflush(NULL);
  }

  return EXIT_SUCCESS;
}
