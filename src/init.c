
#include "pipex.h"

/* 
 * initialize_data:
 *   Initializes and returns a t_data structure with default values.
 *
 * Returns:
 *   An initialized t_data structure with all fields set to their default state.
 *   String pointers are set to NULL and integer values are set to -1.
 *
 * Notes:
 *   - The function sets file descriptors and other integral fields to -1, 
 *	 as 0 is a valid file descriptor and should not be used for initialization.
 */

/**
 * Initializes the t_data structure with default values.
 *
 * This function is responsible for setting initial values for all fields in the t_data structure.
 * It sets pointers to NULL, file descriptors to -1, and integer values to 0 or -1 as appropriate.
 * This initialization is crucial to ensure that the data structure starts in a known state, 
 * preventing undefined behavior from uninitialized values. The initialized structure is used to manage 
 * various aspects of the pipex program, such as environment variables, command arguments, file descriptors, 
 * and child process management.
 *
 * @return An instance of t_data structure with all fields initialized to default values.
 */
static t_data initialize_data(void)
{
	t_data data;

	data.envp = NULL;
	data.ac = -1;
	data.av = NULL;
	data.heredoc = 0;
	data.fd_in = -1;
	data.fd_out = -1;
	data.pipe = NULL;
	data.nb_cmds = -1;
	data.child = -1;
	data.pids = NULL;
	data.cmd_options = NULL;
	data.cmd_path = NULL;
	return (data);
}

/* 
 * create_pipes:
 *   Creates the necessary number of pipes for inter-process communication in pipex.
 *
 * Parameters:
 *   data - Pointer to the t_data structure containing the number of commands and the pipe array.
 *
 * Functionality:
 *   - Iterates over the number of required pipes and creates each using the pipe system call.
 *   - Stores the file descriptors for each pipe in the pipe array within the data structure.
 *   - Exits with an error message if pipe creation fails.
 */
static void create_pipes(t_data *data)
{
	int i;

	i = 0;
	while (i < data->nb_cmds - 1)
	{
		if (pipe(data->pipe + 2 * i) == -1)
			exit_error(msg("Could not create pipe", "", "", 1), data);
		i++;
	}
}


/* 
 * init:
 *   Initializes a t_data structure for pipex, sets up input and output file descriptors, 
 *   and allocates necessary resources for pipes and process IDs.
 *
 * Parameters:
 *   ac - Argument count.
 *   av - Argument vector.
 *   envp - Environment variables.
 *
 * Returns:
 *   An initialized t_data structure ready for use in pipex.
 *
 * Functionality:
 *   - Calls initialize_data to initialize the data structure.
 *   - Sets up the environment, arguments, and here_doc flag.
 *   - Opens input and output files using get_input_file and get_output_file.
 *   - Allocates memory for pipes and process IDs.
 *   - Generates the required pipes using create_pipes.
 *   - Exits with an error message if any memory allocation fails.
 */
t_data init(int ac, char **av, char **envp)
{
	t_data data;

	data = initialize_data();
	data.envp = envp;
	data.ac = ac;
	data.av = av;
	if (!ft_strncmp("here_doc", av[1], 9))
		data.heredoc = 1;

	get_input_file(&data);
	get_output_file(&data);

	data.nb_cmds = ac - 3 - data.heredoc;
	data.pids = malloc(sizeof(*data.pids) * data.nb_cmds);
	if (!data.pids)
		exit_error(msg("PID error", strerror(errno), "", 1), &data);

	data.pipe = malloc(sizeof(*data.pipe) * 2 * (data.nb_cmds - 1));
	if (!data.pipe)
		exit_error(msg("Pipe error", "", "", 1), &data);

	create_pipes(&data);
	return (data);
}
