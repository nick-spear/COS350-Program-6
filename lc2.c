/********************************************************
    COS350 Program #6: linecount
    Author: Nickolas Spear

    Part 2: lc2
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stropts.h>
#include <poll.h>
#include <sys/stat.h>

void linecount2(int argc, char **argv);

/********************************************************
    int main(int, *char[])

    Handles user i/o and passes arg input to appropriate
      functions.
*********************************************************/
int main(int argc, char **argv)
{
  if (argc == 1) { // Wiggled in immediate stdin / single file reading

    dup3(stdin, stdout);
    execlp("./lc1", "lc1", NULL);
    perror("lc1 parse error");
    exit(5);

  } else if (argc == 2) {

    execlp("./lc1", "lc1", argv[1], NULL);
    perror("lc1 parse error");
    exit(5);

  } else {

    linecount2(argc, argv);

  }
}

/********************************************************
    int linecount2(int argc, char **argv)

    Utilizes child processes to linecount each
      file with lc1.
*********************************************************/
void linecount2(int argc, char **argv) {
  int pipefds[argc-1][2];
  int i,
      pid = 1,
      read_len,
      successful_children = 0;
  char buf[BUFSIZ];
  struct pollfd pipepoll[argc-1];

  for(i = 1; i < argc; i++) {

    // Verifies that specified file exists
    struct stat buf;
    if ( stat(argv[i], &buf) == 0) {
      successful_children++;
    } else {
      fprintf(stderr, "%s: %s: File not found\n", argv[0], argv[i]);
      continue;
    }

    // Attempts to construct pipe
    if (pipe(pipefds[i-1]) == -1) {
      perror("Cannot get a pipe");
      exit(1);
    }
    pipepoll[i-1].events = POLLIN;

    // Creates fork
    pid = fork();
    if ( pid == 0 ) { // If it's the child, close appropriate fd's and exec lc1
      close(pipefds[i-1][0]);
      if ( dup2(pipefds[i-1][1], 1) == -1 ) {
        perror("could not redirect stdout");
        exit(4);
      }
      close(pipefds[i-1][1]);
      execlp("./lc1", "lc1", argv[i], NULL);
      perror("lc1 parse error");
      exit(5);
    } else { // Otherwise, assign poll struct's fd and close appropriate pipes
      pipepoll[i-1].fd = pipefds[i-1][0];
      close(pipefds[i-1][1]);
    }
  }

  int children_read = 0,
      children_total = 0;
  while (children_read < successful_children) {

    // Polls children read pipes
    int poll_ret = poll(pipepoll, argc-1, 1);
    if (poll_ret != 0) {
      if (poll_ret == -1) {
        perror("poll error");
        exit(1);
      }

      // Iterates through polls to find affected ones
      for(int j = 0; j < argc-1; j++) {
        if((pipepoll[j].revents & POLLIN) == POLLIN) {
          // If pipe has data to read, we parse it
          if(read(pipepoll[j].fd, buf, BUFSIZ) > 0) {
            char *tok = strtok(buf, " \n\t\r");
            int total = atoi(tok);
            printf("%-8d %s\n", total, argv[j+1]);
            children_read++;
            children_total += total;
            close(pipepoll[j].fd);
          }
        }
      }
    }
  }

  // If we tried to parse multiple files, we spit out the total
  printf("%-8d total\n", children_total);
}
