#include <stdio.h>
#include "MLX42.h"
#include "MLX42_Int.h"
#include "mlx_button.h"

#define WIDTH 800
#define HEIGHT 800


void test_cursor(double xpos, double ypos, void* param)
{
	//printf("PENIS\n");
}

void test_mouse(mouse_key_t button, action_t action, modifier_key_t mods, void* param)
{
	//printf("RIPPPPPPP\n");
}

void end_prog(void *param)
{
	mlx_t* mlx = param;

	if (mlx_is_key_down(mlx, MLX_KEY_ESCAPE))
		mlx_close_window(mlx);
}

void test(void *param)
{
	button_t *btn = param;

	//printf("whwow  %i\n",btn->highl_col);
}
void test_hover(void *param)
{
	button_t *btn = param;

	//printf("HOVER over %i\n",btn->highl_col);
}

int main()
{
	mlx_t *mlx = mlx_init(WIDTH, HEIGHT, "wolfenstein", true);
	mlx_btn_t *btn = mlx_button_init(mlx);
	if (!mlx)
		printf("error\n");

    btn_textures_t* tex = mlx_create_btn_textures(NULL,NULL,NULL);

	button_t *t1 = mlx_create_button(btn,tex,300,100);
	button_t *t2 = mlx_create_button(btn,NULL,80,32);

    mlx_button_to_window(mlx,t1,400,300);
	mlx_button_to_window(mlx,t2,400,200);


	btn_bind_on_click(t1, test,t1);
	btn_bind_on_hover(t1, test_hover,t1);

	//it works pog!!!
	generic_cursor_hook(btn,test_cursor,btn);
    generic_mouse_hook(btn,test_mouse,btn);

	mlx_loop_hook(mlx,end_prog,mlx);
	mlx_loop(mlx);

	mlx_button_terminate(btn);
	mlx_terminate(mlx);
	return (0);
}

