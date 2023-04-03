/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <lmiehler@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 15:55:53 by lmiehler          #+#    #+#             */
/*   Updated: 2022/12/14 15:55:53 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include "utils.h"

int		ft_printf(const char *format, ...);
int		ft_fprintf(int fd, ...);
int		ft_eval_format(t_format *f, va_list *args, const char *format);
void	print_i(t_format *f, int i);
void	print_percent(t_format *f);
void	print_c(t_format *f, unsigned char c);
void	print_s(t_format *f, const char *s);
void	print_u(t_format *f, unsigned int u);
void	print_x(t_format *f, unsigned long long x);
void	print_p(t_format *f, unsigned long long x);

#endif