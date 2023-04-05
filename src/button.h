#ifndef BUTTON_H
#define BUTTON_H

#include <stdio.h>
#include <stdlib.h>

#include "MLX42.h"

typedef struct s_vector2d
{
    int x;
    int y;

} vector2d;

typedef struct s_button
{
    mlx_image_t* img;
    vector2d pos;
    vector2d size;
    uint32_t color;
    uint32_t highlight_color;

} button;

button* mlx_create_button(mlx_t* mlx,vector2d pos,vector2d size,uint32_t color);
void draw_button(button* btn);
int is_in_range(button* btn,int x,int y);

#endif
