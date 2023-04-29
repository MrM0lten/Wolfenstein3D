#include "button.h"

int is_in_range(button* btn,int x,int y)
{
	if(x > btn->pos.x && x < btn->size.x + btn->pos.x &&
	y > btn->pos.y && y < btn->size.y + btn->pos.y)
		return 1;
	return 0;
}


//draws a rectangle to an imagine based on some dimensions
void draw_button(button* btn)
{

	for (int y = 0; y < btn->size.y; y++)
	{
		for (int x = 0; x < btn->size.x; x++)
		{
			mlx_put_pixel(btn->img, x, y, btn->color);
		}
	}
}

button* mlx_create_button(mlx_t* mlx,vector2d pos,vector2d size,uint32_t color)
{
    button* btn;
    btn = malloc(sizeof(button));
    btn->pos = pos;
    btn->size = size;
    btn->color = color;
    btn->img = mlx_new_image(mlx, btn->size.x,btn->size.y);
    mlx_image_to_window(mlx, btn->img, btn->pos.x, btn->pos.y);
    draw_button(btn);

    return btn;
}
