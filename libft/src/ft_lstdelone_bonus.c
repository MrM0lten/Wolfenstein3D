/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   ft_lstdelone.c                                     :+:      :+:    :+:   */
/*													+:+ +:+		 +:+	 */
/*   By: lmiehler <marvin@42.fr>					+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2022/11/29 15:31:44 by lmiehler		  #+#	#+#			 */
/*   Updated: 2022/11/29 17:39:02 by lmiehler         ###   ########.fr       */
/*																			*/
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

void	ft_lstdelone(t_list *lst, void (*del)(void *))
{
	if (!lst)
		return ;
	del(lst->content);
	free(lst);
}
