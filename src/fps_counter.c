#include "wolfenstein.h"

size_t	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec ));
}

// Measure speed
void count_frames(void *param)
{
	meta_t *meta = param;
    double currentTime = get_time();
    meta->fps_counter.nbFrames++;
    if ( currentTime - meta->fps_counter.lastTime >= 1.0 ) {
		update_fps_counter(meta);
       meta->fps_counter.nbFrames = 0;
       meta->fps_counter.lastTime =  get_time();
    }
}

//draw fps counter
void update_fps_counter(meta_t *meta)
{
	char *str_num;
	str_num = ft_itoa(meta->fps_counter.nbFrames);
	mlx_delete_image(meta->mlx, meta->fps_counter.img);
	meta->fps_counter.img = mlx_put_string(meta->mlx, str_num, meta->win_width -32,8);
	free(str_num);
}
