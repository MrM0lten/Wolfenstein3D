#include "button.h"

// PRIVATE //



// PUBLIC //

btn_textures_t *mlx_create_btn_textures(char *deflt,char *highlight)
{
    btn_textures_t *tex;
    tex = malloc(sizeof(btn_textures_t));

    return (tex);
}

//need to decide what happens if no texture data is given
button_t* mlx_create_button(mlx_t* mlx,btn_textures_t *text,uint32_t width,uint32_t height,uint32_t color)
{
    button_t *btn;

    btn = malloc(sizeof(button_t));
    btn->textures = text;
    btn->img = mlx_new_image(mlx, width, height);
    btn->def_col = color;
    btn->highl_col = color;
    btn->img->count = width *height;
    memset(btn->img->pixels,0xF0, btn->img->count * sizeof(int));
    return btn;
}

int32_t mlx_button_to_window(mlx_t* mlx, button_t* btn, int32_t x, int32_t y)
{
    return (mlx_image_to_window(mlx,btn->img,x,y));
}

void mlx_delete_button(mlx_t* mlx,button_t* btn)
{
    mlx_delete_image(mlx,btn->img);
    free(btn->textures);
    free(btn);
}
