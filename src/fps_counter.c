#include "wolfenstein.h"

/* size_t	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec ));
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
} */