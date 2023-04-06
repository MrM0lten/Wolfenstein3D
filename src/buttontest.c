#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <sys/time.h>

#include "MLX42.h"
#include "libft.h"
#include "button.h"

#define IMG_WIDTH 1024
#define IMG_HEIGHT 1024

mlx_image_t* img;
mlx_t* mlx;
button *btn;
mlx_texture_t*text;
mlx_image_t* new;
mlx_texture_t*text2;
mlx_image_t* new2;


uint8_t* temp;

void click(mouse_key_t button, action_t action, modifier_key_t mods, void* param){


	if(button ==  MLX_MOUSE_BUTTON_LEFT && action == MLX_PRESS)
	{
		btn->color = 0xFFFF00FF;
		temp = new->pixels;
		new->pixels = new2->pixels;
		draw_button(btn);
	}
	if(button == MLX_MOUSE_BUTTON_LEFT && action == MLX_RELEASE)
	{
		new->pixels = temp;
		btn->color = 0xFF00FFFF;
		draw_button(btn);

	}


}
void cursortest(double x, double y, void *param)
{
	button *btn = param;
	printf("[%lf][%lf]\n",x,y);
    mlx_put_pixel(img, x, y, 0xFF0000FF);
	if(is_in_range(btn,x,y))
	{
		btn->color = 0xFF0000FF;
		draw_button(btn);
	}
	else
	{
		btn->color = 0xFFFFFFFF;
		draw_button(btn);
	}

}

void ft_hook1(void* param)
{
	mlx_t* mlx = param;
	new->instances[0].z = 100;
	if (mlx_is_key_down(mlx, MLX_KEY_ESCAPE))
		mlx_close_window(mlx);
	if (mlx_is_key_down(mlx, MLX_KEY_UP))
		new->instances[0].y -= 5;
	if (mlx_is_key_down(mlx, MLX_KEY_DOWN))
		new->instances[0].y += 5;
	if (mlx_is_key_down(mlx, MLX_KEY_LEFT))
		new->instances[0].x -= 5;
	if (mlx_is_key_down(mlx, MLX_KEY_RIGHT))
		new->instances[0].x += 5;
}

int main()
{
	mlx = mlx_init(IMG_WIDTH, IMG_HEIGHT, "wolfenstein", true);
	if (!mlx)
		printf("error\n");

	img = mlx_new_image(mlx, IMG_WIDTH, IMG_HEIGHT);
	mlx_image_to_window(mlx, img, 0, 0);
	mlx_win_cursor_t* cursor = mlx_create_std_cursor(MLX_CURSOR_CROSSHAIR);
	mlx_set_cursor(mlx,cursor);


	text =  mlx_load_png("./resources/cena.png");
	new = mlx_texture_to_image(mlx,text);
	mlx_resize_image(new,100,100);

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			mlx_image_to_window(mlx, new, j*100, i*100);
		}

	}
	text2 =  mlx_load_png("./resources/bunny.png");
	new2 = mlx_texture_to_image(mlx,text2);
	mlx_resize_image(new2,100,100);

	mlx_image_to_window(mlx, new2, 0, 0);


    vector2d pos = {.x = 300,.y =812};
    vector2d size = {.x = 300,.y = 100};
    printf("%d,%d\n",size.x,size.y);
    btn = mlx_create_button(mlx,pos,size,0xFFFFFFFF);

	mlx_mouse_hook(mlx, click, btn);
	mlx_cursor_hook(mlx,&cursortest,btn);
	mlx_loop_hook(mlx, ft_hook1, mlx);

	mlx_loop(mlx);
	mlx_terminate(mlx);
	return (0);
}
