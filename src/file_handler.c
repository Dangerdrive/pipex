

#include "pipex.h"


/*
 * handle_heredoc:
 *   Handles the 'here_doc' functionality by reading input from the user and storing it in a temporary file.
 *
 * Parameters:
 *   data - Pointer to a t_data structure containing the limiter and other necessary information.
 *
 * Functionality:
 *   - Creates and opens a temporary file ".heredoc.tmp" for writing.
 *   - Reads lines from standard input until the specified limiter is found.
 *   - Writes the lines to the temporary file unless they match the limiter.
 *   - Closes the temporary file and the duplicated standard input file descriptor upon completion.
 */

/**
 * Opens the output file and sets its file descriptor in the data structure.
 *
 * This function determines the correct flags for opening the output file based on
 * whether the 'here_doc' feature is being used. If 'here_doc' is enabled, it opens
 * the file in append mode (O_APPEND), allowing the contents to be added to the end
 * of the file. If 'here_doc' is not enabled, it opens the file in truncate mode
 * (O_TRUNC), which clears the file's existing contents. In both cases, the file
 * is opened for writing (O_WRONLY) and will be created (O_CREAT) if it doesn't
 * already exist, with standard permissions set (0644).
 *
 * If the file cannot be opened, an error message is printed to standard output
 * using `ft_printf`, displaying the corresponding error from `errno` and the file's name.
 *
 * @param[in,out] data A pointer to the t_data structure containing information
 *                     about the pipeline execution, including the heredoc flag
 *                     and file descriptors for input and output files.
 */
static void handle_heredoc(t_data *data)
{
	int  tmp_fd;
	int  stdin_fd;
	char *line;

	tmp_fd = open(".heredoc.tmp", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	stdin_fd = dup(STDIN_FILENO);
	if (tmp_fd == -1)
	{
		cleanup_n_exit(ERROR, data);
		ft_printf("pipex: here_doc: %s\n", strerror(errno));
	}
	line = "";
	while (1)
	{
		ft_putstr_fd("here_doc > ", 1);
		line = get_next_line(stdin_fd);
		if (line == NULL)
			break;
		if (ft_strlen(data->av[2]) + 1 == ft_strlen(line)
			&& !ft_strncmp(line, data->av[2], ft_strlen(data->av[2] + 1)))
			close(stdin_fd);
		else
			ft_putstr_fd(line, tmp_fd);
		free(line);
	}
	close(tmp_fd);
}

/*
 * get_input_file:
 *   Sets up the input file descriptor for pipex.
 *
 * Parameters:
 *   data - Pointer to a t_data structure containing necessary information.
 *
 * Functionality:
 *   - If heredoc is enabled (data->heredoc == 1), it creates a temporary file using handle_heredoc.
 *   - Opens the temporary file or the specified input file as the input file descriptor.
 *   - If an error occurs while opening, it exits with an error message.
 */
void get_input_file(t_data *data)
{
	if (data->heredoc_flag == 1)
	{
		handle_heredoc(data);
		data->input_fd = open(".heredoc.tmp", O_RDONLY);
		if (data->input_fd == -1)
		{
			cleanup_n_exit(ERROR, data);
			ft_printf("pipex: here_doc: %s\n", strerror(errno));
		}
	}
	else
	{
		data->input_fd = open(data->av[1], O_RDONLY, 644);
		if (data->input_fd == -1)
		{
			ft_printf("pipex: %s: %s\n", strerror(errno), data->av[1]);
		}
	}
}

/*
 * get_output_file:
 *   Opens or creates the output file for pipex.
 *
 * Parameters:
 *   data - Pointer to a t_data structure containing necessary information.
 *
 * Functionality:
 *   - Opens or creates the output file specified in the arguments.
 *   - If heredoc is enabled, the file is opened in append mode (O_APPEND).
 *   - Otherwise, the file is opened in truncate mode (O_TRUNC), overwriting its content.
 *   - If an error occurs while opening, it exits with an error message.
 */
void get_output_file(t_data *data)
{
	if (data->heredoc_flag == 1)
		data->output_fd = open(data->av[data->ac - 1],
			O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		data->output_fd = open(data->av[data->ac - 1],
			O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (data->output_fd == -1)
		ft_printf("pipex: %s: %s\n", strerror(errno), data->av[data->ac - 1]);
}



