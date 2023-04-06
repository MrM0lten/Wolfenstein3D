#include <stdio.h>
#include "MLX42.h"
#include "mlx_button.h"

#define WIDTH 1024
#define HEIGHT 1024

void end_prog(void *param)
{
	mlx_t* mlx = param;

	if (mlx_is_key_down(mlx, MLX_KEY_ESCAPE))
	{

		mlx_close_window(mlx);
	}
}

int main()
{
	mlx_t *mlx = mlx_init(WIDTH, HEIGHT, "wolfenstein", true);
	if (!mlx)
		printf("error\n");

	btn_textures_t* tex = mlx_create_btn_textures("./resources/btn_blue_default.png","./resources/btn_blue_pressed.png");
	button_t *btn = mlx_create_button(mlx,tex,300,100,0xFFFFFFFF);
	button_t *btn2 = mlx_create_button(mlx,NULL,300,100,0xFFFFFFFF);
	mlx_button_to_window(mlx,btn,400,200);
	mlx_button_to_window(mlx,btn2,400,300);

	mlx_loop_hook(mlx,end_prog,mlx);
	mlx_loop(mlx);

	mlx_delete_button(mlx,btn);
	mlx_terminate(mlx);
	return (0);
}

