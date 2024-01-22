/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-alen <fde-alen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 00:39:14 by fde-alen          #+#    #+#             */
/*   Updated: 2024/01/22 04:55:37 by fde-alen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/**
 * Redirects the standard input and output of a process.
 *
 * This function is responsible for redirecting the standard input (STDIN) and
 * standard output (STDOUT) of a process to specified file descriptors. It uses
 * the `dup2` system call to duplicate the given file descriptors to
 * STDIN_FILENO and STDOUT_FILENO. If either of the input or output file
 * descriptors is invalid (less than 0), the function will call
 * `cleanup_n_exit` to handle the situation. Similarly, if an error occurs
 * during the redirection process (e.g., `dup2` fails), the function will call
 * `exit_error` with an appropriate error message or code.
 *
 * @param[in] input The file descriptor to be set as the new standard input
 *                  (STDIN_FILENO).
 * @param[in] output The file descriptor to be set as the new standard output
 *                   (STDOUT_FILENO).
 * @param[in,out] data Pointer to a t_data structure for error handling and
 *                     other purposes.
 */
static void	redirect_io(int input, int output, t_data *data)
{
	if (input < 0 || output < 0)
		cleanup_n_exit(ERROR, data);
	if (dup2(input, STDIN_FILENO) == -1)
		cleanup_n_exit(ERROR, data);
	if (dup2(output, STDOUT_FILENO) == -1)
		cleanup_n_exit(ERROR, data);
}

/**
 * Executes a child process in the Pipex pipeline.
 *
 * This function is responsible for configuring and executing a single command
 * within the pipeline. It redirects the standard input and output for the
 * child process based on its position in the pipeline:
 * - The first child uses the initial input file descriptor and the write-end
 *   of the first pipe.
 * - The last child uses the read-end of the last pipe and the final output
 *   file descriptor.
 * - Middle children use the read-end of their respective input pipe and the
 *   write-end of their respective output pipe. After setting up the
 *   redirection, the function closes any file descriptors that are no longer
 *   needed. It validates the command and its options and then executes the
 *   command using `execve`. If `execve` fails, it handles the error
 *   appropriately.
 *
 * @param[in,out] data Pointer to the `t_data` structure containing necessary
 *                     information for the child process, including file
 *                     descriptors, command options, and environment variables.
 */
static void	execute_child_process(t_data *data)
{
	if (data->child == 0)
	{
		redirect_io(data->input_fd, data->pipe[1], data);
	}
	else if (data->child == data->cmd_count - 1)
	{
		redirect_io(data->pipe[2 * data->child - 2], data->output_fd, data);
	}
	else
	{
		redirect_io(data->pipe[2 * data->child - 2],
			data->pipe[2 * data->child + 1], data);
	}
	close_fds(data);
	if (data->cmd_options == NULL || data->cmd_path == NULL)
	{
		cleanup_n_exit(ERROR, data);
	}
	if (execve(data->cmd_path, data->cmd_options, data->envp) == -1)
	{
		cleanup_n_exit(ERROR, data);
		ft_printf("pipex: %s: %s\n", data->cmd_options[0], strerror(errno));
	}
}

/**
 * Manages the parent process in a pipeline of command executions.
 *
 * This function is responsible for closing unnecessary file descriptors and
 * waiting for child processes to complete their execution. It iterates through
 * the child processes, using `waitpid` to wait for each child to finish. The
 * function captures the exit status of the last child process in the pipeline,
 * which it returns as the overall exit code. It also performs cleanup by
 * freeing memory allocated for pipes and process IDs.
 *
 * @param[in,out] data Pointer to a t_data structure containing necessary
 *                     information, such as the number of commands, child
 *                     process IDs, and pipe file descriptors.
 *
 * @return The exit status code of the last child process in the pipeline. If no
 *         exit status is captured, the function defaults to returning 1.
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
		if (wpid == data->pids[data->cmd_count - 1])
		{
			if ((data->child == (data->cmd_count - 1)) && WIFEXITED(status))
				exit_code = WEXITSTATUS(status);
		}
		data->child--;
	}
	free(data->pipe);
	free(data->pids);
	return (exit_code);
}

/**
 * Manages the execution of a pipeline of commands.
 *
 * This function sets up a pipe and forks child processes for each command in the
 * pipeline. Each child process is responsible for executing one command. The
 * parent process waits for all child processes to complete and then collects
 * their exit codes. It handles the creation of pipes, parsing and execution of
 * commands, and manages potential errors in these processes. If a heredoc is
 * used, the function also ensures the removal of the temporary file after
 * execution.
 *
 * @param[in,out] data Pointer to a t_data structure containing necessary
 *                     information for pipelined command execution.
 *
 * @return The exit code of the last child process in the pipeline.
 */
// static int	pipex(t_data *data)
// {
// 	int	exit_code;

// 	if (pipe(data->pipe) == -1)
// 		cleanup_n_exit(ft_printf("pipex: pipe: %s\n", strerror(errno)),
// 			data);
// 	data->child = 0;
// 	while (data->child < data->cmd_count)
// 	{
// 		data->cmd_options = ft_split(data->av[data->child + 2 + data->heredoc_flag], ' ');
// 		if (!data->cmd_options)
// 			cleanup_n_exit(ft_printf("pipex: cmd_options error: %s\n",
// 					strerror(errno)), data);
// 		data->cmd_path = get_cmd(data->cmd_options[0], data);
// 		data->pids[data->child] = fork();
// 		if (data->pids[data->child] == -1)
// 		cleanup_n_exit(ft_printf("pipex: fork error: %s\n",
// 			strerror(errno)), data);
// 		else if (data->pids[data->child] == 0)
// 			execute_child_process(data);
// //		free_strs(data->cmd_path, data->cmd_options);
// 		free(data->cmd_path);
// 		data->cmd_path = NULL;
// 		free_array(data->cmd_options);
// 		data->child++;
// 	}
// 	exit_code = execute_parent_process(data);
// 	if (data->heredoc_flag == 1)
// 		unlink(".heredoc.tmp");
// 	return (exit_code);
// }

static int	pipex(t_data *d)
{
	int	exit_code;

	if (pipe(d->pipe) == -1)
		cleanup_n_exit(ft_printf(" pipe: %s\n", strerror(errno)), d);
	d->child = 0;
	while (d->child < d->cmd_count)
	{
		d->cmd_options = ft_split(d->av[d->child + 2 + d->heredoc_flag], ' ');
		if (!d->cmd_options)
			cleanup_n_exit(ft_printf("cmd_opt error:%s\n", strerror(errno)), d);
		d->cmd_path = get_cmd(d->cmd_options[0], d);
		d->pids[d->child] = fork();
		if (d->pids[d->child] == -1)
			cleanup_n_exit(ft_printf("fork error: %s\n", strerror(errno)), d);
		else if (d->pids[d->child] == 0)
			execute_child_process(d);
		free(d->cmd_path);
		d->cmd_path = NULL;
		free_array(d->cmd_options);
		d->child++;
	}
	exit_code = execute_parent_process(d);
	if (d->heredoc_flag == 1)
		unlink(".heredoc.tmp");
	return (exit_code);
}

/**
 * Entry point for the Pipex program.
 *
 * This function is responsible for initializing and executing the Pipex pipeline
 * process. It starts by validating the command-line arguments to ensure they
 * meet the program's requirements. If the arguments are insufficient or
 * incorrect, it displays usage instructions. It checks for a valid environment
 * before initializing the pipeline process with provided arguments. Once
 * initialized, it invokes the pipex function to manage the execution of commands
 * within a pipeline. Finally, it returns the exit code from the last command
 * executed by Pipex.
 *
 * @param argc The count of command-line arguments.
 * @param argv The vector of command-line arguments.
 * @param envp The environment variables.
 *
 * @return The exit code of the last command executed by Pipex, or an error code
 *         in case of invalid input or setup failure.
 */
int	main(int argc, char **argv, char **envp)
{
	t_data	data;
	int		exit_code;

	exit_code = -1;
	if (invalid_args(argc, argv, envp))
		return (1);
	data = init_data(argc, argv, envp);
	exit_code = pipex(&data);
	return (exit_code);
}
