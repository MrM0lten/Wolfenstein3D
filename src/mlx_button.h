#ifndef BUTTON_H
#define BUTTON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MLX42.h"
#include "MLX42_Int.h"


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

typedef struct s_button
{
    mlx_image_t* img;
    btn_textures_t* textures;
    mlx_t *mlx;
    int32_t world_posx;
    int32_t world_posy;
    uint32_t def_col;
    uint32_t highl_col;

    //used to save image pixel array
    uint8_t *temp_pixel_arr;

    //public changeable function

    void (*on_hover)(void *);
    void (*on_click)(void *);
    void (*on_release)(void *);

} button_t;

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

//wrappes around mlx and addes functions and param linked lists
typedef struct mlx_btn_s
{
    mlx_t *mlx;
    t_btn_list *buttons; //linked list of all currently existing buttons type: button_t
    t_btn_list *mouse_func; //all functions bound to mouse_hook()        type: mousefunc_node_t
    t_btn_list *cursorfunc; //all functions bound to mouse_hook()        type: cursorfunc_node_t

} mlx_btn_t;

//-------------------sketchy section-------------

//main initializer of button wrapper
mlx_btn_t *mlx_button_init(mlx_t* mlx);
void mlx_button_terminate(mlx_btn_t *btn);
button_t* mlx_create_button(mlx_btn_t *btn,btn_textures_t *text,uint32_t width,uint32_t height,uint32_t color);

void generic_cursor_hook(mlx_btn_t* btn, mlx_cursorfunc func, void* param);
void generic_mouse_hook(mlx_btn_t* btn, mlx_mousefunc func, void* param);




//giving the option to call pass a user defined param, but i dont want them to ever be called? might not make sense


//-------------------okay section-------------

btn_textures_t *mlx_create_btn_textures(char *deflt,char *highlight,char *pressed);
bool mlx_resize_texture(mlx_texture_t* tex, uint32_t nwidth, uint32_t nheight);

int32_t mlx_button_to_window(mlx_t *mlx, button_t* btn, int32_t x, int32_t y);

//give user the option to bind user defined functions on events
void mlx_btn_bind_on_release(button_t *btn,void (*f)(void *));
void mlx_btn_bind_on_click(button_t *btn,void (*f)(void *));
void mlx_btn_bind_on_hover(button_t *btn,void (*f)(void *));


#endif
