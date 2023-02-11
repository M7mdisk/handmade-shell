#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define DELIM " \t\r\n\a"

char **parse_command(char *line) {
  int buf_size = 100;
  int MAX_LEN = 100;
  char *word;
  char **words = (char **)malloc(buf_size * sizeof(char *));
  if (!words) {
    fprintf(stderr, "allocation error\n");
    exit(1);
  }
  int i = 0;
  word = strtok(line, DELIM);
  while (word != NULL) {
    words[i] = word;
    i++;
    if (i >= buf_size) {
      buf_size += 100;
      words = realloc(words, buf_size * sizeof(char *));
      if (!words) {
        fprintf(stderr, "allocation error\n");
        exit(1);
      }
    }
    word = strtok(NULL, DELIM);
  }
  words[i] = NULL;
  return words;
}

int main() {
  printf("Welcome to shell\n");

  while (1) {
    char cwd[256];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
      exit(0);
    }

    printf("%s >>>", cwd);
    char *line = NULL;
    size_t len = 0;
    int nread = getline(&line, &len, stdin);
    if (nread != -1) {
      if (feof(stdin)) {
        exit(0);
        break;
      }
      if (line[nread - 1] == '\n') {
        line[nread - 1] = '\0';
        --nread;
      }
      int rc = fork();
      if (rc < 0) {
        fprintf(stderr, "Something went wrong!\n");
        exit(1);
      }
      if (rc == 0) { // this is the child
        char **args = parse_command(line);
        execvp(args[0], args);
      } else {
        wait(NULL);
        printf("Finished process %d\n", rc);
      }
    } else {
      exit(1);
    }
  }
  return 0;
}
