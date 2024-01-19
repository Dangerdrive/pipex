

#include "pipex.h"

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
    if (data->heredoc == 1)
    {
        handle_heredoc(data);
        data->fd_in = open(".heredoc.tmp", O_RDONLY);
        if (data->fd_in == -1)
            exit_error(msg("here_doc", ": ", strerror(errno), 1), data);
    }
    else
    {
        data->fd_in = open(data->av[1], O_RDONLY, 644);
        if (data->fd_in == -1)
            msg(strerror(errno), ": ", data->av[1], 1);
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
    if (data->heredoc == 1)
        data->fd_out = open(data->av[data->ac - 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
    else
        data->fd_out = open(data->av[data->ac - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (data->fd_out == -1)
        msg(strerror(errno), ": ", data->av[data->ac - 1], 1);
}


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
void handle_heredoc(t_data *data)
{
    int  tmp_fd;
    int  stdin_fd;
    char *line;

    tmp_fd = open(".heredoc.tmp", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    stdin_fd = dup(STDIN_FILENO);
    if (tmp_fd == -1)
        exit_error(msg("here_doc", ": ", strerror(errno), 1), data);

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

