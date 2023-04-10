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


	mlx_set_btn_text(t2,"hello abc 111 aaaaaaaa",TEXT_RIGHT);
	//mlx_set_btn_text(t1,"hello123");

	char str[] = "hi";
	int length = sizeof(str) - 1;
	printf("len = %i\n",length);
	int x = 100;
	int y = x*2;
	mlx_image_t *img = mlx_put_string(mlx,str,200,00);
	mlx_resize_image(img,x *length,y);
	// mlx_new_image()


    mlx_button_to_window(mlx,t1,400,300);
	mlx_button_to_window(mlx,t2,400,200);

	mlx_set_btn_text(t1,"hell0 my name is123131321312123123",TEXT_CENTER);

	btn_bind_on_click(t1, test,t1);
	btn_bind_on_hover(t1, test_hover,t1);


	generic_cursor_hook(btn,test_cursor,btn);
    generic_mouse_hook(btn,test_mouse,btn);

	mlx_loop_hook(mlx,end_prog,mlx);
	mlx_loop(mlx);

	mlx_button_terminate(btn);
	mlx_terminate(mlx);
	return (0);
}

