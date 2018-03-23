#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef enum {
    BEFORE, AFTER, SAME_DAY
} date_cmp;

const char* progName;
struct stat buf;
char mode[10] = {0};
int timeInSec;
date_cmp cmp;

void showHelp () {
    printf ("Use %s <path> [compare] [Month] [Day] \n"
	    " path – program path \n"
	    " compare – < or > or = \n"
	    " Month – Mar\n"
	    " Day – 22\n\n"
	    "Example: %s ~/Documents/ > Mar 22\n"
	    , progName, progName);
    exit (0);
}

void strmode (mode_t mode, char * buf) {
  const char chars[] = "rwxrwxrwx";
  for (size_t i = 0; i < 9; i++) {
    buf[i] = (mode & (1 << (8-i))) ? chars[i] : '-';
  }
  buf[9] = '\0';
}

void stat_searchDir (DIR *dp, struct dirent *dirp) {
    while ((dirp = readdir(dp)) != NULL) {
	if ((stat (dirp->d_name, &buf)) == 0) {
	    printf("Filename: %s\n", dirp->d_name);
	    strmode (buf.st_mode, mode);
	    printf ("Total size, in bytes: %lu\n", buf.st_size);
	    printf ("Mode: %s \n", mode);
	    printf ("Last file modification: %s", ctime(&buf.st_mtime));
	    int isDisplayed = 0;
	    int timediff = timeInSec - buf.st_mtime;
	    if (cmp == BEFORE) 
		isDisplayed = timediff > 60*60*24;
	    else if (cmp == AFTER)
		isDisplayed = timediff < 0;
	    else if (cmp == SAME_DAY)
		isDisplayed = timediff < 60*60*24 && timediff > 0;
	    printf ("Is displayed = %d\n\n", isDisplayed);
	}
	if ((chdir (dirp->d_name)) < 0) {
	} else {

	}
    }
}

int intmonth (char *month) {
    if (strcmp (month, "Jan") == 0) return 0;
    if (strcmp (month, "Feb") == 0) return 1;
    if (strcmp (month, "Mar") == 0) return 2;
    if (strcmp (month, "Apr") == 0) return 3;
    if (strcmp (month, "May") == 0) return 4;
    if (strcmp (month, "Jun") == 0) return 5;
    if (strcmp (month, "Jul") == 0) return 6;
    if (strcmp (month, "Aug") == 0) return 7;
    if (strcmp (month, "Sep") == 0) return 8;
    if (strcmp (month, "Oct") == 0) return 9;
    if (strcmp (month, "Nov") == 0) return 10;
    if (strcmp (month, "Dec") == 0) return 11;
    else return 0;
}

int main (int argc, char* argv[]) {

    srand (time (NULL));
    progName = argv[0];
    if (argc == 4) 
	showHelp();

    DIR *dp;
    struct dirent *dirp = NULL;
    if ((dp = opendir(argv[1])) == NULL) 
	printf ("Error when opening the %s\n", argv[1]);

    if (strcmp (argv[2], "<") == 0) 
	cmp = BEFORE;
    else if (strcmp (argv[2], ">") == 0)
	cmp = AFTER;
    else if (strcmp (argv[2], "=") == 0)
	cmp = SAME_DAY;

    char *month = argv[3];
    int day = atoi (argv[4]);

    struct tm givenTime = {0,0,0,0,0,0,0,0,0,0,0};
    givenTime.tm_sec = 59;
    givenTime.tm_min = 59;
    givenTime.tm_hour = 23;
    givenTime.tm_mday = day;
    givenTime.tm_mon = intmonth (month);
    givenTime.tm_year = 2018 - 1900;
    timeInSec = mktime (&givenTime);

    stat_searchDir (dp, dirp);

    closedir(dp);
    return 0;
}


