#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>

#define CSVFILE "nyc_pop.csv"
#define DATAFILE "nyc_pop.data"

// ===========================================================================================================

struct pop_entry {
  int year;
  int population;
  char boro[15];
};

// ===========================================================================================================
// helper function - prints out a population entry

void print_pop_entry(struct pop_entry *p) {
  printf("year: %d\tboro: %s\tpop: %d\n", p->year, p->boro, p->population);
}

// ===========================================================================================================
// helper function - returns size of a file in bytes

unsigned long file_size(char *file) {
  struct stat sb;
  stat(file, &sb);
  return sb.st_size;
}

// ===========================================================================================================
// helper function - returns number of lines in a CSV file

int lineCounter() {
  int open_call = open(CSVFILE, O_RDONLY);
  char c;

  int lines = 0;
  while (read(open_call, &c, 1) != '\0') {
    if (c == '\n') lines++;
  }

  close(open_call);
  return lines;
}

// ===========================================================================================================
// Read the contents of the file (this is text).
// Store each piece of data using the following struct:

// Create & write a new file containing the population data using struct pop_entry.
// This file should contain byte data.
// If the file already exists, overwrite the existing one.

void read_csv() {

  printf("reading %s\n", CSVFILE);
  int open_call = open(CSVFILE, O_RDONLY);

  int create_call = open(DATAFILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);

  int i;
  for (i = 0; i < lineCounter(); i++) {

    char character;
    character = '\0';

    char line[100];

    int c = 0;
    while (character != '\n') {
      int read_call = read(open_call, &character, 1);
      line[c] = character;
      c++;
    }
    line[c] = '\0';

    struct pop_entry col[5];

    if (i) {

      strcpy(col[0].boro, "Manhattan");
      strcpy(col[1].boro, "Brooklyn");
      strcpy(col[2].boro, "Queens");
      strcpy(col[3].boro, "Bronx");
      strcpy(col[4].boro, "Staten Island");

      int year;
      sscanf(line, "%d,%d,%d,%d,%d,%d\n", &year, &col[0].population, &col[1].population, &col[2].population, &col[3].population, &col[4].population);
      for (c = 0; c < 5; c++) {
        col[c].year = year;
      }

      write(create_call, col, sizeof(col));

    }

  }

  printf("wrote %lu bytes to %s\n", file_size(DATAFILE), DATAFILE);

  close(open_call);
  close(create_call);

}

// ===========================================================================================================
// Read the contents of the data file into an array of struct pop_entry values.
// This array should be dynamically allocated based on the file size.
// Display the data in human-readable format, provide a number for each data entry when displaying.

void read_data() {

  int size = file_size(DATAFILE);
  int numEntries = size / sizeof(struct pop_entry);

  struct pop_entry *entries = calloc(numEntries, sizeof(struct pop_entry));

  int open_call = open(DATAFILE, O_RDONLY);
  int reader = read(open_call, entries, size);

  // printf("numEntries: %d\n", numEntries);

  int i;
  for (i = 0; i < numEntries; i++) {
    printf("%d: year: %d\tboro: %s\tpop: %d\n", i, entries[i].year, entries[i].boro, entries[i].population);
  }

  free(entries);

}

// ===========================================================================================================
// Ask the user for new data to put into a struct pop_entry value.
// Append that data to the end of the data file (not the csv).

void add_data() {

  char year_boro_pop[100];
  printf("Enter year boro pop: ");
  fgets(year_boro_pop, sizeof(year_boro_pop), stdin);
  struct pop_entry add;
  sscanf(year_boro_pop, "%d %s %d", &add.year, add.boro, &add.population);

  int open_call = open(DATAFILE, O_WRONLY | O_APPEND, 0644);
  int write_call = write(open_call, &add, sizeof(add));

  printf("Appended data to file: ");
  print_pop_entry(&add);

  close(open_call);

}

// ===========================================================================================================
// Start like -read_data.
// Then prompt the user to enter the corresponding number of an entry to modify.
// Then prompt the user for new data.
// Update the entry in the data file (not the csv).

void update_data() {

  read_data();

  char entry_input[100];
  printf("Enter entry number: ");
  fgets(entry_input, sizeof(entry_input), stdin);
  int entry;
  sscanf(entry_input, "%d", &entry);

  char data_input[100];
  printf("Enter year boro pop: ");
  fgets(data_input, sizeof(data_input), stdin);
  struct pop_entry update;
  sscanf(data_input, "%d %s %d", &update.year, update.boro, &update.population);

  int open_call = open(DATAFILE, O_RDWR);
  lseek(open_call, entry * sizeof(struct pop_entry), SEEK_SET);
  write(open_call, &update, sizeof(struct pop_entry));

  close(open_call);
  printf("File updated.\n");

}

// ===========================================================================================================

int main(int argc, char *argv[]) {

  if (argc == 2) {
    if (strcasecmp(argv[1], "-read_csv") == 0)     read_csv();
    if (strcasecmp(argv[1], "-read_data") == 0)    read_data();
    if (strcasecmp(argv[1], "-add_data") == 0)     add_data();
    if (strcasecmp(argv[1], "-update_data") == 0)  update_data();
  }
  else if (argc > 2) {
    printf("one at a time!\n");
  }
  else {
    printf("no command detected\n");
  }

  return 0;

}
