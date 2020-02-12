#include <unistd.h>
#include <stdio.h>

int main()
{
  pid_t pid = fork();

  if(pid!=0) //is in the parent process
  {
    sleep(5);
  }

  else //is in the child process
  {
    sleep(30);
    printf("Exit from the child.\n");
  }
}
