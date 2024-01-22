For me, one thing that made it more difficult was a lack of understandment of unix processes. It was not that hard to understand pipes,

```c
int main(int argc, char **argv, char **envp)
{
    // Your code here
}
```


Command Execution with execve()
execve() is a system call used to execute a program. It replaces the current process image with a new process image, specified by a path, and passes arguments and environment variables to it. Key components include:

Path: The file path to the executable you want to run. For example, /bin/ls.
Arguments (argv): An array of strings passed as arguments to the program. The first argument is typically the name of the program.
Environment (envp): An array of strings that represent the environment settings. These are typically a colon-separated list of directories.
For example:
```echo $PATH```
```/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin```
Each directory in this list is a place where your system will search for executable files when you type a command.

So in your program, you will get this string and split it into an array of strings. Then you will search each directory in the array for the executable file you want to run.

You will get PATH from the char **envp parameter. Envp stands for environment pointer.

The envp parameter in a C program contains an array of strings representing the environment variables of the program. These environment variables are key-value pairs that provide various information about the program's environment. The PATH variable is just one of many possible environment variables. Here are some common ones:

HOME: The path to the current user's home directory.
USER: The username of the user running the program.
SHELL: The path to the user's preferred shell (e.g., /bin/bash).
LANG: Specifies the language and character encoding used by the system.
PWD: The current working directory when the program was started.
OLDPWD: The previous working directory.
HOSTNAME: The name of the host system.
DISPLAY: Information about the display (used in graphical environments).
TERM: The terminal type being used.
EDITOR: The default text editor.
PS1, PS2: Defines the primary and secondary prompts in the shell.
PATH: The directories where executables are searched for.
LD_LIBRARY_PATH: Search path for dynamic libraries.
TZ: Timezone setting.

you can list all environment variables by typing ```env``` in your terminal.



The project is designed to deepen students' understanding of UNIX processes, file descriptors, and inter-process communication.

Here's a brief overview of what the project entails:

Objective: The main goal of pipex is to mimic the behavior of a shell pipeline. Specifically, it's designed to replicate the functionality of a command like < infile cmd1 | cmd2 > outfile, where cmd1 and cmd2 are Unix commands, and infile and outfile are files.

Concepts Involved:

File Descriptors: Understanding and using file descriptors to handle input and output streams.
Pipes: Creating and using pipes to redirect the output of one process to the input of another.
Process Creation: Using fork() to create new processes.
Command Execution: Using execve() or similar functions to execute Unix commands within a process.
Redirection: Implementing input and output redirection (similar to <, |, and > in shells).
Implementation Steps:

Read and parse the input arguments (infile, cmd1, cmd2, outfile).
Open infile for reading and outfile for writing.
Create a pipe to connect the output of cmd1 to the input of cmd2.
Use fork() to create processes for cmd1 and cmd2.
In each child process, redirect the appropriate file descriptors (using dup2() or similar) so that cmd1 reads from infile and writes to the pipe, and cmd2 reads from the pipe and writes to outfile.
Execute the commands using execve() or a similar function.
The parent process should wait for both child processes to complete.
Error Handling: Properly handling errors for system calls and edge cases is an important part of the project.

Bonus Features (if applicable): These might include handling multiple pipes (more than two commands in the pipeline), here-documents, and additional error management.




Heredoc, short for "here document", is a feature found in many Unix shells and programming languages, including Bash and Python. It allows you to create a multiline string or stream of input directly within the command line or a script, making it very useful for running commands that require multiline input or embedding a block of text.

In the context of shell scripting, heredoc is often used to feed input to commands interactively or to pass multiline strings to commands. Here's the basic syntax for heredoc in a shell:

bash
Copy code
command <<DELIMITER
multiline
text
DELIMITER
In this syntax, command is the command you want to run, DELIMITER is an identifier you choose to mark the beginning and end of the heredoc, and multiline\ntext is the content you want to pass to the command. The DELIMITER can be any string, as long as it is the same at the beginning and end of the heredoc section.

Example Usage
Feeding Input to a Command:
Suppose you want to provide multiline input to a command like cat:

bash
Copy code
cat <<EOF
Line 1
Line 2
Line 3
EOF
This would output:

mathematica
Copy code
Line 1
Line 2
Line 3
Using with Scripts:
Heredoc can be very useful in scripts to embed large blocks of text. For example, you might use it to write a configuration file:

bash
Copy code
cat <<EOF > config.txt
user=example
path=/var/example
EOF
This creates a file config.txt with the provided content.

Piping to Another Command:
You can also pipe the output of a heredoc to another command. For example:

bash
Copy code
cat <<EOF | grep 'Line'
Line 1
Something else
Line 2
EOF
This will use grep to filter out lines that contain the word "Line".






void get_input_file(t_data *data)
{
	if (data->heredoc_flag == 1)
	{
		handle_heredoc(data);
		data->input_fd = open(".heredoc.tmp", O_RDONLY);
		if (data->input_fd == -1)
		{
			ft_printf("pipex: here_doc: %s\n", strerror(errno));
			cleanup_n_exit(ERROR, data);
		}
	}
	else
	{
		data->input_fd = open(data->av[1], O_RDONLY, 644);
		if (data->input_fd == -1)
		{
			ft_printf("pipex: %s: %s\n", strerror(errno), data->av[1]);
			cleanup_n_exit(ERROR, data);//this one was added
		}
	}
}


void get_output_file(t_data *data)
{
	if (data->heredoc_flag == 1)
		data->output_fd = open(data->av[data->ac - 1],
			O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		data->output_fd = open(data->av[data->ac - 1],
			O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (data->output_fd == -1)
	{
		cleanup_n_exit(ERROR, data);// this one was added
		ft_printf("pipex: %s: %s\n",
			strerror(errno), data->av[data->ac - 1]);
	}
}
