
#include "pipex.h"

/* 
 * close_pipe_fds:
 *   Closes all file descriptors associated with the pipes used in pipex.
 *
 * Parameters:
 *   data - Pointer to the t_data structure containing the pipe file descriptors.
 *
 * Functionality:
 *   - Iterates through the pipe file descriptors array and closes each file descriptor.
 */
static void close_pipe_fds(t_data *data)
{
	int	i;

	i = 0;
	while (i < (data->nb_cmds - 1) * 2)
	{
		close(data->pipe[i]);
		i++;
	}
}


/* 
 * exit_error:
 *   Handles the error situation by freeing allocated resources and closing file descriptors.
 *
 * Parameters:
 *   error_status - The exit status code to be returned.
 *   data - Pointer to the t_data structure with allocated resources and file descriptors.
 *
 * Functionality:
 *   - Closes open file descriptors and frees allocated memory.
 *   - If heredoc was used, removes the temporary file.
 *   - Exits the program with the given error status code.
 */

/**
 * Handles error situations in the program by freeing allocated resources and closing file descriptors.
 *
 * This function is called when an error is encountered. It ensures that all allocated resources
 * and open file descriptors are properly freed and closed to prevent memory leaks and other
 * resource-related issues. Additionally, if a heredoc was used in the program, it removes
 * the temporary file associated with it. After handling these cleanup tasks, the function 
 * terminates the program and returns the specified error status code.
 *
 * @param[in] error_status The exit status code to be returned upon program termination.
 * @param[in,out] data Pointer to the t_data structure containing allocated resources and 
 * file descriptors. This structure is used to perform the necessary cleanup.
 */
void exit_error(int error_status, t_data *data)
{
	if (data)
	{
		close_fds(data);
		if (data->pipe)
			free(data->pipe);
		if (data->pids)
			free(data->pids);
		// if (data->cmd_options || data->cmd_path)
		//   free_strs(data->cmd_path, data->cmd_options);
		if (data->cmd_options)
			free_strs(NULL, data->cmd_options);
		if (data->cmd_path)
		{
			free(data->cmd_path);
			data->cmd_path = NULL;
		}
		 
	}
	if (data->heredoc == 1)
		unlink(".heredoc.tmp");
	exit(error_status);
}


/* 
 * msg:
 *   Writes a formatted error message to standard error (stderr).
 *
 * Parameters:
 *   str1, str2, str3 - Strings to be concatenated and printed as the error message.
 *   erno - The error code to be returned.
 *
 * Returns:
 *   The provided error code (erno).
 *
 * Functionality:
 *   - Concatenates the given strings and prints them to stderr.
 */
int msg(char *str1, char *str2, char *str3, int erno)
{
	ft_putstr_fd("pipex: ", 2);
	ft_putstr_fd(str1, 2);
	ft_putstr_fd(str2, 2);
	ft_putendl_fd(str3, 2);
	return (erno);
}


/* 
 * close_fds:
 *   Closes the input and output file descriptors, as well as any pipe file descriptors.
 *
 * Parameters:
 *   data - Pointer to the t_data structure containing the file descriptors.
 *
 * Functionality:
 *   - Closes the input and output file descriptors if they are valid.
 *   - Calls close_pipe_fds to close all pipe file descriptors.
 */
void close_fds(t_data *data)
{
	if (data->fd_in != -1)
		close(data->fd_in);
	if (data->fd_out != -1)
		close(data->fd_out);
	close_pipe_fds(data);
}


/* 
 * free_strs:
 *   Frees a single string and/or an array of strings.
 *
 * Parameters:
 *   str - A single string to be freed.
 *   strs - An array of strings to be freed.
 *
 * Functionality:
 *   - Frees the provided string and sets its pointer to NULL.
 *   - Iterates through the array of strings, frees each string, and sets their pointers to NULL.
 */


/**
 * Frees a single string and/or an array of strings.
 *
 * This function is responsible for freeing a dynamically allocated string and
 * an array of dynamically allocated strings. It first checks if the provided 
 * single string is not NULL and frees it, then sets its pointer to NULL. 
 * For the array of strings, it iterates through each element, frees each string,
 * and finally frees the array itself. The pointers for each freed string are 
 * also set to NULL to prevent dangling pointers.
 *
 * @param[in,out] str A single string to be freed.
 * @param[in,out] strs An array of strings to be freed.
 */
void free_strs(char *str, char **strs)
{
	int i;

	if (str)
	{
		free(str);
		str = NULL;
	}
	if (strs)
	{
		i = 0;
		while (strs[i])
		{
			free(strs[i]);
			i++;
		}
		free(strs);
		strs = NULL;
	}
}

