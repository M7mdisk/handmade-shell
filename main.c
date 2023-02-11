#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define DELIM " \t\r\n\a"

char *builtins[] = {"cd", "exit"};

int num_builtins() { return sizeof(builtins) / sizeof(char *); }

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

void exec_builtin(char *args[]) {

  if (strcmp(args[0], "cd") == 0) {
    if (chdir(args[1]) == -1) {
      fprintf(stderr, "Could not change directory\n");
      return;
    }

  } else if (strcmp(args[0], "exit") == 0) {
    exit(0);
  }
}

int main() {
  while (1) {
    char cwd[256];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
      exit(0);
    }

    printf("%s>>", cwd);
    char *line = NULL;
    size_t len = 0;
    int nread = getline(&line, &len, stdin);
    if (nread != -1) {
      if (feof(stdin)) {
        exit(0);
        break;
      }
      char **args = parse_command(line);
      int is_builtin = 0;
      for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtins[i]) == 0) {
          exec_builtin(args);
          is_builtin = 1;
          break;
        }
      }
      if (is_builtin)
        continue;
      int rc = fork();
      if (rc < 0) {
        fprintf(stderr, "Something went wrong!\n");
        exit(1);
      }
      if (rc == 0) { // this is the child
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
