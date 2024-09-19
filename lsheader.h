extern char *opts;
int getargs(char **args, int argc, char **argv);
void do_ls(char *dir_path, int show_hidden);
int get_max_len(char *entries[], int entries_count);

void CheckNoArg(int args_count, int show_hidden);
void CheckNoOpt(int args_count, int argc, int show_hidden,
		char **args_only);
void HandleOpts(char **argv, int args_count, int argc, int show_hidden,
		char **args_only);
void getopts(int argc, char **argv);
static int cmpstringp(const void *p1, const void *p2);
void sort_alpha(char **entries, int entries_count, long inodes[]);
void get_sort_time(char **entries, int entries_count, long inodes[],
		   char *dir_path);
void sort_time(long time[], char **entries, int entries_count,
	       long inodes[]);
void a_option(int show_hidden, int args_count, char **args_only);
void print_inode(int terminal_width, int entries_count, int max_len,
		 char **files, long inodes[], char *dir_path);
void i_option(int show_hidden, int args_count, char **args_only);
void d_option(int args_count, int show_hidden, char **args_only);
long int get_ino(char *dir_path);
int get_dir_entries(char *entries[], long inodes[], char *dir_path,
		    int show_hidden);
void l_option(int show_hidden, char **args_only, int args_count);
void get_permission(char *permissions[], char *entries[],
		    int entries_count, const char *arg);
void get_path(char *entrypath, const char *arg, char *entry);
void get_inode_data(long links[], char *user_name[], char *grp_name[],
		    long blksize[], char *str_time[], int entries_count,
		    char *entries[], const char *arg);
void l_option_NoArg(int show_hidden);
void l_option_WithArg(char **args_only, int args_count, int show_hidden);

void get_user_name(long unsigned user_id, char **user_name);
void get_grp_name(long unsigned grp_id, char **grp_name);
void print_l_columns(char *entries[], int entries_count, long inodes[],
		     char *permissions[], long links[], char *user_name[],
		     char *grp_name[], long blksize[], char *str_time[]);
void sort_options(int args_count, int show_hidden, char **args_only);
void oneline_option(int args_count, int show_hidden, char **args_only);
char get_type(char *entry, char *dir_path);
