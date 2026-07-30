/*
 * CanvaCtx.c
 *
 *  Created on: 7 juil. 2026
 *      Author: damien
 */

#include <CanvaCtx.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cairo-svg.h>

void CanvaCtx_init(CanvaCtx *self, int width, int height, int pitch)
{
	memset(self, sizeof(self), 0);
	self->width = width;
	self->height = height;
	self->pitch = pitch;
	self->default_stroke_mode = Fill;
	self->output_mode = SDL;

	self->pixels = malloc(width * height * 4);
	self->output_file = NULL;

	self->surface =
	    cairo_image_surface_create_for_data(
	        (unsigned char*)self->pixels,
	        CAIRO_FORMAT_ARGB32,
	        self->width,
	        self->height,
	        self->pitch
	    );
	self->cr = cairo_create (self->surface);
}

void CanvaCtx_init_for_svg(CanvaCtx *self, int width, int height, const char *name)
{
	memset(self, sizeof(self), 0);
	self->width = width;
	self->height = height;
	self->default_stroke_mode = Fill;

	self->output_mode = SVG;
	self->output_file = strdup(name);

	self->pixels = NULL;

	self->surface =
	    cairo_svg_surface_create(
	        name,
	        self->width,
	        self->height
	    );
	self->cr = cairo_create (self->surface);
}

void CanvaCtx_init_for_png(CanvaCtx *self, int width, int height, const char *name)
{
	memset(self, sizeof(self), 0);
	self->width = width;
	self->height = height;
	self->default_stroke_mode = Fill;

	self->output_mode = SVG;
	self->output_file = strdup(name);

	self->pixels = NULL;

	self->surface =
	    cairo_image_surface_create(
	        CAIRO_FORMAT_ARGB32,
	        self->width,
	        self->height
	    );
	self->cr = cairo_create (self->surface);
}

CanvaCtx *CanvaCtx_new(int width, int height, int pitch)
{
	CanvaCtx *self = malloc(sizeof(CanvaCtx));
	if(self != NULL)
		CanvaCtx_init(self, width, height, pitch);
	return self;
}

CanvaCtx *CanvaCtx_new_for_png(int width, int height, const char *name)
{
	CanvaCtx *self = malloc(sizeof(CanvaCtx));
	if(self != NULL)
		CanvaCtx_init_for_png(self, width, height, name);
	return self;
}

void CanvaCtx_terminate(CanvaCtx *self)
{
    if(self->pixels != NULL)
    	free(self->pixels);
    if(self->output_file != NULL)
    	free(self->output_file);
    if(self->surface != NULL)
    	cairo_surface_destroy(self->surface);
    if(self->cr != NULL)
    	cairo_destroy(self->cr);
}

void CanvaCtx_set_default_color(CanvaCtx *self, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	self->default_color[0] = (double)red / 255.0;
	self->default_color[1] = (double)green / 255.0;
	self->default_color[2] = (double)blue / 255.0;
	self->default_color[3] = (double)alpha / 255.0;
}

void CanvaCtx_set_color(CanvaCtx *self, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	double r, g, b, a;
	printf("Cairo %p set source %u %u %u %u\n", self->cr, red, green, blue, alpha);
	r = (double)red / 255.0;
	g = (double)green / 255.0;
	b = (double)blue / 255.0;
	a = (double)alpha / 255.0;

	printf("Cairo %p set source doubles %f %f %f %f\n", self->cr, r, g, b, a);
	cairo_set_source_rgba (self->cr, r, g, b, a);
}

void CanvaCtx_draw_rectangle(CanvaCtx *self, double x, double y, double w, double h)
{
	printf("Cairo %p rectangle %f %f %f %f\n", self->cr, x, y, w, h);
	cairo_rectangle(self->cr, x, y, w, h);
}

void CanvaCtx_draw_arc(CanvaCtx *self, double x, double y, double r, double a1, double a2)
{
	printf("Cairo %p arc %f %f %f %f %f\n", self->cr, x, y, r, a1, a2);
	cairo_arc(self->cr, x, y, r, a1, a2);
}

void CanvaCtx_draw_line_to(CanvaCtx *self, double x, double y)
{
	printf("Cairo %p draw line to %f %f\n", self->cr, x, y);
	cairo_line_to(self->cr, x, y);
}

void CanvaCtx_send_defaults(CanvaCtx *self)
{
	printf("Cairo %p rgba %f %f %f %f\n", self->cr, self->default_color[0], self->default_color[1], self->default_color[2], self->default_color[3]);
	cairo_set_source_rgba (self->cr, self->default_color[0], self->default_color[1], self->default_color[2], self->default_color[3]);
	printf("Cairo %p line width %f\n", self->cr, self->default_line_with);
	cairo_set_line_width (self->cr, self->default_line_with);
}

void CanvaCtx_fill(CanvaCtx *self)
{
	printf("Cairo fill %p\n", self->cr);
	cairo_fill(self->cr);
}

void CanvaCtx_fill_preserve(CanvaCtx *self)
{
	printf("Cairo fill preserve %p\n", self->cr);
	cairo_fill_preserve(self->cr);
}

void CanvaCtx_rotate(CanvaCtx *self, double a)
{
	printf("Cairo rotate %p %f\n", self->cr, a);
	cairo_rotate(self->cr, a);
}

void CanvaCtx_translate(CanvaCtx *self, double x, double y)
{
	printf("Cairo translate %p %f:%f\n", self->cr, x, y);
	cairo_translate(self->cr, x, y);
}

void CanvaCtx_save(CanvaCtx *self)
{
	printf("Cairo save %p\n", self->cr);
	cairo_save(self->cr);
}

void CanvaCtx_restore(CanvaCtx *self)
{
	printf("Cairo restore %p\n", self->cr);
	cairo_restore(self->cr);
}


void CanvaCtx_set_default_line_width(CanvaCtx *self, double w)
{
	self->default_line_with = w;
}


void CanvaCtx_set_default_stroke_mode(CanvaCtx *self, CanvaCtxStrokeMode m)
{
	self->default_stroke_mode = m;
}

void CanvaCtx_auto_stroke(CanvaCtx *self)
{
	switch(self->default_stroke_mode)
	{
	case Fill :			CanvaCtx_fill(self);
						break;
	case Stroke :		CanvaCtx_stroke(self);
						break;
	case FillStroke :	CanvaCtx_fill_preserve(self);
						CanvaCtx_stroke(self);
						break;
	case StrokeFill :	CanvaCtx_stroke_preserve(self);
						CanvaCtx_fill(self);
						break;
	case None :
			break;
	}
}

void CanvaCtx_set_line_width(CanvaCtx *self, double w)
{
	printf("Cairo %p set line width %f\n", self->cr, w);
	cairo_set_line_width (self->cr, w);
}

double CanvaCtx_get_line_width(CanvaCtx *self)
{
	double w = cairo_get_line_width (self->cr);
	printf("Cairo %p get line width %f\n", self->cr, w);
	return w;
}

void CanvaCtx_set_font(CanvaCtx *self, const char *name, const char *slant, const char *weight)
{
	cairo_font_slant_t s = CAIRO_FONT_SLANT_NORMAL;
	cairo_font_weight_t w = CAIRO_FONT_WEIGHT_NORMAL;

	if(strcmp(slant, "italic") == 0)
		s = CAIRO_FONT_SLANT_ITALIC;
	else if(strcmp(slant, "oblique") == 0)
		s = CAIRO_FONT_SLANT_OBLIQUE;

	if(strcmp(weight, "bold") == 0)
		w = CAIRO_FONT_WEIGHT_BOLD;

	printf("Cairo %p font '%s' %d ('%s') %d ('%s')\n", self->cr, name, s, slant, w, weight);
	cairo_select_font_face (self->cr, name, s, w);
}

void CanvaCtx_get_text_extents(CanvaCtx *self, const char *text, CanvaCtxTextExtent *e)
{
	cairo_text_extents_t ex;
	cairo_text_extents (self->cr, text, &ex);
	e->x_advance = ex.x_advance;
	e->y_advance = ex.y_advance;
	e->width = ex.width;
	e->height = ex.height;
	e->x_bearing = ex.x_bearing;
	e->y_bearing = ex.y_bearing;
}

void CanvaCtx_get_font_extents(CanvaCtx *self, CanvaCtxFontExtent *e)
{
	cairo_font_extents_t ex;
	cairo_font_extents (self->cr, &ex);
	e->ascent = ex.ascent;
	e->descent = ex.descent;
	e->height = ex.height;
	e->max_x_advance = ex.max_x_advance;
	e->max_y_advance = ex.max_y_advance;
}

void CanvaCtx_set_font_size(CanvaCtx *self, double size)
{
	printf("Cairo %p font size %f\n", self->cr, size);
	cairo_set_font_size (self->cr, size);
}

void CanvaCtx_move_to(CanvaCtx *self, double x, double y)
{
	printf("Cairo %p move to %f, %f\n", self->cr, x, y);
	cairo_move_to(self->cr, x, y);
}

void CanvaCtx_draw_text(CanvaCtx *self, const char *text)
{
	printf("Cairo %p draw text '%s'\n", self->cr, text);
	cairo_show_text(self->cr, text);
}

void CanvaCtx_draw_text_path(CanvaCtx *self, const char *text)
{
	printf("Cairo %p draw text path '%s'\n", self->cr, text);
	cairo_text_path(self->cr, text);
}

void CanvaCtx_draw(CanvaCtx *self)
{
	double xc = 128.0;
	double yc = 128.0;
	double radius = 100.0;
	double angle1 = 45.0  * (M_PI/180.0);  /* angles are specified */
	double angle2 = 180.0 * (M_PI/180.0);  /* in radians           */

	CanvaCtx_set_color(self, 255, 255, 255, 255);
	CanvaCtx_draw_rectangle(self, 0, 0, self->width, self->height);
	CanvaCtx_fill(self);


	CanvaCtx_set_color(self, 0, 0, 0, 255);


	CanvaCtx_set_line_width (self, 10.0);
	cairo_arc (self->cr, xc, yc, radius, angle1, angle2);
	cairo_stroke (self->cr);

	/* draw helping lines */
	CanvaCtx_set_color(self, 255, 80, 80, 200);
	CanvaCtx_set_line_width (self, 6.0);

	cairo_arc (self->cr, xc, yc, 10.0, 0, 2*M_PI);
	CanvaCtx_fill(self);

	cairo_arc (self->cr, xc, yc, radius, angle1, angle1);
	cairo_line_to (self->cr, xc, yc);
	cairo_arc (self->cr, xc, yc, radius, angle2, angle2);
	cairo_line_to (self->cr, xc, yc);
	cairo_stroke (self->cr);
}

void CanvaCtx_stroke(CanvaCtx *self)
{
	cairo_pattern_t *pat = cairo_get_source(self->cr);
	double r, g, b, a;
	cairo_pattern_get_rgba(pat, &r, &g, &b, &a);
	printf("colors at stroke %f %f %f %f\n", r, g, b, a);
	printf("Cairo %p stroke\n", self->cr);
	cairo_stroke (self->cr);
}

void CanvaCtx_stroke_preserve(CanvaCtx *self)
{
	cairo_pattern_t *pat = cairo_get_source(self->cr);
	double r, g, b, a;
	cairo_pattern_get_rgba(pat, &r, &g, &b, &a);
	printf("colors at stroke %f %f %f %f\n", r, g, b, a);
	printf("Cairo %p stroke preserve\n", self->cr);
	cairo_stroke_preserve (self->cr);
}

void CanvaCtx_update_display(CanvaCtx *self)
{
	cairo_surface_flush(self->surface);
    SDL_UpdateTexture(
        self->texture,
        NULL,
        self->pixels,
        self->width * 4
    );
}

void CanvaCtx_write_to_png(CanvaCtx *self)
{
	cairo_surface_write_to_png(self->surface, self->output_file);
}

void CanvaCtx_finish(CanvaCtx *self)
{
	cairo_surface_finish(self->surface);
}

void CanvaCtx_set_texture(CanvaCtx *self, SDL_Texture *t)
{
	self->texture = t;
}

void CanvaCtx_free(CanvaCtx *self)
{
	if(self != NULL)
	{
		CanvaCtx_terminate(self);
		free(self);
	}
}
