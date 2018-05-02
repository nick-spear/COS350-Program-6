/********************************************************
    COS350 Program #6: linecount
    Author: Nickolas Spear

    Part 3: lc3

    The class presentation of thread synchronization
    was very helpful in my understanding, so I followed
    its conventions in coding this. I attempted to
    a version of this without the 'mailbox' shared
    variable, but it turned out to be slower for how
    I handled it.
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>

void *linecountthread(void *);
void *linecount3(int argc, char *argv[]);
int linecount(FILE *fp);

struct thread_args {
  char *fname;
  int count;
};

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mailReady = PTHREAD_COND_INITIALIZER;
pthread_cond_t mailEmpty = PTHREAD_COND_INITIALIZER;
struct thread_args *mailbox = NULL;

/********************************************************
    int main(int, *char[])

    Handles user i/o and passes arg input to appropriate
      functions.
*********************************************************/
int main(int argc, char *argv[])
{
  if (argc == 1) { // Wiggled in immediate stdin / single file reading

    dup3(stdin, stdout);
    execlp("./lc1", "lc1", NULL);
    perror("lc1 parse error");
    exit(5);

  } else if (argc == 2) { // Just do a lc1 for 1 file, seems more efficient

    execlp("./lc1", "lc1", argv[1], NULL);
    perror("lc1 parse error");
    exit(5);

  } else {

    linecount3(argc, argv);

  }
}

/********************************************************
    int linecount3(void *arg)

    Assign threads to input files
*********************************************************/
void *linecount3(int argc, char *argv[]) {
  pthread_t threads[argc-1];
  struct thread_args argsa[argc-1];
  int total_words = 0, i;

  // Initialize all required report
  for (i = 1; i < argc; i++) {
    argsa[i-1].fname = argv[i];
    argsa[i-1].count = 0;
    pthread_create(&threads[i-1], NULL, linecountthread, &argsa[i-1]);
  }

  // Begin checking for reports
  int report_cnt = 0;
  while (report_cnt < argc-1) {

    // Wait for a report to return to the mailbox
    pthread_mutex_lock(&lock);
    while (mailbox == NULL)
      pthread_cond_wait(&mailReady, &lock);

    // Print report, tally line count, and join thread
    printf("%d: %s\n", mailbox->count, mailbox->fname);
    total_words += mailbox->count;
    for (i = 0; i < argc-1; i++) {
      if ( mailbox == &argsa[i])
        pthread_join(threads[i], NULL);
    }

    // Clear and unlock mailbox
    mailbox = NULL;
    pthread_cond_signal(&mailEmpty);
    pthread_mutex_unlock(&lock);

    report_cnt++;
  }

  printf("%-8d total\n", total_words);
}

/********************************************************
    int linecount3(void *arg)

    Linecount procedure and global variable setting
*********************************************************/
void *linecountthread(void *arg)
{
  struct thread_args *thisReport = arg;
  FILE *fp;
  char c;
  int lc = 0;

  // Linecount the file
  if ( (fp = fopen(thisReport->fname, "r")) != NULL ) {
    while ((c = fgetc(fp)) != EOF) {
      if (c == '\n') {
        lc++;
      }
    }
 	  thisReport->count = lc;
    fclose(fp);
  } else
    perror(thisReport->fname);

  // Wait until mailbox is ready and stash your stuff in there
  pthread_mutex_lock(&lock);
  while (mailbox != NULL)
    pthread_cond_wait(&mailEmpty, &lock);
  mailbox = thisReport;
  pthread_cond_signal(&mailReady);
  pthread_mutex_unlock(&lock);
}
