#ifndef BUTTON_H
#define BUTTON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MLX42.h"

typedef struct s_button_text
{
    u_int8_t *tex_def;
    u_int8_t *tex_hlight;

} btn_textures_t;

typedef struct s_button
{
    mlx_image_t* img;
    btn_textures_t* textures;
    uint32_t def_col;
    uint32_t highl_col;

} button_t;

btn_textures_t *mlx_create_btn_textures(char *deflt,char *highlight);

button_t* mlx_create_button(mlx_t* mlx,btn_textures_t *text,uint32_t width,uint32_t height,uint32_t color);
void mlx_delete_button(mlx_t* mlx,button_t* btn);
int32_t mlx_button_to_window(mlx_t* mlx, button_t* btn, int32_t x, int32_t y);


#endif
