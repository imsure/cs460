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

#define MAX_BUF_LEN 4096

typedef struct record {
  int age;
  int birth_year;
  int birth_month;
  int birth_day;
  char *text;
} record;

void write_binary_file(FILE *fp_ascii, FILE *fp_bin)
{
  char buf[MAX_BUF_LEN];
  int age, year, month, day;
  char fname[50];
  char lname[50];
  char birth_city[50];
  int record_len;
  
  while (fscanf(fp_ascii, "%s %s %d %d %d %d %s\n",
		fname, lname, &age, &year, &month,
		&day, birth_city) != EOF) {
    /* Concatenate strings together */
    strcat(buf, lname);
    strcat(buf, ",");
    strcat(buf, fname);
    strcat(buf, ",");
    strcat(buf, birth_city);
    printf("buf: %s\n", buf);

    // 5 ints, including length of records and string buf.
    record_len = sizeof(int) * 5 + strlen(buf)+1;
      
    fwrite(&record_len, sizeof(int), 1, fp_bin);
    fwrite(&age, sizeof(int), 1, fp_bin);
    fwrite(&year, sizeof(int), 1, fp_bin);
    fwrite(&month, sizeof(int), 1, fp_bin);
    fwrite(&day, sizeof(int), 1, fp_bin);
    fwrite(buf, strlen(buf)+1, 1, fp_bin);

    memset(buf, 0, sizeof(buf)); // reset buffer
  }
}

void show_reocrds(FILE *fp_bin)
{
  int age, year, month, day, record_len, buf_len;
  char *buf;

  while (fread(&record_len, sizeof(int), 1, fp_bin) > 0) {
    fread(&age, sizeof(int), 1, fp_bin);
    fread(&year, sizeof(int), 1, fp_bin);
    fread(&month, sizeof(int), 1, fp_bin);
    fread(&day, sizeof(int), 1, fp_bin);
    buf_len = record_len - 5*sizeof(int);
    buf = (char *) malloc(buf_len);
    fread(buf, buf_len, 1, fp_bin);
    printf("Text: %s, age=%d, year=%d, month=%d, day=%d\n",
	   buf, age, year, month, day);
    free(buf);
  }
}

/**
 * The main program.
 */
int main( int argc, char *argv[] )
{
  int fd_ascii, fd_bin;
  struct stat stat_buf_ascii, stat_buf_bin;

  FILE *fp_ascii = fopen("record1.txt", "r");
  FILE *fp_bin = fopen("record1.bin", "w+");

  write_binary_file(fp_ascii, fp_bin);

  rewind(fp_bin); // reset point to the beginning of the file.
  show_reocrds(fp_bin);

  fclose(fp_ascii);
  fclose(fp_bin);

  fd_ascii = open("record1.txt", O_RDONLY);
  fd_bin = open("record1.bin", O_RDONLY);

  fstat(fd_ascii, &stat_buf_ascii);
  fstat(fd_bin, &stat_buf_bin);

  printf("Size of ascii file:  %lld bytes\n", stat_buf_ascii.st_size);
  printf("Size of binary file: %lld bytes\n", stat_buf_bin.st_size);
  
  return 0;
}
