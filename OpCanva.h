/*
 * ops.h
 *
 *  Created on: 10 juil. 2026
 *      Author: damien
 */

#ifndef OPCANVA_H_
#define OPCANVA_H_

#include <LinkedList.h>
#include <stddef.h>
#include <CanvaCtx.h>
#include <Op.h>
#include <stdbool.h>

typedef struct op_canva_context
{
	OpContext super;
	CanvaCtx *Canva;
	int width;
	int height;
	CanvaCtxOutputMode output_mode;
}OpCanvaContext;


typedef struct op_circle
{
	Op super;
	bool negative_arc;
	Op *x;
	Op *y;
	Op *r;
	Op *a1;
	Op *a2;
	Op *params;

}OpCircle;

typedef struct op_curve
{
	Op super;
	Op *x;
	Op *y;
	Op *m1x;
	Op *m1y;
	Op *m2x;
	Op *m2y;
	Op *params;

}OpCurve;


typedef struct op_rectangle
{
	Op super;
	Op *x;
	Op *y;
	Op *w;
	Op *h;
	Op *params;

}OpRectangle;


typedef struct op_color
{
	Op super;
	Op *red;
	Op *blue;
	Op *green;
	Op *alpha;
	Op *params;

}OpColor;


typedef struct op_set_background_color
{
	Op super;
	Op *red;
	Op *blue;
	Op *green;
	Op *alpha;
	Op *params;

}OpSetBGColor;

typedef struct op_set_line_width
{
	Op super;
	Op *width;
}OpSetLineWidth;

typedef struct op_get_line_width
{
	Op super;
}OpGetLineWidth;

typedef struct op_set_out_size
{
	Op super;
	Op *width;
	Op *height;
}OpSetOutputSize;

typedef struct op_rotate
{
	Op super;
	Op *angle;
}OpRotate;

typedef struct op_translate
{
	Op super;
	Op *x;
	Op *y;
	Op *params;
}OpTranslate;

typedef struct op_move_to
{
	Op super;
	Op *x;
	Op *y;
	Op *params;
}OpMoveTo;

typedef struct op_scale
{
	Op super;
	Op *x;
	Op *y;
	Op *params;
}OpScale;

typedef struct op_draw_line_to
{
	Op super;
	Op *x;
	Op *y;
	Op *params;
}OpDrawLineTo;

typedef struct op_canva_bloc
{
	OpBloc super;
	bool auto_stroke;
}OpCanvaBloc;



typedef struct op_stroke
{
	Op super;
}OpStroke;

typedef struct op_fill
{
	Op super;
}OpFill;

typedef struct op_stroke_preserve
{
	Op super;
}OpStrokePreserve;

typedef struct op_fill_preserve
{
	Op super;
}OpFillPreserve;


typedef struct op_font_selector
{
	Op super;
	char *font;
	char *slant;
	char *weight;
}OpFontSelector;

typedef struct op_set_font_size
{
	Op super;
	Op *size;
}OpSetFontSize;


typedef enum text_mode
{
	show,
	path
}TextMode;

typedef struct op_draw_text
{
	Op super;
	Op *text;
	TextMode mode;
}OpDrawText;

typedef struct op_get_text_extent
{
	Op super;
	Op *text;
}OpGetTextExtents;

typedef struct op_get_font_extent
{
	Op super;
}OpGetFontExtents;

typedef struct op_get_red_color
{
	Op super;
}OpGetRedColor;

typedef struct op_get_green_color
{
	Op super;
}OpGetGreenColor;

typedef struct op_get_blue_color
{
	Op super;
}OpGetBlueColor;

typedef struct op_get_black_color
{
	Op super;
}OpGetBlackColor;

typedef struct op_get_white_color
{
	Op super;
}OpGetWhiteColor;


typedef struct op_get_output_size
{
	Op super;
}OpGetOutputSize;

typedef struct op_set_output_png
{
	Op super;
	char *filename;
}OpSetOutputPNG;

typedef struct op_set_output_svg
{
	Op super;
	char *filename;
}OpSetOutputSVG;


#include <OpCanva.proto.h>

#endif /* OPCANVA_H_ */
