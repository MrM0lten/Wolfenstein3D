/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/29 14:44:50 by lmiehler          #+#    #+#             */
/*   Updated: 2022/12/02 12:04:52 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdlib.h>
#include "libft.h"

void	free_strs(char *strs[])
{
	int	i;

	i = 0;
	while (strs[i])
	{
		free(strs[i]);
		i++;
	}
	free(strs);
}

int	count_words(char const *str, char sep)
{
	int	count;

	count = 0;
	while (*str)
	{
		if (*str != sep)
		{
			++count;
			while (*str && *str != sep)
				++str;
		}
		else
			++str;
	}
	return (count);
}

// static char	*strextract(char *str, int index, int len)
// {
// 	char	*ret;

// 	ret = malloc(sizeof(char) * (len + 1));
// 	if (!ret)
// 		return (NULL);
// 	ret[len] = '\0';
// 	ft_memcpy(ret, str - len, len);
// 	return (ret);
// }

char	**ft_split(char const *s, char c)
{
	char	**ret;
	size_t	len;
	size_t	i;

	i = 0;
	ret = malloc(sizeof(*ret) * (count_words(s, c) + 1));
	if (!ret)
		return (NULL);
	while (*s)
	{
		if (*s != c)
		{
			len = 0;
			while (*s && *s != c)
			{
				++s;
				++len;
			}
			ret[i] = malloc(sizeof(char) * (len + 1));
			ret[i][len] = '\0';
			ft_memcpy(ret[i++], s - len, len);
		}
		else
			++s;
	}
	ret[i] = NULL;
	return (ret);
}
