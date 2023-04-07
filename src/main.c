#include <stdio.h>
#include "MLX42.h"
#include "mlx_button.h"

#define WIDTH 800
#define HEIGHT 800

mlx_t *mlx;


void end_prog(void *param)
{
	mlx_t* mlx = param;

	if (mlx_is_key_down(mlx, MLX_KEY_ESCAPE))
		mlx_close_window(mlx);
}

void mousekeypressed(mouse_key_t button, action_t action, modifier_key_t mods, void* param)
{
	button_t **btn_arr = param;

	if(button != MLX_MOUSE_BUTTON_LEFT)
		return;
	int x;
	int y;
	mlx_get_mouse_pos(mlx,&x,&y);
	for(int i = 0; btn_arr[i] != NULL; i++)
	{
		if(action == MLX_PRESS && mouse_over_button(btn_arr[i], x, y))
			btn_arr[i]->fclick(btn_arr[i],param); //is this preferred over fclick(btn_arr[i], param);
		else if(action == MLX_RELEASE && mouse_over_button(btn_arr[i], x, y))
			btn_arr[i]->frelease(btn_arr[i],param);
	}
}

void mouse_pos(double xpos, double ypos, void* param)
{
	button_t **btn_arr = param;

	for(int i = 0; btn_arr[i] != NULL; i++)
	{
		if(mouse_over_button(btn_arr[i], xpos, ypos))
			btn_arr[i]->fhover(btn_arr[i],param);
		else
			btn_arr[i]->fhover_end(btn_arr[i],param);
	}
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
	mlx = mlx_init(WIDTH, HEIGHT, "wolfenstein", true);
	if (!mlx)
		printf("error\n");

	btn_textures_t* tex = mlx_create_btn_textures("./resources/btn_blue_default.png","./resources/btn_blue_highlight.png","./resources/btn_blue_pressed.png");

	button_t **btn_arr = malloc(sizeof(button_t*) * (2 + 1));

	btn_arr[0] = mlx_create_button(mlx,tex,300,100,0xFFFFFFFF);
	btn_arr[1] = mlx_create_button(mlx,NULL,300,100,0xFFFFFFFF);
	btn_arr[2] = NULL;

	mlx_button_to_window(mlx,btn_arr[0],400,300);
	mlx_button_to_window(mlx,btn_arr[1],400,200);

	mlx_loop_hook(mlx,end_prog,mlx);

	mlx_cursor_hook(mlx, mouse_pos, btn_arr);


	mlx_mouse_hook(mlx, mousekeypressed,btn_arr);


	mlx_loop(mlx);

	mlx_delete_button(mlx,btn_arr[0]);
	mlx_delete_button(mlx,btn_arr[1]);


	free(btn_arr);
	mlx_terminate(mlx);
	return (0);
}

