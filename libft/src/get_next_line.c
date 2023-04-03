/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <lmiehler@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/22 17:23:23 by lmiehler          #+#    #+#             */
/*   Updated: 2023/01/13 21:22:45 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "get_next_line.h"

static char	*read_backup(t_line_info *l, t_backup *b)
{
	if (b->read_state == 0)
		return (NULL);
	l->nl_pos = find_nl(b->backup, b->nl_pos + 1, b->last_pos);
	if (l->nl_pos == -1)
	{
		l->line = str_extract(b->backup, b->nl_pos + 1, b->last_pos);
		l->linesize = b->last_pos - b->nl_pos;
		b->read_state = 0;
	}
	else
	{
		l->line = str_extract(b->backup, b->nl_pos + 1, l->nl_pos);
		l->linesize = l->nl_pos - b->nl_pos;
		if (l->nl_pos == b->last_pos)
			b->read_state = 0;
		else
			b->read_state = 1;
		b->nl_pos = l->nl_pos;
	}
	return (l->line);
}

static char	*line_update(t_line_info *l, char *buff, ssize_t bytes)
{
	if (l->nl_pos == -1)
	{
		l->line = gnl_strcat(l->line, buff, l->linesize, bytes);
		l->linesize += bytes;
	}
	else
	{
		l->line = gnl_strcat(l->line, buff, l->linesize, l->nl_pos + 1);
		l->linesize += l->nl_pos + 1;
	}
	return (l->line);
}

static void	ub(t_line_info *l, t_backup *b, char *buff, ssize_t bytes)
{
	ssize_t	i;

	i = 0;
	if (l->nl_pos == bytes - 1)
	{
		b->read_state = 0;
		return ;
	}
	b->nl_pos = l->nl_pos;
	while (l->nl_pos + i < bytes)
	{
		b->backup[l->nl_pos + i] = buff[l->nl_pos + i];
		i++;
	}
	b->last_pos = bytes - 1;
	b->read_state = 1;
}

static char	*read_file(t_line_info *l, t_backup *b, int fd)
{
	l->buff = malloc(BUFFER_SIZE);
	if (!l->buff)
		return (NULL);
	while (l->bytes != 0)
	{
		l->bytes = read(fd, l->buff, BUFFER_SIZE);
		if (bytecheck(l, l->buff, l->bytes) == 0)
			return (NULL);
		l->nl_pos = find_nl(l->buff, 0, l->bytes - 1);
		line_update(l, l->buff, l->bytes);
		if (l->nl_pos != -1)
		{
			ub(l, b, l->buff, l->bytes);
			free(l->buff);
			return (l->line);
		}
	}
	free(l->buff);
	if (l->linesize != 0)
		return (l->line);
	free(l->line);
	return (NULL);
}

char	*get_next_line(int fd)
{
	static t_backup	b;
	t_line_info		l;

	init_l_info(&l);
	l.line = read_backup(&l, &b);
	if (l.nl_pos == -1)
		l.line = read_file(&l, &b, fd);
	return (l.line);
}
