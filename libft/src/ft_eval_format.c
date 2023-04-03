/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_eval_format.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <lmiehler@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 15:56:28 by lmiehler          #+#    #+#             */
/*   Updated: 2022/12/14 15:56:28 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdarg.h>
#include "utils.h"
#include "ft_printf.h"
#include "libft.h"

static int	ft_flags(t_format *f, const char *format)
{
	int	i;

	i = 0;
	while (format[i])
	{
		if (format[i] == '-')
			f->minus = 1;
		else if (format[i] == '0')
			f->zero = 1;
		else if (format[i] == '#')
			f->hash = 1;
		else if (format[i] == ' ')
			f->space = 1;
		else if (format[i] == '+')
			f->plus = 1;
		else
			break ;
		i++;
	}
	return (i);
}

static int	ft_width(t_format *f, const char *format)
{
	f->width = ft_atoi(format);
	if (f->width == 0)
		return (0);
	return (nbrlen(f->width, 10));
}

static int	ft_precision(t_format *f, const char *format)
{
	int	i;

	i = 0;
	if (format[i] == '.')
	{
		f->precision = ft_atoi(&format[i + 1]);
		if (ft_isdigit(format[i + 1]))
			i++;
		i += nbrlen(f->precision, 10);
	}
	return (i);
}

static int	ft_type(t_format *f, const char *format, va_list *args)
{
	if (*format == 'c')
		print_c(f, va_arg(*args, int));
	else if (*format == 's')
		print_s(f, va_arg(*args, const char *));
	else if (*format == 'd' || *format == 'i')
		print_i(f, va_arg(*args, int));
	else if (*format == 'u')
		print_u(f, va_arg(*args, unsigned long long));
	else if (*format == 'x')
		print_x(f, va_arg(*args, unsigned int));
	else if (*format == 'X')
	{
		f->lower = 0;
		print_x(f, va_arg(*args, unsigned int));
	}
	else if (*format == 'p')
		print_p(f, va_arg(*args, unsigned long long));
	else if (*format == '%')
		print_c(f, '%');
	return (1);
}

int	ft_eval_format(t_format *f, va_list *args, const char *format)
{
	int	len;

	len = 0;
	len += ft_flags(f, format);
	len += ft_width(f, format + len);
	len += ft_precision(f, format + len);
	len += ft_type(f, format + len, args);
	return (len);
}
