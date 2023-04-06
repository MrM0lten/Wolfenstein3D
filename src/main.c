#include <stdio.h>
#include "MLX42.h"
#include "button.h"

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

	btn_textures_t* tex = mlx_create_btn_textures("a","b");
	button_t *btn = mlx_create_button(mlx,tex,300,100,0xFFFFFFFF);
	mlx_button_to_window(mlx,btn,400,200);

	mlx_loop_hook(mlx,end_prog,mlx);
	mlx_loop(mlx);

	mlx_delete_button(mlx,btn);
	mlx_terminate(mlx);
	return (0);
}

