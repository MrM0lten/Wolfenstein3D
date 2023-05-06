#include "wolfenstein.h"

void store_map_texture(map_t* map,char *line)
{
    unsigned int i = 2;
    while(line[i] == ' ' || line[i] == '\t' ) //detecting any spaces after the id
        i++;
    unsigned int len = i;
    while(line[len] &&  line[len] != ' '
        && line[len] != '\t' && line[len] != '\n' ) //making sure to only take the first word
        len++;
    if(line[i] == '\n' ) //checking if there is no path after an identifier
        return log_string("Found Texture identifier, but line is empty",1);

    enum texture_type type = find_texture_id(line);

    if(map->file_data[type] != NULL)
            free_and_warn(&map->file_data[type]);
        map->file_data[type] = ft_substr(line,i,len-i);

}

void store_map_color(map_t* map,char *line)
{
    //printf("found color\n");
    unsigned int i = 1;
    while(line[i] == ' ' || line[i] == '\t' ) //detecting any spaces after the id
        i++;
    unsigned int len = i;
    while(line[len] &&  line[len] != ' '
        && line[len] != '\t' && line[len] != '\n' ) //making sure to only take the first word
        len++;
    if(line[i] == '\n' ) //checking if there is no path after an identifier
        return log_string("Found Color identifier, but line is empty",1);
    char *col_str = ft_substr(line,i,len-i);
    if(!col_str)
        return log_string("Substring error",2);
    char **col_data = ft_split(col_str,',');
    if(!col_data || ft_strarr_len(col_data) != 3)
        return log_string("Didnt find appropriate R,G,B value format ",1);

    uint32_t col = str_to_col(col_data);

    if(line[0] == 'F') {
        if(map->col_floor != 0)
            log_string("Addional Ceiling Color found!",1);
        map->col_floor = col;
    }
    else if(line[0] == 'C') {
        if(map->col_ceil != 0)
            log_string("Addional Floor Color found!",1);
        map->col_ceil = col;
    }


    //free utilities
    free(col_str);
    int pos = 0;
    while(col_data[pos])
        free(col_data[pos++]);
    free(col_data);
}

//fills the maps int array with values, it will detect errors, but will parse anyways
//actual error checking will be done later
//fill actual map array by looping through the list
//'0' = 0
//'1' = 1
//' ' = 2
//'N/E/S/W' = 0 but save info player info
//err = -1
static void fill_map(map_t* map,t_list* lst_line)
{
    map->map = malloc(sizeof(int) * (map->map_dim));
    char* content;
    int pos = 0;
    int i;
    while(lst_line)
    {
        content = (char *)lst_line->content;
        i = 0;
        while(content[i] && content[i] != '\n') {
            if(content[i] == '0')
                map->map[pos] = GD_FREE;
            else if(content[i] == '1')
                map->map[pos] = GD_WALL;
            else if(content[i] == '2')
                map->map[pos] = GD_DOOR;
            else if(content[i] == ' ')
                map->map[pos] = GD_VOID;
            else if(ft_strchr("NSEW",content[i])) {
                if(map->p_pos_x != -1)
                    log_string("Addional Player location found, prev Value overriden!",1);
                map->map[pos] = GD_FREE;
                map->p_orient = player_rot_from_char(content[i]);
                map->p_pos_x = i;
                map->p_pos_y = (pos + 1) / map->map_x;
            }
            else {
                map->map[pos] = -1;
            }
            i++;
            pos++;
        }
        while(i++ < map->map_x) //emulate spaces for rectangle shape
            map->map[pos++] = 2;
        lst_line = lst_line->next;
    }
}

//will finish reading the map and get map
void store_map_array(map_t* map,char *line,int fd)
{
    //figure out dimensions of the map, generate list of lines
    t_list* lst_line = NULL;
    int tempx;
    while(line)
    {
        tempx = (int)ft_strlen((const char*)line);
        if(tempx == 1) //stop as soon as a line is empty
        {
            free(line);
            break;
        }//
        if(line[tempx - 1] == '\n') //removing 1 if last character is new line
            tempx--;
        if(tempx > map->map_x)
            map->map_x = tempx;

        ft_lstadd_back(&lst_line,ft_lstnew(line));
        map->map_y++;
        line = get_next_line(fd);
    }
    map->map_dim = map->map_x * map->map_y;

    fill_map(map,lst_line);
    ft_lstclear(&lst_line,free);
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

map_t* read_map(char *path)
{
    if(!ft_strnstr(path,".cub",ft_strlen(path))) {
        log_string("Input file does not have the required '.cub' extension",2);
        return NULL;
    }
    int fd = open(path,O_RDONLY);
    if(fd == -1)
        return NULL;
    //check if the format is .cub

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
        free_map(map);
        return NULL;
    }
    debug_map(map);
    return map;
}

//returns 1 on success
// 0 on failure
int validate_map(map_t* map)
{
    int ret = 1;

    for (int i = 0; i < map->total_textures; i++)
    {

        if(map->file_data[i] == NULL || access(map->file_data[i],F_OK | R_OK)) {
            if(i == TXT_DOOR) {
                log_string("No DOOR texture found, replacing doors with empty tiles",1);
                for (int i = 0; i < map->map_dim; i++){
                    if(map->map[i] == GD_DOOR)
                        map->map[i] = GD_FREE;
                }
            }
            else {
                log_string("No texture found",2);
                ret = 0;
            }
        }
    }

    if(map->col_ceil == 0){
        log_string("No Ceiling color found",2);
        ret = 0;
    }
    if(map->col_floor == 0){
        log_string("No Ceiling color found",2);
        ret = 0;
    }

    if(map->p_pos_x == -1){
        log_string("player Position was not set within the map",2);
        ret = 0;
    }

    int val;
    //check if each 0 has only 1 and 0 as adjacent positions
    for (int y = 0; y < map->map_y; y++)
    {
        for (int x = 0; x < map->map_x; x++)
        {
            val = map->map[y * map->map_x + x];
            if(val == GD_VOID)
                continue;
            if(val == -1 || !is_walled(x,y,map))
            {
                log_string("Map is impossible.",2);
                return 0;
            }
        }
    }

    for (int i = 0; i < map->total_textures; i++){
        if(map->file_data[i] != NULL)
            map->texture_data[i] = mlx_load_png(map->file_data[i]);
    }

    return ret;
}

void free_map(map_t *map)
{
    for (int i = 0; i < map->total_textures; i++)
    {
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
