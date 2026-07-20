/*
 * opcs.c
 *
 *  Created on: 10 juil. 2026
 *      Author: damien
 */


#include <OpCanva.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void OpCanvaContext_init(OpCanvaContext *self)
{
	OpContext_init(&self->super);
	self->Canva = NULL;
}

void OpCanvaContext_set_Canva(OpCanvaContext *self, CanvaCtx *c)
{
	self->Canva = c;
}

void OpCanvaContext_terminate(OpCanvaContext *self)
{
	OpContext_terminate(&self->super);
}

OpIsa OpCircle_isa = {
		.name="Circle",
		.size=sizeof(OpCircle),
		.init = (void(*)(Op*))OpCircle_init,
		.terminate = (void(*)(Op*))OpCircle_terminate,
		.execute = (int(*)(Op*, OpContext*))OpCircle_execute,
		.check_args = NULL,
};

void OpCircle_init(OpCircle *self)
{
	Op_init(&self->super);
	self->x = NULL;
	self->y = NULL;
	self->r = NULL;
	self->a1 = NULL;
	self->a2 = NULL;
}

void OpCircle_terminate(OpCircle *self)
{
	Op_terminate(&self->super);
	OpCircle_set_x(self, NULL);
	OpCircle_set_y(self, NULL);
	OpCircle_set_r(self, NULL);
	OpCircle_set_a1(self, NULL);
	OpCircle_set_a2(self, NULL);
}

int OpCircle_execute(OpCircle *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double x = NAN, y = NAN, r = NAN, a1 = NAN, a2 = NAN;
	if(self->x != NULL)
	{
		ret = Op_execute(self->x, ctx);
		x = OpContext_get_current_value_double(ctx);
	}

	if(ret == 0 && self->y != NULL)
	{
		ret = Op_execute(self->y, ctx);
		y = OpContext_get_current_value_double(ctx);
	}

	if(ret == 0 && self->r != NULL)
	{
		ret = Op_execute(self->r, ctx);
		r = OpContext_get_current_value_double(ctx);
	}

	if(ret == 0 && self->a1 != NULL)
	{
		ret = Op_execute(self->a1, ctx);
		a1 = OpContext_get_current_value_double(ctx);
	}

	if(ret == 0 && self->a2 != NULL)
	{
		ret = Op_execute(self->a2, ctx);
		a2 = OpContext_get_current_value_double(ctx);
	}
	printf("Op Draw Circle %f %f %f %f %f\n", x, y, r, a1, a2);
	CanvaCtx_draw_arc(canvactx->Canva, x, y, r, a1, a2);
	return ret;
}

void OpCircle_set_x(OpCircle *self, Op *v)
{
	if(self->x != NULL)
		Op_free(self->x);
	self->x = v;
}

void OpCircle_set_y(OpCircle *self, Op *v)
{
	if(self->y != NULL)
		Op_free(self->y);
	self->y = v;
}

void OpCircle_set_r(OpCircle *self, Op *v)
{
	if(self->r != NULL)
		Op_free(self->r);
	self->r = v;
}

void OpCircle_set_a1(OpCircle *self, Op *v)
{
	if(self->a1 != NULL)
		Op_free(self->a1);
	self->a1 = v;
}

void OpCircle_set_a2(OpCircle *self, Op *v)
{
	if(self->a2 != NULL)
		Op_free(self->a2);
	self->a2 = v;
}

Op *OpCircle_new(void)
{
	return Op_new(&OpCircle_isa);
}



OpIsa OpRectangle_isa = {
		.name="Rectangle",
		.size=sizeof(OpRectangle),
		.init = (void(*)(Op*))OpRectangle_init,
		.terminate = (void(*)(Op*))OpRectangle_terminate,
		.execute = (int(*)(Op*, OpContext*))OpRectangle_execute,
		.check_args = NULL,
};

void OpRectangle_init(OpRectangle *self)
{
	Op_init(&self->super);
	self->x = NULL;
	self->y = NULL;
	self->w = NULL;
	self->h = NULL;
}

void OpRectangle_terminate(OpRectangle *self)
{
	Op_terminate(&self->super);
	OpRectangle_set_x(self, NULL);
	OpRectangle_set_y(self, NULL);
	OpRectangle_set_w(self, NULL);
	OpRectangle_set_h(self, NULL);
}

int OpRectangle_execute(OpRectangle *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double x = NAN, y = NAN, w = NAN, h = NAN;
	if(self->x != NULL)
	{
		ret = Op_execute(self->x, ctx);
		x = OpContext_get_current_value_double(ctx);
	}

	if(ret == 0 && self->y != NULL)
	{
		ret = Op_execute(self->y, ctx);
		y = OpContext_get_current_value_double(ctx);
	}

	if(ret == 0 && self->w != NULL)
	{
		ret = Op_execute(self->w, ctx);
		w = OpContext_get_current_value_double(ctx);
	}

	if(ret == 0 && self->h != NULL)
	{
		ret = Op_execute(self->h, ctx);
		h = OpContext_get_current_value_double(ctx);
	}
	printf("Op Draw Rectangle %f %f %f %f\n", x, y, w, h);
	CanvaCtx_draw_rectangle(canvactx->Canva, x, y, w, h);
	return ret;
}

void OpRectangle_set_x(OpRectangle *self, Op *v)
{
	if(self->x != NULL)
		Op_free(self->x);
	self->x = v;
}

void OpRectangle_set_y(OpRectangle *self, Op *v)
{
	if(self->y != NULL)
		Op_free(self->y);
	self->y = v;
}

void OpRectangle_set_w(OpRectangle *self, Op *v)
{
	if(self->w != NULL)
		Op_free(self->w);
	self->w = v;
}

void OpRectangle_set_h(OpRectangle *self, Op *v)
{
	if(self->h != NULL)
		Op_free(self->h);
	self->h = v;
}

Op *OpRectangle_new(void)
{
	return Op_new(&OpRectangle_isa);
}



OpIsa OpColor_isa = {
		.name="Color",
		.size=sizeof(OpColor),
		.init = (void(*)(Op*))OpColor_init,
		.terminate = (void(*)(Op*))OpColor_terminate,
		.execute = (int(*)(Op*, OpContext*))OpColor_execute,
		.check_args = NULL,
};

void OpColor_init(OpColor *self)
{
	Op_init(&self->super);
	self->red = NULL;
	self->blue = NULL;
	self->green = NULL;
	self->alpha = NULL;
}

void OpColor_terminate(OpColor *self)
{
	Op_terminate(&self->super);
	OpColor_set_red(self, NULL);
	OpColor_set_green(self, NULL);
	OpColor_set_blue(self, NULL);
	OpColor_set_alpha(self, NULL);
}

int OpColor_execute(OpColor *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double red = NAN, green = NAN, blue = NAN, alpha = NAN;
	if(self->red != NULL)
	{
		ret = Op_execute(self->red, ctx);
		red = OpContext_get_current_value_double(ctx);
	}

	if(ret == 0 && self->green != NULL)
	{
		ret = Op_execute(self->green, ctx);
		green = OpContext_get_current_value_double(ctx);
	}

	if(ret == 0 && self->blue != NULL)
	{
		ret = Op_execute(self->blue, ctx);
		blue = OpContext_get_current_value_double(ctx);
	}

	if(ret == 0 && self->alpha != NULL)
	{
		ret = Op_execute(self->alpha, ctx);
		alpha = OpContext_get_current_value_double(ctx);
	}
	printf("Op Set Color %f %f %f %f\n", red, green, blue, alpha);
	CanvaCtx_set_color(canvactx->Canva, red, green, blue, alpha);
	return ret;
}

void OpColor_set_red(OpColor *self, Op *r)
{
	if(self->red != NULL)
		Op_free(self->red);
	self->red = r;
}

void OpColor_set_green(OpColor *self, Op *g)
{
	if(self->green != NULL)
		Op_free(self->green);
	self->green = g;
}


void OpColor_set_blue(OpColor *self, Op *b)
{
	if(self->blue != NULL)
		Op_free(self->blue);
	self->blue = b;
}


void OpColor_set_alpha(OpColor *self, Op *a)
{
	if(self->alpha != NULL)
		Op_free(self->alpha);
	self->alpha = a;
}

Op *OpColor_new(void)
{
	return Op_new(&OpColor_isa);
}



OpIsa OpStroke_isa = {
		.name="Stroke",
		.size=sizeof(OpStroke),
		.init = (void(*)(Op*))OpStroke_init,
		.terminate = (void(*)(Op*))OpStroke_terminate,
		.execute = (int(*)(Op*, OpContext*))OpStroke_execute,
		.check_args = NULL,
};

void OpStroke_init(OpStroke *self)
{
	Op_init(&self->super);
}

void OpStroke_terminate(OpStroke *self)
{
	Op_terminate(&self->super);
}

int OpStroke_execute(OpStroke *self, OpCanvaContext *ctx)
{
	CanvaCtx_stroke(ctx->Canva);
	return 0;
}

Op *OpStroke_new(void)
{
	return Op_new(&OpStroke_isa);
}

OpIsa OpStrokePreserve_isa = {
		.name="StrokePreserve",
		.size=sizeof(OpStrokePreserve),
		.init = (void(*)(Op*))OpStrokePreserve_init,
		.terminate = (void(*)(Op*))OpStrokePreserve_terminate,
		.execute = (int(*)(Op*, OpContext*))OpStrokePreserve_execute,
		.check_args = NULL,
};

void OpStrokePreserve_init(OpStrokePreserve *self)
{
	Op_init(&self->super);
}

void OpStrokePreserve_terminate(OpStrokePreserve *self)
{
	Op_terminate(&self->super);
}

int OpStrokePreserve_execute(OpStrokePreserve *self, OpCanvaContext *ctx)
{
	CanvaCtx_stroke_preserve(ctx->Canva);
	return 0;
}

Op *OpStrokePreserve_new(void)
{
	return Op_new(&OpStrokePreserve_isa);
}


OpIsa OpSetLineWidth_isa = {
		.name="SetLineWidth",
		.size=sizeof(OpSetLineWidth),
		.init = (void(*)(Op*))OpSetLineWidth_init,
		.terminate = (void(*)(Op*))OpSetLineWidth_terminate,
		.execute = (int(*)(Op*, OpContext*))OpSetLineWidth_execute,
		.check_args = NULL,
};

void OpSetLineWidth_init(OpSetLineWidth *self)
{
	Op_init(&self->super);
	self->width = NULL;
}

void OpSetLineWidth_terminate(OpSetLineWidth *self)
{
	Op_terminate(&self->super);
	OpSetLineWidth_set_width(self, NULL);
}

void OpSetLineWidth_set_width(OpSetLineWidth *self, Op *v)
{
	if(self->width != NULL)
		Op_free(self->width);
	self->width = v;
}

int OpSetLineWidth_execute(OpSetLineWidth *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double w = NAN;
	if(self->width != NULL)
	{
		ret = Op_execute(self->width, ctx);
		w = OpContext_get_current_value_double(ctx);
	}

	if(ret == 0)
	{
		printf("Op Set Line Width %f\n", w);
		CanvaCtx_set_line_width(canvactx->Canva, w);
	}
	return ret;
}

Op *OpSetLineWidth_new(void)
{
	return Op_new(&OpSetLineWidth_isa);
}


OpIsa OpRotate_isa = {
		.name="Rotate",
		.size=sizeof(OpRotate),
		.init = (void(*)(Op*))OpRotate_init,
		.terminate = (void(*)(Op*))OpRotate_terminate,
		.execute = (int(*)(Op*, OpContext*))OpRotate_execute,
		.check_args = NULL,
};

void OpRotate_init(OpRotate *self)
{
	Op_init(&self->super);
	self->angle = NULL;
}

void OpRotate_terminate(OpRotate *self)
{
	Op_terminate(&self->super);
	OpRotate_set_angle(self, NULL);
}

void OpRotate_set_angle(OpRotate *self, Op *v)
{
	if(self->angle != NULL)
		Op_free(self->angle);
	self->angle = v;
}

int OpRotate_execute(OpRotate *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double a = NAN;
	if(self->angle != NULL)
	{
		ret = Op_execute(self->angle, ctx);
		a = OpContext_get_current_value_double(ctx);
	}

	if(ret == 0)
	{
		printf("Op Rotate %f\n", a);
		CanvaCtx_rotate(canvactx->Canva, a);
	}
	return ret;
}

Op *OpRotate_new(void)
{
	return Op_new(&OpRotate_isa);
}


OpIsa OpTranslate_isa = {
		.name="Translate",
		.size=sizeof(OpTranslate),
		.init = (void(*)(Op*))OpTranslate_init,
		.terminate = (void(*)(Op*))OpTranslate_terminate,
		.execute = (int(*)(Op*, OpContext*))OpTranslate_execute,
		.check_args = NULL,
};

void OpTranslate_init(OpTranslate *self)
{
	Op_init(&self->super);
	self->x = NULL;
	self->y= NULL;
}

void OpTranslate_terminate(OpTranslate *self)
{
	Op_terminate(&self->super);
	OpTranslate_set_x(self, NULL);
	OpTranslate_set_y(self, NULL);
}

void OpTranslate_set_x(OpTranslate *self, Op *v)
{
	if(self->x != NULL)
		Op_free(self->x);
	self->x = v;
}

void OpTranslate_set_y(OpTranslate *self, Op *v)
{
	if(self->y != NULL)
		Op_free(self->y);
	self->y = v;
}

int OpTranslate_execute(OpTranslate *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double x = NAN, y = NAN;
	if(self->x != NULL)
	{
		ret = Op_execute(self->x, ctx);
		x = OpContext_get_current_value_double(ctx);
	}
	if(ret == 0 && self->y != NULL)
	{
		ret = Op_execute(self->y, ctx);
		y = OpContext_get_current_value_double(ctx);
	}

	if(ret == 0)
	{
		printf("Op Translate %f %f\n", x, y);
		CanvaCtx_translate(canvactx->Canva, x, y);
	}
	return ret;
}

Op *OpTranslate_new(void)
{
	return Op_new(&OpTranslate_isa);
}


OpIsa OpMoveTo_isa = {
		.name="MoveTo",
		.size=sizeof(OpMoveTo),
		.init = (void(*)(Op*))OpMoveTo_init,
		.terminate = (void(*)(Op*))OpMoveTo_terminate,
		.execute = (int(*)(Op*, OpContext*))OpMoveTo_execute,
		.check_args = NULL,
};

void OpMoveTo_init(OpMoveTo *self)
{
	Op_init(&self->super);
	self->x = NULL;
	self->y= NULL;
}

void OpMoveTo_terminate(OpMoveTo *self)
{
	Op_terminate(&self->super);
	OpMoveTo_set_x(self, NULL);
	OpMoveTo_set_y(self, NULL);
}

void OpMoveTo_set_x(OpMoveTo *self, Op *v)
{
	if(self->x != NULL)
		Op_free(self->x);
	self->x = v;
}

void OpMoveTo_set_y(OpMoveTo *self, Op *v)
{
	if(self->y != NULL)
		Op_free(self->y);
	self->y = v;
}

int OpMoveTo_execute(OpMoveTo *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double x = NAN, y = NAN;
	if(self->x != NULL)
	{
		ret = Op_execute(self->x, ctx);
		x = OpContext_get_current_value_double(ctx);
	}
	if(ret == 0 && self->y != NULL)
	{
		ret = Op_execute(self->y, ctx);
		y = OpContext_get_current_value_double(ctx);
	}

	if(ret == 0)
	{
		printf("Op Move To %f %f\n", x, y);
		CanvaCtx_move_to(canvactx->Canva, x, y);
	}
	return ret;
}

Op *OpMoveTo_new(void)
{
	return Op_new(&OpMoveTo_isa);
}


OpIsa OpFill_isa = {
		.name="Fill",
		.size=sizeof(OpFill),
		.init = (void(*)(Op*))OpFill_init,
		.terminate = (void(*)(Op*))OpFill_terminate,
		.execute = (int(*)(Op*, OpContext*))OpFill_execute,
		.check_args = NULL,
};

void OpFill_init(OpFill *self)
{
	Op_init(&self->super);
}

void OpFill_terminate(OpFill *self)
{
	Op_terminate(&self->super);
}

int OpFill_execute(OpFill *self, OpCanvaContext *ctx)
{
	CanvaCtx_fill(ctx->Canva);
	return 0;
}

Op *OpFill_new(void)
{
	return Op_new(&OpFill_isa);
}


OpIsa OpFillPreserve_isa = {
		.name="FillPreserve",
		.size=sizeof(OpFillPreserve),
		.init = (void(*)(Op*))OpFillPreserve_init,
		.terminate = (void(*)(Op*))OpFillPreserve_terminate,
		.execute = (int(*)(Op*, OpContext*))OpFillPreserve_execute
};

void OpFillPreserve_init(OpFillPreserve *self)
{
	Op_init(&self->super);
}

void OpFillPreserve_terminate(OpFillPreserve *self)
{
	Op_terminate(&self->super);
}

int OpFillPreserve_execute(OpFillPreserve *self, OpCanvaContext *ctx)
{
	CanvaCtx_fill_preserve(ctx->Canva);
	return 0;
}

Op *OpFillPreserve_new(void)
{
	return Op_new(&OpFillPreserve_isa);
}


OpIsa OpCanvaBloc_isa = {
		.name="CanvaBloc",
		.size=sizeof(OpCanvaBloc),
		.init = (void(*)(Op*))OpCanvaBloc_init,
		.terminate = (void(*)(Op*))OpCanvaBloc_terminate,
		.execute = (int(*)(Op*, OpContext*))OpCanvaBloc_execute,
		.check_args = NULL,
};

void OpCanvaBloc_init(OpCanvaBloc *self)
{
	OpBloc_init(&self->super);
	self->auto_stroke = false;
}

void OpCanvaBloc_terminate(OpCanvaBloc *self)
{
	OpBloc_terminate(&self->super);
}

void OpCanvaBloc_set_auto_stroke(OpCanvaBloc *self)
{
	self->auto_stroke = true;
}

int OpCanvaBloc_execute(OpCanvaBloc *self, OpCanvaContext *ctx)
{
	int ret = 0;
	printf("Canva %p Save\n", ctx->Canva);
	CanvaCtx_save(ctx->Canva);
	ret = OpBloc_execute(&self->super, (OpContext*)ctx);
	if(ret == 0)
	{
		if(self->auto_stroke == true)
			CanvaCtx_auto_stroke(ctx->Canva);
	}
	printf("Canva %p Restore\n", ctx->Canva);
	CanvaCtx_restore(ctx->Canva);
	return ret;
}

Op *OpCanvaBloc_new(void)
{
	return Op_new(&OpCanvaBloc_isa);
}


OpIsa OpFontSelector_isa = {
		.name="FontSelector",
		.size=sizeof(OpFontSelector),
		.init = (void(*)(Op*))OpFontSelector_init,
		.terminate = (void(*)(Op*))OpFontSelector_terminate,
		.execute = (int(*)(Op*, OpContext*))OpFontSelector_execute,
		.check_args = NULL,
};

void OpFontSelector_init(OpFontSelector *self)
{
	Op_init(&self->super);
	self->font = NULL;
	self->slant = NULL;
	self->weight = NULL;
}

void OpFontSelector_terminate(OpFontSelector *self)
{
	Op_terminate(&self->super);
	OpFontSelector_set_font_name(self, NULL);
	OpFontSelector_set_font_slant(self, NULL);
	OpFontSelector_set_font_weight(self, NULL);
}

void OpFontSelector_set_font_name(OpFontSelector *self, const char *name)
{
	if(self->font != NULL)
		free(self->font);
	if(name != NULL)
		self->font = strdup(name);
	else
		self->font = NULL;
}

void OpFontSelector_set_font_slant(OpFontSelector *self, const char *slant)
{
	if(self->slant != NULL)
		free(self->slant);
	if(slant!= NULL)
		self->slant= strdup(slant);
	else
		self->slant = NULL;
}

void OpFontSelector_set_font_weight(OpFontSelector *self, const char *weight)
{
	if(self->weight != NULL)
		free(self->weight);
	if(weight != NULL)
		self->weight = strdup(weight);
	else
		self->weight = NULL;
}

int OpFontSelector_execute(OpFontSelector *self, OpCanvaContext *ctx)
{
	CanvaCtx_set_font(ctx->Canva, self->font, self->slant, self->weight);
	return 0;
}

Op *OpFontSelector_new(void)
{
	return Op_new(&OpFontSelector_isa);
}


OpIsa OpDrawText_isa = {
		.name="DrawText",
		.size=sizeof(OpDrawText),
		.init = (void(*)(Op*))OpDrawText_init,
		.terminate = (void(*)(Op*))OpDrawText_terminate,
		.execute = (int(*)(Op*, OpContext*))OpDrawText_execute,
		.check_args = NULL,
};

void OpDrawText_init(OpDrawText *self)
{
	Op_init(&self->super);
	self->text = NULL;
	self->mode = show;
}

void OpDrawText_terminate(OpDrawText *self)
{
	Op_terminate(&self->super);
	OpDrawText_set_text(self, NULL);
}

void OpDrawText_set_text(OpDrawText *self, const char *t)
{
	if(self->text != NULL)
		free(self->text);
	if(t != NULL)
		self->text = strdup(t);
	else
		self->text = NULL;
}

void OpDrawText_set_text_mode(OpDrawText *self, TextMode m)
{
	self->mode = m;
}

int OpDrawText_execute(OpDrawText *self, OpCanvaContext *ctx)
{
	switch(self->mode)
	{
	case show : CanvaCtx_draw_text(ctx->Canva, self->text);
				break;
	case path : CanvaCtx_draw_text_path(ctx->Canva, self->text);
				break;
	}
	return 0;
}

Op *OpDrawText_new(void)
{
	return Op_new(&OpDrawText_isa);
}


OpIsa OpSetFontSize_isa = {
		.name="SetFontSize",
		.size=sizeof(OpSetFontSize),
		.init = (void(*)(Op*))OpSetFontSize_init,
		.terminate = (void(*)(Op*))OpSetFontSize_terminate,
		.execute = (int(*)(Op*, OpContext*))OpSetFontSize_execute,
		.check_args = NULL,
};

void OpSetFontSize_init(OpSetFontSize *self)
{
	Op_init(&self->super);
	self->size = NULL;
}

void OpSetFontSize_terminate(OpSetFontSize *self)
{
	Op_terminate(&self->super);
	OpSetFontSize_set_size(self, NULL);
}

void OpSetFontSize_set_size(OpSetFontSize *self, Op *s)
{
	if(self->size != NULL)
		Op_free(self->size);
	self->size = s;
}

int OpSetFontSize_execute(OpSetFontSize *self, OpCanvaContext *ctx)
{
	int ret = 0;
	double s = NAN;
	if(self->size != NULL)
	{
		ret = Op_execute(self->size, (OpContext*)ctx);
		s = OpContext_get_current_value_double((OpContext*)ctx);
		printf("Op Set Font Size %f\n", s);
	}
	if(ret == 0)
		CanvaCtx_set_font_size(ctx->Canva, s);
	return ret;
}

Op *OpSetFontSize_new(void)
{
	return Op_new(&OpSetFontSize_isa);
}
