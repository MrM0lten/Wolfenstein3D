#include "mlx_button.h"

// PRIVATE //

//libft list shit

void	btn_lstclear(t_btn_list **lst, void (*del)(void *))
{
	t_btn_list	*tmp;

	while ((*lst))
	{
		tmp = (*lst)->next;
		del((*lst)->content);
		free((*lst));
		(*lst) = tmp;
	}
	*lst = NULL;
}

t_btn_list	*btn_lstlast(t_btn_list *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next)
		lst = lst->next;
	return (lst);
}

void	btn_lstadd_back(t_btn_list **lst, t_btn_list *new_elem)
{
	t_btn_list	*tmp;

	if (!(*lst))
	{
		*lst = new_elem;
		return ;
	}
	tmp = btn_lstlast(*lst);
	tmp->next = new_elem;
}

t_btn_list	*btn_lstnew(void *content)
{
	t_btn_list	*new;

	new = malloc(sizeof(t_btn_list));
	if (!new)
		return (NULL);
	new->content = content;
	new->next = NULL;
	return (new);
}

void mlx_delete_button(void* content)
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

void	btn_lstadd_front(t_btn_list **lst, t_btn_list *new_elem)
{
	new_elem->next = *lst;
	*lst = new_elem;
}

static bool mlx_resize_texture(mlx_texture_t* tex, uint32_t nwidth, uint32_t nheight)
{
	MLX_NONNULL(tex);

	if (!nwidth || !nheight || nwidth > INT16_MAX || nheight > INT16_MAX)
		return (mlx_error(MLX_INVDIM));
	if (nwidth != tex->width || nheight != tex->height)
	{
		uint32_t* origin = (uint32_t*)tex->pixels;
		float wstep = (float)tex->width / nwidth;
		float hstep = (float)tex->height / nheight;

		uint8_t* tempbuff = calloc(nwidth * nheight, sizeof(uint32_t));
		if (!tempbuff)
			return (mlx_error(MLX_MEMFAIL));
		tex->pixels = tempbuff;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nwidth, nheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->pixels);

		uint32_t* destin = (uint32_t*)tex->pixels;
		for (uint32_t j = 0; j < nheight; j++)
			for (uint32_t i = 0; i < nwidth; i++)
				destin[j * nwidth + i] = origin[(uint32_t)(j * hstep) * tex->width + (uint32_t)(i * wstep)];
		(*(uint32_t*)&tex->width) = nwidth;
		(*(uint32_t*)&tex->height) = nheight;
		free(origin);
	}
	return (true);
}


btn_data_t *init_btn_data(void)
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

static void f_hover(button_t *btn,void* param)
{
    btn->img->pixels = btn->textures->tex_hlight->pixels;
    if(btn->btn_data->on_hover != NULL)
        btn->btn_data->on_hover(btn->btn_data->param_on_hover);
}

static void f_hover_end(button_t *btn,void *param)
{
    btn->img->pixels = btn->textures->tex_def->pixels;
/*     if(btn->on_hover != NULL)
        btn->on_hover(); */
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
bool mouse_over_button(button_t* btn, int mx,int my)
{
    if(mx < btn->world_posx || my < btn->world_posy
    || mx > btn->img->width + btn->world_posx
    || my > btn->img->height + btn->world_posy)
    {
        return false;
    }
    return true;
}

static int check_texture_dim(mlx_texture_t *dftl,mlx_texture_t *other)
{
    if(dftl->width == other->width && dftl->height == other->height)
        return (1);
    return 0;
}

static mlx_texture_t *create_texture_from_color(uint32_t width,uint32_t height,uint32_t color)
{
    mlx_texture_t *ret;
    ret = malloc(sizeof(mlx_texture_t));
    int size = sizeof(uint32_t) * width * height;
    ret->pixels = malloc(size);

    //fill pixel array with color
    for(int i = 0;i < size; i+=4)
    {
        ret->pixels[i] = (uint8_t)(color >> 24);
        ret->pixels[i+1] = (uint8_t)(color >> 16);
        ret->pixels[i+2] = (uint8_t)(color >> 8);
        ret->pixels[i+3] = (uint8_t)color & 0xFF;
    }
    ret->bytes_per_pixel = sizeof(uint32_t);
    ret->height = height;
    ret->width = width;
    return ret;
}


//calls all mouse hook events that are part of the linked list
void default_mouse_hook(mouse_key_t button, action_t action, modifier_key_t mods, void* param)
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
void default_cursor_hook(double xpos, double ypos, void* param)
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
void btn_mouse_hook(mouse_key_t button, action_t action, modifier_key_t mods, void* param)
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

void btn_cursor_hook(double xpos, double ypos, void* param)
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



// PUBLIC //

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

    btn_lstclear(&btn->buttons,mlx_delete_button);
    btn_lstclear(&btn->mouse_func, free);
    btn_lstclear(&btn->cursorfunc, free);

    //do shit
    free(btn);
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



//note: default should probably define the initial size of all textures
//do proper error checking, figure out what to display if a texture fails
btn_textures_t *mlx_create_btn_textures(char *deflt,char *highlight,char *pressed)
{
    btn_textures_t *tex;
    tex = malloc(sizeof(btn_textures_t));
    tex->tex_def = mlx_load_png(deflt);
    tex->tex_hlight = mlx_load_png(highlight);
    tex->tex_pressed = mlx_load_png(pressed);

    //scale every image to be the right size based on default
    if(check_texture_dim( tex->tex_def,tex->tex_hlight) == 0)
        mlx_resize_texture(tex->tex_hlight,tex->tex_def->width,tex->tex_def->height);
    if(check_texture_dim( tex->tex_def,tex->tex_pressed) == 0)
        mlx_resize_texture(tex->tex_pressed,tex->tex_def->width,tex->tex_def->height);

    //mlx_resize_texture(tex->tex_def,750,30); // random check if it works

    return (tex);
}

//might need to rething how the user will access these functions if the buttons are part of a linked list
//edit: should actually be fine, becuase create button returns a pointer to the correct object
void mlx_btn_bind_on_hover(button_t *btn,void (*f)(void *))
{
    btn->btn_data->on_hover = f;
}

void mlx_btn_bind_on_click(button_t *btn,void (*f)(void *))
{
    btn->btn_data->on_click = f;
}

void mlx_btn_bind_on_release(button_t *btn,void (*f)(void *))
{
    btn->btn_data->on_release = f;
}

//need to decide what happens if no texture data is given
//probably makes sense to split button creating based on texture or not
button_t* mlx_create_button(mlx_btn_t *btn,btn_textures_t *text,uint32_t width,uint32_t height,uint32_t color)
{
    button_t *ret;

    ret = malloc(sizeof(button_t));
    ret->btn_data = init_btn_data();
    ret->mlx = btn->mlx;
    ret->def_col = color;
    ret->highl_col = 0xFF0000FF;
    ret->world_posx = -1; //not put to the image yet
    ret->world_posy = -1;
    //setup internals




    if(text != NULL){
        ret->img = mlx_new_image(btn->mlx, text->tex_def->width, text->tex_def->height);
        ret->textures = text;
        ret->temp_pixel_arr = ret->img->pixels; //do i want that?
        ret->img->pixels = text->tex_def->pixels;
        ret->img->count = text->tex_def->height * text->tex_def->width;
    }
    else{

        //generate default btn_texture to assign them in the right place
        ret->img = mlx_new_image(btn->mlx, width, height);
        ret->textures = malloc(sizeof(btn_textures_t));

        ret->textures->tex_def = create_texture_from_color(width,height,color);
        ret->textures->tex_hlight = create_texture_from_color(width,height,0x00FFFFFF);
        ret->textures->tex_pressed = create_texture_from_color(width,height,0xFF0000FF);

        ret->temp_pixel_arr = ret->img->pixels;
        ret->img->pixels = ret->textures->tex_def->pixels;
        ret->img->count = ret->textures->tex_def->height * ret->textures->tex_def->width;
    }

    //IMPORTANT before returning add it to the linked list of mlx_btn_t
    //use lst add front from libft
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

