/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <lmiehler@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/22 17:32:54 by lmiehler          #+#    #+#             */
/*   Updated: 2022/12/25 16:34:24 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

void	init_l_info(t_line_info *l_info)
{
	l_info->line = NULL;
	l_info->linesize = 0;
	l_info->nl_pos = -1;
	l_info->bytes = 1;
}

ssize_t	find_nl(char *buff, ssize_t start, ssize_t end)
{
	ssize_t	i;

	i = start;
	while (i <= end)
	{
		if (buff[i] == '\n')
		{
			return (i);
		}
		i++;
	}
	return (-1);
}

int	bytecheck(t_line_info *l, char *buff, ssize_t bytes)
{
	if (bytes == -1)
	{
		free(l->line);
		free (buff);
		return (0);
	}
	return (1);
}

char	*gnl_strcat(char *buff1, char *buff2, size_t b1s, size_t b2s)
{
	char	*buff;
	size_t	i;

	if (buff1 == NULL && b2s == 0)
		return (NULL);
	i = 0;
	buff = malloc(b1s + b2s + 1);
	if (!buff)
		return (NULL);
	while (i < b1s)
	{
		buff[i] = buff1[i];
		i++;
	}
	i = 0;
	while (i < b2s)
	{
		buff[b1s + i] = buff2[i];
		i++;
	}
	buff[b1s + b2s] = '\0';
	free(buff1);
	return (buff);
}

char	*str_extract(char *buff, ssize_t start, ssize_t end)
{
	char	*ret;
	ssize_t	i;

	i = 0;
	ret = malloc((end - start) + 1 + 1);
	if (!ret)
		return (NULL);
	while (start + i <= end)
	{
		ret[i] = buff[start + i];
		i++;
	}
	ret[i] = '\0';
	return (ret);
}
