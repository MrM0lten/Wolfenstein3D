#ifndef BUTTON_H
#define BUTTON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MLX42.h"
#include "button_internal.h"

// An enum that gives options to display text at certain positions within a button
typedef enum text_alignment
{
	TEXT_LEFT = 0,  //left allignment
	TEXT_CENTER,    //center allignment
    TEXT_RIGHT,     //right allignment

}	text_alignment_t;

/**
 * optional Button text data.
 * It contains all information necessary to construct and display text.
 * A Text is displayed as an image itself on top of a button
 *
 * @param literal_text The literal string displayed on the button.
 * @param text The text represented as an image.
 * @param alignment The alignment within a button (LEFT,CENTER,RIGHT)
 */
typedef struct btn_txt_data_s
{
    char *literal_text;
    mlx_image_t* text;
    text_alignment_t alignment;

} btn_txt_data_t;

/**
 * Button data.
 * It contains all information necessary to construct a button
 *
 * @param img The mlx image.
 * @param text The text of the button represented as an image.
 * @param textures The textureset for different states(default,hover,highlight)
 * @param mlx The mlx instance reference.
 * @param world_posx The X position of the button within the window.
 * @param world_posy The Y position of the button within the window.
 * @param btn_data The struct containing information for function execution
 * @param enabled The flag whether or not the button is active
 * @param temp_pixel_arr The temporary pixel array storage, that holds the default image pixel
 */
typedef struct s_button
{
    mlx_image_t* img;
    btn_txt_data_t* text_data;
    btn_textures_t* textures;
    mlx_t *mlx;
    int32_t world_posx;
    int32_t world_posy;
    btn_data_t *btn_data;
    bool enabled;
    uint8_t *temp_pixel_arr;
} button_t;

/**
 * Base BTN Object instance that holds all information about Buttons and their functions.
 *
 * NOTE: By default mlx only allows to bind to one function,
 * with these linked lists multiple function can be executed at the same time.
 *
 * @param mlx The mlx reference.
 * @param buttons A linked list that contains all buttons you ever create. content = button_t
 * @param mouse_func A linked list that contains information about all mouse_hook functions that may be called. content = mousefunc_node_t
 * @param cursorfunc A linked list that contains information about all cursor_hook functions that may be called. content = cursorfunc_node_t
 */
typedef struct mlx_btn_s
{
    mlx_t *mlx;
    t_btn_list *buttons;
    t_btn_list *mouse_func;
    t_btn_list *cursorfunc;

} mlx_btn_t;


/**
 * Initializes a new BTN Instance.
 *
 * @param[in] mlx The MLX instance.
 * @returns Ptr to the BTN handle or null on failure.
 */
mlx_btn_t *mlx_button_init(mlx_t* mlx);

/**
 * Terminates BTN and cleans up any of its used resources.
 * Using any functions that require BTN afterwards will
 * be considered undefined behaviour, beware of segfaults.
 *
 * WARNING: You have to call this function before mlx_terminate()!
 *
 * @param[in] btn The MLX instance handle.
 */
void mlx_button_terminate(mlx_btn_t *btn);

/**
 * Creates a new button instance, based on the input parameters.
 * if btn_textures_t is NULL, a default texture object will be assigned
 *
 * WARNING: If you use buttons, are HAVE to use this function instead of the built in one
 * in order to have the desired results
 * NOTE: By default mlx only allows to bind to one function, this way multiple function can be executed at the same time
 *
 * @param[in] btn The BTN instance handle.
 * @param[in] text The user defined textures set or NULL.
 * @param[in] width The width of the newly created button object
 * @param[in] height The height of the newly created button object
 * @returns the newly created button.
 */
button_t* mlx_create_button(mlx_btn_t *btn,btn_textures_t *text,uint32_t width,uint32_t height);

/**
 * The new method to be called if you want to bind your own function to a cursor_hook event
 *
 * WARNING: If you use buttons, are HAVE to use this function instead of the built in one
 * in order to have the desired results
 * NOTE: By default mlx only allows to bind to one function, this way multiple function can be executed at the same time
 *
 * @param[in] btn The BTN instance handle.
 * @param[in] func The cursor callback function.
 * @param[in] param An additional optional parameter.
 */
void generic_cursor_hook(mlx_btn_t* btn, mlx_cursorfunc func, void* param);

/**
 * The new method to be called if you want to bind your own function to a mouse_hook event
 *
 * WARNING: If you use buttons, are HAVE to use this function instead of the built in one
 * in order to have the desired results
 * NOTE: By default mlx only allows to bind to one function, this way multiple function can be executed at the same time
 *
 * @param[in] btn The BTN instance handle.
 * @param[in] func The mouse callback function.
 * @param[in] param An additional optional parameter.
 */
void generic_mouse_hook(mlx_btn_t* btn, mlx_mousefunc func, void* param);

/**
 * Binds a user defined function to the button when release left mouse
 * will automatically be called once a button is "unclicked"
 *
 * @param[in] btn The Button that should execute the function.
 * @param[in] f The function to be executed.
 * @param[in] param The otional parameter to be passed.
 */
void btn_bind_on_release(button_t *btn, void (f)(void *), void *param);

/**
 * Binds a user defined function to the button when clicking
 * will automatically be called once a button is clicked
 *
 * @param[in] btn The Button that should execute the function.
 * @param[in] f The function to be executed.
 * @param[in] param The otional parameter to be passed.
 */
void btn_bind_on_click(button_t *btn, void (f)(void *), void *param);

/**
 * Binds a user defined function to the button when hovering
 * will automatically be called once a button is hovered over
 *
 * @param[in] btn The Button that should execute the function.
 * @param[in] f The function to be executed.
 * @param[in] param The otional parameter to be passed.
 */
void btn_bind_on_hover(button_t *btn, void (f)(void *), void *param);

/**
 * Creates a texture collected for a button.
 *
 * WARNING: Don't remove the default textures within the resource folder
 * resized, be aware!
 * NOTE: If you just want to create a default button, pass NULL as parameter
 *
 * @param[in] deflt The Path to the default png texture, or NULL.
 * @param[in] highlight The Path to the highlight png texture,or NULL.
 * @param[in] pressed The Path to the pressed png texture, or NULL.
 * @returns the button texture collection.
 */
btn_textures_t *mlx_create_btn_textures(char *deflt,char *highlight,char *pressed);

/**
 * Creates a texture collected for a button from color values.
 *
 * @param[in] width The width of the initial texture.
 * @param[in] height The height of the initial texture.
 * @param[in] deflt The Color value for the default texture.
 * @param[in] highlight The Color value for the highlight texture.
 * @param[in] pressed The Color value for the pressed texture.
 * @returns the button texture collection.
 */
btn_textures_t *mlx_create_btn_textures_from_colors(uint32_t width, uint32_t height, int32_t deflt,int32_t highlight,int32_t pressed);

/**
 * Wrapper around mlx_image_to_window
 * Displays button on screen and sets the world x and y pos
 *
 * @param[in] mlx The mlx
 * @param[in] btn The btn to be displayed.
 * @param[in] x The x position within the screen.
 * @param[in] y The y position within the screen.
 */
int32_t mlx_button_to_window(mlx_t *mlx, button_t* btn, int32_t x, int32_t y);


/**
 * Sets a text to be displayed on top of a button.
 * It will automatically scale down the text if the button is too small.
 *
 * NOTE: The default font is 10x20p per character, text cannot wrap, and the smallest size is defined as 4x8p per char
 *
 * @param[in] btn The btn which's text to be set
 * @param[in] text The literal text
 * @param[in] alignment The alignment of the text within the button
 */
void mlx_set_btn_text(button_t* btn, const char *text,text_alignment_t alignment);


/**
 * Creates a deep copy of the bassed btn and returns a new button.
 *
 * NOTE: Even previously functions will be copied. So be aware
 *
 * @param[in] btn_obj The mlx btn instance.
 * @param[in] btn The button to be copied
 * @returns The newly created button Object.
 */
button_t* btn_copy(mlx_btn_t* btn_obj,button_t* btn);

/**
 * Toggles the enabled state of a button.
 * 0 = off
 * 1 = on
 *
 * @param[in] btn The button to be toggled
 * @param[in] enabled The new value
 */
void btn_set_status(button_t* btn,bool enabled);

#endif
