#include <stdio.h>
#include "MLX42.h"
#include "mlx_button.h"

#define WIDTH 800
#define HEIGHT 800




void end_prog(void *param)
{
	mlx_t* mlx = param;

	if (mlx_is_key_down(mlx, MLX_KEY_ESCAPE))
		mlx_close_window(mlx);
}


//potential solution to abstraction issue
/* void handle_mouse(void* param)
{
	if (mouse_x > canvas)
		t_menu = param;
		menu(t_menu);
	else
		t_draw = param;
		draw(t_draw);
}
 */

int main()
{
	mlx_t *mlx = mlx_init(WIDTH, HEIGHT, "wolfenstein", true);
	mlx_btn_t *btn = mlx_button_init(mlx);
	if (!mlx)
		printf("error\n");

    btn_textures_t* tex = mlx_create_btn_textures("./resources/btn_blue_default.png","./resources/btn_blue_highlight.png","./resources/btn_blue_pressed.png");


	button_t *t1 = mlx_create_button(btn,tex,300,100,0xFFFFFFFF);
	button_t *t2 = mlx_create_button(btn,NULL,300,100,0xFFFFFFFF);

    //should this be void instead?
    mlx_button_to_window(mlx,t1,400,300);
	mlx_button_to_window(mlx,t2,400,200);



	mlx_loop_hook(mlx,end_prog,mlx);
	mlx_loop(mlx);

	//need to find a good structure here! might make sense to have a wrapper around terminate as well
	// mlx_delete_button(mlx,btn_arr[0]);
	// mlx_delete_button(mlx,btn_arr[1]);
	// free(btn_arr);

	mlx_button_terminate(btn);
	mlx_terminate(mlx);
	return (0);
}

