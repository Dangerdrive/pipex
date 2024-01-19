


#include "pipex.h"

/**
 * Extracts the value of the PATH environment variable.
 *
 * Iterates through the array of environment variables (`envp`) to find the PATH
 * variable. It uses `ft_strnstr` to search for the substring "PATH=" in each
 * environment string. Once found, it extracts the value of PATH using `ft_substr`,
 * skipping the first 5 characters which constitute "PATH=". The search stops as soon
 * as the PATH variable is found or when the end of the array is reached.
 *
 * @param[in] envp Array of environment variables, each as a string.
 *
 * @return A pointer to a newly allocated string containing the value of PATH.
 *     Returns NULL if PATH is not found or in case of memory allocation failure.
 */
static char *extract_path_var(char **envp)
{
	char	*path;
	char	*found;
	int	 i;

	path = NULL;
	i = 0;
	while (envp[i] != NULL && envp[i][0] != '\0' && path == NULL)
	{
		found = ft_strnstr(envp[i], "PATH=", 5);
		if (found)
			path = ft_substr(found, 5, ft_strlen(found) - 5);
		i++;
	}
	return (path);
}

// /* 
//  * make_usable_paths:
//  *   Appends a '/' character at the end of each path string in the array.
//  *
//  * Parameters:
//  *   paths - Array of path strings.
//  *
//  * Returns:
//  *   The modified array of path strings with a '/' appended to each.
//  *
//  * Notes:
//  *   - Iterates through the paths array and appends a '/' to each string.
//  *   - Uses dynamic memory allocation for the new strings; the original strings are freed.
//  */
// static char **make_usable_paths(char **paths)
// {
//   int i;
//   char *tmp;

//   i = 0;
//   tmp = NULL;
//   while (paths[i])
//   {
//     tmp = paths[i];
//     paths[i] = ft_strjoin(paths[i], "/");
//     //free_strs(tmp, NULL);
//     free(tmp);
//    tmp = NULL;
//     i++;
//   }
//   return (paths);
// }




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


/**
 * Retrieves and processes the PATH environment variable.
 *
 * Extracts the PATH environment variable from the provided 'envp' array and splits
 * it into individual paths using ':' as the delimiter. Each path string is then
 * appended with a '/' character to make them directly usable for file path construction.
 * The function first calls 'extract_path_var' to extract the PATH variable and then
 * uses 'ft_split' to separate it into individual paths. After splitting, it appends
 * '/' to each path. If any memory allocation fails during these processes, the function
 * will free the allocated memory and return NULL.
 *
 * @param[in] envp The array of environment variables.
 *
 * @return An array of strings, each representing a path from the PATH environment
 *     variable with a '/' appended, or NULL if any allocation fails.
 */
static char **get_env_paths(char **envp)
{
    char *env_path_str;
    char **paths;
    int i;
    char *tmp;

    i = 0;
    env_path_str = extract_path_var(envp);
    if (!env_path_str)
        return (NULL);
    paths = ft_split(env_path_str, ':');
    free(env_path_str);
    if (!paths)
        return (NULL);
    while (paths[i])
    {
        tmp = paths[i];
        paths[i] = ft_strjoin(paths[i], "/");
        free(tmp);
        if (!paths[i])
            return (NULL);
        i++;
    }
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

/**
 * Finds the full path of a command by searching through a set of directories.
 *
 * This function takes a command name and an array of directory paths, then
 * iterates through each directory to check if the command exists and is
 * executable within that directory. It constructs the full path of the command
 * by joining the directory path with the command name.
 *
 * @param cmd The command name to search for.
 * @param paths An array of directory paths to search the command in.
 * @return A dynamically allocated string containing the full path to the command if found,
 *     otherwise returns NULL. The caller is responsible for freeing this string.
 *
 * Note: If the command is not found in any of the provided paths, or if there is an
 *     error during path construction, the function returns NULL. If an error occurs,
 *     the function also frees the paths array and exits with an error message.
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
        //free_strs(cmd_path, NULL);
        free(cmd_path);
        cmd_path = NULL;
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
