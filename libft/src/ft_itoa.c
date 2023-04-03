/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/29 13:33:28 by lmiehler          #+#    #+#             */
/*   Updated: 2022/11/30 18:55:28 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

static int	nbrlen(int n)
{
	int	count;

	count = 0;
	if (n <= 0)
		++count;
	while (n)
	{
		n /= 10;
		++count;
	}
	return (count);
}

char	*ft_itoa(int n)
{
	char	*ret;
	int		len;
	long	nn;

	nn = n;
	len = nbrlen(n);
	ret = malloc(sizeof(*ret) * (len + 1));
	if (!ret)
		return (NULL);
	ret[len] = '\0';
	if (n == 0)
		ret[0] = '0';
	if (n < 0)
	{
		nn = -nn;
		ret[0] = '-';
	}
	while (nn)
	{
		ret[len - 1] = '0' + (nn % 10);
		nn /= 10;
		--len;
	}
	return (ret);
}
