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
    debug_map(meta->map);
    debug_raycaster(&meta->raycaster);
    //debug_fpscounter(&meta->fps_counter);
    printf("Distance to Projection: [%f]\n",meta->dist_to_proj);
}

void debug_map(map_t *map)
{
    printf("--------------MAP DATA-----------------\n");
    for (int i = 0; i < map->total_textures; i++)
        printf("Texture [%i] path: [%s]\n",i,map->file_data[i]);

    printf("Ceiling Color: %u (RGBA = %i,%i,%i,%i)\n",map->col_ceil,
    (map->col_ceil>>24),
    ((map->col_ceil<<8)>>24),
    ((map->col_ceil<<16)>>24),
    ((map->col_ceil<<24)>>24));
    printf("Floor Color: %u (RGBA = %i,%i,%i,%i)\n",map->col_floor,
    (map->col_floor>>24),
    ((map->col_floor<<8)>>24),
    ((map->col_floor<<16)>>24),
    ((map->col_floor<<24)>>24));
    printf("Player Position in Grid: [%d][%d]\n",map->p_pos_x,map->p_pos_y);
    printf("Player Start orientation %f\n", map->p_orient);

    printf("Map x,y: [%d][%d]\n", map->map_x, map->map_y);
    printf("Map dimensions = %d\n",map->map_dim);
    print_map(map);
}

void print_map(map_t *map)
{
    for (int i = 0; i < map->map_x + 1; i++)
        printf("_");
    printf("\n");
    for (int i = 0; i < map->map_dim; i++)
    {
        if(i % map->map_x == 0)
            printf("|");
        if(map->map[i] == GD_VOID)
            printf(" ");
        else if(map->map[i] == -1)
            printf("?");
        else
            printf("%d",map->map[i]);
        if((i + 1) % map->map_x  == 0 && i != 0)
            printf("|\n");
    }
    for (int i = 0; i < map->map_x + 1; i++)
        printf("-");
    printf("\n");
}
