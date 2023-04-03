/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*													+:+ +:+		 +:+	 */
/*   By: lmiehler <marvin@42.fr>					+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2022/11/29 15:10:25 by lmiehler		  #+#	#+#			 */
/*   Updated: 2022/11/29 17:38:55 by lmiehler         ###   ########.fr       */
/*																			*/
/* ************************************************************************** */

#include <stdlib.h>
#include <stddef.h>
#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*tmp;

	while ((*lst))
	{
		tmp = (*lst)->next;
		del((*lst)->content);
		free((*lst));
		(*lst) = tmp;
	}
	*lst = NULL;
}
