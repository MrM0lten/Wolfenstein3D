/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*													+:+ +:+		 +:+	 */
/*   By: lmiehler <marvin@42.fr>					+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2022/11/29 15:12:25 by lmiehler		  #+#	#+#			 */
/*   Updated: 2022/11/30 19:41:07 by lmiehler         ###   ########.fr       */
/*																			*/
/* ************************************************************************** */

#include <stddef.h>

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*p;

	p = (unsigned char *)s;
	while (n--)
	{
		*p = (unsigned char)c;
		p++;
	}
	return (s);
}
