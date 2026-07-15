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
