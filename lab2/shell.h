#ifndef SHELL_H
#define SHELL_H

#define BUF_SIZE 200

#define TRUE 1
#define FALSE 0

#define READ 0
#define WRITE 1

#define PIPE '|'
#define GREATER '>'
#define LESS '<'
#define AMP '&'

#define TOK_BUFSIZE 128
#define TOK_DELIM " \t\r\n\a"

// main shell program
void shell_loop();
// read line
char *read_line();
// split line
char **split_line(char *);
// process launch
int proc_launch(char **);
// shell execute
int sh_execute(char **);
// Fork wrapper
pid_t Fork(void);

/*
    BUILT IN COMMANDS
*/
int num_builtins();
// cd command
int sh_cd(char **);
// help command
int sh_help(char **);
// quit command
int sh_quit(char **);
// echo command
int sh_echo(char **);
// environ command
int sh_environ(char **);
// export command
int sh_export(char **);

#endif