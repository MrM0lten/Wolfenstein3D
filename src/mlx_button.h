#ifndef BUTTON_H
#define BUTTON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MLX42.h"
#include "MLX42_Int.h"

typedef struct s_button_text
{
    mlx_texture_t *tex_def;
    mlx_texture_t *tex_hlight;

} btn_textures_t;

typedef struct s_button
{
    mlx_image_t* img;
    btn_textures_t* textures;
    int32_t world_posx;
    int32_t world_posy;
    uint32_t def_col;
    uint32_t highl_col;

    //used to save image pixel array
    uint8_t *temp_pixel_arr;

    //internal function shouldn"t be changed
    void (*fhover)(void *);
    void (*fhover_end)(void *);

    //public changeable function
    void (*on_hover)(void);

} button_t;

//not so well thought of architecture
bool mouse_over_button(button_t* btn, int mx,int my);
void f_hover(void *param);
void f_hover_end(void *param);

btn_textures_t *mlx_create_btn_textures(char *deflt,char *highlight);
bool mlx_resize_texture(mlx_texture_t* tex, uint32_t nwidth, uint32_t nheight);

button_t* mlx_create_button(mlx_t* mlx,btn_textures_t *text,uint32_t width,uint32_t height,uint32_t color);
void mlx_delete_button(mlx_t* mlx,button_t* btn);
int32_t mlx_button_to_window(mlx_t* mlx, button_t* btn, int32_t x, int32_t y);


#endif
