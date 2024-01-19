#include "pipex.h"

/* 
 * redirect_io:
 *   Redirects the standard input and standard output to the given file descriptors.
 * 
 * Parameters:
 *   input - The file descriptor to be set as the new standard input (STDIN_FILENO).
 *   output - The file descriptor to be set as the new standard output (STDOUT_FILENO).
 *   data - Pointer to a t_data structure for error handling and other purposes.
 *
 * Notes:
 *   - The function uses dup2() to duplicate the given file descriptors to STDIN_FILENO and STDOUT_FILENO.
 *   - If an error occurs during this process, the function calls exit_error() to handle it.
 */

/**
 * Executes a child process in the Pipex pipeline.
 *
 * This function is responsible for configuring and executing a single command within the pipeline.
 * It redirects the standard input and output for the child process based on its position in the pipeline:
 * - The first child uses the initial input file descriptor and the write-end of the first pipe.
 * - The last child uses the read-end of the last pipe and the final output file descriptor.
 * - Middle children use the read-end of their respective input pipe and the write-end of their respective output pipe.
 * After setting up the redirection, the function closes any file descriptors that are no longer needed.
 * It validates the command and its options and then executes the command using `execve`.
 * If `execve` fails, it handles the error appropriately.
 *
 * @param[in,out] data Pointer to the `t_data` structure containing necessary information for the child process,
 * including file descriptors, command options, and environment variables.
 */
static void redirect_io(int input, int output, t_data *data)
{
	// Check for invalid file descriptors
	if (input < 0 || output < 0)
		exit_error(1, data); // or use a specific error code/message

	// Redirect standard input to 'input'
	if (dup2(input, STDIN_FILENO) == -1)
		exit_error(1, data); // Specific error for input redirection failure


	// Redirect standard output to 'output'
	if (dup2(output, STDOUT_FILENO) == -1)
		exit_error(1, data); // Specific error for output redirection failure
}

		

/* 
 * execute_child_process:
 *   Configures and executes a child process in a pipelined command execution context.
 * 
 * Parameters:
 *   data - Pointer to a t_data structure containing the necessary information for command execution.
 *
 * Functionality:
 *   - Depending on its position (first, middle, or last) in the pipeline, the child process's standard input and output
 *	 are redirected to appropriate file descriptors using the redirect_io function.
 *   - Closes unnecessary file descriptors using close_fds.
 *   - Executes the command using execve. If execve fails, handles the error using exit_error.
 *
 * Notes:
 *   - The position of the child in the pipeline determines which file descriptors are used for its standard input and output.
 *   - The function ensures that the command to be executed (data->cmd_path) and its options (data->cmd_options) are not NULL.
 *   - If execve fails, the function retrieves the error message using strerror and constructs a specific error message using msg.
 */

/**
 * Redirects the standard input and output of a process.
 *
 * This function is responsible for redirecting the standard input (STDIN) and standard output (STDOUT)
 * of a process to specified file descriptors. It uses the `dup2` system call to duplicate the given 
 * file descriptors to STDIN_FILENO and STDOUT_FILENO. If either of the input or output file descriptors
 * is invalid (less than 0), the function will call `exit_error` to handle the situation. Similarly, 
 * if an error occurs during the redirection process (e.g., `dup2` fails), the function will call 
 * `exit_error` with an appropriate error message or code.
 *
 * @param[in] input The file descriptor to be set as the new standard input (STDIN_FILENO).
 * @param[in] output The file descriptor to be set as the new standard output (STDOUT_FILENO).
 * @param[in,out] data Pointer to a t_data structure for error handling and other purposes.
 */
static void execute_child_process(t_data *data)
{
	// Redirect the input and output based on the child's position in the pipeline
	if (data->child == 0)
	{
		// First child: Use the initial input file descriptor and the write-end of the first pipe
		redirect_io(data->fd_in, data->pipe[1], data);
	} 
	else if (data->child == data->nb_cmds - 1)
	{
		// Last child: Use the read-end of the last pipe and the final output file descriptor
		redirect_io(data->pipe[2 * data->child - 2], data->fd_out, data);
	}
	else {
		// Middle child: Use the read-end of the current pipe and the write-end of the next pipe
		redirect_io(data->pipe[2 * data->child - 2], data->pipe[2 * data->child + 1], data);
	}

	// Close file descriptors that are no longer needed
	close_fds(data);

	// Check if the command and its options are valid
	if (data->cmd_options == NULL || data->cmd_path == NULL) {
		exit_error(1, data); // Consider using a specific error message or code
	}

	// Execute the command
	if (execve(data->cmd_path, data->cmd_options, data->envp) == -1) {
		exit_error(msg(data->cmd_options[0], ": ", strerror(errno), 1), data);
	}
}




/* execute_parent_process:
*	Waits for the children processes to finish and fetches the status of the last
*	child.
*	Returns the exit status code of the last child process.
*/


/**
 * Manages the parent process in a pipeline of command executions.
 *
 * This function is responsible for closing unnecessary file descriptors and waiting for child processes 
 * to complete their execution. It iterates through the child processes, using `waitpid` to wait for each 
 * child to finish. The function captures the exit status of the last child process in the pipeline, which 
 * it returns as the overall exit code. It also performs cleanup by freeing memory allocated for pipes and 
 * process IDs.
 *
 * @param[in,out] data Pointer to a t_data structure containing necessary information, such as the number of 
 * commands, child process IDs, and pipe file descriptors.
 * 
 * @return The exit status code of the last child process in the pipeline. If no exit status is captured, the 
 * function defaults to returning 1.
 */
static int	execute_parent_process(t_data *data)
{
	pid_t	wpid;
	int		status;
	int		exit_code;

	close_fds(data);
	data->child--;
	exit_code = 1;
	while (data->child >= 0)
	{
		wpid = waitpid(data->pids[data->child], &status, 0);
		if (wpid == data->pids[data->nb_cmds - 1])
		{
			if ((data->child == (data->nb_cmds - 1)) && WIFEXITED(status))
				exit_code = WEXITSTATUS(status);
		}
		data->child--;
	}
	free(data->pipe);
	free(data->pids);
	return (exit_code);
}



//  * Functionality:
//  *   - Sets up a pipe and forks child processes for each command in the pipeline.
//  *   - Each child process handles the execution of one command.
//  *   - The parent process waits for all child processes to complete and collects their exit codes.
//  *
//  * Returns:
//  *   The exit code of the last child in the pipeline.
//  *
//  * Notes:
//  *   - The function creates a pipe using the pipe system call and checks for errors.
//  *   - Each command and its options are parsed and executed in separate child processes.
//  *   - The function handles errors in pipe creation, command parsing, and process forking.
//  *   - If a heredoc is used, the temporary file is removed after execution.


/**
 * Manages the execution of a pipeline of commands.
 *
 * This function sets up a pipe and forks child processes for each command in the pipeline.
 * Each child process is responsible for executing one command. The parent process waits
 * for all child processes to complete and then collects their exit codes. It handles
 * the creation of pipes, parsing and execution of commands, and manages potential errors
 * in these processes. If a heredoc is used, the function also ensures the removal of
 * the temporary file after execution.
 *
 * @param[in,out] data Pointer to a t_data structure containing necessary information
 * for pipelined command execution.
 * 
 * @return The exit code of the last child process in the pipeline.
 */
static int pipex(t_data *data)
{
	int exit_code;

	// Create a pipe and check for errors
	if (pipe(data->pipe) == -1) {
		exit_error(msg("pipe", ": ", strerror(errno), 1), data);
	}

	// Fork child processes for each command in the pipeline
	data->child = 0;
	while (data->child < data->nb_cmds) {
		// Parse the command and its options
		data->cmd_options = ft_split(data->av[data->child + 2 + data->heredoc], ' ');
		if (!data->cmd_options) {
			exit_error(msg("unexpected error", "", "", 1), data);
		}

		// Get the full path of the command
		data->cmd_path = get_cmd(data->cmd_options[0], data);

		// Fork a new process
		data->pids[data->child] = fork();
		if (data->pids[data->child] == -1) {
			// Handle errors in fork
			exit_error(msg("fork", ": ", strerror(errno), 1), data);
		} else if (data->pids[data->child] == 0) {
			// Child process: Execute the command
			execute_child_process(data);
		}

		// Free allocated strings for the current command
		free_strs(data->cmd_path, data->cmd_options);

		// Move to the next command
		data->child++;
	}

	// Parent process: Wait for children and get the exit code
	exit_code = execute_parent_process(data);

	// Remove temporary file if heredoc was used
	if (data->heredoc == 1) {
		unlink(".heredoc.tmp");
	}

	return exit_code;
}

// static int pipex(t_data *data)
// {
//	 int exit_code;
//	 if (pipe(data->pipe) == -1) {
//		 exit_error(msg("pipe", ": ", strerror(errno), 1), data);
//	 }
//	 data->child = 0;
//	 while (data->child < data->nb_cmds) {
//		 data->cmd_options = ft_split(data->av[data->child + 2 + data->heredoc], ' ');
//		 if (!data->cmd_options) {
//			 exit_error(msg("unexpected error", "", "", 1), data);
//		 }
//		 data->cmd_path = get_cmd(data->cmd_options[0], data);
//		 data->pids[data->child] = fork();
//		 if (data->pids[data->child] == -1) {
//			 exit_error(msg("fork", ": ", strerror(errno), 1), data);
//		 } else if (data->pids[data->child] == 0) {
//			 execute_child_process(data);
//		 }
//		 free_strs(data->cmd_path, data->cmd_options);
//		 data->child++;
//	 }
//	 exit_code = execute_parent_process(data);
//	 if (data->heredoc == 1) {
//		 unlink(".heredoc.tmp");
//	 }
//	 return exit_code;
// }





/* 
 * main:
 *   Entry point for the pipex program. It parses arguments, initializes necessary structures,
 *   and launches the pipex process to handle command execution.
 *
 * Parameters:
 *   ac - Argument count.
 *   av - Argument vector (array of strings).
 *   envp - Environment variables.
 *
 * Functionality:
 *   - Validates the number of arguments and provides usage messages if the arguments are incorrect.
 *   - Checks for a valid environment.
 *   - Initializes the data structure for pipex.
 *   - Calls the pipex function to execute the commands and returns its exit code.
 *
 * Returns:
 *   The exit code of the last command executed by pipex or an error code in case of invalid input.
 *
 * Notes:
 *   - The program supports a 'here_doc' feature which alters the expected argument format.
 *   - Usage messages guide the user on how to properly invoke the program.
 */

/**
 * Entry point for the Pipex program.
 *
 * This function is responsible for initializing and executing the Pipex pipeline process. 
 * It starts by validating the command-line arguments to ensure they meet the program's requirements.
 * If the arguments are insufficient or incorrect, it displays usage instructions. 
 * It checks for a valid environment before initializing the pipeline process with provided arguments.
 * Once initialized, it invokes the pipex function to manage the execution of commands within a pipeline.
 * Finally, it returns the exit code from the last command executed by Pipex.
 *
 * @param ac The count of command-line arguments.
 * @param av The vector of command-line arguments.
 * @param envp The environment variables.
 *
 * @return The exit code of the last command executed by Pipex, or an error code in case of invalid input or setup failure.
 */
int main(int ac, char **av, char **envp)
{
	t_data data;
	int exit_code;

	// Validate argument count and provide usage instructions if necessary
	if (ac < 5) {
		if (ac >= 2 && !ft_strncmp("here_doc", av[1], 9))
			return (msg("Usage: ", "./pipex here_doc LIMITER cmd1 cmd2 ... cmdn file2.", "", 1));
		return (msg("Usage: ", "./pipex file1 cmd1 cmd2 ... cmdn file2.", "", 1));
	}
	else if (ac < 6 && !ft_strncmp("here_doc", av[1], 9))
		return (msg("Usage: ", "./pipex here_doc LIMITER cmd1 cmd2 ... cmdn file2.", "", 1));

	// Check for valid environment
	if (!envp || envp[0][0] == '\0')
		exit_error(msg("Unexpected error.", "", "", 1), &data);

	// Initialize data structure and start the pipex process
	data = init(ac, av, envp);
	exit_code = pipex(&data);

	// Return the exit code from pipex
	return (exit_code);
}









/*
examples of incorrect or problematic uses of pipes in shell commands. These examples are likely to result in errors or unintended behavior:

Missing Command Between Pipes:

cat file1 | | grep "pattern" > file2
In this example, there's a missing command between the two pipes (|). The shell expects a command on each side of a pipe.
Redirection Error with Pipe:

cat file1 > | grep "pattern" > file2
Here, the use of > before the pipe is incorrect. The > is a redirection operator and should not be used directly before a pipe.
Invalid File Descriptor:

cat file1 2>&1 | grep "pattern" > file3
This command is technically correct, but if file1 doesn't exist, it will result in an error. 
The 2>&1 part is redirecting standard error (stderr) to standard output (stdout), which is then piped to grep. 
If file1 is missing, cat will write an error message to stderr, which then gets redirected.
Using a Non-Existent Command:

cat file1 | nonExistentCommand > file2
If nonExistentCommand doesn't exist in your shell's path, this line will result in a "command not found" error.
Incorrect Order of Redirection and Pipe:

cat file1 > grep "pattern" | sort > file2
The redirection operator (>) is incorrectly placed before grep. The correct order is to pipe (|) the output of cat file1 to grep and then optionally to another command like sort.
Writing to a Read-Only File:

cat file1 | grep "pattern" > /path/to/readonly/file2
If file2 is a read-only file, this command will fail with a "Permission denied" error.
Missing Input File:

cat nonExistentFile | sort > file2
If nonExistentFile does not exist, cat will output an error, and sort will receive no input, resulting in an empty file2.
These examples demonstrate common mistakes in shell command syntax or logic, leading to errors or unintended results. Remember, the effectiveness of a shell command largely depends on the correctness of the command syntax and the context in which it is executed.
*/


/*
To replicate the examples of pipe misuse in your program, which is designed to replicate pipe behavior, you would structure the commands similarly to how they are misused in a shell, but within the context of how your pipex program expects arguments. Remember, your pipex program should be structured to take an input file, two commands, and an output file as arguments. Here's how you could replicate each example:

Missing Command Between Pipes:

./pipex infile "cat" "" "grep 'pattern'" outfile
Here, the second command is an empty string, representing a missing command.
Redirection Error with Pipe:

This type of error is specific to shell syntax and may not be directly replicable in your pipex program.
Invalid File Descriptor:

./pipex nonExistentFile "cat" "grep 'pattern'" outfile
If nonExistentFile doesn't exist, it should cause an error similar to a shell error when a file is missing.
Using a Non-Existent Command:

./pipex infile "nonExistentCommand" "grep 'pattern'" outfile
The first command is a non-existent command, which should result in an error.
Incorrect Order of Redirection and Pipe:

This is another shell-specific syntax issue and may not apply to your pipex program.
Writing to a Read-Only File:

./pipex infile "cat" "grep 'pattern'" /path/to/readonly/outfile
If /path/to/readonly/outfile is read-only, this should result in a "Permission denied" error.
Missing Input File:

./pipex nonExistentFile "cat" "sort" outfile
The input file doesn't exist, so cat will fail, resulting in sort receiving no input.
Since your pipex program is intended to replicate the behavior of Unix pipes, you'll need to translate these shell-specific misuse examples into the format that your program expects. It's important to remember that some shell-specific behaviors (like redirection errors) may not be directly translatable to your program's structure.

*/