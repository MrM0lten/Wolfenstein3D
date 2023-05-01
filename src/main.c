#include "wolfenstein.h"

#define IMG_WIDTH 1024
#define IMG_HEIGHT 512
#define MAP_HEIGHT 8
#define MAP_WIDTH 8
#define MAP_DIM MAP_HEIGHT * MAP_WIDTH
#define SQ_DIM MAP_DIM

#define RAYS 512

#define FOV PI/2
#define HALF_FOV FOV/2
#define HALF_SCREEN IMG_WIDTH/2

float dist_to_proj;


float p_x = 256.f;
float p_y = 300.f;
float pdx = 0;
float pdy = 0;
float pa = 2*PI;


mlx_image_t* image;
mlx_image_t* image2;
mlx_image_t* texture;
map_t* map;
	int x = 8;
	int y = 8;
	int* arr;
/* 	int arr[] = {
	1,1,1,1,1,1,1,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,1,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,
}; */
//double ray_data[RAYS];
ray ray_data[RAYS];


//some frame counting
int nbFrames = 0;
#define DELAYPROCESS 100000000

// function determines to what quadrant a given radian points to
// is not defined for radian == 0 and radian == PI
int is_up(double radian){
	return (radian > PI && radian < PI*2);
}
int is_down(double radian){
	return (radian > 0 && radian < PI);
}

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
	int dx = bx - ax;
	int dy = by - ay;
	double x;
	double y;
	double xinc;
	double yinc;
	int steps;

	if (abs(dx) > abs(dy))
		steps = abs(dx);
	else
		steps = abs(dy);
	xinc = (double)dx / steps;
	yinc = (double)dy / steps;
	x = ax;
	y = ay;
	while (steps--)
	{
		mlx_put_pixel(image, (int)x, (int)y, color);
		x = x + xinc;
		y = y + yinc;
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
	//printf("Player pos [%f][%f]\n",p_x,p_y);
	//printf("Player angle [%f]\n",pa);
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

double dist2(float x1, float y1, float x2, float y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx*dx + dy*dy);
}

int is_wall(double rx, double ry)
{
	if (rx < 0 || ry < 0)
		return 0;
	int mx = (int)rx>>6;
	int my = (int)ry>>6;
	int mp = my * MAP_WIDTH + mx;
	if (mp >= 0 && mp < MAP_DIM && arr[mp] == 1) {
		return 1;
	}
	return 0;
}

point raycast_hor(double radian)
{
	double px = p_x, py = p_y, rx, ry, xo, yo;
	if (radian > PI) {
		ry = (((int)py>>6)<<6)-0.002;
		rx = px-((py-ry)/tan(radian));
		yo = -64, xo = yo/tan(radian);
	}
	else if (radian < PI) {
		ry = (((int)py>>6)<<6) + 64;
		rx = px-((py-ry)/tan(radian));
		yo = 64, xo = yo/tan(radian);
	}
	if (radian==0 || radian==PI) {return (point){px,py};}
	for (int i=0; i<8 && !is_wall(rx, ry); i++) {
		rx = rx + xo;
		ry = ry + yo;
	}
	return (point){rx,ry};
}

point raycast_ver(double radian)
{
	double px = p_x, py = p_y, rx, ry, xo, yo;
	if (radian > PI/2 && radian < 1.5*PI) {
		rx = (((int)px>>6)<<6)-0.002;
		ry = py - (tan(radian) * (px-rx));
		xo = -64, yo = tan(radian) * xo;
	}
	else if (radian < PI/2 || radian > 1.5*PI) {
		rx = (((int)px>>6)<<6) + 64;
		ry = py - (tan(radian) * (px-rx));
		xo = 64, yo = tan(radian) * xo;
		//printf("rx,ry [%f][%f]\n",rx,ry);
	}
	if (radian==PI/2 || radian==PI*1.5) {return (point){px,py};}
	for (int i=0; i<8 && !is_wall(rx, ry); i++) {
		rx = rx + xo;
		ry = ry + yo;
	}

	return (point){rx,ry};
}

// py = 166 166/64
ray raycast(double radian)
{
	//printf("in rayacst\n");
	point hray = raycast_hor(radian);
	point vray = raycast_ver(radian);
	double len_hor = vector2d_len(hray.x-p_x,hray.y- p_y);
	double len_vert = vector2d_len(vray.x -p_x,vray.y -p_y);
	//printf("horlen = %f verlen = %f\n", len_hor, len_vert);
	ray ray;
	if (len_vert == 0) {
		printf("1 SETTING HOR RAY\n");
		ray.hit = hray;
		ray.len = len_hor;
		ray.dir = radian;
		ray.hit_type = 1;
		return ray;
	}
	if (len_hor == 0) {
		printf("1 SETTING VERT RAY\n");
		ray.hit = vray;
		ray.len = len_vert;
		ray.dir = radian;
		ray.hit_type = 2;
		return ray;
	}
	if (len_hor > len_vert) {
		printf("2 SETTING VER RAY\n");
		ray.hit = vray;
		ray.len = len_vert;
		ray.dir = radian;
		ray.hit_type = 2;
		return ray;
	}
	else {
		printf("2 SETTING HOR RAY\n");
		ray.hit = hray;
		ray.len = len_hor;
		ray.dir = radian;
		ray.hit_type = 1;
		return ray;
	}
}

inline void draw_ray(point ray) {
	drawline(p_x,p_y,ray.x,ray.y,0xFF00FFFF);
}

void raycaster(int nb_rays, double fov,ray *arr)
{
	if (nb_rays == 1) {
		draw_ray(raycast(pa).hit);
		return;
	}
	double start_radian = pa-fov/2;
	double step = fov / (nb_rays -1);
	for (int i=0; i < nb_rays; i++) {
		if (start_radian > 2*PI)
			start_radian -= 2*PI;
		else if (start_radian < 0)
			start_radian += 2*PI;
		//printf("angle for ray = %f\n", start_radian);
		arr[i] = raycast(start_radian);
		draw_ray(arr[i].hit);
		//arr[i] = vector2d_len(p.x - p_x, p.y - p_y);
		double temp = pa - start_radian;
		if(temp > 2*PI)
			temp -= 2*PI;
		else if(temp < 0)
			temp += 2*PI;
		arr[i].len = (64 * dist_to_proj)/(arr[i].len * cos(temp));

		if(arr[i].len > HALF_SCREEN)
			arr[i].len = HALF_SCREEN;
		start_radian += step;
	}
}

// void draw_wall()
// {

// }

// void draw_vert_slice()
// {
// 	//ceiling
// 	for (int i = 0; i < IMG_HEIGHT, i++)
// 	{
// 	}
// 	//wall
// 	for (int i = 0; i < IMG_HEIGHT, i++)
// 	{
// 	}
// 	//floor
// 	for (int i = 0; i < IMG_HEIGHT, i++)
// 	{
// 	}

// 	//alternitive
// 	int pixel[]
// 	for (int i = 0; i < IMG_HEIGHT, i++)
// 	{
// 		put_pixel(pixel_arr);
// 	}
// }

void drawRay()
{
	ray ray = raycast(pa);
	drawline(p_x,p_y,ray.hit.x,ray.hit.y,0xFF00FFFF);

}

void print_raydata()
{
	printf("-----------------PRINTING RAY DATA---------\n");
	for (int i = 0; i < RAYS; i++)
	{
		printf("Ray [%i]; len=[%f]; hit_type=[%i]\n",i,ray_data[i].len,ray_data[i].hit_type);

	}

}
void draw_3dView()
{
	// refresh screen on right side
/* 	for (int i = 512; i < 1024; i++)
	{
		for (int j = 0; j < 512; j++)
		{
			mlx_put_pixel(image,i,j,0x000000FF);
		}
	} */



	for (int i = 0; i < RAYS; i++)
	{
		printf("yes\n");
		drawline((int)(i+HALF_SCREEN),0,(int)(i+HALF_SCREEN),(int)((IMG_HEIGHT- ray_data[i].len)/2),map->col_ceil);
		if (ray_data[i].hit_type == 1) {
			printf("IS HOR RAY\n");
			if (is_up(ray_data[i].dir)) {
				drawline((int)(i+HALF_SCREEN),(int)((IMG_HEIGHT- ray_data[i].len)/2),(int)(i+HALF_SCREEN),
				(int)((IMG_HEIGHT+ray_data[i].len)/2),0x000000FF);
			}
			else {
				drawline((int)(i+HALF_SCREEN),(int)((IMG_HEIGHT- ray_data[i].len)/2),(int)(i+HALF_SCREEN),
				(int)((IMG_HEIGHT+ray_data[i].len)/2),0xFFFFFFFF);
			}
		}
		else {
			printf("IS VERT RAY\n");
			if (ray_data[i].dir > PI/2 && ray_data[i].dir < PI*1.5) {
				drawline((int)(i+HALF_SCREEN),(int)((IMG_HEIGHT- ray_data[i].len)/2),(int)(i+HALF_SCREEN),
				(int)((IMG_HEIGHT+ray_data[i].len)/2),0x5A5A5AFF);
			}
			else {
				drawline((int)(i+HALF_SCREEN),(int)((IMG_HEIGHT- ray_data[i].len)/2),(int)(i+HALF_SCREEN),
				(int)((IMG_HEIGHT+ray_data[i].len)/2),0xD3D3D3FF);
			}
		}
		drawline((int)(i+HALF_SCREEN),(int)((IMG_HEIGHT+ray_data[i].len)/2),(int)(i+HALF_SCREEN),IMG_HEIGHT,map->col_floor);
		printf("drawline from [%d][%d] to [%d][%d]\n",(int)(i+HALF_SCREEN),(int)(IMG_HEIGHT- ray_data[i].len/2)
		,(int)(i+HALF_SCREEN),(int)((IMG_HEIGHT+ray_data[i].len)/2));
	}
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

	raycaster(RAYS, FOV, ray_data);
	print_raydata();

	draw_3dView();
}

int main()
{
	map = read_map("./resources/maps/default.cub");
	arr = map->map;
	if(map == NULL)
		return 1;
	printf("MAP IS VALID\n");
	dist_to_proj = HALF_SCREEN/tan(HALF_FOV);
	mlx_t* mlx = mlx_init(IMG_WIDTH, IMG_HEIGHT, "wolfenstein", true);
	if (!mlx)
		printf("error\n");

	image = mlx_new_image(mlx, IMG_WIDTH, IMG_HEIGHT);

	lastTime = get_time();
	mlx_loop_hook(mlx, draw_minimap, mlx);
	mlx_loop_hook(mlx, ft_hook, mlx);
	mlx_loop_hook(mlx, count_frames, mlx);
	//mlx_loop_hook(mlx, slow_process, mlx); //turn this on to test FPS counter
	mlx_image_to_window(mlx, image, 0, 0);

	mlx_texture_t* arrow = mlx_load_png("./resources/textures/player_arrow.png");
	texture= mlx_texture_to_image(mlx,arrow);
	mlx_resize_image(texture,100,100);
	mlx_image_to_window(mlx,texture,0,0);


	mlx_loop(mlx);
	delete_map(map);
	mlx_terminate(mlx);
	return (0);
}
