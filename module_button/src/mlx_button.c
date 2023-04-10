#include "mlx_button.h"

// --------------- PRIVATE --------------//

static btn_data_t *init_btn_data(void)
{
    btn_data_t *data = malloc(sizeof(btn_data_t));
    data->on_hover = NULL;
    data->on_click = NULL;
    data->on_release = NULL;
    data->param_on_click = NULL;
    data->param_on_hover = NULL;
    data->param_on_release = NULL;

    return data;
}

//a button with a specific signature to be passed to a linked list to clear its content -> private
static void delete_button(void* content)
{
    button_t* btn = (button_t *)content;
    //reassigning initial image pixel array for proper cleaning, helps to clean the individual arrays themselves
    btn->img->pixels = btn->temp_pixel_arr;
    mlx_delete_image(btn->mlx,btn->img);

    //delete text data
    free(btn->text_data->literal_text);
    mlx_delete_image(btn->mlx,btn->text_data->text);
    free(btn->text_data);

    //clean up texture references
    mlx_delete_texture(btn->textures->tex_def);
    mlx_delete_texture(btn->textures->tex_hlight);
    mlx_delete_texture(btn->textures->tex_pressed);
    free(btn->textures);
    free(btn->btn_data);
    free(btn);

}

//scale the image based on width and height
void btn_resize(button_t *btn,uint32_t width,uint32_t height)
{
    //scale every image to be the right size based on default //actually should probably be done in create button
    if(check_texture_dim(btn->textures->tex_def,width, height) == 0)
        mlx_resize_texture(btn->textures->tex_def,width, height);
    if(check_texture_dim(btn->textures->tex_hlight,width, height) == 0)
        mlx_resize_texture(btn->textures->tex_hlight,width, height);
    if(check_texture_dim(btn->textures->tex_pressed,width, height) == 0)
        mlx_resize_texture(btn->textures->tex_pressed,width, height);


    mlx_resize_image(btn->img,width,height);
}


mlx_image_t *find_text_pos(button_t* btn, char *text)
{
    int x;
    int y;
    int str_size;
    mlx_image_t *ret;
    int new_size_x = CHARX;
    int new_size_y = CHARY;

    str_size = strlen(text)*CHARX;
    if(str_size > btn->img->width)
    {
        while(new_size_x > 4 && str_size > btn->img->width)
        {
            new_size_x -=2;
            str_size = strlen(text)*new_size_x;
        }
        new_size_y = new_size_x *2;
    }
    switch (btn->text_data->alignment)
    {
    case TEXT_LEFT:
        x = btn->world_posx;
        y = btn->world_posy + btn->img->height/2 - new_size_y/2;
        break;
    case TEXT_CENTER:
        x = btn->world_posx + (btn->img->width/2) - (strlen(text)*new_size_x/2);
        y = btn->world_posy + btn->img->height/2 - new_size_y/2;
        break;
    case TEXT_RIGHT:
        x = btn->world_posx + btn->img->width - strlen(text)*new_size_x;
        y = btn->world_posy + btn->img->height/2 - new_size_y/2;
        break;
    }
    ret = mlx_put_string(btn->mlx,text,x,y);
    if(new_size_x != CHARX)
        mlx_resize_image(ret,new_size_x * strlen(text),new_size_y);
    return (ret);
}


static void f_hover(button_t *btn,void* param)
{
    btn->img->pixels = btn->textures->tex_hlight->pixels;
    if(btn->btn_data->on_hover != NULL)
        btn->btn_data->on_hover(btn->btn_data->param_on_hover);
}

static void f_hover_end(button_t *btn,void *param)
{
    btn->img->pixels = btn->textures->tex_def->pixels;
}

static void f_click(button_t *btn,void* param)
{
    btn->img->pixels = btn->textures->tex_pressed->pixels;
    if(btn->btn_data->on_click != NULL)
        btn->btn_data->on_click(btn->btn_data->param_on_click);
}

static void f_release(button_t *btn,void *param)
{
    btn->img->pixels = btn->textures->tex_def->pixels;
    if(btn->btn_data->on_release != NULL)
        btn->btn_data->on_release(btn->btn_data->param_on_release);
}

//checks if mouse is within button bounds, returns its result
static bool mouse_over_button(button_t* btn, int mx,int my)
{
    if(mx < btn->world_posx || my < btn->world_posy
    || mx > btn->img->width + btn->world_posx
    || my > btn->img->height + btn->world_posy)
    {
        return false;
    }
    return true;
}

//calls all mouse hook events that are part of the linked list
static void default_mouse_hook(mouse_key_t button, action_t action, modifier_key_t mods, void* param)
{
	mlx_btn_t *btn = param;
    t_btn_list *curr = btn->mouse_func;
    mousefunc_node_t *node;

    while(curr != NULL)
    {
        node = (mousefunc_node_t *)curr->content;
        node->mousefunc(button,action,mods,node->param);
        curr = curr->next;
    }

}

static void default_cursor_hook(double xpos, double ypos, void* param)
{
	mlx_btn_t *btn = param;
    t_btn_list *curr = btn->cursorfunc;
    cursorfunc_node_t *node;

    while(curr != NULL)
    {
        node = (cursorfunc_node_t *)curr->content;
        node->cursorfunc(xpos,ypos,node->param);
        curr = curr->next;
    }

}

static void btn_mouse_hook(mouse_key_t button, action_t action, modifier_key_t mods, void* param)
{
	mlx_btn_t *btn = param;
	if(button != MLX_MOUSE_BUTTON_LEFT)
		return;
	int x;
	int y;
	mlx_get_mouse_pos(btn->mlx,&x,&y);

    t_btn_list *curr = btn->buttons;
    button_t *node;
    while(curr != NULL)
    {
        node = (button_t *)curr->content;
        if(node->enabled == 1)
        {
            if(action == MLX_PRESS && mouse_over_button(node, x, y))
		    	f_click(node,param);
		    else if(action == MLX_RELEASE && mouse_over_button(node, x, y))
		    	f_release(node,param);
        }
        curr = curr->next;
    }
}

static void btn_cursor_hook(double xpos, double ypos, void* param)
{
	mlx_btn_t *btn = param;

    t_btn_list *curr = btn->buttons;
    button_t *node;
    while(curr != NULL)
    {
        node = (button_t *)curr->content;
        if(node->enabled == 1)
        {
            if(mouse_over_button(node, xpos, ypos))
		    	f_hover(node,param);
		    else
		    	f_hover_end(node,param);
        }
        curr = curr->next;
    }
}

static mlx_texture_t* mlx_copy_texture(mlx_texture_t *old)
{
    mlx_texture_t* new = malloc(sizeof(mlx_texture_t));
    new->width = old->width;
    new->height = old->height;
    new->bytes_per_pixel = old->bytes_per_pixel;
    size_t bytes = sizeof(uint8_t)*old->width*old->height *old->bytes_per_pixel;
    new->pixels = malloc(bytes);
    memcpy(new->pixels,old->pixels,bytes);

    return new;
}


// --------------- PUBLIC --------------//

mlx_btn_t *mlx_button_init(mlx_t* mlx)
{
    mlx_btn_t *btn = malloc(sizeof(mlx_btn_t));
    btn->mlx = mlx;
    btn->cursorfunc = NULL;
    btn->mouse_func = NULL;
    btn->buttons = NULL;

    mlx_cursor_hook(mlx, default_cursor_hook, btn);
	mlx_mouse_hook(mlx, default_mouse_hook,btn);

    generic_cursor_hook(btn,btn_cursor_hook,btn);
    generic_mouse_hook(btn,btn_mouse_hook,btn);

    return (btn);
}


void mlx_button_terminate(mlx_btn_t *btn)
{
    btn_lstclear(&btn->buttons,delete_button);
    btn_lstclear(&btn->mouse_func, free);
    btn_lstclear(&btn->cursorfunc, free);
    free(btn);
}

button_t* mlx_create_button(mlx_btn_t *btn,btn_textures_t *text,uint32_t width,uint32_t height)
{
    button_t *ret;

    ret = malloc(sizeof(button_t));
    ret->btn_data = init_btn_data();
    ret->mlx = btn->mlx;
    ret->world_posx = -1;
    ret->world_posy = -1;

    ret->text_data = malloc(sizeof(btn_txt_data_t));
    ret->text_data->text = NULL;
    ret->text_data->literal_text = NULL;
    ret->text_data->alignment = TEXT_LEFT;

    ret->img = mlx_new_image(btn->mlx, width, height);

    if(text != NULL)
        ret->textures = text;
    else
        ret->textures = mlx_create_btn_textures(NULL,NULL,NULL);

    ret->enabled = 1;
    btn_resize(ret,width,height);
    ret->temp_pixel_arr = ret->img->pixels;
    ret->img->pixels = ret->textures->tex_def->pixels;
    ret->img->count = ret->textures->tex_def->height * ret->textures->tex_def->width;

    if(btn->buttons == NULL)
        btn->buttons = btn_lstnew(ret);
    else
        btn_lstadd_front(&(btn->buttons),btn_lstnew(ret));
    return ret;
}

int32_t mlx_button_to_window(mlx_t *mlx, button_t* btn, int32_t x, int32_t y)
{
    btn->world_posx = x;
    btn->world_posy = y;

    uint32_t ret = mlx_image_to_window(mlx,btn->img,x,y);
    if(btn->text_data->text != NULL)
        mlx_set_btn_text(btn,btn->text_data->literal_text,btn->text_data->alignment);

    return (ret);
}

void mlx_set_btn_text(button_t* btn, const char *text,text_alignment_t alignment)
{
    if(text == NULL)
        return;
    char *temp = strdup(text);
    btn->text_data->alignment = alignment;
    if(btn->text_data->text != NULL)
        mlx_delete_image(btn->mlx,btn->text_data->text);
    if(btn->text_data->literal_text != NULL)
        free(btn->text_data->literal_text);
    btn->text_data->literal_text = strdup(temp);
    btn->text_data->text = find_text_pos(btn,temp);
    free(temp);
}

//creates a copy of an existing button without putting it to the window
//useful if you want to create many buttons quickly
button_t* btn_copy(mlx_btn_t* my_btn_lst,button_t* btn)
{
    button_t *new = malloc(sizeof(button_t));

    new->mlx = btn->mlx;
    new->img = mlx_new_image(btn->mlx, btn->img->width, btn->img->height);

    new->text_data = malloc(sizeof(btn_txt_data_t));
    new->text_data->literal_text = NULL;
    new->text_data->text = NULL;
    new->text_data->alignment = btn->text_data->alignment;

    mlx_set_btn_text(new,btn->text_data->literal_text, btn->text_data->alignment);

    new->textures = malloc(sizeof(btn_textures_t));
    new->textures->tex_def = mlx_copy_texture(btn->textures->tex_def);
    new->textures->tex_hlight = mlx_copy_texture(btn->textures->tex_hlight);
    new->textures->tex_pressed = mlx_copy_texture(btn->textures->tex_pressed);

    new->world_posx = btn->world_posx;
    new->world_posy = btn->world_posy;

    new->btn_data = malloc(sizeof(btn_data_t));
    new->btn_data->on_hover = btn->btn_data->on_hover;
    new->btn_data->param_on_hover = btn->btn_data->param_on_hover;
    new->btn_data->on_click = btn->btn_data->on_click;
    new->btn_data->param_on_click = btn->btn_data->param_on_click;
    new->btn_data->on_release = btn->btn_data->on_release;
    new->btn_data->param_on_release = btn->btn_data->param_on_release;

    new->enabled = btn->enabled;

    new->temp_pixel_arr = new->img->pixels;
    new->img->pixels = new->textures->tex_def->pixels;
    btn_lstadd_back( &my_btn_lst->buttons,btn_lstnew(new));

    return new;
}

//allows you to enable or disable a button
void btn_set_status(button_t* btn,bool enabled)
{
    if(enabled == 1)
    {
        btn->enabled = 1;
        btn->img->instances[0].enabled = 1;
        btn->text_data->text->instances[0].enabled = 1;
    }
    else if(enabled == 0)
    {
        btn->enabled = 0;
        btn->img->instances[0].enabled = 0;
        btn->text_data->text->instances[0].enabled = 0;
    }
}

void generic_cursor_hook(mlx_btn_t* btn, mlx_cursorfunc func, void* param)
{
    cursorfunc_node_t *new = malloc(sizeof(cursorfunc_node_t));
    new->cursorfunc = func;
    new->param = param;

    btn_lstadd_back(&btn->cursorfunc,btn_lstnew(new));
}

void generic_mouse_hook(mlx_btn_t* btn, mlx_mousefunc func, void* param)
{
    mousefunc_node_t *new = malloc(sizeof(mousefunc_node_t));
    new->mousefunc = func;
    new->param = param;

    btn_lstadd_back(&btn->mouse_func,btn_lstnew(new));
}

void btn_bind_on_hover(button_t *btn, void (f)(void *), void *param)
{
	btn->btn_data->on_hover = f;
	btn->btn_data->param_on_hover = param;
}

void btn_bind_on_click(button_t *btn, void (f)(void *), void *param)
{
	btn->btn_data->on_click = f;
	btn->btn_data->param_on_click = param;
}

void btn_bind_on_release(button_t *btn, void (f)(void *), void *param)
{
	btn->btn_data->on_release = f;
	btn->btn_data->param_on_release = param;
}
