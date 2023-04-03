/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   ft_lstsize.c                                       :+:      :+:    :+:   */
/*													+:+ +:+		 +:+	 */
/*   By: lmiehler <marvin@42.fr>					+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2022/11/29 15:18:15 by lmiehler		  #+#	#+#			 */
/*   Updated: 2022/11/29 17:39:40 by lmiehler         ###   ########.fr       */
/*																			*/
/* ************************************************************************** */

#include <stddef.h>
#include "libft.h"

int	ft_lstsize(t_list *lst)
{
	int	count;

	count = 0;
	while (lst)
	{
		++count;
		lst = lst->next;
	}
	return (count);
}
