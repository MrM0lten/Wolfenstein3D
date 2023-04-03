/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <lmiehler@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 15:56:34 by lmiehler          #+#    #+#             */
/*   Updated: 2022/12/14 15:56:34 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdarg.h>
#include <unistd.h>
#include "ft_printf.h"
#include "utils.h"

void	print_c(t_format *f, unsigned char c)
{
	if (f->width && !f->minus)
		while (--f->width)
			f->len += write(f->fd, " ", 1);
	f->len += write(f->fd, &c, 1);
	if (f->width && f->minus)
		while (--f->width)
			f->len += write(f->fd, " ", 1);
}

int	ft_vfprint(int fd, const char *format, va_list *args)
{
	int			print_len;
	t_format	f;

	print_len = 0;
	f.fd = fd;
	while (*format)
	{
		if (*format == '%')
		{
			init_f(&f);
			format += ft_eval_format(&f, args, format + 1);
			print_len += f.len;
		}
		else
			print_len += write(f.fd, format, 1);
		format++;
	}
	return (print_len);
}

int	ft_fprintf(int fd, ...)
{
	va_list		args;
	int			print_len;
	char		*format;

	print_len = 0;
	va_start(args, fd);
	format = va_arg(args, char *);
	print_len = ft_vfprint(fd, format, &args);
	va_end(args);
	return (print_len);
}

int	ft_printf(const char *format, ...)
{
	va_list		args;
	int			print_len;

	print_len = 0;
	va_start(args, format);
	print_len = ft_vfprint(STDOUT_FILENO, format, &args);
	va_end(args);
	return (print_len);
}

int	ft_error(const char *format, ...)
{
	va_list		args;
	int			print_len;

	print_len = 0;
	va_start(args, format);
	print_len = ft_vfprint(STDERR_FILENO, format, &args);
	va_end(args);
	return (print_len);
}
