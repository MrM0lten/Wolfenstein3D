/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jisserst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/08 13:58:58 by jisserst          #+#    #+#             */
/*   Updated: 2023/01/08 13:59:34 by jisserst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

void	*ft_realloc(void *old, size_t old_size, size_t new_size)
{
	void	*new;
	size_t	copy_size;

	new = malloc(new_size);
	copy_size = old_size;
	if (new_size < old_size)
		copy_size = new_size;
	ft_memcpy(new, old, copy_size);
	free(old);
	return (new);
}
