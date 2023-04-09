#ifndef BUTTON_H
#define BUTTON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MLX42.h"
#include "button_internal.h"



typedef struct s_button
{
    mlx_image_t* img;
    btn_textures_t* textures;
    mlx_t *mlx;
    int32_t world_posx;
    int32_t world_posy;
    btn_data_t *btn_data;
    uint8_t *temp_pixel_arr;
} button_t;

//wrappes around mlx and addes functions and param linked lists
typedef struct mlx_btn_s
{
    mlx_t *mlx;
    t_btn_list *buttons; //linked list of all currently existing buttons type: button_t
    t_btn_list *mouse_func; //all functions bound to mouse_hook()        type: mousefunc_node_t
    t_btn_list *cursorfunc; //all functions bound to mouse_hook()        type: cursorfunc_node_t

} mlx_btn_t;


//main initializer of button wrapper
mlx_btn_t *mlx_button_init(mlx_t* mlx);
void mlx_button_terminate(mlx_btn_t *btn);

button_t* mlx_create_button(mlx_btn_t *btn,btn_textures_t *text,uint32_t width,uint32_t height);
void btn_resize(button_t *btn,uint32_t width,uint32_t height);

void generic_cursor_hook(mlx_btn_t* btn, mlx_cursorfunc func, void* param);
void generic_mouse_hook(mlx_btn_t* btn, mlx_mousefunc func, void* param);

//bind to functions
void btn_bind_on_release(button_t *btn, void (f)(void *), void *param);
void btn_bind_on_click(button_t *btn, void (f)(void *), void *param);
void btn_bind_on_hover(button_t *btn, void (f)(void *), void *param);


btn_textures_t *mlx_create_btn_textures(char *deflt,char *highlight,char *pressed);
btn_textures_t *mlx_create_btn_textures_from_colors(uint32_t width, uint32_t height, int32_t deflt,int32_t highlight,int32_t pressed);

int32_t mlx_button_to_window(mlx_t *mlx, button_t* btn, int32_t x, int32_t y);

#endif
