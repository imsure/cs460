/**
 * Convert a Ascii file to a binary file.
 * *************************************************
 *
 * Author: Shuo Yang
 * Email: imsure95@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_BUF_LEN 1024 // max length allocated for a buffer.
#define MAX_STR_SZ 256 // max size used for each string field in a record.

typedef struct record_stats {
  int max_len_lname; // maximum length for last name
  int max_lname_record_index; // the index of the reard that contains the max last name
  
  int max_len_fname; // maximum length for first name
  int max_fname_record_index; 
  
  int max_len_mname; // maximum length for middle name
  int max_mname_record_index;

  int max_len_suffix; // maximum length for suffix
  int max_suffix_record_index;

  int max_len_birthcity; // maximum length for birth city
  int max_birthcity_record_index;

  /* Mapping from number of fields to number of records that
     have this many of fields.
     index of the array represents number of fields;
     the value of array represents the number of records. */
  int fields_count[9];
} record_stats;

void parse_line(const char *line, int line_num, record_stats *stats)
{
  int i, tab_counter;  
  int len = strlen(line);

  char lname[MAX_STR_SZ];
  char fname[MAX_STR_SZ];
  char mname[MAX_STR_SZ];
  char suffix[MAX_STR_SZ];
  int birth_month, birth_day, birth_year;
  char gender;
  char birth_city[MAX_STR_SZ];
  char mon[3];  // at most 2 digits
  char day[3];  // at most 2 digits
  char year[5]; // exactly 5 digits 

  /* Initialize each field: char as 0,
     string as a empty string, int as -1, */
  memset(lname, 0, MAX_STR_SZ);
  memset(fname, 0, MAX_STR_SZ);
  memset(mname, 0, MAX_STR_SZ);
  memset(suffix, 0, MAX_STR_SZ);
  birth_month = birth_day = birth_year = -1;
  gender = '\0';
  memset(birth_city, 0, MAX_STR_SZ);

  tab_counter = 0;
  for (i = 0; i < len; ++i) {
    if (line[i] == '\t') { // fields are separated by tab.
      tab_counter++;
      i++; // advance index to avoid tab
      /* skip consecutive tabs which represents missing field. */
      if (line[i] == '\t') { 
	tab_counter++;
	continue;
      }
    }

    /* Process each field according to the number of tabs encountered. */
    switch (tab_counter) {
    case 0: // last name
      lname[strlen(lname)] = line[i];
      break;
    case 1: // first name
      fname[strlen(fname)] = line[i];
      break;
    case 2: // middle name
      mname[strlen(mname)] = line[i];
      break;
    case 3: // suffix
      suffix[strlen(suffix)] = line[i];
      break;
    case 4: // month
      memset(mon, 0, 3);
      if (isdigit(line[i])) { // first digit
	mon[0] = line[i];
      } else {
	fprintf(stderr, "Bad record at line %d: month field contains a" \
		" non-digit character:%c\n", line_num, line[i]);
	fprintf(stderr, "%s\n", line);
	exit(-1);
      }

      if (isdigit(line[i+1])) { // second digit if any
	mon[1] = line[i+1];
	i += 1;
      } else if (line[i+1] != '\t') { // neither a digit nor a tab, sth goes wrong
	fprintf(stderr, "Bad record at line %d: month field contains a non-digit" \
		" character after a digit character:%c\n", line_num, line[i+1]);	
	fprintf(stderr, "%s\n", line);
	exit(-1);
      }
      birth_month = atoi(mon);
      break;
    case 5: // day
      memset(day, 0, 3);
      if (isdigit(line[i])) { // first digit
	day[0] = line[i];
      } else {
	fprintf(stderr, "Bad record at line %d: day field contains a" \
		" non-digit character:%c\n", line_num, line[i]);
	fprintf(stderr, "%s\n", line);
	exit(-1);
      }

      if (isdigit(line[i+1])) { // second digit if any
	day[1] = line[i+1];
	i += 1;
      } else if (line[i+1] != '\t') { // neither a digit nor a tab, sth goes wrong
	fprintf(stderr, "Bad record at line %d: day field contains a non-digit" \
		" character after a digit character:%c\n", line_num, line[i+1]);	
	fprintf(stderr, "%s\n", line);
	exit(-1);
      }
      birth_day = atoi(day);   
      break;
    case 6: // year
      memset(year, 0, 5);
      if (isdigit(line[i]) && isdigit(line[i+1]) &&
		  isdigit(line[i+2]) && isdigit(line[i+3])) {
	year[0] = line[i];
	year[1] = line[i+1];
	year[2] = line[i+2];
	year[3] = line[i+3];
	i += 3;
	birth_year = atoi(year);
      } else {
	fprintf(stderr, "Bad record at line %d: year field contains" \
		" non-digit characters:%c\n", line_num, line[i]);
	fprintf(stderr, "%s\n", line);
	exit(-1);
      }

      if (line[i+1] != '\t') { // neither a digit nor a tab, sth goes wrong
	fprintf(stderr, "Bad record at line %d: year field contains a non-digit" \
		" character after 4 digit characters:%c\n", line_num, line[i+1]);
	fprintf(stderr, "%s\n", line);
	exit(-1);
      }
      break;
    case 7:
      if (line[i] != 'F' && line[i] != 'M') {
	fprintf(stderr, "Bad record at line %d: gender field contains" \
		" character:%c other than F or M!\n", line_num, line[i]);
	fprintf(stderr, "%s\n", line);
	exit(-1);	
      } else {
	gender = line[i];
      }

      if (line[i+1] != '\t') { // neither a digit nor a tab, sth goes wrong
	fprintf(stderr, "Bad record at line %d: year field contains a non-digit" \
		" character after 4 digit characters:%c\n", line_num, line[i+1]);	
	fprintf(stderr, "%s\n", line);
	exit(-1);
      }
      break;
    case 8:
      birth_city[strlen(birth_city)] = line[i];
      break;
    default:
      fprintf(stderr, "Bad record at line %d: more than 8 tabs appeared in the record:\n%s\n", 
	      line_num, line);
      break;
    }
  }

  /* Gather statistics. */
  if (stats->max_len_lname < (int)strlen(lname)) {
    stats->max_len_lname = strlen(lname);
    stats->max_lname_record_index = line_num;
  }
  if (stats->max_len_fname < (int)strlen(fname)) {
    stats->max_len_fname = strlen(fname);
    stats->max_fname_record_index = line_num;
  }
  if (stats->max_len_mname < (int)strlen(mname)) {
    stats->max_len_mname = strlen(mname);
    stats->max_mname_record_index = line_num;
  }
  if (stats->max_len_suffix < (int)strlen(suffix)) {
    stats->max_len_suffix = strlen(suffix);
    stats->max_suffix_record_index = line_num;
  }
  if (stats->max_len_birthcity < (int)strlen(birth_city)) {
    stats->max_len_birthcity = strlen(birth_city);
    stats->max_birthcity_record_index = line_num;
  }

  stats->fields_count[tab_counter]++;

  /* printf("Fildes of record # %d:\n", line_num); */
  /* printf("last name=%s, first name=%s, middle name=%s, suffix=%s, " \ */
  /* 	 "birth month=%d, birth day=%d, birth year=%d, gender=%c, birth city=%s\n", */
  /* 	 lname, fname, mname, suffix, birth_month, birth_day, birth_year, gender, birth_city); */
}

void sanity_check_and_stats_gathering(FILE *fp_ascii)
{
  char buf[MAX_BUF_LEN];
  int rec_counter, i;
  record_stats stats;

  memset(&stats, 0, sizeof(stats)); // init record stats 
  rec_counter = 0;
  while (fgets(buf, sizeof(buf), fp_ascii) != NULL) {
    rec_counter++;
    buf[strlen(buf)-1] = '\0'; // get rid of newline: '\n'.
    parse_line(buf, rec_counter, &stats);
  }

  /* Display stats collected. */
  printf("Number of records = %d\n", rec_counter);
  printf("Number of records with < 2 fields: %d\n", (stats.fields_count)[0]);
  for (i = 1; i < 9; ++i) {
    printf("Number of records with  %d  fields: %d\n", i+1, (stats.fields_count)[i]);
  }
  printf("Maximum length of last name: %d at line %d\n",
	 stats.max_len_lname, stats.max_lname_record_index);
  printf("Maximum length of first name: %d at line %d\n",
	 stats.max_len_fname, stats.max_fname_record_index);
  printf("Maximum length of middle name: %d at line %d\n",
	 stats.max_len_mname, stats.max_mname_record_index);
  printf("Maximum length of suffix: %d at line %d\n",
	 stats.max_len_suffix, stats.max_suffix_record_index);
  printf("Maximum length of birth city: %d at line %d\n",
	 stats.max_len_birthcity, stats.max_birthcity_record_index);
}

/**
 * The main program.
 */
int main( int argc, char *argv[] )
{
  int fd_ascii, fd_bin;

  FILE *fp_ascii = fopen("../TexasBirths1950-1954.txt", "r");
  //FILE *fp_ascii = fopen("../sample.txt", "r");

  sanity_check_and_stats_gathering(fp_ascii);
  return 0;
}
