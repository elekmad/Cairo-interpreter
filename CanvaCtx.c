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
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <cairo-svg.h>
#include <b64/cencode.h>

void CanvaCtx_init(CanvaCtx *self, int width, int height, int pitch)
{
	memset(self, sizeof(self), 0);
	self->width = width;
	self->height = height;
	self->pitch = pitch;
	self->default_stroke_mode = Stroke;
	CanvaCtx_set_default_fill_color(self, 0, 0, 0, 255);
	CanvaCtx_set_default_stroke_color(self, 0, 0, 0, 255);
	self->pending_path = false;
	self->color_set_up = false;
#ifndef NOSDL
	self->output_mode = SDL;
#else
	self->output_mode = SVG;
#endif

	self->pixels = malloc(width * height * 4);
	String_init(&self->output_buffer);

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


void CanvaCtx_init_for_svg(CanvaCtx *self, int width, int height)
{
	memset(self, sizeof(self), 0);
	self->width = width;
	self->height = height;
	self->default_stroke_mode = Stroke;
	CanvaCtx_set_default_fill_color(self, 0, 0, 0, 255);
	CanvaCtx_set_default_stroke_color(self, 0, 0, 0, 255);
	self->pending_path = false;
	self->color_set_up = false;

	self->output_mode = SVG;

	self->pixels = NULL;

	self->surface =
	    cairo_svg_surface_create_for_stream(
    		(cairo_status_t (*) (void *, const unsigned char *, unsigned int))CanvaCtx_write_into_buffer,
	        self,
	        self->width,
	        self->height
	    );
	self->cr = cairo_create (self->surface);
}

void CanvaCtx_init_for_png(CanvaCtx *self, int width, int height)
{
	memset(self, sizeof(self), 0);
	self->width = width;
	self->height = height;
	self->default_stroke_mode = Stroke;
	CanvaCtx_set_default_fill_color(self, 0, 0, 0, 255);
	CanvaCtx_set_default_stroke_color(self, 0, 0, 0, 255);
	self->pending_path = false;
	self->color_set_up = false;

	self->output_mode = PNG;

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

CanvaCtx *CanvaCtx_new_for_png(int width, int height)
{
	CanvaCtx *self = malloc(sizeof(CanvaCtx));
	if(self != NULL)
		CanvaCtx_init_for_png(self, width, height);
	return self;
}

void CanvaCtx_terminate(CanvaCtx *self)
{
	String_finalize(&self->output_buffer);
    if(self->pixels != NULL)
    	free(self->pixels);
    if(self->surface != NULL)
    	cairo_surface_destroy(self->surface);
    if(self->cr != NULL)
    	cairo_destroy(self->cr);
}

void CanvaCtx_set_default_stroke_color(CanvaCtx *self, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	self->default_stroke_color[0] = (double)red / 255.0;
	self->default_stroke_color[1] = (double)green / 255.0;
	self->default_stroke_color[2] = (double)blue / 255.0;
	self->default_stroke_color[3] = (double)alpha / 255.0;
}

void CanvaCtx_set_default_fill_color(CanvaCtx *self, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	self->default_fill_color[0] = (double)red / 255.0;
	self->default_fill_color[1] = (double)green / 255.0;
	self->default_fill_color[2] = (double)blue / 255.0;
	self->default_fill_color[3] = (double)alpha / 255.0;
}

void CanvaCtx_set_color(CanvaCtx *self, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	double r, g, b, a;
	fprintf(stderr, "Cairo %p set source %u %u %u %u\n", self->cr, red, green, blue, alpha);
	r = (double)red / 255.0;
	g = (double)green / 255.0;
	b = (double)blue / 255.0;
	a = (double)alpha / 255.0;

	_CanvaCtx_set_color(self, r, b, g, a);
	self->color_set_up = true;
}

void _CanvaCtx_send_default_stroke_color(CanvaCtx *self)
{
	fprintf(stderr, "Sending default stroke color %f %f %f %f\n",
			self->default_stroke_color[0],
			self->default_stroke_color[1],
			self->default_stroke_color[2],
			self->default_stroke_color[3]);

	_CanvaCtx_set_color(self,
			self->default_stroke_color[0],
			self->default_stroke_color[1],
			self->default_stroke_color[2],
			self->default_stroke_color[3]);
}

void _CanvaCtx_send_default_fill_color(CanvaCtx *self)
{
	fprintf(stderr, "Sending default fill color %f %f %f %f\n",
			self->default_fill_color[0],
			self->default_fill_color[1],
			self->default_fill_color[2],
			self->default_fill_color[3]);

	_CanvaCtx_set_color(self,
			self->default_fill_color[0],
			self->default_fill_color[1],
			self->default_fill_color[2],
			self->default_fill_color[3]);
}

void _CanvaCtx_set_color(CanvaCtx *self, double r, double g, double b, double a)
{
	fprintf(stderr, "Cairo %p set source doubles %f %f %f %f\n", self->cr, r, g, b, a);
	cairo_set_source_rgba (self->cr, r, g, b, a);
}

void CanvaCtx_draw_rectangle(CanvaCtx *self, double x, double y, double w, double h)
{
	fprintf(stderr, "Cairo %p rectangle %f %f %f %f\n", self->cr, x, y, w, h);
	cairo_rectangle(self->cr, x, y, w, h);
	self->pending_path = true;
}

void CanvaCtx_draw_arc(CanvaCtx *self, double x, double y, double r, double a1, double a2)
{
	fprintf(stderr, "Cairo %p arc %f %f %f %f %f\n", self->cr, x, y, r, a1, a2);
	cairo_arc(self->cr, x, y, r, a1, a2);
	self->pending_path = true;
}

void CanvaCtx_draw_arc_negative(CanvaCtx *self, double x, double y, double r, double a1, double a2)
{
	fprintf(stderr, "Cairo %p arc negative %f %f %f %f %f\n", self->cr, x, y, r, a1, a2);
	cairo_arc_negative(self->cr, x, y, r, a1, a2);
	self->pending_path = true;
}

void CanvaCtx_draw_line_to(CanvaCtx *self, double x, double y)
{
	fprintf(stderr, "Cairo %p draw line to %f %f\n", self->cr, x, y);
	cairo_line_to(self->cr, x, y);
	self->pending_path = true;
}

void CanvaCtx_draw_bezier(CanvaCtx *self, double x, double y, double m1x, double m1y, double m2x, double m2y)
{
	fprintf(stderr, "Cairo %p beziez to {%f, %f} handle 1 {%f, %f} handle 2 {%f, %f}\n", self->cr, x, y, m1x, m1y, m2x, m2y);
	cairo_curve_to(self->cr, m1x, m1y, m2x, m2y, x, y);
	self->pending_path = true;
}

void CanvaCtx_fill(CanvaCtx *self)
{
	cairo_pattern_t *pat = cairo_get_source(self->cr);
	double r, g, b, a;
	cairo_pattern_get_rgba(pat, &r, &g, &b, &a);
	fprintf(stderr, "colors at fill %f %f %f %f\n", r, g, b, a);
	fprintf(stderr, "Cairo fill %p\n", self->cr);
	cairo_fill(self->cr);
	self->pending_path = false;
	self->color_set_up = false;
}

void CanvaCtx_fill_preserve(CanvaCtx *self)
{
	cairo_pattern_t *pat = cairo_get_source(self->cr);
	double r, g, b, a;
	cairo_pattern_get_rgba(pat, &r, &g, &b, &a);
	fprintf(stderr, "colors at fill %f %f %f %f\n", r, g, b, a);
	fprintf(stderr, "Cairo fill preserve %p\n", self->cr);
	cairo_fill_preserve(self->cr);
}

void CanvaCtx_paint(CanvaCtx *self)
{
	cairo_pattern_t *pat = cairo_get_source(self->cr);
	double r, g, b, a;
	cairo_pattern_get_rgba(pat, &r, &g, &b, &a);
	fprintf(stderr, "colors at paint %f %f %f %f\n", r, g, b, a);
	fprintf(stderr, "Cairo paint %p\n", self->cr);
	cairo_paint(self->cr);
}

void CanvaCtx_rotate(CanvaCtx *self, double a)
{
	fprintf(stderr, "Cairo rotate %p %f\n", self->cr, a);
	cairo_rotate(self->cr, a);
}

void CanvaCtx_translate(CanvaCtx *self, double x, double y)
{
	fprintf(stderr, "Cairo translate %p %f:%f\n", self->cr, x, y);
	cairo_translate(self->cr, x, y);
}

void CanvaCtx_save(CanvaCtx *self)
{
	fprintf(stderr, "Cairo save %p\n", self->cr);
	cairo_save(self->cr);
}

void CanvaCtx_restore(CanvaCtx *self)
{
	fprintf(stderr, "Cairo restore %p\n", self->cr);
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
	if(self->pending_path == false)
		return;
	switch(self->default_stroke_mode)
	{
	case Fill :			if(self->color_set_up == false)
								_CanvaCtx_send_default_fill_color(self);
						CanvaCtx_fill(self);
						break;
	case Stroke :		if(self->color_set_up == false)
							_CanvaCtx_send_default_stroke_color(self);
						CanvaCtx_stroke(self);
						break;
	case FillStroke :	if(self->color_set_up == false)
							_CanvaCtx_send_default_fill_color(self);
						CanvaCtx_fill_preserve(self);
						if(self->color_set_up == false)
							_CanvaCtx_send_default_stroke_color(self);
						CanvaCtx_stroke(self);
						break;
	case StrokeFill :	if(self->color_set_up == false)
							_CanvaCtx_send_default_stroke_color(self);
						CanvaCtx_stroke_preserve(self);
						if(self->color_set_up == false)
							_CanvaCtx_send_default_fill_color(self);
						CanvaCtx_fill(self);
						break;
	case None :
			break;
	}
}

void CanvaCtx_set_line_width(CanvaCtx *self, double w)
{
	fprintf(stderr, "Cairo %p set line width %f\n", self->cr, w);
	cairo_set_line_width (self->cr, w);
}

double CanvaCtx_get_line_width(CanvaCtx *self)
{
	double w = cairo_get_line_width (self->cr);
	fprintf(stderr, "Cairo %p get line width %f\n", self->cr, w);
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

	fprintf(stderr, "Cairo %p font '%s' %d ('%s') %d ('%s')\n", self->cr, name, s, slant, w, weight);
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
	fprintf(stderr, "Cairo %p font size %f\n", self->cr, size);
	cairo_set_font_size (self->cr, size);
}

void CanvaCtx_move_to(CanvaCtx *self, double x, double y)
{
	fprintf(stderr, "Cairo %p move to %f, %f\n", self->cr, x, y);
	cairo_move_to(self->cr, x, y);
}

void CanvaCtx_scale(CanvaCtx *self, double x, double y)
{
	fprintf(stderr, "Cairo %p scale %f, %f\n", self->cr, x, y);
	cairo_scale(self->cr, x, y);
}

void CanvaCtx_draw_text(CanvaCtx *self, const char *text)
{
	fprintf(stderr, "Cairo %p draw text '%s'\n", self->cr, text);
	cairo_show_text(self->cr, text);
	self->pending_path = true;
}

void CanvaCtx_draw_text_path(CanvaCtx *self, const char *text)
{
	fprintf(stderr, "Cairo %p draw text path '%s'\n", self->cr, text);
	cairo_text_path(self->cr, text);
	self->pending_path = true;
}

void CanvaCtx_stroke(CanvaCtx *self)
{
	cairo_pattern_t *pat = cairo_get_source(self->cr);
	double r, g, b, a;
	cairo_pattern_get_rgba(pat, &r, &g, &b, &a);
	fprintf(stderr, "colors at stroke %f %f %f %f\n", r, g, b, a);
	fprintf(stderr, "Cairo %p stroke\n", self->cr);
	cairo_stroke (self->cr);
	self->pending_path = false;
	self->color_set_up = false;
}

void CanvaCtx_stroke_preserve(CanvaCtx *self)
{
	cairo_pattern_t *pat = cairo_get_source(self->cr);
	double r, g, b, a;
	cairo_pattern_get_rgba(pat, &r, &g, &b, &a);
	fprintf(stderr, "colors at stroke %f %f %f %f\n", r, g, b, a);
	fprintf(stderr, "Cairo %p stroke preserve\n", self->cr);
	cairo_stroke_preserve (self->cr);
}

#ifndef NOSDL
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
#endif

void CanvaCtx_write_to_fd(CanvaCtx *self, int fd)
{
	if(String_get_length(&self->output_buffer) > 0)
	{
		if(write(fd, (void*)String_get_data(&self->output_buffer), String_get_length(&self->output_buffer)) < 0)
		{
			fprintf(stderr, "Write Failed : %s", strerror(errno));
		}
	}
}

void CanvaCtx_write_to_png(CanvaCtx *self)
{
	cairo_surface_write_to_png_stream(self->surface, (cairo_status_t (*) (void *, const unsigned char *, unsigned int))CanvaCtx_write_into_buffer, self);
}

void CanvaCtx_export_to_xml(CanvaCtx *self, String *xml)
{
	String_append_char_string(xml, "<out><![CDATA[");

	if(self->output_mode == PNG)
	{
		base64_encodestate state;
		base64_init_encodestate(&state);

		char *encoded = malloc(String_get_length(&self->output_buffer) * 2);

		int len = base64_encode_block(
			String_get_data(&self->output_buffer),
			String_get_length(&self->output_buffer),
			encoded,
			&state
		);

		len += base64_encode_blockend(
			encoded + len,
			&state
		);

		String_append_data(xml, (size_t)len, (const void*)encoded);
		free(encoded);
	}
	else
		String_append_String(xml, &self->output_buffer);
	String_append_char_string(xml, "]]></out>");
}

void CanvaCtx_finish(CanvaCtx *self)
{
	cairo_surface_finish(self->surface);
}

cairo_status_t CanvaCtx_write_into_buffer (CanvaCtx *self,
                                              const unsigned char *data,
                                              unsigned int         length)
{
	String_append_data(&self->output_buffer, (size_t)length, (const void*)data);
	return CAIRO_STATUS_SUCCESS;
}

#ifndef NOSDL

void CanvaCtx_set_texture(CanvaCtx *self, SDL_Texture *t)
{
	self->texture = t;
}

#endif

void CanvaCtx_free(CanvaCtx *self)
{
	if(self != NULL)
	{
		CanvaCtx_terminate(self);
		free(self);
	}
}
