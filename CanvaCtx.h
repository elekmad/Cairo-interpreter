/*
 * CanvaCtx.h
 *
 *  Created on: 7 juil. 2026
 *      Author: damien
 */

#ifndef CANVACTX_H_
#define CANVACTX_H_

#ifndef NOSDL
#include <SDL2/SDL.h>
#endif
#include <cairo.h>
#include <inttypes.h>
#include <String.h>
#include <stdbool.h>

typedef enum canva_ctx_stroke_mode
{
	None = 0,
	Fill,
	Stroke,
	FillStroke,
	StrokeFill
} CanvaCtxStrokeMode;

typedef enum canva_ctx_output_mode
{
#ifndef NOSDL
	SDL,
#endif
	PNG,
	SVG
}CanvaCtxOutputMode;

typedef struct canva_ctx_text_extent
{
    double x_bearing;
    double y_bearing;
    double width;
    double height;
    double x_advance;
    double y_advance;
}CanvaCtxTextExtent;

typedef struct canva_ctx_font_extent
{
    double ascent;
    double descent;
    double height;
    double max_x_advance;
    double max_y_advance;
} CanvaCtxFontExtent;

typedef struct {
    int width;
    int height;
    int pitch;
    uint32_t *pixels;
#ifndef NOSDL
    SDL_Texture *texture;
#endif

    CanvaCtxOutputMode output_mode;
    String output_buffer;

    cairo_surface_t *surface;
    cairo_t *cr;
    double default_line_with;
    double default_stroke_color[4];
    double default_fill_color[4];
    CanvaCtxStrokeMode default_stroke_mode;
    bool pending_path;
    bool color_set_up;

} CanvaCtx;

#include <CanvaCtx.proto.h>


#endif /* CANVACTX_H_ */
