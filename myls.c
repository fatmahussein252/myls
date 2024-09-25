#include <ctype.h>
#include <stdio.h>
#include <dirent.h>
#include <term.h>
#include <sys/ioctl.h>
#include<sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <limits.h>
#include "lsheader.h"

char *opts = NULL;
/* Description:
   this function is used to get the type of the entry for colored print*/
char get_type(char *entry, char *dir_path)
{
    struct stat buf;
    char entrypath[PATH_MAX] = { 0 };
    int mode = 0;
    char *str;

    get_path(entrypath, dir_path, entry);

    if (lstat(entrypath, &buf) < 0) {
	perror("Error in stat");
	exit(1);
    }
    mode = buf.st_mode;
    switch (mode & S_IFMT) {
    case S_IFDIR:
	return 'd';
    case S_IFREG:
	{
	if ((mode & S_IXUSR) == S_IXUSR || (mode & S_IXGRP) == S_IXGRP || (mode & S_IXOTH) == S_IXOTH)
	return 'x';  
	}
    }
}
/* Description:
   this function is used to find the longest file name*/
int get_max_len(char *entries[], int entries_count)
{
    int len = 0;
    int max_len = 0;
    for (int i = 0; i < entries_count; i++) {
	len = strlen(entries[i]);
	if (len > max_len) {
	    max_len = len;
	}
    }
    return max_len;
}
/* Description:
   this function is used to print entries in table format.*/
void print_in_columns(char **files, int entries_count, int terminal_width,
		      long inodes[], char *dir_path)
{
    int max_len = 0;
    float cols;
    char type;
    // Find the longest file name
    max_len = get_max_len(files, entries_count); 

    if (opts != NULL && strchr(opts, 'i') != NULL) // print inode if 'i' specified.
	print_inode(terminal_width, entries_count, max_len, files, inodes,
		    dir_path);
    else {
	// Calculate the number of columns
	if (opts != NULL && strchr(opts, '1') != NULL)
	    cols = 1;
	else
	    cols = ceilf(terminal_width / (max_len + 2));	// +2 for space between columns
	// Print files in columns
	for (int i = 0; i < entries_count; i++) {
	    type = get_type(files[i], dir_path);
	    if (type == 'd')
		printf("\033[1;34m%-*s\033[0m", max_len + 2, files[i]);
	    else if(type == 'x')
	        printf("\033[1;32m%-*s\033[0m", max_len + 2, files[i]);
	    else
		printf("%-*s", max_len + 2, files[i]);
	    if ((i + 1) % (int) cols == 0) {
		printf("\n");	// New line after filling the row
	    }
	}
	printf("\n\n");
    }

}
/* Description:
   this function is used to to get terminal width.*/
int get_terminal_width()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;		// Return the number of columns
}
/* Description:
   this function is used to sort directory entries by time.*/
void sort_time(long time[], char **entries, int entries_count,
	       long inodes[])
{
    for (int i = 0; i < entries_count - 1; i++) {
	for (int j = i + 1; j < entries_count; j++) {
	    if (time[j] > time[i]) {
		// swap time
		long temp_time = time[i];
		time[i] = time[j];
		time[j] = temp_time;
		// Swap entries
		char *temp_entry = entries[i];
		entries[i] = entries[j];
		entries[j] = temp_entry;

		// Swap corresponding inodes
		long temp_inode = inodes[i];
		inodes[i] = inodes[j];
		inodes[j] = temp_inode;
	    }
	}
    }
}
/* Description:
   this function is used to get the time to sort by it according to options specified, then call sort funtion to sort by it. */
void get_sort_time(char **entries, int entries_count, long inodes[],
		   char *dir_path)
{
    struct stat buf;
    char entrypath[PATH_MAX] = { 0 };
    long time[5000];
    char type;

    if (strchr(opts, 'c') && strchr(opts, 'u')) { // if 'c' and 'u' were specified, call sort the last one.
	if (strchr(opts, 'c') > strchr(opts, 'u'))
	    type = 'c';		
	else
	    type = 'a';		
    } else if (strchr(opts, 'c')) // if 'c' only, call sort with the ctime.
	type = 'c';		
    else if (strchr(opts, 'u'))   // if 'u' only, call sort with the atime.
	type = 'a';		
    else if (strchr(opts, 't'))   // if 't' only, call sort with the mtime.
	type = 'm';		

    for (int i = 0; i < entries_count; i++) {

	get_path(entrypath, dir_path, entries[i]); // get path of each entry of the directory.

	if (lstat(entrypath, &buf) < 0) {
	    perror("Error in stat");
	    exit(1);
	}
	if (type == 'c')
	    time[i] = buf.st_ctime;
	else if (type == 'a')
	    time[i] = buf.st_atime;
	else if (type == 'm')
	    time[i] = buf.st_mtime;
	else {
	    printf("Error in sorting with time\n");
	    exit(0);
	}

    }

    sort_time(time, entries, entries_count, inodes); // sort entries by the determined time.
}
/* Description:
   this function is used to sort directory entries alphapetically.*/
void sort_alpha(char **entries, int entries_count, long inodes[])
{

    for (int i = 0; i < entries_count - 1; i++) {
	for (int j = i + 1; j < entries_count; j++) {
	    if (strcasecmp(entries[i], entries[j]) > 0) {
		// Swap entries
		char *temp_entry = entries[i];
		entries[i] = entries[j];
		entries[j] = temp_entry;

		// Swap corresponding inodes
		long temp_inode = inodes[i];
		inodes[i] = inodes[j];
		inodes[j] = temp_inode;
	    }
	}
    }
}
/* Description:
   this function is used to get the entries of each directory (name and inode).*/
int get_dir_entries(char ***entries, long inodes[], char *dir_path,
		    int show_hidden)
{
    struct dirent *entry;
    DIR *dp = opendir(dir_path);
    if (dp == NULL) {
	fprintf(stderr, "Cannot open directory:%s\n", dir_path);
	exit(0);
    }
    errno = 0;
    int entries_count = 0;
    while ((entry = readdir(dp)) != NULL) {
	if (entry == NULL && errno != 0) {
	    perror("readdir failed");
	    exit(1);
	} else {
	    if (entry->d_name[0] == '.' && show_hidden == 0)
		continue;
	    *entries = realloc(*entries, (entries_count+1) *sizeof(char*));
	    (*entries)[entries_count] = strdup(entry->d_name);
	    inodes[entries_count] = entry->d_ino;
	    entries_count++;
	}

    }
    closedir(dp);
    return entries_count;
}
/* Description: 
   this function is used to call mainly three functions to: 
   1- get the entries of each directory
   2- sort them according to the options specified
   3- print them in table format
*/
void do_ls(char *dir_path, int show_hidden)
{
    int terminal_width = get_terminal_width();
    char **entries = NULL;
    long inodes[5000];

    int entries_count =
	get_dir_entries(&entries, inodes, dir_path, show_hidden);

    if (opts != NULL && strchr(opts, 'f'));	// Do not sort
    else if (opts == NULL
	     || (!strchr(opts, 't') && !strchr(opts, 'c')
		 && !strchr(opts, 'u')))
	sort_alpha(entries, entries_count, inodes);	// sort alphapetically
    else
	get_sort_time(entries, entries_count, inodes, dir_path); // sort by time

    print_in_columns(entries, entries_count, terminal_width, inodes,
		     dir_path);
    for (int i = 0; i < entries_count; i++) {
	if (entries[i] != NULL)
	    free(entries[i]);
	entries[i] = NULL;
    }
}
/* Description:
   this funtion is used to check if no directory path was specified to list contents of the cwd.*/  
void CheckNoArg(int args_count, int show_hidden)
{
    if (args_count == 1) {
	printf("Directory listing of cwd:\n");
	do_ls(".", show_hidden);
    }
}
/* Description:
   this funtion is used to get the options.*/  
void getopts(int argc, char **argv)
{
    opts = (char *) calloc(10, sizeof(char));
    int opt;
    int i = 0;
    while ((opt = getopt(argc, argv, ":lai1dfcut?")) != -1) {
	if (opt == '?') {
	    printf("ls: invalid option -- '%c'\n", optopt); 
	    exit(0);
	}
	opts[i] = opt;
	i++;
    }

}
/* Description:
   this funtion is used to check if there are options, get them and handle each one.*/  
void HandleOpt(char **argv, int args_count, int argc, int show_hidden,
	       char **args_only)
{
    if (args_count != argc) {
	getopts(argc, argv); // get the options
	if (strchr(opts, 'd')) {
	    d_option(args_count, show_hidden, args_only);
	    return;
	}
	if (strchr(opts, 'l') && (!strchr(opts, 'f') || strchr(opts, 'i'))) {
	    l_option(show_hidden, args_only, args_count);
	    return;
	}
	if (strchr(opts, 'a') || strchr(opts, 'f')) {
	    show_hidden = 1;
	    a_option(show_hidden, args_count, args_only);
	    return;
	}
	if (strchr(opts, 'i')) {
	    i_option(show_hidden, args_count, args_only);
	    return;
	}
	if (strchr(opts, 't') || strchr(opts, 'c') || strchr(opts, 'u'))
	    sort_options(args_count, show_hidden, args_only);
	if(strchr(opts, '1'))
            oneline_option(args_count, show_hidden, args_only);
    }

}
/* Description:
   this funtion is used to check if no option was specified.*/  
void CheckNoOpt(int args_count, int argc, int show_hidden,
		char **args_only)
{
    if (args_count == argc) {
	CheckNoArg(args_count, show_hidden); // check if no directory path was specified to list contents of the cwd.
	for (int i = 1; i < args_count; i++) {
	    printf("Directory listing of %s:\n", args_only[i]);
	    do_ls(args_only[i], show_hidden);
	}
    }
}
/* Description:
   this funtion is used to get the arguments only of the command (without options).*/  
int getargs(char **args, int argc, char **argv)
{
    int i = 0;
    int args_count = 0;
    for (i = 0; i < argc; i++) {
	if (argv[i][0] == '-')
	    continue;
	args[args_count] =
	    (char *) calloc(strlen(argv[i]) + 1, sizeof(char));
	if (args[args_count] == NULL) {
	    perror("Failed to allocate memory");
	    exit(EXIT_FAILURE);	// Exit on allocation failure.
	}

	strcpy(args[args_count], argv[i]);
	args_count++;
    }
    return args_count;
}

int main(int argc, char *argv[])
{
    int show_hidden = 0;
    char **args_only = (char **) calloc(argc, sizeof(char *));

    if (args_only == NULL) {
	perror("Failed to allocate memory for args_only");
	return 1;		// Exit on allocation failure.
    }


    int args_count = getargs(args_only, argc, argv); // get arguments only from the command.

    CheckNoOpt(args_count, argc, show_hidden, args_only); // check for no options specified.
    HandleOpt(argv, args_count, argc, show_hidden, args_only); // Handle options specified if any.
    
    // free allocated heap memory
    for (int i = 0; i < args_count; i++) {
	if (args_only[i] != NULL)
	    free(args_only[i]);
	args_only[i] = NULL;
    }
    free(args_only);
    if (opts != NULL) {
	free(opts);
	opts = NULL;
    }
}
