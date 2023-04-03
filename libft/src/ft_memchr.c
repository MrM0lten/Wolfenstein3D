/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*													+:+ +:+		 +:+	 */
/*   By: lmiehler <marvin@42.fr>					+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2022/11/29 15:21:37 by lmiehler		  #+#	#+#			 */
/*   Updated: 2022/11/29 17:42:26 by lmiehler         ###   ########.fr       */
/*																			*/
/* ************************************************************************** */

#include <stddef.h>

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char	*ptr;

	ptr = (unsigned char *)s;
	while (n--)
	{
		if (*ptr == (unsigned char)c)
			return ((void *)ptr);
		ptr++;
	}
	return (NULL);
}
