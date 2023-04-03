/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <lmiehler@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 15:56:17 by lmiehler          #+#    #+#             */
/*   Updated: 2023/01/23 19:38:09 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

typedef struct s_format
{
	int		fd;
	int		len;
	int		hash;
	int		zero;
	int		minus;
	int		space;
	int		plus;
	int		width;
	int		precision;
	int		base;
	int		lower;
	char	prefix[3];
}t_format;

t_format	*init_f(t_format *f);
int			nbrlen(long n, int base);
int			prefix_len(t_format *f);
void		print_prefix(t_format *f);
void		putnbr_base(t_format *f, unsigned long long u);
void		l_align(t_format *f, unsigned long long i);
void		r_align(t_format *f, unsigned long long i);

#endif