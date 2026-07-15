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
		.execute = (void(*)(Op*, OpContext*))OpCircle_execute
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

void OpCircle_execute(OpCircle *self, OpCanvaContext *canvactx)
{
	OpContext *ctx = (OpContext*)canvactx;
	double x = NAN, y = NAN, r = NAN, a1 = NAN, a2 = NAN;
	if(self->x != NULL)
	{
		Op_execute(self->x, ctx);
		x = OpContext_get_current_value(ctx);
	}

	if(self->y != NULL)
	{
		Op_execute(self->y, ctx);
		y = OpContext_get_current_value(ctx);
	}

	if(self->r != NULL)
	{
		Op_execute(self->r, ctx);
		r = OpContext_get_current_value(ctx);
	}

	if(self->a1 != NULL)
	{
		Op_execute(self->a1, ctx);
		a1 = OpContext_get_current_value(ctx);
	}

	if(self->a2 != NULL)
	{
		Op_execute(self->a2, ctx);
		a2 = OpContext_get_current_value(ctx);
	}
	printf("Op Draw Circle %f %f %f %f %f\n", x, y, r, a1, a2);
	CanvaCtx_draw_arc(canvactx->Canva, x, y, r, a1, a2);
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
		.execute = (void(*)(Op*, OpContext*))OpRectangle_execute
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

void OpRectangle_execute(OpRectangle *self, OpCanvaContext *canvactx)
{
	OpContext *ctx = (OpContext*)canvactx;
	double x = NAN, y = NAN, w = NAN, h = NAN;
	if(self->x != NULL)
	{
		Op_execute(self->x, ctx);
		x = OpContext_get_current_value(ctx);
	}

	if(self->y != NULL)
	{
		Op_execute(self->y, ctx);
		y = OpContext_get_current_value(ctx);
	}

	if(self->w != NULL)
	{
		Op_execute(self->w, ctx);
		w = OpContext_get_current_value(ctx);
	}

	if(self->h != NULL)
	{
		Op_execute(self->h, ctx);
		h = OpContext_get_current_value(ctx);
	}
	printf("Op Draw Rectangle %f %f %f %f\n", x, y, w, h);
	CanvaCtx_draw_rectangle(canvactx->Canva, x, y, w, h);
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
		.execute = (void(*)(Op*, OpContext*))OpColor_execute
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

void OpColor_execute(OpColor *self, OpCanvaContext *canvactx)
{
	OpContext *ctx = (OpContext*)canvactx;
	double red = NAN, green = NAN, blue = NAN, alpha = NAN;
	if(self->red != NULL)
	{
		Op_execute(self->red, ctx);
		red = OpContext_get_current_value(ctx);
	}

	if(self->green != NULL)
	{
		Op_execute(self->green, ctx);
		green = OpContext_get_current_value(ctx);
	}

	if(self->blue != NULL)
	{
		Op_execute(self->blue, ctx);
		blue = OpContext_get_current_value(ctx);
	}

	if(self->alpha != NULL)
	{
		Op_execute(self->alpha, ctx);
		alpha = OpContext_get_current_value(ctx);
	}
	printf("Op Set Color %f %f %f %f\n", red, green, blue, alpha);
	CanvaCtx_set_color(canvactx->Canva, red, green, blue, alpha);
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
		.execute = (void(*)(Op*, OpContext*))OpStroke_execute
};

void OpStroke_init(OpStroke *self)
{
	Op_init(&self->super);
}

void OpStroke_terminate(OpStroke *self)
{
	Op_terminate(&self->super);
}

void OpStroke_execute(OpStroke *self, OpCanvaContext *ctx)
{
	CanvaCtx_stroke(ctx->Canva);
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
		.execute = (void(*)(Op*, OpContext*))OpStrokePreserve_execute
};

void OpStrokePreserve_init(OpStrokePreserve *self)
{
	Op_init(&self->super);
}

void OpStrokePreserve_terminate(OpStrokePreserve *self)
{
	Op_terminate(&self->super);
}

void OpStrokePreserve_execute(OpStrokePreserve *self, OpCanvaContext *ctx)
{
	CanvaCtx_stroke_preserve(ctx->Canva);
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
		.execute = (void(*)(Op*, OpContext*))OpSetLineWidth_execute
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

void OpSetLineWidth_execute(OpSetLineWidth *self, OpCanvaContext *canvactx)
{
	OpContext *ctx = (OpContext*)canvactx;
	double w = NAN;
	if(self->width != NULL)
	{
		Op_execute(self->width, ctx);
		w = OpContext_get_current_value(ctx);
	}

	printf("Op Set Line Width %f\n", w);
	CanvaCtx_set_line_width(canvactx->Canva, w);
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
		.execute = (void(*)(Op*, OpContext*))OpRotate_execute
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

void OpRotate_execute(OpRotate *self, OpCanvaContext *canvactx)
{
	OpContext *ctx = (OpContext*)canvactx;
	double a = NAN;
	if(self->angle != NULL)
	{
		Op_execute(self->angle, ctx);
		a = OpContext_get_current_value(ctx);
	}

	printf("Op Rotate %f\n", a);
	CanvaCtx_rotate(canvactx->Canva, a);
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
		.execute = (void(*)(Op*, OpContext*))OpTranslate_execute
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

void OpTranslate_execute(OpTranslate *self, OpCanvaContext *canvactx)
{
	OpContext *ctx = (OpContext*)canvactx;
	double x = NAN, y = NAN;
	if(self->x != NULL)
	{
		Op_execute(self->x, ctx);
		x = OpContext_get_current_value(ctx);
	}
	if(self->y != NULL)
	{
		Op_execute(self->y, ctx);
		y = OpContext_get_current_value(ctx);
	}

	printf("Op Translate %f %f\n", x, y);
	CanvaCtx_translate(canvactx->Canva, x, y);
}

Op *OpTranslate_new(void)
{
	return Op_new(&OpTranslate_isa);
}


OpIsa OpFill_isa = {
		.name="Fill",
		.size=sizeof(OpFill),
		.init = (void(*)(Op*))OpFill_init,
		.terminate = (void(*)(Op*))OpFill_terminate,
		.execute = (void(*)(Op*, OpContext*))OpFill_execute
};

void OpFill_init(OpFill *self)
{
	Op_init(&self->super);
}

void OpFill_terminate(OpFill *self)
{
	Op_terminate(&self->super);
}

void OpFill_execute(OpFill *self, OpCanvaContext *ctx)
{
	CanvaCtx_fill(ctx->Canva);
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
		.execute = (void(*)(Op*, OpContext*))OpFillPreserve_execute
};

void OpFillPreserve_init(OpFillPreserve *self)
{
	Op_init(&self->super);
}

void OpFillPreserve_terminate(OpFillPreserve *self)
{
	Op_terminate(&self->super);
}

void OpFillPreserve_execute(OpFillPreserve *self, OpCanvaContext *ctx)
{
	CanvaCtx_fill_preserve(ctx->Canva);
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
		.execute = (void(*)(Op*, OpContext*))OpCanvaBloc_execute
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

void OpCanvaBloc_execute(OpCanvaBloc *self, OpCanvaContext *ctx)
{
	printf("Canva %p Save\n", ctx->Canva);
	CanvaCtx_save(ctx->Canva);
	OpBloc_execute(&self->super, (OpContext*)ctx);
	if(self->auto_stroke == true)
		CanvaCtx_auto_stroke(ctx->Canva);
	printf("Canva %p Restore\n", ctx->Canva);
	CanvaCtx_restore(ctx->Canva);
}

Op *OpCanvaBloc_new(void)
{
	return Op_new(&OpCanvaBloc_isa);
}
