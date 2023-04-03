/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/29 15:20:25 by lmiehler          #+#    #+#             */
/*   Updated: 2022/12/02 13:00:32 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdlib.h>
#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*substr;
	size_t	i;
	size_t	sublen;

	i = 0;
	if (start >= ft_strlen(s))
		sublen = 1;
	else if (ft_strlen(s) - start + 1 > len + 1)
		sublen = len + 1;
	else
		sublen = ft_strlen(s) - start + 1;
	substr = malloc(sizeof(char) * sublen);
	if (!substr)
		return (NULL);
	while (i < sublen - 1)
	{
		substr[i] = s[start + i];
		++i;
	}
	substr[i] = '\0';
	return (substr);
}
