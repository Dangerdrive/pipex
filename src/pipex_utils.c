/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-alen <fde-alen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 00:38:58 by fde-alen          #+#    #+#             */
/*   Updated: 2024/01/22 00:39:01 by fde-alen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/**
 * Closes all file descriptors associated with the pipes used in pipex.
 *
 * This function iterates through the array of file descriptors stored in the
 * `pipe` field of the `t_data` structure and closes each one. The number of
 * file descriptors to close is determined by the number of commands minus one,
 * multiplied by two (since each pipe consists of two file descriptors).
 *
 * @param[in,out] data Pointer to a t_data structure containing the pipe file
 * descriptors and the number of commands.
 */
static void	close_pipe_fds(t_data *data)
{
	int	i;

	i = 0;
	while (i < (data->cmd_count - 1) * 2)
	{
		close(data->pipe[i]);
		i++;
	}
}

/**
 * Closes all file descriptors used in the pipex process.
 *
 * This function is responsible for closing the input and output file
 * descriptors, as well as all pipe file descriptors associated with the pipex
 * process. It checks if the input and output file descriptors are valid (not
 * equal to -1) before attempting to close them. The function then calls
 * `close_pipe_fds` to close all pipe file descriptors.
 *
 * @param[in,out] data Pointer to a t_data structure that contains the file
 *                     descriptors for input, output, and pipes.
 */
void	close_fds(t_data *data)
{
	if (data->input_fd != -1)
		close(data->input_fd);
	if (data->output_fd != -1)
		close(data->output_fd);
	close_pipe_fds(data);
}

/**
 * Handles error situations in the program by freeing allocated resources and
 * closing file descriptors.
 *
 * This function is called when an error is encountered. It ensures that all
 * allocated resources and open file descriptors are properly freed and closed
 * to prevent memory leaks and other resource-related issues. Additionally, if a
 * heredoc was used in the program, it removes the temporary file associated with
 * it. After handling these cleanup tasks, the function terminates the program
 * and returns the specified error status code.
 *
 * @param[in] error_status The exit status code to be returned upon program
 *                        termination.
 * @param[in,out] data Pointer to the t_data structure containing allocated
 *                     resources and file descriptors. This structure is used to
 *                     perform the necessary cleanup.
 */
void	cleanup_n_exit(int error_status, t_data *data)
{
	if (data)
	{
		close_fds(data);
		if (data->pipe)
			free(data->pipe);
		if (data->pids)
			free(data->pids);
		if (data->cmd_options)
			free_array(data->cmd_options);
		if (data->cmd_path)
		{
			free(data->cmd_path);
			data->cmd_path = NULL;
		}
	}
	if (data->heredoc_flag == 1)
		unlink(".heredoc.tmp");
	exit(error_status);
}

/**
 * Frees a null-terminated array of strings.
 *
 * This function iteratively frees each string in a null-terminated array of
 * strings and then frees the array itself. It checks if the array is not NULL
 * before attempting to free it. After freeing the array, it sets the pointer
 * to NULL to prevent dangling pointer issues. This function is typically used
 * for cleaning up dynamically allocated arrays of strings.
 *
 * @param[in,out] strs Pointer to the array of strings to be freed.
 */
void	free_array(char **strs)
{
	int		i;

	i = 0;
	if (strs)
	{
		while (strs[i])
		{
			free(strs[i]);
			i++;
		}
		free(strs);
		strs = NULL;
	}
}

/**
 * Validates argument count and environment variables for pipex.
 *
 * This function checks if the number of command-line arguments meets the
 * requirements for the pipex program. It handles different scenarios including
 * standard usage and 'here_doc' functionality. It also verifies the presence
 * and validity of environment variables (envp). If the arguments are
 * insufficient or envp is invalid, it prints an error message and returns 1,
 * indicating an error. If valid, it returns 0.
 *
 * @param[in] argc Count of command-line arguments.
 * @param[in] argv Array of command-line argument strings.
 * @param[in] envp Array of environment variable strings.
 * @return 1 on invalid arguments or envp, 0 if valid.
 */
int	invalid_args(int argc, char **argv, char **envp)
{
	if (argc < 5)
	{
		if (argc >= 2 && !ft_strncmp("here_doc", argv[1], 9))
		{
			ft_putendl_fd("Incorrect number of arguments.\nUsage:"
				" ./pipex here_doc LIMITER cmd1 cmd2 ... cmdn outfile.", 2);
			return (1);
		}
		ft_putendl_fd("Incorrect number of arguments.\nUsage:"
			"./pipex infile cmd1 cmd2 ... cmdn outfile", 2);
		return (1);
	}
	else if (argc < 6 && !ft_strncmp("here_doc", argv[1], 9))
	{
		ft_putendl_fd("Incorrect number of arguments.\nUsage:"
			"./pipex infile cmd1 cmd2 ... cmdn outfile", 2);
		return (1);
	}
	if (!envp || envp[0][0] == '\0')
	{
		ft_putendl_fd("envp error.", 2);
		return (1);
	}
	return (0);
}
