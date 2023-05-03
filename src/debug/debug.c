#include "wolfenstein.h"

void debug_point(point_t* point)
{
     printf("Point: [%f][%f]\n",point->x,point->y);
}

void debug_player(player_t* player)
{
    printf("----------------PLAYER---------------\n");
    printf("Player Pos: [%f][%f]\n",player->pos.x,player->pos.y);
    printf("Player delta [%f][%f]\n",player->dx,player->dy);
    printf("Player rotation angle: [%f]\n",player->a);
    printf("Player FOV: [%f]\n",player->fov);
    printf("Player speed: [%f]\n",player->speed);
}

void debug_ray(ray *ray)
{
    printf("Ray: Hit Pos [%f][%f]\n",ray->hit.x,ray->hit.y);
    printf("Ray: Length: [%f]\n",ray->len);
    printf("Ray: Direction: [%f]\n",ray->dir);
    printf("Ray: Orientation: [%d]\n",ray->hit_dir);
}


void debug_raycaster(raycaster_t* raycaster)
{
    printf("----------------RAYCASTER---------------\n");
    printf("Ray: Num rays = [%d]\n",raycaster->num_rays);
    for (int i = 0; i < raycaster->num_rays; i++){
        printf("Ray: Ray [%i]\n",i);
        debug_ray(&raycaster->rays[i]);
    }

}

void debug_meta(meta_t *meta)
{
    debug_player(&meta->player);
    printf("Window dimensions [%d][%d]\n",meta->win_width,meta->win_height);
    display_map_data(meta->map);
    debug_raycaster(&meta->raycaster);
    //debug_fpscounter(&meta->fps_counter);
    printf("Distance to Projection: [%f]\n",meta->dist_to_proj);
}
