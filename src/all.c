my program/project is divided into 5 files:
env_parsing.c
file_handler.c
init.c
pipex.c
utils.c

what would be a better way to organize my code?



//env_parsing.c

/* 
 * get_env_path:
 *   Searches for the "PATH=" entry in the environment variable array.
 *
 * Parameters:
 *   envp - Environment variables array.
 *
 * Returns:
 *   A dynamically allocated string containing the PATH environment variable (excluding "PATH=").
 *   Returns NULL if the PATH variable is not found or if memory allocation fails.
 *
 * Notes:
 *   - The function iterates through the envp array to find the entry starting with "PATH=".
 *   - Upon finding the PATH entry, it allocates a new string containing just the paths (removing "PATH=").
 */
static char *get_env_path(char **envp)
{
    char *path;
    int i;

    i = 0;
    path = NULL;
    while (envp[i] != NULL && envp[i][0] != '\0')
    {
        path = ft_strnstr(envp[i], "PATH=", 5);
        if (path)
        {
            path = ft_substr(path, 5, ft_strlen(path));
            break;
        }
        i++;
    }
    return (path);
}

/* 
 * make_usable_paths:
 *   Appends a '/' character at the end of each path string in the array.
 *
 * Parameters:
 *   paths - Array of path strings.
 *
 * Returns:
 *   The modified array of path strings with a '/' appended to each.
 *
 * Notes:
 *   - Iterates through the paths array and appends a '/' to each string.
 *   - Uses dynamic memory allocation for the new strings; the original strings are freed.
 */
static char **make_usable_paths(char **paths)
{
    int i;
    char *tmp;

    i = 0;
    tmp = NULL;
    while (paths[i])
    {
        tmp = paths[i];
        paths[i] = ft_strjoin(paths[i], "/");
        free_strs(tmp, NULL);
        i++;
    }
    return (paths);
}




/* 
 * get_env_paths:
 *   Retrieves the PATH environment variable and splits it into individual paths.
 *
 * Parameters:
 *   envp - Environment variables array.
 *
 * Returns:
 *   An array of strings, each representing a path from the PATH environment variable.
 *   Returns NULL if the PATH variable is not found or if memory allocation fails.
 *
 * Notes:
 *   - Calls get_env_path to obtain the PATH variable.
 *   - Splits the PATH string into individual paths using ':' as the delimiter.
 *   - Calls make_usable_paths to append a '/' to each path string.
 */
static char **get_env_paths(char **envp)
{
    char *env_path_str;
    char **paths;

    env_path_str = get_env_path(envp);
    if (!env_path_str)
        return (NULL);
    paths = ft_split(env_path_str, ':');
    free_strs(env_path_str, NULL);
    if (!paths)
        return (NULL);
    paths = make_usable_paths(paths);
    if (!paths)
        return (NULL);
    return (paths);
}


/* 
 * get_cmd_path:
 *   Searches for the specified command in the given paths.
 *
 * Parameters:
 *   cmd - The command to find.
 *   paths - Array of path strings.
 *
 * Returns:
 *   A dynamically allocated string containing the full path to the command if found.
 *   Returns NULL if the command is not found in the given paths.
 *
 * Notes:
 *   - Iterates through each path and checks if the command exists and is executable in that path.
 *   - If found, returns the full path to the command.
 *   - If the command is not found, or if memory allocation fails, returns NULL.
 */
static char *get_cmd_path(char *cmd, char **paths)
{
    int i;
    char *cmd_path;

    cmd_path = NULL;
    i = 0;
    while (paths[i])
    {
        cmd_path = ft_strjoin(paths[i], cmd);
        if (!cmd_path)
        {
            free_strs(NULL, paths);
            exit_error(msg("unexpected error", "", "", 1), NULL);
        }
        if (access(cmd_path, F_OK | X_OK) == 0)
            return (cmd_path);
        free_strs(cmd_path, NULL);
        i++;
    }
    return (NULL);
}


/* 
 * get_cmd:
 *   Retrieves the full path of a given command from the environment paths.
 *
 * Parameters:
 *   cmd - The command to find.
 *   data - Pointer to a t_data structure containing environment variables.
 *
 * Returns:
 *   A dynamically allocated string containing the full path to the command.
 *   Returns NULL if the command is not found in the environment paths.
 *
 * Notes:
 *   - First checks if the command is directly accessible (e.g., absolute path or in the current directory).
 *   - If not, it retrieves the environment paths and searches for the command in them.
 *   - If the command is not found, a "command not found" message is displayed.
 */
char *get_cmd(char *cmd, t_data *data)
{
    char **env_paths;
    char *cmd_path;

    if (access(cmd, F_OK | X_OK) == 0)
        return (ft_strdup(cmd));
    env_paths = get_env_paths(data->envp);
    if (!env_paths)
        return (NULL);
    cmd_path = get_cmd_path(cmd, env_paths);
    if (!cmd_path)
        msg("command not found", ": ", data->av[data->child + 2], 1);
    free_strs(NULL, env_paths);
    return (cmd_path);
}








//file_handler.c

/* 
 * get_input_file:
 *   Sets up the input file descriptor for pipex.
 *
 * Parameters:
 *   data - Pointer to a t_data structure containing necessary information.
 *
 * Functionality:
 *   - If heredoc is enabled (data->heredoc == 1), it creates a temporary file using get_heredoc.
 *   - Opens the temporary file or the specified input file as the input file descriptor.
 *   - If an error occurs while opening, it exits with an error message.
 */
void get_input_file(t_data *data)
{
    if (data->heredoc == 1)
    {
        get_heredoc(data);
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
 * get_heredoc:
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
void get_heredoc(t_data *data)
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





//init.c

/* 
 * clean_init:
 *   Initializes and returns a t_data structure with default values.
 *
 * Returns:
 *   An initialized t_data structure with all fields set to their default state.
 *   String pointers are set to NULL and integer values are set to -1.
 *
 * Notes:
 *   - The function sets file descriptors and other integral fields to -1, 
 *     as 0 is a valid file descriptor and should not be used for initialization.
 */
static t_data clean_init(void)
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
 * generate_pipes:
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
static void generate_pipes(t_data *data)
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
 *   - Calls clean_init to initialize the data structure.
 *   - Sets up the environment, arguments, and here_doc flag.
 *   - Opens input and output files using get_input_file and get_output_file.
 *   - Allocates memory for pipes and process IDs.
 *   - Generates the required pipes using generate_pipes.
 *   - Exits with an error message if any memory allocation fails.
 */
t_data init(int ac, char **av, char **envp)
{
    t_data data;

    data = clean_init();
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

    generate_pipes(&data);
    return (data);
}
















//pipex.c
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
 * child:
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
static void child(t_data *data)
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




/* parent:
*	Waits for the children processes to finish and fetches the status of the last
*	child.
*	Returns the exit status code of the last child process.
*/
static int	parent(t_data *data)
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


/* 
 * pipex:
 *   Manages the execution of a pipeline of commands.
 *
 * Parameters:
 *   data - Pointer to a t_data structure containing necessary information for pipelined command execution.
 *
 * Functionality:
 *   - Sets up a pipe and forks child processes for each command in the pipeline.
 *   - Each child process handles the execution of one command.
 *   - The parent process waits for all child processes to complete and collects their exit codes.
 *
 * Returns:
 *   The exit code of the last child in the pipeline.
 *
 * Notes:
 *   - The function creates a pipe using the pipe system call and checks for errors.
 *   - Each command and its options are parsed and executed in separate child processes.
 *   - The function handles errors in pipe creation, command parsing, and process forking.
 *   - If a heredoc is used, the temporary file is removed after execution.
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
            child(data);
        }

        // Free allocated strings for the current command
        free_strs(data->cmd_path, data->cmd_options);

        // Move to the next command
        data->child++;
    }

    // Parent process: Wait for children and get the exit code
    exit_code = parent(data);

    // Remove temporary file if heredoc was used
    if (data->heredoc == 1) {
        unlink(".heredoc.tmp");
    }

    return exit_code;
}



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












//utils.c


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
    int i;

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
void exit_error(int error_status, t_data *data)
{
    if (data)
    {
        close_fds(data);
        if (data->pipe)
            free(data->pipe);
        if (data->pids)
            free(data->pids);
        if (data->cmd_options || data->cmd_path)
            free_strs(data->cmd_path, data->cmd_options);
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

