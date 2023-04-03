/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <lmiehler@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 15:56:46 by lmiehler          #+#    #+#             */
/*   Updated: 2023/01/23 19:55:51 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "utils.h"

t_format	*init_f(t_format *f)
{
	f->len = 0;
	f->hash = 0;
	f->zero = 0;
	f->minus = 0;
	f->space = 0;
	f->plus = 0;
	f->width = 0;
	f->precision = -1;
	f->base = 10;
	f->lower = 'a' - 'A';
	f->prefix[0] = 0;
	f->prefix[1] = 0;
	f->prefix[2] = 0;
	return (f);
}

int	nbrlen(long n, int base)
{
	int	len;

	if (n == 0)
		return (1);
	len = 0;
	while (n)
	{
		n /= base;
		len++;
	}
	return (len);
}

void	putnbr_base(t_format *f, unsigned long long u)
{
	if (u >= (unsigned long long)f->base || f->precision > 1)
	{
		f->precision--;
		putnbr_base(f, u / f->base);
	}
	if (f->lower)
		f->len += write(f->fd, &"0123456789abcdef"[u % f->base], 1);
	else
		f->len += write(f->fd, &"0123456789ABCDEF"[u % f->base], 1);
}

void	print_prefix(t_format *f)
{
	int	i;

	i = 0;
	while (f->prefix[i])
	{
		f->len += write(f->fd, &(f->prefix[i]), 1);
		i++;
	}
}

int	prefix_len(t_format *f)
{
	int	i;

	i = 0;
	while (f->prefix[i])
		i++;
	return (i);
}
