/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   align.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <lmiehler@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 15:57:12 by lmiehler          #+#    #+#             */
/*   Updated: 2023/01/23 19:58:03 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "utils.h"

void	l_align(t_format *f, unsigned long long i)
{
	if (*f->prefix)
		print_prefix(f);
	putnbr_base(f, i);
	while (f->width - f->len > 0)
		f->len += write(f->fd, " ", 1);
}

void	r_align(t_format *f, unsigned long long i)
{
	char	fill;
	int		occupied;

	fill = ' ';
	if (f->precision == -1 && f->zero)
	{
		fill = '0';
		if (*f->prefix)
			print_prefix(f);
	}
	occupied = 0;
	occupied += nbrlen(i, f->base);
	if (f->precision > occupied)
		occupied = f->precision;
	if (*f->prefix && !(f->precision == -1 && f->zero))
		occupied += prefix_len(f);
	while (f->width - occupied - f->len > 0)
		f->len += write(f->fd, &fill, 1);
	if ((!f->zero && *f->prefix) || f->precision != -1)
		print_prefix(f);
	putnbr_base(f, i);
}
