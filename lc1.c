/********************************************************
    COS350 Program #6: linecount
    Author: Nickolas Spear

    Part 1: lc1
*********************************************************/

#include <stdio.h>
#include <stdlib.h>

int linecount(FILE *fp);

/********************************************************
    int main(int, *char[])

    Handles user i/o and passes file input to appropriate
      functions.
*********************************************************/
int main(int argc, char **argv)
{
  if (argc == 1) { // If no files given, we read from stdin

    int lc = linecount(stdin);
    if (lc < 0) {
      fprintf(stderr, "%s: Error reading from stdin\n", argv[0]);
    } else {
      printf("%-8d stdin\n", lc);
    }

  } else if (argc == 2) { // If only 1 file given, we don't tally toal

    int lc  =  linecount(fopen(argv[1], "r"));
    if (lc < 0) {
      fprintf(stderr, "%s: %s: Error reading file\n", argv[0], argv[1]);
    } else {
      printf("%-8d %s\n", lc, argv[1]);
    }

  } else { // Otherwise, we roll through the files as usual

    int totallc = 0;
    int i;
    for(i = 1; i < argc; i++) {
      int lc  =  linecount(fopen(argv[i], "r"));
      if (lc < 0) {
        fprintf(stderr, "%s: %s: Error reading file\n", argv[0], argv[i]);
      } else {
        totallc += lc;
        printf("%-8d %s\n", lc, argv[i]);
      }
    }
    printf("%-8d total\n", totallc);

  }

}


/********************************************************
    int linecount(FILE *fp)

    Counts lines from passed file pointer.
*********************************************************/
int linecount(FILE *fp)
{
  if (fp == NULL) return -1;

  int lc = 0;
  char c;

  while ((c = fgetc(fp)) != EOF) {
    if (c == '\n') {
      lc++;
    }
  }

  return lc;
}
