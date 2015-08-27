/**
 * Convert a Ascii file to a binary file.
 * *************************************************
 *
 * Author: Shuo Yang
 * Email: imsure95@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct birth_record {
  char last_name[50];
  char first_name[50];
  char birth_city[50];
  int age;
  int birth_year;
  int birth_month;
  int birth_day;
} birth_record;

/**
 * The main program.
 */
int main( int argc, char *argv[] )
{
  int record_index;
  int fd_ascii, fd_bin;

  struct stat stat_buf_ascii, stat_buf_bin;

  FILE *fp_ascii = fopen("record1.txt", "r");
  FILE *fp_bin = fopen("record1.bin", "w");

  birth_record record;
  while (fscanf(fp_ascii, "%s %s %d %d %d %d %s\n",
		record.first_name, record.last_name,
		&record.age, &record.birth_year,
		&record.birth_month, &record.birth_day,
		record.birth_city) != EOF) {
    fwrite(&record, sizeof(record), 1, fp_bin);
  }

  fclose(fp_ascii);
  fclose(fp_bin);

  fp_bin = fopen("record1.bin", "r");
  record_index = 0;
  while (fread(&record, sizeof(record), 1, fp_bin) != 0) {
    printf("record %d:\n", record_index++);
    printf("%s %s %d %d %d %d %s\n", record.first_name, record.last_name,
	   record.age, record.birth_year, record.birth_month,
	   record.birth_day, record.birth_city);
  }

  fclose(fp_bin);

  fd_ascii = open("record1.txt", O_RDONLY);
  fd_bin = open("record1.bin", O_RDONLY);

  fstat(fd_ascii, &stat_buf_ascii);
  fstat(fd_bin, &stat_buf_bin);

  printf("Size of ascii file:  %lld bytes\n", stat_buf_ascii.st_size);
  printf("Size of binary file: %lld bytes\n", stat_buf_bin.st_size);
  
  return 0;
}
