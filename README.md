For me, one thing that made it more difficult was a lack of understandment of unix processes. It was not that hard to understand pipes, 

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