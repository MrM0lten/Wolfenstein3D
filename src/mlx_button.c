#include "mlx_button.h"

// PRIVATE //

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

    //memset(ret->pixels,0xFF,sizeof(int) * size);

    //fill pixel array with color
    for(int i = 0;i < size; i+=4)
    {
        ret->pixels[i] = (uint8_t)(color >> 24);
        ret->pixels[i+1] = (uint8_t)(color >> 16);
        ret->pixels[i+2] = (uint8_t)(color >> 8);
        ret->pixels[i+3] = (uint8_t)color;
    }
    ret->bytes_per_pixel = sizeof(uint32_t);
    ret->height = height;
    ret->width = width;

    return ret;
}


// PUBLIC //


//note: default should probably define the initial size of all textures
//do proper error checking, figure out what to display if a texture fails
btn_textures_t *mlx_create_btn_textures(char *deflt,char *highlight)
{
    btn_textures_t *tex;
    tex = malloc(sizeof(btn_textures_t));
    tex->tex_def = mlx_load_png(deflt);
    tex->tex_hlight = mlx_load_png(highlight);

    //scale every image to be the right size based on default
    if(check_texture_dim( tex->tex_def,tex->tex_hlight) == 0)
        mlx_resize_texture(tex->tex_hlight,tex->tex_def->width,tex->tex_def->height);

    //mlx_resize_texture(tex->tex_def,750,30); // random check if it works

    return (tex);
}

//need to decide what happens if no texture data is given
//probably makes sense to split button creating based on texture or not
button_t* mlx_create_button(mlx_t* mlx,btn_textures_t *text,uint32_t width,uint32_t height,uint32_t color)
{
    button_t *btn;

    btn = malloc(sizeof(button_t));
    btn->def_col = color;
    btn->highl_col = 0xFF0000FF;
    btn->world_posx = -1; //not put to the image yet
    btn->world_posy = -1;
    btn->fhover = f_hover;
    btn->fhover_end = f_hover_end;
    btn->on_hover = NULL;

    if(text != NULL){
        btn->img = mlx_new_image(mlx, text->tex_def->width, text->tex_def->height);
        btn->textures = text;
        btn->temp_pixel_arr = btn->img->pixels; //do i want that?
        btn->img->pixels = text->tex_def->pixels;
        btn->img->count = text->tex_def->height * text->tex_def->width;
    }
    else{

        //generate default btn_texture to assign them in the right place
        btn->img = mlx_new_image(mlx, width, height);
        btn->textures = malloc(sizeof(btn_textures_t));

        btn->textures->tex_def = create_texture_from_color(width,height,color);
        btn->textures->tex_hlight = create_texture_from_color(width,height,0xFF0000FF);

        btn->temp_pixel_arr = btn->img->pixels;
        btn->img->pixels = btn->textures->tex_def->pixels;
        btn->img->count = btn->textures->tex_def->height * btn->textures->tex_def->width;
    }
    return btn;
}

int32_t mlx_button_to_window(mlx_t* mlx, button_t* btn, int32_t x, int32_t y)
{
    btn->world_posx = x; //not put to the image yet
    btn->world_posy = y;
    return (mlx_image_to_window(mlx,btn->img,x,y));
}

void mlx_delete_button(mlx_t* mlx,button_t* btn)
{
    //reassigning initial image pixel array for proper cleaning, helps to clean the individual arrays themselves
    btn->img->pixels = btn->temp_pixel_arr;
    mlx_delete_image(mlx,btn->img);


    //figure out which pointer to free depending on the state
    mlx_delete_texture(btn->textures->tex_def);
    mlx_delete_texture(btn->textures->tex_hlight);
    free(btn->textures);
    free(btn);


}


//gotta think more about the architecture
//checks if mouse is within button bounds, returns its result
bool mouse_over_button(button_t* btn, int mx,int my)
{
    printf("help [%d][%d],[%d][%d]\n",btn->world_posx,btn->world_posy,btn->img->width,btn->img->height);
    if(mx < btn->world_posx || my < btn->world_posy
    || mx > btn->img->width + btn->world_posx
    || my > btn->img->height + btn->world_posy)
    {
        return false;
    }
    return true;
}


void f_hover(void* param)
{
    button_t *btn = param;
    printf("PENIS\n");
    btn->img->pixels = btn->textures->tex_hlight->pixels;
    printf("asd\n");
    if(btn->on_hover != NULL)
        btn->on_hover();
}

void f_hover_end(void *param)
{
    button_t *btn = param;
    printf("PENIS\n");
    btn->img->pixels = btn->textures->tex_def->pixels;
    printf("asd\n");
/*     if(btn->on_hover != NULL)
        btn->on_hover(); */
}






//note: this is a copy of the original function just using a texture instead of an image
bool mlx_resize_texture(mlx_texture_t* tex, uint32_t nwidth, uint32_t nheight)
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
