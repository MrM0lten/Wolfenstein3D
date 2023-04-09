#include "button_internal.h"

// --------------- PRIVATE --------------//

void	btn_lstclear(t_btn_list **lst, void (*del)(void *))
{
	t_btn_list	*tmp;

	while ((*lst))
	{
		tmp = (*lst)->next;
		del((*lst)->content);
		free((*lst));
		(*lst) = tmp;
	}
	*lst = NULL;
}

t_btn_list	*btn_lstlast(t_btn_list *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next)
		lst = lst->next;
	return (lst);
}

void	btn_lstadd_back(t_btn_list **lst, t_btn_list *new_elem)
{
	t_btn_list	*tmp;

	if (!(*lst))
	{
		*lst = new_elem;
		return ;
	}
	tmp = btn_lstlast(*lst);
	tmp->next = new_elem;
}

t_btn_list	*btn_lstnew(void *content)
{
	t_btn_list	*new;

	new = malloc(sizeof(t_btn_list));
	if (!new)
		return (NULL);
	new->content = content;
	new->next = NULL;
	return (new);
}

void	btn_lstadd_front(t_btn_list **lst, t_btn_list *new_elem)
{
	new_elem->next = *lst;
	*lst = new_elem;
}
