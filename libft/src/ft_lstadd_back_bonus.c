/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmiehler <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/29 13:18:50 by lmiehler          #+#    #+#             */
/*   Updated: 2022/12/02 13:00:56 by lmiehler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new_elem)
{
	t_list	*tmp;

	if (!(*lst))
	{
		*lst = new_elem;
		return ;
	}
	tmp = ft_lstlast(*lst);
	tmp->next = new_elem;
}
