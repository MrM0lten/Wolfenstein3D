/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*													+:+ +:+		 +:+	 */
/*   By: lmiehler <marvin@42.fr>					+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2022/11/29 15:15:43 by lmiehler		  #+#	#+#			 */
/*   Updated: 2022/11/29 17:43:21 by lmiehler         ###   ########.fr       */
/*																			*/
/* ************************************************************************** */

#include <stddef.h>

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	unsigned char	*p1;
	unsigned char	*p2;

	p1 = (unsigned char *)s1;
	p2 = (unsigned char *)s2;
	while (n--)
	{
		if (*p1 > *p2)
			return (1);
		else if (*p1 < *p2)
			return (-1);
		p1++;
		p2++;
	}
	return (0);
}
