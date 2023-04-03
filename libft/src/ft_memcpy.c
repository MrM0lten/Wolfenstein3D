/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*													+:+ +:+		 +:+	 */
/*   By: lmiehler <marvin@42.fr>					+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2022/11/29 14:47:58 by lmiehler		  #+#	#+#			 */
/*   Updated: 2022/11/29 17:43:42 by lmiehler         ###   ########.fr       */
/*																			*/
/* ************************************************************************** */

#include <stddef.h>

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char	*p;
	unsigned char	*s;

	p = (unsigned char *)dest;
	s = (unsigned char *)src;
	while (n--)
	{
		*p = *s;
		p++;
		s++;
	}
	return (dest);
}
