/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prints.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <lmiehler@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 15:56:40 by lmiehler          #+#    #+#             */
/*   Updated: 2023/01/23 19:58:17 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "libft.h"
#include "utils.h"

void	print_s(t_format *f, const char *s)
{
	int	len;
	int	tmp;

	if (!s)
		len = 6;
	else
	{
		len = ft_strlen(s);
		if (f->precision != -1 && f->precision < len)
			len = f->precision;
	}
	if (!s && f->precision != -1 && f->precision < 7)
		len = 0;
	if (f->width && !f->minus)
		while (f->width-- - len > 0)
			f->len += write(f->fd, " ", 1);
	tmp = len;
	if (!s && (f->precision > 5 || f->precision == -1))
		f->len += write(f->fd, "(null)", 6);
	else
		while (len--)
			f->len += write(f->fd, s++, 1);
	if (f->width && f->minus)
		while (f->width-- - tmp > 0)
			f->len += write(f->fd, " ", 1);
}

void	print_i(t_format *f, int i)
{
	unsigned long long	n;

	if (f->precision == 0 && i == 0)
	{
		while (f->width-- > 0)
			f->len += write(f->fd, " ", 1);
		return ;
	}
	if (i >= 0)
		n = (unsigned long long)i;
	if (i < 0)
	{
		f->prefix[0] = '-';
		n = -(long)i;
	}
	else if (f->plus)
		f->prefix[0] = '+';
	else if (f->space)
		f->prefix[0] = ' ';
	if (f->width && f->minus)
		l_align(f, n);
	else
		r_align(f, n);
}

void	print_x(t_format *f, unsigned long long x)
{
	if (f->precision == 0 && x == 0)
	{
		while (f->width-- > 0)
			f->len += write(f->fd, " ", 1);
		return ;
	}
	f->base = 16;
	if (f->hash && x != 0)
	{
		f->prefix[0] = '0';
		f->prefix[1] = 'X' + f->lower;
	}
	if (f->width && f->minus)
		l_align(f, x);
	else
		r_align(f, x);
}

void	print_p(t_format *f, unsigned long long x)
{
	if (x == 0)
	{
		f->precision = -1;
		print_s(f, "(nil)");
		return ;
	}
	f->hash = 1;
	print_x(f, x);
}

void	print_u(t_format *f, unsigned int u)
{
	if (f->precision == 0 && u == 0)
	{
		while (f->width-- > 0)
			f->len += write(f->fd, " ", 1);
		return ;
	}
	if (f->width && f->minus)
		l_align(f, u);
	else
		r_align(f, u);
}
