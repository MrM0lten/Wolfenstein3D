#include "wolfenstein.h"

map_t* read_map(char *path)
{
    if(!ft_strnstr(path,".cub",ft_strlen(path))) {
        log_string(2,1,"Input file does not have the required '.cub' extension");
        return NULL;
    }
    int fd = open(path,O_RDONLY);
    if(fd == -1){
        log_string(2,2,"Can't find input file:",path);
        return NULL;
    }

    map_t* map = init_map();
    char *line;
    line = get_next_line(fd);
    enum map_type_id id;
    while(line) {
        id = identify_line(line);
        if(id == MP_TEXT)
            store_map_texture(map,line);
        else if(id == MP_COL)
            store_map_color(map,line);
        else if(id == MP_MAP) //will finish reading the map and get map
        {
            store_map_array(map,line,fd);
            break;
        }
        free(line);
        line = get_next_line(fd);
    }
    close(fd);
    if(!validate_map(map)) {
		debug_map(map);
        free_map(map);
        return NULL;
    }
    debug_map(map);
    return map;
}

enum map_type_id identify_line(char *line)
{
    if(line == NULL)
        return MP_ERR;
    size_t len = ft_strlen((const char*)line);
    if(len < 2)
        return MP_ERR;
    if(find_texture_id(line) != -1)
        return MP_TEXT;
    else if(!ft_strncmp(line,"F",1) || !ft_strncmp(line,"C",1))
        return MP_COL;
    else if(ft_strchr(line,'1') != NULL || ft_strchr(line,'1') != NULL)
        return MP_MAP;

    return MP_ERR;
}

// only there for allocating a map and default initializing it
map_t* init_map(void)
{
    map_t* map = malloc(sizeof(map_t));
    map->col_ceil = 0; // black, but no transparency
    map->col_floor = 0; // black, but no transparency
    map->p_orient = -1;
    map->p_pos_x = -1;
    map->p_pos_y = -1;
    map->map = NULL;
    map->map_x = 0;
    map->map_y = 0;
    map->map_dim = 0;

    map->total_textures = 5;

    map->file_data = malloc(sizeof(char *) *(map->total_textures + 1));
    map->texture_data = malloc(sizeof(mlx_texture_t *) *(map->total_textures + 1));
    for (int i = 0; i < map->total_textures + 1; i++) {
        map->file_data[i] = NULL;
        map->texture_data[i] = NULL;
    }

    return map;

}

//returns 1 on success
// 0 on failure
int validate_map(map_t* map)
{
    int ret = 1;

    for (int i = 0; i < map->total_textures; i++) {

        if(map->file_data[i] == NULL || access(map->file_data[i],F_OK | R_OK)) {
            if(i == TXT_DOOR) {
                log_string(1,1,"No DOOR texture found, replacing doors with empty tiles");
                for (int i = 0; i < map->map_dim; i++){
                    if(map->map[i] == GD_DOOR_CLOSE)
                        map->map[i] = GD_FREE;
                }
            }
            else {
                log_string(2,1,"No texture found");
                ret = 0;
            }
        }
    }

    if(map->col_ceil == 0) {
        log_string(2,1,"No Ceiling color found");
        ret = 0;
    }
    if(map->col_floor == 0) {
        log_string(2,1,"No Ceiling color found");
        ret = 0;
    }
    if(map->p_pos_x == -1) {
        log_string(2,1,"player Position was not set within the map");
        ret = 0;
    }

    int val;
    //check if each 0 has only 1 and 0 as adjacent positions
    for (int y = 0; y < map->map_y; y++) {
        for (int x = 0; x < map->map_x; x++)
        {
            val = map->map[y * map->map_x + x];
            if(val == GD_VOID)
                continue;
            if(val == -1 || !is_walled(x,y,map))
            {
                log_string(2, 1, "Map is impossible.");
                return 0;
            }
        }
    }

    //load pngs into texture buffers, resize them to 64x64,remove transparency
    for (int i = 0; i < map->total_textures; i++) {
        int flag = 0;
        if(map->file_data[i] != NULL)
            map->texture_data[i] = mlx_load_png(map->file_data[i]);
        if(map->texture_data[i] != NULL
            && (map->texture_data[i]->height != 64 || map->texture_data[i]->width != 64)) {
                log_string(1, 2, map->file_data[i], " does not conform to the 64x64pixel size requirement. Resizing now...");
                resize_texture(map->texture_data[i],64,64);
            }
        size_t total = map->texture_data[i]->height * map->texture_data[i]->width * 4;
        for (size_t j = 3; j < total; j+= 4) {
            if(map->texture_data[i]->pixels[j] != 0xFF) {
                map->texture_data[i]->pixels[j] = 0xFF;
                flag = 1;
            }
        }
        if(flag)
            log_string(1, 2, map->file_data[i], ": Walls and Doors need to be fully opaque. Fixing now...");
    }

    return ret;
}

void free_map(map_t *map)
{
    for (int i = 0; i < map->total_textures; i++) {
        free(map->file_data[i]);
        if(map->texture_data[i] != NULL){
            free(map->texture_data[i]->pixels);
            free(map->texture_data[i]);
        }
    }
    free(map->file_data);
    free(map->texture_data);

    free(map->map);
    free(map);
}
