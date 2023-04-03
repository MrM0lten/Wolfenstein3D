/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_front.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/29 15:20:03 by lmiehler          #+#    #+#             */
/*   Updated: 2022/11/29 17:38:35 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stddef.h>
#include "libft.h"

void	ft_lstadd_front(t_list **lst, t_list *new_elem)
{
	new_elem->next = *lst;
	*lst = new_elem;
}
