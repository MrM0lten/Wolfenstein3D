#include "mlx_button.h"

// PRIVATE //

static int check_texture_dim(mlx_texture_t *dftl,mlx_texture_t *other)
{
    if(dftl->width == other->width && dftl->height == other->height)
        return (1);
    return 0;
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
button_t* mlx_create_button(mlx_t* mlx,btn_textures_t *text,uint32_t width,uint32_t height,uint32_t color)
{
    button_t *btn;

    btn = malloc(sizeof(button_t));
    btn->def_col = color;
    btn->highl_col = color;

    if(text != NULL){
        btn->img = mlx_new_image(mlx, text->tex_def->width, text->tex_def->height);
        btn->img->pixels = text->tex_def->pixels;
        btn->img->count = text->tex_def->height * text->tex_def->width;
    }
    else{
        btn->img = mlx_new_image(mlx, width, height);
        btn->img->count = width *height;
        memset(btn->img->pixels,0xF0, btn->img->count * sizeof(int));
    }
    return btn;
}

int32_t mlx_button_to_window(mlx_t* mlx, button_t* btn, int32_t x, int32_t y)
{
    return (mlx_image_to_window(mlx,btn->img,x,y));
}

void mlx_delete_button(mlx_t* mlx,button_t* btn)
{
    mlx_delete_image(mlx,btn->img);

    mlx_delete_texture(btn->textures->tex_def);
    mlx_delete_texture(btn->textures->tex_hlight);
    free(btn->textures);
    free(btn);


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
