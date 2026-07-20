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
}OpCanvaContext;


typedef struct op_circle
{
	Op super;
	Op *x;
	Op *y;
	Op *r;
	Op *a1;
	Op *a2;

}OpCircle;


typedef struct op_rectangle
{
	Op super;
	Op *x;
	Op *y;
	Op *w;
	Op *h;

}OpRectangle;


typedef struct op_color
{
	Op super;
	Op *red;
	Op *blue;
	Op *green;
	Op *alpha;

}OpColor;

typedef struct op_set_line_width
{
	Op super;
	Op *width;
}OpSetLineWidth;

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
}OpTranslate;

typedef struct op_move_to
{
	Op super;
	Op *x;
	Op *y;
}OpMoveTo;

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
	char *text;
	TextMode mode;
}OpDrawText;


#include <OpCanva.proto.h>

#endif /* OPCANVA_H_ */
