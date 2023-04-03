/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*													+:+ +:+		 +:+	 */
/*   By: lmiehler <marvin@42.fr>					+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2022/11/29 15:18:45 by lmiehler		  #+#	#+#			 */
/*   Updated: 2022/11/29 15:19:00 by lmiehler         ###   ########.fr       */
/*																			*/
/* ************************************************************************** */

#include <stddef.h>

char	*ft_strrchr(const char *s, int c)
{
	char	*ptr;

	ptr = NULL;
	while (*s)
	{
		if (*s == (unsigned char)c)
			ptr = (char *)s;
		s++;
	}
	if (c == 0)
		return ((char *)s);
	return (ptr);
}
