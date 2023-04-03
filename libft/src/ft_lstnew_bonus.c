/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*													+:+ +:+		 +:+	 */
/*   By: lmiehler <marvin@42.fr>					+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2022/11/29 15:19:29 by lmiehler		  #+#	#+#			 */
/*   Updated: 2022/11/29 17:39:33 by lmiehler         ###   ########.fr       */
/*																			*/
/* ************************************************************************** */

#include <stdlib.h>
#include <stddef.h>
#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*new;

	new = malloc(sizeof(t_list));
	if (!new)
		return (NULL);
	new->content = content;
	new->next = NULL;
	return (new);
}
