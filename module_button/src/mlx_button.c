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

static void delete_button(void* content)
{
    button_t* btn = (button_t *)content;
    //reassigning initial image pixel array for proper cleaning, helps to clean the individual arrays themselves
    btn->img->pixels = btn->temp_pixel_arr;
    mlx_delete_image(btn->mlx,btn->img);


    //clean up texture references
    mlx_delete_texture(btn->textures->tex_def);
    mlx_delete_texture(btn->textures->tex_hlight);
    mlx_delete_texture(btn->textures->tex_pressed);
    free(btn->textures);
    free(btn->btn_data);
    free(btn);

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

//calls all cursor hook events that ar part of the linked list
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

//initial very specific button ho still need to work on the param versions what to actually pass in
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
        if(action == MLX_PRESS && mouse_over_button(node, x, y))
			f_click(node,param);
		else if(action == MLX_RELEASE && mouse_over_button(node, x, y))
			f_release(node,param);
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
        if(mouse_over_button(node, xpos, ypos))
			f_hover(node,param);
		else
			f_hover_end(node,param);
        curr = curr->next;
    }
}


// --------------- PUBLIC --------------//

//creates a button object and binds to mouse_hook() and cursor_hook()
//serves as a wrapper for all hook functions
mlx_btn_t *mlx_button_init(mlx_t* mlx)
{
    mlx_btn_t *btn = malloc(sizeof(mlx_btn_t));
    btn->mlx = mlx;
    btn->cursorfunc = NULL;
    btn->mouse_func = NULL;
    btn->buttons = NULL;

    //generic functions that should call all other functions respectively
    mlx_cursor_hook(mlx, default_cursor_hook, btn);
	mlx_mouse_hook(mlx, default_mouse_hook,btn);

    //first binding to allow for all kinds of events for buttons
    generic_cursor_hook(btn,btn_cursor_hook,btn);
    generic_mouse_hook(btn,btn_mouse_hook,btn);

    return (btn);
}

//cleans up everything related to buttons
void mlx_button_terminate(mlx_btn_t *btn)
{
    btn_lstclear(&btn->buttons,delete_button);
    btn_lstclear(&btn->mouse_func, free);
    btn_lstclear(&btn->cursorfunc, free);
    free(btn);
}

//need to decide what happens if no texture data is given
//probably makes sense to split button creating based on texture or not
button_t* mlx_create_button(mlx_btn_t *btn,btn_textures_t *text,uint32_t width,uint32_t height)
{
    button_t *ret;

    ret = malloc(sizeof(button_t));
    ret->btn_data = init_btn_data();
    ret->mlx = btn->mlx;
    ret->world_posx = -1;
    ret->world_posy = -1;

    ret->img = mlx_new_image(btn->mlx, width, height);
    if(text != NULL)
        ret->textures = text;
    else
        ret->textures = mlx_create_btn_textures(NULL,NULL,NULL);

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
    btn->world_posx = x; //not put to the image yet
    btn->world_posy = y;
    return (mlx_image_to_window(mlx,btn->img,x,y));
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

}

//create a new node and add it to linked list
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
