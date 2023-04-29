#include "wolfenstein.h"

void store_map_texture(map_t* map,char *line)
{
    printf("found texture\n");
}

void store_map_color(map_t* map,char *line)
{
    printf("found color\n");
}

//will finish reading the map and get map
void store_map_array(map_t* map,char *line,int fd)
{
    printf("found map\n");
    size_t len = ft_strlen((const char*)line);
    //kill me -.- this is bs
    //could either allocate a lot of memory at first and than use realloc
    //could store each string line in a list, until i completely parsed all required input
    //keep track of line length since initial line can be smaller than later line
    //e.g
    //111   = 3
    //10111 = 5
    //10001 = 5
    //11011 = 5
    // 111  = 4
    //since map is stored in 1d array i need to fill the "gaps" with a value
    //fun....

}


enum map_type_id identify_line(char *line)
{
    if(line == NULL)
        return MP_ERR;
    size_t len = ft_strlen((const char*)line);
    if(len < 2)
        return MP_ERR;
    if(!ft_strncmp(line,"NO",2) || !ft_strncmp(line,"SO",2)
        || !ft_strncmp(line,"WE",2) || !ft_strncmp(line,"EA",2))
        return MP_TEXT;
    else if(!ft_strncmp(line,"F",1) || !ft_strncmp(line,"C",1))
        return MP_COL;
    else if(ft_strchr(line,'1') != NULL || ft_strchr(line,'1') != NULL)
        return MP_MAP;

    return MP_ERR;
}

map_t* read_map(char *path)
{
    int fd = open(path,O_RDONLY);
    if(fd == -1)
        return NULL;
    //check if the format is .cub

    map_t* map = malloc(sizeof(map_t));
    char *line;
    line = get_next_line(fd);
    enum map_type_id id;
    while(line)
    {
        id = identify_line(line);
        printf("id = %d\n",id);
        printf("line = {%s}\n",line);
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
    if(!validate_map(map))
    {
        delete_map(map);
        return NULL;
    }
    return map;
}
//returns 1 on success
// 0 on failure
int validate_map(map_t* map)
{
    return 1;
}

void delete_map(map_t *map)
{
    (void)map;
}
