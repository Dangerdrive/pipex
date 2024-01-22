/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-alen <fde-alen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 00:39:33 by fde-alen          #+#    #+#             */
/*   Updated: 2024/01/22 03:04:55 by fde-alen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/**
 * Initializes the t_data structure with default values.
 *
 * This function is responsible for setting initial values for all fields in the
 * t_data structure. It sets pointers to NULL, file descriptors to -1, and
 * integer values to 0 or -1 as appropriate. This initialization is crucial to
 * ensure that the data structure starts in a known state, preventing undefined
 * behavior from uninitialized values. The initialized structure is used to
 * manage various aspects of the pipex program, such as environment variables,
 * command arguments, file descriptors, and child process management.
 *
 * @return An instance of t_data structure with all fields initialized to default
 *         values.
 */
static t_data	initialize_data(void)
{
	t_data	data;

	data.envp = NULL;
	data.ac = -1;
	data.av = NULL;
	data.heredoc_flag = 0;
	data.input_fd = -1;
	data.output_fd = -1;
	data.pipe = NULL;
	data.cmd_count = -1;
	data.child = -1;
	data.pids = NULL;
	data.cmd_options = NULL;
	data.cmd_path = NULL;
	return (data);
}

/**
 * Creates pipe file descriptors for inter-process communication in pipex.
 *
 * This function is responsible for setting up the necessary pipes for the pipex
 * program. It iterates through the required number of pipes, which is one less
 * than the command count, and creates each pipe. The file descriptors for each
 * pipe are stored in the 'pipe' field of the t_data structure. If a pipe
 * creation fails, the function prints an error message and exits the program
 * after performing cleanup.
 *
 * @param[in,out] data Pointer to a t_data structure containing the command
 *                     count and the array to store pipe file descriptors.
 */
static void	create_pipes(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->cmd_count - 1)
	{
		if (pipe(data->pipe + 2 * i) == -1)
		{
			ft_printf("pipex: Could not create pipe: %s\n",
				strerror(errno));
			cleanup_n_exit(ERROR, data);
		}
		i++;
	}
}

/**
 * Initializes the t_data structure for the pipex program.
 *
 * This function initializes and sets up the t_data structure, which is used
 * throughout the pipex program. It stores command line arguments, environment
 * variables, and sets flags for 'here_doc' functionality. The function also
 * allocates memory for storing process IDs (pids) and pipe file descriptors,
 * handles input and output file setup, and creates the necessary pipes for
 * inter-process communication.
 *
 * @param[in] ac The count of command line arguments.
 * @param[in] av Array of command line argument strings.
 * @param[in] envp Array of environment variable strings.
 * @return An initialized t_data structure.
 */
t_data	init_data(int ac, char **av, char **envp)
{
	t_data	data;

	data = initialize_data();
	data.envp = envp;
	data.ac = ac;
	data.av = av;
	if (!ft_strncmp("here_doc", av[1], 9))
		data.heredoc_flag = 1;
	get_input_file(&data);
	get_output_file(&data);
	data.cmd_count = ac - 3 - data.heredoc_flag;
	data.pids = malloc(sizeof(*data.pids) * data.cmd_count);
	if (!data.pids)
	{
		ft_printf("pipex: PID error: %s\n", strerror(errno));
		cleanup_n_exit(ERROR, &data);
	}
	data.pipe = malloc(sizeof(*data.pipe) * 2 * (data.cmd_count - 1));
	if (!data.pipe)
	{
		ft_printf("pipex: Pipe error: %s\n", strerror(errno));
		cleanup_n_exit(ERROR, &data);
	}
	create_pipes(&data);
	return (data);
}
