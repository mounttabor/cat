/*
// This version was created by Gemini assistent based on my original version.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE (4 * 1024)

const char *VersionString = "Version 1.0";
const char *HelpString = "options: -h, --help, -v, --version, -n, --number, -E, --show-ends, -T, --show-tabs";

// Function prototypes
void print_help(void);
void print_version(void);
int print_file(const char *filename, int flags);

int main(int argc, char *argv[]) {
  int opt;
  int flags = 0;  // Initialize flags to 0

  // Parse options using getopt
  while ((opt = getopt(argc, argv, "hvnET")) != -1) {
    switch (opt) {
      case 'h':
      case ':':  // Handle missing argument for short options
        print_help();
        return EXIT_SUCCESS;
      case 'v':
      case '-':  // Handle long options prefixed with '-'
        print_version();
        return EXIT_SUCCESS;
      case 'n':
      case 'E':
      case 'T':
        flags |= (1 << (opt - 'n'));  // Set corresponding flag bit
        break;
      default:
        fprintf(stderr, "Invalid option '-%c'\n", opt);
        return EXIT_FAILURE;
    }
  }

  // Check for remaining non-option arguments (filenames)
  if (optind >= argc) {
    fprintf(stderr, "Missing filename\n");
    return EXIT_FAILURE;
  }

  // Process each file based on flags
  for (int i = optind; i < argc; i++) {
    if (print_file(argv[i], flags) != 0) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

// Function to print help message
void print_help(void) {
  printf("%s\n", HelpString);
  // ... add detailed help message here ...
}

// Function to print version information
void print_version(void) {
  printf("%s\n", VersionString);
}

// Function to open, read, and print a file with flags
int print_file(const char *filename, int flags) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("fopen");
    return -1;
  }

  char buffer[BUFFER_SIZE];
  int nreads;

  // Print line numbers if --number flag is set
  if (flags & 1) {
    int line_number = 1;
    while ((nreads = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
      for (int i = 0; i < nreads; i++) {
        if (buffer[i] == '\n') {
          printf("%d ", line_number++);
        }
        putchar(buffer[i]);
      }
    }
  } else {
    // Normal read and write
    while ((nreads = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
      fwrite(buffer, 1, nreads, stdout);
    }
  }

  if (ferror(fp)) {
    perror("fread");
    fclose(fp);
    return -1;
  }

  // Print newline and show line ending if flags are set
  if (!(flags & 2)) {  // Check if show-ends flag is not set
    putchar('\n');
  }
  if (flags & 4) {  // Check if show-tabs flag is set
    printf("$");
  }

  fclose(fp);
  return 0;
}
