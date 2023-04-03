#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "MLX42.h"


#define IMG_WIDTH 1024
#define IMG_HEIGHT 512
#define SQ_DIM 64

int p_x = 256;
int p_y = 256;

mlx_image_t* image;

void ft_hook(void* param)
{
	mlx_t* mlx = param;

	if (mlx_is_key_down(mlx, MLX_KEY_ESCAPE))
		mlx_close_window(mlx);
	if (mlx_is_key_down(mlx, MLX_KEY_UP))
		p_y -= 1;
	if (mlx_is_key_down(mlx, MLX_KEY_DOWN))
		p_y += 1;
	if (mlx_is_key_down(mlx, MLX_KEY_LEFT))
		p_x -= 1;
	if (mlx_is_key_down(mlx, MLX_KEY_RIGHT))
		p_x += 1;
}


int32_t ft_pixel(int32_t r, int32_t g, int32_t b, int32_t a)
{
    return (r << 24 | g << 16 | b << 8 | a);
}

//draws a rectangle to an imagine based on some dimensions
void draw_rect(mlx_image_t* image, int x, int y, int val)
{
	//calculate max position -1 to create a border around squares
	int x_max = x*SQ_DIM + SQ_DIM -1;
	int y_max = y*SQ_DIM + SQ_DIM -1;
	for (int i = y*SQ_DIM; i < y_max; i++)
	{
		for (int j = x*SQ_DIM; j < x_max; j++)
		{
			if(val == 1)
				mlx_put_pixel(image, j, i, 0xFFFFFFFF);
			else
				mlx_put_pixel(image, j, i, 0x000000FF);
		}

	}
}

void draw_minimap(void* param)
{
	int x = 8;
	int y = 8;
	int total = x*y;
	int arr[] = {
	1,1,1,1,1,1,1,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,1,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

	for (int i = 0; i < y; i++)
	{
		for (int j = 0; j < x; j++)
			draw_rect(image, j, i, arr[(i*8)+j]);
	}

	//draw player
	int p_size = 5;
	for (int i = 0; i < p_size; i++)
	{
		for (int j = 0; j < p_size; j++)
			mlx_put_pixel(image, p_x+j, p_y+i, 0xFF0000FF);
	}
}

int main()
{

	mlx_t* mlx = mlx_init(IMG_WIDTH, IMG_HEIGHT, "wolfenstein", true);
	if (!mlx)
		printf("error\n");


	image = mlx_new_image(mlx, IMG_WIDTH / 2, IMG_HEIGHT);


	mlx_loop_hook(mlx, draw_minimap, mlx);
	mlx_loop_hook(mlx, ft_hook, mlx);
	mlx_image_to_window(mlx, image, 0, 0);

	mlx_loop(mlx);
	mlx_terminate(mlx);
	return (0);
}
