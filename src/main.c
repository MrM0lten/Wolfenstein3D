#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <sys/time.h>

#include "MLX42.h"
#include "libft.h"


#define IMG_WIDTH 1024
#define IMG_HEIGHT 512
#define SQ_DIM 64

#define PI 3.1415926

float p_x = 256.f;
float p_y = 300.f;
float pdx = 0;
float pdy = 0;
float pa = 2*PI;

mlx_image_t* image;
mlx_image_t* image2;

	int x = 8;
	int y = 8;
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

//some frame counting
int nbFrames = 0;
#define DELAYPROCESS 100000000

size_t	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec ));
}
size_t lastTime;

void draw_fps_counter(mlx_t* mlx)
{
	//draw fps counter
	char *test;
	test = ft_itoa(nbFrames);
	//mlx_put_string(mlx,test,768,256);
	mlx_delete_image(mlx,image2);
	image2 = mlx_put_string(mlx,test,IMG_WIDTH -32,8);
	free(test);
}

void count_frames(void* param)
{
	mlx_t* mlx = param;
     // Measure speed
     double currentTime = get_time();
     nbFrames++;
     if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
         // printf and reset timer
		draw_fps_counter(mlx);

        //printf("%f ms/frame, FPS = %d\n", 1000.0/(double)nbFrames,nbFrames);
        nbFrames = 0;
        lastTime =  get_time();
     }
}
//slow down process for FPS testing
void slow_process(void* param)
{
	int i = 0;
	while(i < DELAYPROCESS)
	{
		i++;
	}
}

double	vector2d_len(double x, double y)
{
	return (sqrt(pow(x, 2) + pow(y, 2)));
}

void	drawline(int ax,int ay, int bx,int by, int color)
{
	double	len;
	double	i;
	double	t;
	double	tempx;
	double	tempy;
	double	valx;
	double	valy;

	if (ax >= IMG_WIDTH/2 || ax <= 0 || ay >= IMG_HEIGHT || ay <= 0
		|| bx >= IMG_WIDTH/2 || bx <= 0 || by >= IMG_HEIGHT || by <= 0)
		return ;
	len = vector2d_len(ax -bx,ay-by);
	i = 0;
	while (i < len)
	{
		t = i / len;
		tempx = ax-bx;
		tempy = ay-by;
		valx = (1 - t) * tempx + bx;
		valy = (1 - t) * tempy + by;
		mlx_put_pixel(image, valx, valy, color);
		i += 1;
	}
}


//adjusts player locations
void ft_hook(void* param)
{
	mlx_t* mlx = param;

	if (mlx_is_key_down(mlx, MLX_KEY_ESCAPE))
		mlx_close_window(mlx);
	//reduce angle
	if (mlx_is_key_down(mlx, MLX_KEY_LEFT))
	{
		pa -=0.1;
		if(pa < 0)
			pa += 2 * PI;

		pdx = cos(pa) * 5;
		pdy = sin(pa) * 5;




		//p_x -= 1;

	}
	//add angle
	if (mlx_is_key_down(mlx, MLX_KEY_RIGHT))
	{
		pa +=0.1;
		if(pa > 2 * PI)
			pa -= 2 * PI;
		pdx = cos(2*PI - pa) * 5;
		pdy = sin(pa) * 5;


		//p_x += 1;

	}
	if (mlx_is_key_down(mlx, MLX_KEY_UP))
	{
		p_x += pdx;
		p_y += pdy;


		//p_y -= 1;

	}
	if (mlx_is_key_down(mlx, MLX_KEY_DOWN))
	{
		p_x -= pdx;
		p_y -= pdy;


		//p_y += 1;

	}
	//printf("%f\n",pa);
	printf("Player pos [%f][%f]\n",p_x,p_y);
	printf("Player angle [%f]\n",pa);
}


int32_t ft_pixel(int32_t r, int32_t g, int32_t b, int32_t a)
{
    return (r << 24 | g << 16 | b << 8 | a);
}



//draws a rectangle to an imagine based on some dimensions
void draw_rect(mlx_image_t* image, int x, int y, int val)
{
	//calculate max position
	int x_max = x*SQ_DIM + SQ_DIM ;
	int y_max = y*SQ_DIM + SQ_DIM ;
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

	//debugging to draw border around cubes
	uint32_t color = 0x2A2A2AFF;
	for (int i = y_max-1; i < y_max; i++)
	{
		for (int j = x_max-SQ_DIM; j < x_max; j++)
			mlx_put_pixel(image, j, i, color);
	}
	for (int i = y_max-SQ_DIM; i < y_max; i++)
			mlx_put_pixel(image, x_max-1, i, color);




}

typedef struct point_s
{
	float x;
	float y;
}point;


// 3|4
// 2|1

point gethorizontalRay(double ra)
{
	//double ra=pa;
	float ry = 0;
	float rx = 0;
	int it = 0;
	float x_offset = 0;
	float y_offset = 64;
	x_offset = (64)/tan(ra);
	for (int i = 0; i < 1; i++)
	{
		//horizontal check
		if(ra > PI)
		{
			while (p_y > ry + 64 )
				ry += 64;
			if (ra > 1.5 * PI)// quadrant 4
			{
				rx = (p_y - ry)/tan(2*PI - ra);
				rx = p_x + rx;
				x_offset = (64)/tan(2*PI - ra);
			}
			else // quadrant 3
			{
				rx = (p_y - ry)/tan(ra - PI);
				rx = p_x - rx;
				x_offset = (64)/tan(ra - PI);
			}
		}
		else
		{
			while (p_y > ry)
				ry += 64;
			if(ra > PI/2) // quadrant 2
			{
				printf("p_y and ry [%f][%f]\n",p_y,ry);
				rx = (ry - p_y)/tan(PI - ra);
				rx = p_x - rx;
				x_offset = (64)/tan(PI - ra);
			}
			else // quadrant 1
			{
				rx = (ry - p_y)/tan(ra);
				rx = p_x + rx;
				x_offset = (64)/tan(ra);
			}
		}

 		for (int i = 0; i < 8; i++)
		{
			printf("x_offset = %f\n", x_offset);
			int yG = ry/64;
			int xG = rx/64;
			if ( yG >= 0 && xG >=0 && yG < 8 && xG < 8 && ra > PI) {
				yG--;
				if (arr[yG * 8 + xG]) {
					printf("Is wall!\n");
					break;
				}
				yG++;
			}
			else if (yG < 8 && xG < 8 && yG >= 0 && xG >=0) {
				if (arr[yG * 8 + xG]) {
					printf("Is wall!\n");
					break;
				}
			}
			if (ra < 1.5 * PI && ra > PI/2)
				rx = rx - x_offset;
			else
				rx = rx + x_offset;
			if (ra > PI)
				ry = ry - y_offset;
			else
				ry = ry + y_offset;
		}

			// printf("p_y and ry [%f][%f]\n",p_y,ry);

	}
		point s ={rx,ry};
	return s;
}

point getverticalRay(double ra)
{
	//double ra=pa;
	float ry = 0;
	float rx = 0;
	int it = 0;
	float x_offset = 64;
	float y_offset = 0;
	for (int i = 0; i < 1; i++)
	{
		//horizontal check
		if(ra > PI/2 && ra < 1.5*PI)
		{
			while (p_x > rx +64)
				rx += 64;

			printf("rx = %f\n",rx);
			if (ra > PI)// quadrant 3
			{
				printf("in quadrant 3\n");
				printf("ry = %f, py = %f\n",ry,p_y);
				ry = tan(ra - PI) * (p_x - rx);
				ry = p_y - ry;
				y_offset = (64) * tan(ra - PI);
			}
			else // quadrant 2
			{
				ry = tan(PI- ra) * (p_x - rx);
				ry = p_y + ry;
				y_offset = (64) * tan(PI - ra);
			}
		}
		else
		{
			while (p_x > rx)
				rx += 64;
			if (ra > PI)// quadrant 4
			{
				ry = tan(2*PI - ra) * (rx - p_x);
				ry = p_y - ry;
				y_offset = (64) * tan(2*PI -ra);
			}
			else // quadrant 1
			{
				ry = tan(ra) * (rx - p_x);
				ry = p_y + ry;
				y_offset = (64) * tan(ra);
			}
		}

 		for (int i = 0; i < 8; i++)
		{
			printf("--------iteration [%i]--------------\n",i);
			printf("x_offset = %f\n", x_offset);
			printf("rx in iter [%f] ry in iter[%f]!\n",rx,ry);
			int yG = ry/64;
			int xG = rx/64;
			printf(" before check xG [%d] yG [%d]!\n",xG,yG);

			if (yG >= 0 && xG >= 0 && yG < 8 && xG < 8 && ra > PI/2 && ra < 1.5 * PI) {
				printf("Checking left wall\n");
				xG--;
				if (arr[yG * 8 + xG]) {
					printf("LEFT xG [%d] yG [%d]!\n",xG,yG);
					break;
				}
			}
			else if (yG >= 0 && xG >= 0 && yG < 8 && xG < 8) {
				printf("Checking right wall\n");
				if (arr[yG * 8 + xG]) {
					printf("RIGHT xG [%d] yG [%d]!\n",xG,yG);
					break;
				}
			}
			if (ra < 1.5 * PI && ra > PI/2)
				rx = rx - x_offset;
			else
				rx = rx + x_offset;
			if (ra > PI)
				ry = ry - y_offset;
			else
				ry = ry + y_offset;
		}

		printf("rx and ry [%f][%f]\n",rx,ry);

	}
		point s ={rx,ry};
	return s;
}

double dist2(float x1, float y1, float x2, float y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx*dx + dy*dy);
}


void drawRay()
{
	double d = 2* PI;
	int val = 100;
	for (int i = 0; i < val; i++)
	{
		point hor = gethorizontalRay(pa-d);
		point vert = getverticalRay(pa-d);
		/* code */
		// double len_hor = vector2d_len(hor.x-p_x,hor.y- p_y);
		// double len_vert = vector2d_len(vert.x -p_x,vert.y -p_y);
		double len_hor = dist2(hor.x,hor.y,p_x,p_y);
		double len_vert = dist2(vert.x,vert.y,p_x,p_y);
		if(len_hor < len_vert)
			drawline(p_x,p_y,hor.x,hor.y,0xFF00FFFF);
		else
			drawline(p_x,p_y,vert.x,vert.y,0x00FFFFFF);
		d -= 2*PI/val;
	}



/* 	printf("len hor [%f]len vert [%f]\n",len_hor,len_vert);

		printf("next vertical line [%f][%f]\n",vert.x,vert.y); */

	//	drawline(p_x,p_y,hor.x,hor.y,0xFF00FFFF);


}

void draw_minimap(void* param)
{
	mlx_t* mlx = param;


	for (int i = 0; i < y; i++)
	{
		for (int j = 0; j < x; j++)
			draw_rect(image, j, i, arr[(i*8)+j]);
	}

	//draw player
	int p_size = 6;
	for (int i = 0; i < p_size; i++)
	{
		for (int j = 0; j < p_size; j++)
			mlx_put_pixel(image, p_x+j-p_size/2, p_y+i-p_size/2, 0xFF0000FF);
	}
	//draw player forward vector
	float line_mult = 5;
	drawline(p_x,p_y,p_x+pdx*line_mult,p_y+pdy*line_mult,0xFF0000FF);

	drawRay();

}

int main()
{

	mlx_t* mlx = mlx_init(IMG_WIDTH, IMG_HEIGHT, "wolfenstein", true);
	if (!mlx)
		printf("error\n");

	image = mlx_new_image(mlx, IMG_WIDTH / 2, IMG_HEIGHT);

	lastTime = get_time();
	mlx_loop_hook(mlx, draw_minimap, mlx);
	mlx_loop_hook(mlx, ft_hook, mlx);
	mlx_loop_hook(mlx, count_frames, mlx);
	//mlx_loop_hook(mlx, slow_process, mlx); //turn this on to test FPS counter
	mlx_image_to_window(mlx, image, 0, 0);


	mlx_loop(mlx);
	mlx_terminate(mlx);
	return (0);
}
