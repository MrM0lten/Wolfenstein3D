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
        return log_string(1,1,"Found Texture identifier, but line is empty");

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
        return log_string(1,1,"Found Color identifier, but line is empty");
    char *col_str = ft_substr(line,i,len-i);
    if(!col_str)
        return log_string(2,1,"Substring error");
    char **col_data = ft_split(col_str,',');
    if(!col_data || ft_strarr_len(col_data) != 3)
        return log_string(1,1,"Didnt find appropriate R,G,B value format");

    uint32_t col = str_to_col(col_data);

    if(line[0] == 'F') {
        if(map->col_floor != 0)
            log_string(1,1,"Addional Ceiling Color found!");
        map->col_floor = col;
    }
    else if(line[0] == 'C') {
        if(map->col_ceil != 0)
            log_string(1,1,"Addional Floor Color found!");
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
                    log_string(1,1,"Addional Player location found, prev Value overriden!");
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
