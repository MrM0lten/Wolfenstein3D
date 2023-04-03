#include <stdio.h>
#include "MLX42.h"

int main()
{
	printf("Hello\n");
	mlx_t *mlx = mlx_init(400, 400, "wolfenstein", true);
	if (!mlx)
		printf("error\n");
	mlx_loop(mlx);

	
	//mlx_terminate();
}