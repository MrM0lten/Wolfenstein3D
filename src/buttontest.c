#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <sys/time.h>

#include "MLX42.h"
#include "libft.h"
#include "button.h"

#define IMG_WIDTH 1024
#define IMG_HEIGHT 512

mlx_image_t* img;
mlx_t* mlx;
button *btn;


void click(mouse_key_t button, action_t action, modifier_key_t mods, void* param){


	if(button ==  MLX_MOUSE_BUTTON_LEFT && action == MLX_PRESS)
	{
		btn->color = 0xFFFF00FF;
		draw_button(btn);
	}
	if(button == MLX_MOUSE_BUTTON_LEFT && action == MLX_RELEASE)
	{
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

int main()
{
	mlx = mlx_init(IMG_WIDTH, IMG_HEIGHT, "wolfenstein", true);
	if (!mlx)
		printf("error\n");

	img = mlx_new_image(mlx, IMG_WIDTH, IMG_HEIGHT);
	//mlx_loop_hook(mlx, slow_process, mlx); //turn this on to test FPS counter
	mlx_image_to_window(mlx, img, 0, 0);
	mlx_win_cursor_t* cursor = mlx_create_std_cursor(MLX_CURSOR_CROSSHAIR);
	mlx_set_cursor(mlx,cursor);

    vector2d pos = {.x = 200,.y =100};
    vector2d size = {.x = 300,.y = 100};
    printf("%d,%d\n",size.x,size.y);
    btn = mlx_create_button(mlx,pos,size,0xFFFFFFFF);

	mlx_mouse_hook(mlx, click, btn);
	mlx_cursor_hook(mlx,&cursortest,btn);

	mlx_loop(mlx);
	mlx_terminate(mlx);
	return (0);
}
