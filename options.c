#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include<sys/stat.h>
#include "lsheader.h"
#include <limits.h>

extern char *opts;

// function to handle '-a' option 
void a_option(int show_hidden, int args_count, char **args_only)
{
    CheckNoArg(args_count, show_hidden);
    for (int i = 1; i < args_count; i++) {
	printf("Directory listing of %s:\n", args_only[i]);
	do_ls(args_only[i], show_hidden);
    }

}
// function to handle '-i' option 
void i_option(int show_hidden, int args_count, char **args_only)
{
    CheckNoArg(args_count, show_hidden);
    for (int i = 1; i < args_count; i++) {
	printf("Directory listing of %s:\n", args_only[i]);
	do_ls(args_only[i], show_hidden);
    }

}
// function print the entries with inode number
void print_inode(int terminal_width, int entries_count, int max_len,
		 char **files, long inodes[], char *dir_path)
{
    // Calculate the number of columns
    float cols;
    char type;
    if (strchr(opts, '1') != NULL)
	cols = 1;
    else
	cols = ceilf(terminal_width / (max_len + sizeof((char *)inodes[0]) + 2));	// +2 for space between columns
    // Print files in columns
    for (int i = 0; i < entries_count; i++) {
	type = get_type(files[i], dir_path);
	if (type == 'd')
	    printf("%7li \033[1;34m%-*s\033[0m", inodes[i],
		   (int) (max_len + 2), files[i]);
	else if(type == 'x')
	    printf("%7li \033[1;32m%-*s\033[0m", inodes[i],
		   (int) (max_len + 2), files[i]);
	else
	    printf("%7li %-*s", inodes[i], (int) (max_len + 2), files[i]);
	if ((i + 1) % (int) cols == 0) {
	    printf("\n");	// New line after filling the row
	}
    }
    printf("\n\n");
}
// function to get the inode number
long int get_ino(char *dir_path)
{
    struct dirent *entry;
    DIR *dp = opendir(dir_path);
    if (dp == NULL) {
	fprintf(stderr, "Cannot open directory:%s\n", dir_path);
	exit(0);
    }
    errno = 0;
    entry = readdir(dp);
    if (entry == NULL && errno != 0) {
	perror("readdir failed");
	exit(1);
    } else {
	closedir(dp);
	return entry->d_ino;
    }
}
// function to handle the '-d' option.
void d_option(int args_count, int show_hidden, char **args_only)
{
    if (strchr(opts, 'i') != NULL) {
	if (args_count == 1) {
	    printf("%li .\n\n", get_ino("."));
	} else {
	    for (int i = 1; i < args_count; i++) {
		if (strchr(opts, '1') != NULL)
		    printf("%li \033[1;34m%s\033[0m\n", get_ino(args_only[i]),
			   args_only[i]);
		else
		    printf("%7li \033[1;34m%-*s\033[0m", get_ino(args_only[i]),
			   (int) sizeof(args_only[i]), args_only[i]);
	    }
	    printf("\n\n");

	}
    } else {
	if (args_count == 1)
	    printf(".\n\n");
	else {
	    for (int i = 1; i < args_count; i++) {
		if (strchr(opts, '1') != NULL)
		    printf("\033[1;34m%s\033[0m\n", args_only[i]);
		else
		    printf("\033[1;34m%-*s\033[0m", (int) sizeof(args_only[i]),
			   args_only[i]);
	    }
	    printf("\n\n");
	}
    }

}
// function to get group name from group id.
void get_grp_name(long unsigned grp_id, char **grp_name)
{
    errno = 0;
    struct group *grp = getgrgid(grp_id);

    errno = 0;
    if (grp == NULL) {
	if (errno == 0)
	    printf("Record not found in /etc/group file.\n");
	else
	    perror("getgrgid failed");
    } else
	*grp_name = strdup(grp->gr_name);

}
// function to get the user name from user id.
void get_user_name(long unsigned user_id, char **user_name)
{
    errno = 0;
    struct passwd *pwd = getpwuid(user_id);

    if (pwd == NULL) {
	if (errno == 0)
	    printf("Record not found in passwd file.\n");
	else
	    perror("getpwuid failed");
    } else
	*user_name = strdup(pwd->pw_name);

}
// function to get contents of the inode.
void get_inode_data(long links[], char *user_name[], char *grp_name[],
		    long blksize[], char *str_time[], int entries_count,
		    char *entries[], const char *arg)
{
    struct stat buf;
    char entrypath[PATH_MAX] = { 0 };
    long time[5000];

    for (int i = 0; i < entries_count; i++) {

	get_path(entrypath, arg, entries[i]);

	if (lstat(entrypath, &buf) < 0) {
	    perror("Error in stat");
	    exit(1);
	}

	links[i] = buf.st_nlink;
	get_user_name(buf.st_uid, &user_name[i]);
	get_grp_name(buf.st_gid, &grp_name[i]);
	blksize[i] = buf.st_size;
	if (strchr(opts, 'c') && strchr(opts, 'u')) {
	    if (strchr(opts, 'c') > strchr(opts, 'u'))
		time[i] = buf.st_ctime;
	    else
		time[i] = buf.st_atime;
	} else {
	    if (strchr(opts, 'c'))
		time[i] = buf.st_ctime;
	    else if (strchr(opts, 'u'))
		time[i] = buf.st_atime;
	    else
		time[i] = buf.st_mtime;
	}
	str_time[i] = strdup(ctime(&time[i]));
	strtok(str_time[i], "\n");
    }
}
// function to get the path of each file name.
void get_path(char *entrypath, const char *arg, char *entry)
{
    strcpy(entrypath, arg);
    strncat(entrypath, "/", 2);
    strncat(entrypath, entry, strlen(entry));

}
// function to get the permissions of each file.
void get_permission(char *permissions[], char *entries[],
		    int entries_count, const char *arg)
{
    struct stat buf;
    char entrypath[PATH_MAX] = { 0 };
    int mode = 0;
    char *str;
    for (int i = 0; i < entries_count; i++) {

	get_path(entrypath, arg, entries[i]);

	if (lstat(entrypath, &buf) < 0) {
	    perror("Error in stat");
	    exit(1);
	}
	mode = buf.st_mode;
	str = (char *) calloc(11, sizeof(char));
	strcpy(str, "----------");
//owner  permissions
	if ((mode & S_IRUSR) == S_IRUSR)
	    str[1] = 'r';
	if ((mode & S_IWUSR) == S_IWUSR)
	    str[2] = 'w';
	if ((mode & S_IXUSR) == S_IXUSR)
	    str[3] = 'x';
//group permissions
	if ((mode & S_IRGRP) == S_IRGRP)
	    str[4] = 'r';
	if ((mode & S_IWGRP) == S_IWGRP)
	    str[5] = 'w';
	if ((mode & S_IXGRP) == S_IXGRP)
	    str[6] = 'x';
//others  permissions
	if ((mode & S_IROTH) == S_IROTH)
	    str[7] = 'r';
	if ((mode & S_IWOTH) == S_IWOTH)
	    str[8] = 'w';
	if ((mode & S_IXOTH) == S_IXOTH)
	    str[9] = 'x';
//special  permissions
	if ((mode & S_ISUID) == S_ISUID)
	    str[3] = 's';
	if ((mode & S_ISGID) == S_ISGID)
	    str[6] = 's';
	if ((mode & S_ISVTX) == S_ISVTX)
	    str[9] = 't';
//File Type
	switch (mode & S_IFMT) {
	case S_IFSOCK: str[0] = 's'; break;
	case S_IFLNK:  str[0] = 'l'; break;
	case S_IFREG:  str[0] = '-'; break;
	case S_IFBLK:  str[0] = 'b'; break;
	case S_IFDIR:  str[0] = 'd'; break;
	case S_IFCHR:  str[0] = 'c'; break;
	case S_IFIFO:  str[0] = 'p'; break;
	}
	permissions[i] = str;
    }
}

// function to print the entries in long format.
void print_l_columns(char *entries[], int entries_count, long inodes[],
		     char *permissions[], long links[], char *user_name[],
		     char *grp_name[], long blksize[], char *str_time[])
{
    int max_len_uname = get_max_len(user_name, entries_count);
    int max_len_gname = get_max_len(grp_name, entries_count);
    char *blksize_char[5000];
    for (int i = 0; i < entries_count; i++) {
	blksize_char[i] = (char *) malloc(20 * sizeof(char));
	sprintf(blksize_char[i], "%ld", blksize[i]);
    }
    int max_len_blksize = get_max_len(blksize_char, entries_count);
    for (int i = 0; i < entries_count; i++) {
	if (strchr(opts, 'i') != NULL) {
	    printf("%7li %s %3ld %-*s %-*s %*ld %-*s %s\n", inodes[i],
		   permissions[i], links[i], max_len_uname, user_name[i],
		   max_len_gname, grp_name[i], max_len_blksize, blksize[i],
		   (int) strlen(str_time[0]), str_time[i], entries[i]);
	} else {
	    printf("%s %3ld %-*s %-*s  %*ld %-*s %s\n", permissions[i],
		   links[i], max_len_uname, user_name[i], max_len_gname,
		   grp_name[i], max_len_blksize, blksize[i],
		   (int) strlen(str_time[0]), str_time[i], entries[i]);
	}
	free(permissions[i]);
	free(entries[i]);
	free(user_name[i]);
	free(grp_name[i]);
	free(blksize_char[i]);
	free(str_time[i]);
    }
    printf("\n");

}
// function to list with long format for each given path.
void l_option_WithArg(char **args_only, int args_count, int show_hidden)
{
    char **entries = NULL;
    long inodes[5000];
    int entries_count = 0;
    char *permissions[5000] = { NULL };
    long links[5000];
    char *user_name[5000] = { NULL };
    char *grp_name[5000] = { NULL };
    long blksize[5000];
    char *str_time[5000] = { NULL };

    for (int i = 1; i < args_count; i++) {

	entries_count =
	    get_dir_entries(&entries, inodes, args_only[i], show_hidden);
	if (!strchr(opts, 't'))
	    sort_alpha(entries, entries_count, inodes);	// sort alphapetically
	else
	    get_sort_time(entries, entries_count, inodes, args_only[i]);

	get_permission(permissions, entries, entries_count,
		       (const char *) args_only[i]);
	get_inode_data(links, user_name, grp_name, blksize, str_time,
		       entries_count, entries, args_only[i]);

	printf("Directory listing of %s:\n", args_only[i]);
	print_l_columns(entries, entries_count, inodes, permissions, links,
			user_name, grp_name, blksize, str_time);
    }
}
// function to list with long format for the cwd.
void l_option_NoArg(int show_hidden)
{
    char **entries =  NULL;
    long inodes[5000];
    int entries_count = 0;
    char *permissions[5000] = { NULL };
    long links[5000];
    char *user_name[5000] = { NULL };
    char *grp_name[5000] = { NULL };
    long blksize[5000];
    char *str_time[5000] = { NULL };

    entries_count = get_dir_entries(&entries, inodes, ".", show_hidden); // get entries
    if (!strchr(opts, 't'))
	sort_alpha(entries, entries_count, inodes);	              // sort alphapetically if '-t' not specified
    else
	get_sort_time(entries, entries_count, inodes, ".");           // sort by time
    get_permission(permissions, entries, entries_count, ".");          
    get_inode_data(links, user_name, grp_name, blksize, str_time,
		   entries_count, entries, ".");
    printf("Directory listing of cwd:\n");
    print_l_columns(entries, entries_count, inodes, permissions, links,
		    user_name, grp_name, blksize, str_time);

}
// function to handle '-l' optoin
void l_option(int show_hidden, char **args_only, int args_count)
{
    if (strchr(opts, 'a') != NULL)
	show_hidden = 1;
    if (args_count == 1) {
	l_option_NoArg(show_hidden); // list with long format for the cwd
    } else {
	l_option_WithArg(args_only, args_count, show_hidden);
    }


}
// function to handle sorting options
void sort_options(int args_count, int show_hidden, char **args_only)
{
    CheckNoArg(args_count, show_hidden);
    for (int i = 1; i < args_count; i++) {
	printf("Directory listing of %s:\n", args_only[i]);
	do_ls(args_only[i], show_hidden);
    }
}
// function to handle '-1' option
void oneline_option(int args_count, int show_hidden, char **args_only)
{
    CheckNoArg(args_count, show_hidden);
    for (int i = 1; i < args_count; i++) {
	printf("Directory listing of %s:\n", args_only[i]);
	do_ls(args_only[i], show_hidden);
    }
}
