#define DANGO_RL_BUFSIZE 1024

#define DANGO_TOK_BUFSIZE 64
#define DANGO_TOK_DELIM " \t\r\n\a"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int dango_cd(char **args);
int dango_self(char **args);
int dango_exit(char **args);

char* username;
char hostname[20];

char *builtin_str[] = 
{
	"cd",
	"dango",
	"exit"
};

int (*builtin_func[]) (char **) = 
{
	&dango_cd,
	&dango_self,
	&dango_exit
};

int dango_num_builtins() 
{
	return sizeof(builtin_str) / sizeof(char *);
}

int dango_cd(char **args)
{
	if (args[1] == NULL) {
		
	  char* path = NULL;
		int usernameLen = strlen(username);
		path = malloc(6 + usernameLen + 1);
		
		strcpy(path, "/home/");
		strcat(path, username);
		chdir(path);
		free(path);

	} else {
		if (chdir(args[1]) != 0) {
			perror("dango");
		}
	}

	return 1;
}

int dango_self(char **args) 
{
	if (args[1] == NULL) {
		return 1;
	}

	if (strcmp(args[1], "--version") == 0) {
		printf("dango 0.0.2\n");
		return 1;
	}
	
	if (strcmp(args[1], "--help") == 0) {

		int num_cmds = dango_num_builtins()-1;

		for (int i = 0; i < num_cmds; i++) {
			printf(" %s,", builtin_str[i]);
		}

		printf(" %s", builtin_str[num_cmds]);
		printf("\n");

		return 1;
	}
}

int dango_exit(char **args) 
{
	return 0;
}

char *dango_read_line(void)
{
	char *line = NULL;
	ssize_t bufsize = 0;

	if (getline(&line, &bufsize, stdin) == -1) {
		if (feof(stdin)) {
			exit(EXIT_SUCCESS);
		} else {
			perror("readline");
			exit(EXIT_FAILURE);
		}
	}

	return line;
}

char **dango_split_line(char *line)
{
	int bufsize = DANGO_TOK_BUFSIZE, position = 0;
	char **tokens = (char**)malloc(bufsize * sizeof(char*));
	char *token, **tokens_backup;

	if (!tokens) {
		fprintf(stderr, "dango: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, DANGO_TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += DANGO_TOK_BUFSIZE;
			tokens_backup = tokens;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "dango: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, DANGO_TOK_DELIM);
	}

	tokens[position] = NULL;
	return tokens;
}

int dango_launch(char **args)
{
	pid_t pid;
	int status;

	pid = fork();

	if (pid == 0) {
		if (execvp(args[0], args) == -1) {
			fprintf(stderr, "dango: command not found: %s\n", args[0]);
		}

		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		perror("dango");
	} else {
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

int dango_execute(char **args)
{
if (args[0] == NULL) {
		return 1;
	}

	for (int i = 0; i < dango_num_builtins(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}

	return dango_launch(args);
}

void dango_loop(void)
{
	char *line;
	char **args;
	int status;

	char cwd[30];

	do
	{
		printf("[%s@%s]$ ", username, hostname);
		line = dango_read_line();
		args = dango_split_line(line);
		status = dango_execute(args);

		free(line);
		free(args);
	} while (status);
}

int main(int argc, char **argv)
{
	username = getlogin();

	gethostname(hostname, 20);

	dango_loop();

	return EXIT_SUCCESS;
}
