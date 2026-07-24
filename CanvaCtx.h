/*
 * CanvaCtx.h
 *
 *  Created on: 7 juil. 2026
 *      Author: damien
 */

#ifndef CANVACTX_H_
#define CANVACTX_H_


#include <SDL2/SDL.h>
#include <cairo.h>

typedef enum canva_ctx_stroke_mode
{
	None = 0,
	Fill,
	Stroke,
	FillStroke,
	StrokeFill
} CanvaCtxStrokeMode;

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

    cairo_surface_t *surface;
    cairo_t *cr;
    double default_line_with;
    double default_color[4];
    CanvaCtxStrokeMode default_stroke_mode;

    SDL_Texture *texture;
} CanvaCtx;

#include <CanvaCtx.proto.h>


#endif /* CANVACTX_H_ */
