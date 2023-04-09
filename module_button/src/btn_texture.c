#include "button_internal.h"

// --------------- PRIVATE --------------//


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

mlx_texture_t *btn_create_texture(char *user_path,char *dflt_path)
{
    mlx_texture_t *texture = NULL;
    if(user_path != NULL)
       texture = mlx_load_png(user_path);
    if(texture == NULL)
         texture = mlx_load_png(dflt_path);
    return texture;
}

int check_texture_dim(mlx_texture_t *dftl, uint32_t width, uint32_t height)
{
    if(dftl->width == width && dftl->height == height)
        return (1);
    return 0;
}

//note change this to be public and instead of 1 texture create a texture set
mlx_texture_t *create_texture_from_color(uint32_t width,uint32_t height,uint32_t color)
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


// --------------- PUBLIC --------------//


//note: default should probably define the initial size of all textures

btn_textures_t *mlx_create_btn_textures(char *deflt,char *highlight,char *pressed)
{
    btn_textures_t *tex;
    tex = malloc(sizeof(btn_textures_t));

    tex->tex_def = btn_create_texture(deflt,"./module_button/resources/btn_default.png");
    tex->tex_hlight = btn_create_texture(highlight,"./module_button/resources/btn_highlight.png");
    tex->tex_pressed = btn_create_texture(pressed,"./module_button/resources/btn_pressed.png");

    return (tex);
}

btn_textures_t *mlx_create_btn_textures_from_colors(uint32_t width, uint32_t height, int32_t deflt,int32_t highlight,int32_t pressed)
{
    btn_textures_t *tex;
    tex = malloc(sizeof(btn_textures_t));

    tex->tex_def = create_texture_from_color(width,height,deflt);
    tex->tex_hlight = create_texture_from_color(width,height,highlight);
    tex->tex_pressed = create_texture_from_color(width,height,pressed);

    return (tex);
}
