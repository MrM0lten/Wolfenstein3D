#ifndef BUTTON_INTERNAL
#define BUTTON_INTERNAL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MLX42_Int.h"
#include "MLX42.h"

#define CHARX 10
#define CHARY 20

typedef struct s_btn_list
{
	void			*content;
	struct s_btn_list	*next;
}t_btn_list;

typedef struct s_button_text
{
    mlx_texture_t *tex_def;
    mlx_texture_t *tex_hlight;
    mlx_texture_t *tex_pressed;

} btn_textures_t;

typedef struct s_btn_data
{
    void (*on_hover)(void *);
    void *param_on_hover;
    void (*on_click)(void *);
    void *param_on_click;
    void (*on_release)(void *);
    void *param_on_release;
}btn_data_t;

//node for a mlx_mousefunc linked list
typedef struct mousefunc_node_s
{
    void (*mousefunc)(mouse_key_t, action_t, modifier_key_t , void* );
    void *param;

} mousefunc_node_t;

//node for a mlx_mousefunc linked list
typedef struct cursorfunc_node_s
{
    void (*cursorfunc)(double xpos, double ypos, void* param);
    void *param;

} cursorfunc_node_t;


void	    btn_lstclear(t_btn_list **lst, void (*del)(void *));
t_btn_list	*btn_lstlast(t_btn_list *lst);
void	    btn_lstadd_back(t_btn_list **lst, t_btn_list *new_elem);
t_btn_list	*btn_lstnew(void *content);
void	    btn_lstadd_front(t_btn_list **lst, t_btn_list *new_elem);

mlx_texture_t *create_texture_from_color(uint32_t width,uint32_t height,uint32_t color);
int check_texture_dim(mlx_texture_t *dftl, uint32_t width, uint32_t height);
bool mlx_resize_texture(mlx_texture_t* tex, uint32_t nwidth, uint32_t nheight);

#endif
