/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <lmiehler@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/22 17:19:21 by lmiehler          #+#    #+#             */
/*   Updated: 2022/12/25 16:34:33 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <sys/types.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 4096
# endif

typedef struct s_backup
{
	char	backup[BUFFER_SIZE];
	char	read_state;
	ssize_t	last_pos;
	ssize_t	nl_pos;
}t_backup;

typedef struct s_line_info
{
	char	*line;
	ssize_t	bytes;
	char	*buff;
	size_t	linesize;
	ssize_t	nl_pos;
}t_line_info;

char	*get_next_line(int fd);
void	init_l_info(t_line_info *l_info);
ssize_t	find_nl(char *buff, ssize_t start, ssize_t size);
char	*gnl_strcat(char *buff1, char *buff2, size_t b1s, size_t b2s);
char	*str_extract(char *buff, ssize_t start, ssize_t end);
int		bytecheck(t_line_info *l, char *buff, ssize_t bytes);

#endif
