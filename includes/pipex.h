/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-alen <fde-alen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/07 22:36:23 by fde-alen          #+#    #+#             */
/*   Updated: 2024/01/21 19:30:48 by fde-alen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# define ERROR 1

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <errno.h>

# include <fcntl.h>
# include <unistd.h>
# include <sys/wait.h>

# include "../libs/ft_printf/ft_printf.h"
# include "../libs/libft/libft.h"


/*  Data    */
typedef struct s_data
{
	char	**envp;
	char	**av;
	int		ac;
	int		heredoc_flag;
	int		input_fd;
	int		output_fd;
	int		*pipe;
	int		cmd_count;
	int		child;
	int		*pids;
	char	**cmd_options;
	char	*cmd_path;
}		t_data;

/*  file_handler.c */
void	get_input_file(t_data *data);
void	get_output_file(t_data *data);

/*  data_init.c    */
t_data	init_data(int ac, char **av, char **envp);

/*  env_parsing.c   */
char	*get_cmd(char *cmd, t_data *data);

/*  pipex_utils.c */
void	cleanup_n_exit(int error_status, t_data *data);
void	close_fds(t_data *data);
void	free_strs(char *str, char **strs);
int		invalid_args(int argc, char **argv, char **envp);



/*functions from libft*/
/*ft_putchar_fd
ft_putendl_fd
ft_putstr_fd
ft_split
ft_strjoin
ft_strlen
ft_strncmp
ft_strnstr
ft_substr
ft_strlcpy
ft_strdup
get_next_line*/

/*functions from ft_printf*/
//ft_printf - writes to STDERR

#endif
