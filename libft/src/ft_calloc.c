/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*													+:+ +:+		 +:+	 */
/*   By: lmiehler <marvin@42.fr>					+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2022/11/29 14:45:23 by lmiehler		  #+#	#+#			 */
/*   Updated: 2022/12/02 12:57:46 by lmiehler         ###   ########.fr       */
/*																			*/
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*ret;

	ret = malloc(nmemb * size);
	if (ret == NULL)
		return (NULL);
	ft_bzero(ret, nmemb * size);
	return (ret);
}
