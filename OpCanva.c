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
#include <string.h>

void OpCanvaContext_init(OpCanvaContext *self)
{
	OpContext_init(&self->super);
	self->Canva = NULL;
	self->width = 0;
	self->height = 0;
#ifndef NOSDL
	self->output_mode = SDL;
#else
	self->output_mode = SVG;
#endif
}

void OpCanvaContext_set_Canva(OpCanvaContext *self, CanvaCtx *c)
{
	self->Canva = c;
}

void OpCanvaContext_set_width(OpCanvaContext *self, int width)
{
	self->width = width;
}

int OpCanvaContext_get_width(OpCanvaContext *self)
{
	return self->width;
}

int OpCanvaContext_get_height(OpCanvaContext *self)
{
	return self->height;
}

void OpCanvaContext_set_height(OpCanvaContext *self, int height)
{
	self->height = height;
}

void OpCanvaContext_set_output_png(OpCanvaContext *self)
{
	self->output_mode = PNG;
}

void OpCanvaContext_set_output_svg(OpCanvaContext *self)
{
	self->output_mode = SVG;
}

CanvaCtxOutputMode OpCanvaContext_get_output_mode(OpCanvaContext *self)
{
	return self->output_mode;
}

void OpCanvaContext_export_messages_to_xml(OpCanvaContext *self, String *xml, bool with_output)
{
	String_append_char_string(xml, "<xml>");
	if(with_output && self->Canva != NULL)
		CanvaCtx_export_to_xml(self->Canva, xml);
	OpContext_export_messages_to_xml(&self->super, xml);
	String_append_char_string(xml, "</xml>");
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
		.fix_operandes = (int(*)(Op*, OpContext*))OpCircle_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpCircle_execute,
		.check_args = (int(*)(Op*, OpContext*))OpCircle_check_args,
};

void OpCircle_init(OpCircle *self)
{
	Op_init(&self->super);
	self->negative_arc = false;
	self->x = NULL;
	self->y = NULL;
	self->r = NULL;
	self->a1 = NULL;
	self->a2 = NULL;
	self->params = NULL;
}

void OpCircle_terminate(OpCircle *self)
{
	Op_terminate(&self->super);
	_OpCircle_set_x(self, NULL);
	_OpCircle_set_y(self, NULL);
	_OpCircle_set_r(self, NULL);
	_OpCircle_set_a1(self, NULL);
	_OpCircle_set_a2(self, NULL);
	_OpCircle_set_params(self, NULL);
}

void OpCircle_set_negative(OpCircle *self)
{
	self->negative_arc = true;
}

int OpCircle_check_args(OpCircle *self, OpCanvaContext *canvactx)
{
	if(self->params != NULL || (self->x != NULL && self->y != NULL && self->r != NULL && self->a1 != NULL && self->a2 != NULL))
		return 0;
	return -1;
}

int OpCircle_execute(OpCircle *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double x = NAN, y = NAN, r = NAN, a1 = NAN, a2 = NAN;
	if(self->params != NULL)
	{
		double *d;
		size_t size;
		ret = Op_execute_get_doubles(self->params, (Op*)self, ctx, &d, &size, 5);
		if(ret == 0)
		{
			x=d[0];
			y=d[1];
			r=d[2];
			a1=d[3];
			a2=d[4];
		}
	}
	else if(self->x != NULL)
	{
		ret = Op_execute_get_double(self->x, (Op*)self, ctx, &x);
		if(ret == 0)
			ret = Op_execute_get_double(self->y, (Op*)self, ctx, &y);
		if(ret == 0)
			ret = Op_execute_get_double(self->r, (Op*)self, ctx, &r);
		if(ret == 0)
			ret = Op_execute_get_double(self->a1, (Op*)self, ctx, &a1);
		if(ret == 0)
			ret = Op_execute_get_double(self->a2, (Op*)self, ctx, &a2);
	}

	if(ret == 0)
	{
		if(self->negative_arc == false)
		{
			fprintf(stderr, "Op Draw Circle %f %f %f %f %f\n", x, y, r, a1, a2);
			CanvaCtx_draw_arc(canvactx->Canva, x, y, r, a1, a2);
		}
		else
		{
			fprintf(stderr, "Op Draw Negative Circle %f %f %f %f %f\n", x, y, r, a1, a2);
			CanvaCtx_draw_arc_negative(canvactx->Canva, x, y, r, a1, a2);
		}
	}
	return ret;
}

void _OpCircle_set_x(OpCircle *self, Op *v)
{
	OP_SET_OPERANDE(self, x, v);
}

void _OpCircle_set_y(OpCircle *self, Op *v)
{
	OP_SET_OPERANDE(self, y, v);
}

void _OpCircle_set_r(OpCircle *self, Op *v)
{
	OP_SET_OPERANDE(self, r, v);
}

void _OpCircle_set_a1(OpCircle *self, Op *v)
{
	OP_SET_OPERANDE(self, a1, v);
}

void _OpCircle_set_a2(OpCircle *self, Op *v)
{
	OP_SET_OPERANDE(self, a2, v);
}

void _OpCircle_set_params(OpCircle *self, Op *v)
{
	OP_SET_OPERANDE(self, params, v);
}

#define OPCIRCLE_X 0
#define OPCIRCLE_Y 1
#define OPCIRCLE_R 2
#define OPCIRCLE_A1 3
#define OPCIRCLE_A2 4
#define OPCIRCLE_PARAMS 5

void OpCircle_set_x(OpCircle *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPCIRCLE_X);
}

void OpCircle_set_y(OpCircle *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPCIRCLE_Y);
}

void OpCircle_set_r(OpCircle *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPCIRCLE_R);
}

void OpCircle_set_a1(OpCircle *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPCIRCLE_A1);
}

void OpCircle_set_a2(OpCircle *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPCIRCLE_A2);
}

void OpCircle_set_params(OpCircle *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPCIRCLE_PARAMS);
}

int OpCircle_fix_operandes(OpCircle *self, OpContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >= 5)
	{
		Op *x, *y, *r, *a1, *a2, *params = NULL;
		x = self->super.operandes[OPCIRCLE_X];
		y = self->super.operandes[OPCIRCLE_Y];
		r = self->super.operandes[OPCIRCLE_R];
		a1 = self->super.operandes[OPCIRCLE_A1];
		a2 = self->super.operandes[OPCIRCLE_A2];
		if(self->super.nb_ops >= 6)
			params = self->super.operandes[OPCIRCLE_PARAMS];

		if(x != NULL && y != NULL && r != NULL && a1 != NULL && a2 != NULL)
		{
			ret = 0;
			_OpCircle_set_x(self, x);
			_OpCircle_set_y(self, y);
			_OpCircle_set_r(self, r);
			_OpCircle_set_a1(self, a1);
			_OpCircle_set_a2(self, a2);
		}
		else if(params != NULL)
		{
			ret = 0;
			_OpCircle_set_params(self, params);
		}
	}
	return ret;
}

Op *OpCircle_new(void)
{
	return Op_new(&OpCircle_isa);
}

OpIsa OpCurve_isa = {
		.name="Curve",
		.size=sizeof(OpCurve),
		.init = (void(*)(Op*))OpCurve_init,
		.terminate = (void(*)(Op*))OpCurve_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpCurve_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpCurve_execute,
		.check_args = (int(*)(Op*, OpContext*))OpCurve_check_args,
};

void OpCurve_init(OpCurve *self)
{
	Op_init(&self->super);
	self->x = NULL;
	self->y = NULL;
	self->m1x = NULL;
	self->m1y = NULL;
	self->m2x = NULL;
	self->m2y = NULL;
	self->params = NULL;
}

void OpCurve_terminate(OpCurve *self)
{
	Op_terminate(&self->super);
	_OpCurve_set_x(self, NULL);
	_OpCurve_set_y(self, NULL);
	_OpCurve_set_m1x(self, NULL);
	_OpCurve_set_m1y(self, NULL);
	_OpCurve_set_m2x(self, NULL);
	_OpCurve_set_m2y(self, NULL);
	_OpCurve_set_params(self, NULL);
}

int OpCurve_check_args(OpCurve *self, OpCanvaContext *canvactx)
{
	if(self->params != NULL || (self->x != NULL && self->y != NULL && self->m1x != NULL && self->m1y != NULL && self->m2x != NULL && self->m2y != NULL))
		return 0;
	return -1;
}

int OpCurve_execute(OpCurve *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double x = NAN, y = NAN, m1x = NAN, m1y = NAN, m2x = NAN, m2y = NAN;
	if(self->params != NULL)
	{
		double *d;
		size_t size;
		ret = Op_execute_get_doubles(self->params, (Op*)self, ctx, &d, &size, 6);
		if(ret == 0)
		{
			m1x=d[0];
			m1y=d[1];
			m2x=d[2];
			m2y=d[3];
			x=d[4];
			y=d[5];
		}
	}
	else if(self->x != NULL)
	{
		ret = Op_execute_get_double(self->x, (Op*)self, ctx, &x);
		if(ret == 0)
			ret = Op_execute_get_double(self->y, (Op*)self, ctx, &y);
		if(ret == 0)
			ret = Op_execute_get_double(self->m1x, (Op*)self, ctx, &m1x);
		if(ret == 0)
			ret = Op_execute_get_double(self->m1y, (Op*)self, ctx, &m1y);
		if(ret == 0)
			ret = Op_execute_get_double(self->m2x, (Op*)self, ctx, &m2x);
		if(ret == 0)
			ret = Op_execute_get_double(self->m2y, (Op*)self, ctx, &m2y);
	}

	if(ret == 0)
	{
		fprintf(stderr, "Op Draw Bezier Curve to {%f, %f}, handle 1 {%f, %f}, handle 2 {%f, %f}\n", x, y, m1x, m1y, m2x, m2y);
		CanvaCtx_draw_bezier(canvactx->Canva, x, y, m1x, m1y, m2x, m2y);
	}
	return ret;
}

void _OpCurve_set_x(OpCurve *self, Op *v)
{
	OP_SET_OPERANDE(self, x, v);
}

void _OpCurve_set_y(OpCurve *self, Op *v)
{
	OP_SET_OPERANDE(self, y, v);
}

void _OpCurve_set_m1x(OpCurve *self, Op *v)
{
	OP_SET_OPERANDE(self, m1x, v);
}

void _OpCurve_set_m1y(OpCurve *self, Op *v)
{
	OP_SET_OPERANDE(self, m1y, v);
}

void _OpCurve_set_m2x(OpCurve *self, Op *v)
{
	OP_SET_OPERANDE(self, m2x, v);
}

void _OpCurve_set_m2y(OpCurve *self, Op *v)
{
	OP_SET_OPERANDE(self, m2y, v);
}

void _OpCurve_set_params(OpCurve *self, Op *v)
{
	OP_SET_OPERANDE(self, params, v);
}

#define OPCURVE_X 0
#define OPCURVE_Y 1
#define OPCURVE_M1X 2
#define OPCURVE_M1Y 3
#define OPCURVE_M2X 4
#define OPCURVE_M2Y 5
#define OPCURVE_PARAMS 6

void OpCurve_set_x(OpCurve *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPCURVE_X);
}

void OpCurve_set_y(OpCurve *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPCURVE_Y);
}

void OpCurve_set_m1x(OpCurve *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPCURVE_M1X);
}

void OpCurve_set_m1y(OpCurve *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPCURVE_M1Y);
}

void OpCurve_set_m2x(OpCurve *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPCURVE_M2X);
}

void OpCurve_set_m2y(OpCurve *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPCURVE_M2Y);
}

void OpCurve_set_params(OpCurve *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPCURVE_PARAMS);
}

int OpCurve_fix_operandes(OpCurve *self, OpContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >= 6)
	{
		Op *x, *y, *m1x, *m1y, *m2x, *m2y, *params = NULL;
		x = self->super.operandes[OPCURVE_X];
		y = self->super.operandes[OPCURVE_Y];
		m1x = self->super.operandes[OPCURVE_M1X];
		m1y = self->super.operandes[OPCURVE_M1Y];
		m2x = self->super.operandes[OPCURVE_M2X];
		m2y = self->super.operandes[OPCURVE_M2Y];
		if(self->super.nb_ops >= 6)
			params = self->super.operandes[OPCURVE_PARAMS];

		if(x != NULL && y != NULL && m1x != NULL && m1y != NULL && m2x != NULL && m2y != NULL)
		{
			ret = 0;
			_OpCurve_set_x(self, x);
			_OpCurve_set_y(self, y);
			_OpCurve_set_m1x(self, m1x);
			_OpCurve_set_m1y(self, m1y);
			_OpCurve_set_m2x(self, m2x);
			_OpCurve_set_m2y(self, m2y);
		}
		else if(params != NULL)
		{
			ret = 0;
			_OpCurve_set_params(self, params);
		}
	}
	return ret;
}

Op *OpCurve_new(void)
{
	return Op_new(&OpCurve_isa);
}



OpIsa OpRectangle_isa = {
		.name="Rectangle",
		.size=sizeof(OpRectangle),
		.init = (void(*)(Op*))OpRectangle_init,
		.terminate = (void(*)(Op*))OpRectangle_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpRectangle_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpRectangle_execute,
		.check_args = (int(*)(Op*, OpContext*))OpRectangle_check_args,
};

void OpRectangle_init(OpRectangle *self)
{
	Op_init(&self->super);
	self->x = NULL;
	self->y = NULL;
	self->w = NULL;
	self->h = NULL;
	self->params = NULL;
}

void OpRectangle_terminate(OpRectangle *self)
{
	Op_terminate(&self->super);
	_OpRectangle_set_x(self, NULL);
	_OpRectangle_set_y(self, NULL);
	_OpRectangle_set_w(self, NULL);
	_OpRectangle_set_h(self, NULL);
	_OpRectangle_set_params(self, NULL);
}


int OpRectangle_check_args(OpRectangle *self, OpCanvaContext *canvactx)
{
	if(self->params != NULL || (self->x != NULL && self->y != NULL && self->w != NULL && self->h != NULL))
		return 0;
	return -1;
}

int OpRectangle_execute(OpRectangle *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double x = NAN, y = NAN, w = NAN, h = NAN;
	if(self->params != NULL)
	{
		double *d;
		size_t size;
		ret = Op_execute_get_doubles(self->params, (Op*)self, ctx, &d, &size, 4);
		if(ret == 0)
		{
			x=d[0];
			y=d[1];
			w=d[2];
			h=d[3];
		}
	}
	else if(self->x != NULL)
	{
		ret = Op_execute_get_double(self->x, (Op*)self, ctx, &x);
		if(ret == 0)
			ret = Op_execute_get_double(self->y, (Op*)self, ctx, &y);
		if(ret == 0)
			ret = Op_execute_get_double(self->w, (Op*)self, ctx, &w);
		if(ret == 0)
			ret = Op_execute_get_double(self->h, (Op*)self, ctx, &h);

	}
	if(ret == 0)
	{
		fprintf(stderr, "Op Draw Rectangle %f %f %f %f\n", x, y, w, h);
		CanvaCtx_draw_rectangle(canvactx->Canva, x, y, w, h);
	}
	return ret;
}

void _OpRectangle_set_x(OpRectangle *self, Op *v)
{
	OP_SET_OPERANDE(self, x, v);
}

void _OpRectangle_set_y(OpRectangle *self, Op *v)
{
	OP_SET_OPERANDE(self, y, v);
}

void _OpRectangle_set_w(OpRectangle *self, Op *v)
{
	OP_SET_OPERANDE(self, w, v);
}

void _OpRectangle_set_h(OpRectangle *self, Op *v)
{
	OP_SET_OPERANDE(self, h, v);
}

void _OpRectangle_set_params(OpRectangle *self, Op *v)
{
	OP_SET_OPERANDE(self, params, v);
}

#define OPRECTANGLE_X 0
#define OPRECTANGLE_Y 1
#define OPRECTANGLE_W 2
#define OPRECTANGLE_H 3
#define OPRECTANGLE_PARAMS 4


void OpRectangle_set_x(OpRectangle *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPRECTANGLE_X);
}

void OpRectangle_set_y(OpRectangle *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPRECTANGLE_Y);
}

void OpRectangle_set_w(OpRectangle *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPRECTANGLE_W);
}

void OpRectangle_set_h(OpRectangle *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPRECTANGLE_H);
}

void OpRectangle_set_params(OpRectangle *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPRECTANGLE_PARAMS);
}

int OpRectangle_fix_operandes(OpRectangle *self, OpContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >= 4)
	{
		Op *x, *y, *w, *h, *params = NULL;
		x = self->super.operandes[OPRECTANGLE_X];
		y = self->super.operandes[OPRECTANGLE_Y];
		w = self->super.operandes[OPRECTANGLE_W];
		h = self->super.operandes[OPRECTANGLE_H];
		if(self->super.nb_ops >= 5)
			params = self->super.operandes[OPRECTANGLE_PARAMS];

		if(x != NULL && y != NULL && w != NULL && h != NULL)
		{
			ret = 0;
			_OpRectangle_set_x(self, x);
			_OpRectangle_set_y(self, y);
			_OpRectangle_set_w(self, w);
			_OpRectangle_set_h(self, h);
		}
		else if(params != NULL)
		{
			ret = 0;
			_OpRectangle_set_params(self, params);
		}
	}
	return ret;
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
		.fix_operandes = (int(*)(Op*, OpContext*))OpColor_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpColor_execute,
		.check_args = (int(*)(Op*, OpContext*))OpColor_check_args,
};

void OpColor_init(OpColor *self)
{
	Op_init(&self->super);
	self->red = NULL;
	self->blue = NULL;
	self->green = NULL;
	self->alpha = NULL;
	self->params = NULL;
}

void OpColor_terminate(OpColor *self)
{
	Op_terminate(&self->super);
	_OpColor_set_red(self, NULL);
	_OpColor_set_green(self, NULL);
	_OpColor_set_blue(self, NULL);
	_OpColor_set_alpha(self, NULL);
	_OpColor_set_params(self, NULL);
}

int OpColor_check_args(OpColor *self, OpCanvaContext *canvactx)
{
	if(self->params != NULL || (self->red != NULL && self->blue != NULL && self->green != NULL && self->alpha != NULL))
		return 0;
	return -1;
}

int OpColor_execute(OpColor *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double r = NAN, g = NAN, b = NAN, a = 255;
	if(self->params != NULL)
	{
		double *d;
		size_t size;
		ret = Op_execute_get_doubles(self->params, (Op*)self, ctx, &d, &size, 3);
		if(ret == 0)
		{
			r=d[0];
			g=d[1];
			b=d[2];
			if(size > 3)
				a=d[3];
		}
	}
	else if(self->red != NULL)
	{
		ret = Op_execute_get_double(self->red, (Op*)self, ctx, &r);
		if(ret == 0)
			ret = Op_execute_get_double(self->green, (Op*)self, ctx, &g);
		if(ret == 0)
			ret = Op_execute_get_double(self->blue, (Op*)self, ctx, &b);
		if(ret == 0)
			ret = Op_execute_get_double(self->alpha, (Op*)self, ctx, &a);

	}
	if(ret == 0)
	{
		fprintf(stderr, "Op Set Color %f %f %f %f\n", r, g, b, a);
		CanvaCtx_set_color(canvactx->Canva, r, g, b, a);
	}
	return ret;
}

void _OpColor_set_red(OpColor *self, Op *r)
{
	OP_SET_OPERANDE(self, red, r);
}

void _OpColor_set_green(OpColor *self, Op *g)
{
	OP_SET_OPERANDE(self, green, g);
}


void _OpColor_set_blue(OpColor *self, Op *b)
{
	OP_SET_OPERANDE(self, blue, b);
}


void _OpColor_set_alpha(OpColor *self, Op *a)
{
	OP_SET_OPERANDE(self, alpha, a);
}

void _OpColor_set_params(OpColor *self, Op *p)
{
	OP_SET_OPERANDE(self, params, p);
}

#define OPCOLOR_RED 0
#define OPCOLOR_GREEN 1
#define OPCOLOR_BLUE 2
#define OPCOLOR_ALPHA 3
#define OPCOLOR_PARAMS 4

void OpColor_set_red(OpColor *self, Op *r)
{
	OP_ADD_OPERANDE(self, r, OPCOLOR_RED);
}

void OpColor_set_green(OpColor *self, Op *g)
{
	OP_ADD_OPERANDE(self, g, OPCOLOR_GREEN);
}


void OpColor_set_blue(OpColor *self, Op *b)
{
	OP_ADD_OPERANDE(self, b, OPCOLOR_BLUE);
}


void OpColor_set_alpha(OpColor *self, Op *a)
{
	OP_ADD_OPERANDE(self, a, OPCOLOR_ALPHA);
}

void OpColor_set_params(OpColor *self, Op *p)
{
	OP_ADD_OPERANDE(self, p, OPCOLOR_PARAMS);
}

int OpColor_fix_operandes(OpColor *self, OpContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >= 4)
	{
		Op *r, *g, *b, *a, *params = NULL;
		r = self->super.operandes[OPCOLOR_RED];
		g = self->super.operandes[OPCOLOR_GREEN];
		b = self->super.operandes[OPCOLOR_BLUE];
		a = self->super.operandes[OPCOLOR_ALPHA];
		if(self->super.nb_ops >= 5)
			params = self->super.operandes[OPCOLOR_PARAMS];

		if(r != NULL && g != NULL && b != NULL && a != NULL)
		{
			ret = 0;
			_OpColor_set_red(self, r);
			_OpColor_set_green(self, g);
			_OpColor_set_blue(self, b);
			_OpColor_set_alpha(self, a);
		}
		else if(params != NULL)
		{
			ret = 0;
			_OpColor_set_params(self, params);
		}
	}
	return ret;
}

Op *OpColor_new(void)
{
	return Op_new(&OpColor_isa);
}



OpIsa OpSetBGColor_isa = {
		.name="SetBGColor",
		.size=sizeof(OpColor),
		.init = (void(*)(Op*))OpColor_init,
		.terminate = (void(*)(Op*))OpColor_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpColor_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpSetBGColor_execute,
		.check_args = (int(*)(Op*, OpContext*))OpColor_check_args,
};

int OpSetBGColor_execute(OpColor *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double r = NAN, g = NAN, b = NAN, a = 255;
	if(self->params != NULL)
	{
		double *d;
		size_t size;
		ret = Op_execute_get_doubles(self->params, (Op*)self, ctx, &d, &size, 3);
		if(ret == 0)
		{
			r=d[0];
			g=d[1];
			b=d[2];
			if(size > 3)
				a=d[3];
		}
	}
	else if(self->red != NULL)
	{
		ret = Op_execute_get_double(self->red, (Op*)self, ctx, &r);
		if(ret == 0)
			ret = Op_execute_get_double(self->green, (Op*)self, ctx, &g);
		if(ret == 0)
			ret = Op_execute_get_double(self->blue, (Op*)self, ctx, &b);
		if(ret == 0)
			ret = Op_execute_get_double(self->alpha, (Op*)self, ctx, &a);

	}
	if(ret == 0)
	{
		int w, h;
		w = OpCanvaContext_get_width(canvactx);
		h = OpCanvaContext_get_height(canvactx);
		fprintf(stderr, "Op Set BG Color %f %f %f %f\n", r, g, b, a);
		CanvaCtx_set_color(canvactx->Canva, r, g, b, a);
		CanvaCtx_paint(canvactx->Canva);
	}
	return ret;
}

Op *OpSetBGColor_new(void)
{
	return Op_new(&OpSetBGColor_isa);
}



OpIsa OpSetDefaultStrokeColor_isa = {
		.name="SetDefaultStrokeColor",
		.size=sizeof(OpColor),
		.init = (void(*)(Op*))OpColor_init,
		.terminate = (void(*)(Op*))OpColor_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpColor_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpSetDefaultStrokeColor_execute,
		.check_args = (int(*)(Op*, OpContext*))OpColor_check_args,
};

int OpSetDefaultStrokeColor_execute(OpColor *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double r = NAN, g = NAN, b = NAN, a = 255;
	if(self->params != NULL)
	{
		double *d;
		size_t size;
		ret = Op_execute_get_doubles(self->params, (Op*)self, ctx, &d, &size, 3);
		if(ret == 0)
		{
			r=d[0];
			g=d[1];
			b=d[2];
			if(size > 3)
				a=d[3];
		}
	}
	else if(self->red != NULL)
	{
		ret = Op_execute_get_double(self->red, (Op*)self, ctx, &r);
		if(ret == 0)
			ret = Op_execute_get_double(self->green, (Op*)self, ctx, &g);
		if(ret == 0)
			ret = Op_execute_get_double(self->blue, (Op*)self, ctx, &b);
		if(ret == 0)
			ret = Op_execute_get_double(self->alpha, (Op*)self, ctx, &a);

	}
	if(ret == 0)
	{
		int w, h;
		w = OpCanvaContext_get_width(canvactx);
		h = OpCanvaContext_get_height(canvactx);
		fprintf(stderr, "Op Set Default Stroke Color %f %f %f %f\n", r, g, b, a);
		CanvaCtx_set_default_stroke_color(canvactx->Canva, r, g, b, a);
		CanvaCtx_paint(canvactx->Canva);
	}
	return ret;
}

Op *OpSetDefaultStrokeColor_new(void)
{
	return Op_new(&OpSetDefaultStrokeColor_isa);
}


OpIsa OpSetDefaultFillColor_isa = {
		.name="SetDefaultFillColor",
		.size=sizeof(OpColor),
		.init = (void(*)(Op*))OpColor_init,
		.terminate = (void(*)(Op*))OpColor_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpColor_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpSetDefaultFillColor_execute,
		.check_args = (int(*)(Op*, OpContext*))OpColor_check_args,
};

int OpSetDefaultFillColor_execute(OpColor *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double r = NAN, g = NAN, b = NAN, a = 255;
	if(self->params != NULL)
	{
		double *d;
		size_t size;
		ret = Op_execute_get_doubles(self->params, (Op*)self, ctx, &d, &size, 3);
		if(ret == 0)
		{
			r=d[0];
			g=d[1];
			b=d[2];
			if(size > 3)
				a=d[3];
		}
	}
	else if(self->red != NULL)
	{
		ret = Op_execute_get_double(self->red, (Op*)self, ctx, &r);
		if(ret == 0)
			ret = Op_execute_get_double(self->green, (Op*)self, ctx, &g);
		if(ret == 0)
			ret = Op_execute_get_double(self->blue, (Op*)self, ctx, &b);
		if(ret == 0)
			ret = Op_execute_get_double(self->alpha, (Op*)self, ctx, &a);

	}
	if(ret == 0)
	{
		int w, h;
		w = OpCanvaContext_get_width(canvactx);
		h = OpCanvaContext_get_height(canvactx);
		fprintf(stderr, "Op Set Default Fill Color %f %f %f %f\n", r, g, b, a);
		CanvaCtx_set_default_fill_color(canvactx->Canva, r, g, b, a);
	}
	return ret;
}

Op *OpSetDefaultFillColor_new(void)
{
	return Op_new(&OpSetDefaultFillColor_isa);
}


OpIsa OpStroke_isa = {
		.name="Stroke",
		.size=sizeof(Op),
		.init = (void(*)(Op*))Op_init,
		.terminate = (void(*)(Op*))Op_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpStroke_execute,
		.check_args = NULL,
};

int OpStroke_execute(Op *self, OpCanvaContext *ctx)
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
		.size=sizeof(Op),
		.init = (void(*)(Op*))Op_init,
		.terminate = (void(*)(Op*))Op_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpStrokePreserve_execute,
		.check_args = NULL,
};

int OpStrokePreserve_execute(Op *self, OpCanvaContext *ctx)
{
	CanvaCtx_stroke_preserve(ctx->Canva);
	return 0;
}

Op *OpStrokePreserve_new(void)
{
	return Op_new(&OpStrokePreserve_isa);
}


OpIsa OpFill_isa = {
		.name="Fill",
		.size=sizeof(Op),
		.init = (void(*)(Op*))Op_init,
		.terminate = (void(*)(Op*))Op_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpFill_execute,
		.check_args = NULL,
};

int OpFill_execute(Op *self, OpCanvaContext *ctx)
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
		.size=sizeof(Op),
		.init = (void(*)(Op*))Op_init,
		.terminate = (void(*)(Op*))Op_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpFillPreserve_execute
};

int OpFillPreserve_execute(Op *self, OpCanvaContext *ctx)
{
	CanvaCtx_fill_preserve(ctx->Canva);
	return 0;
}

Op *OpFillPreserve_new(void)
{
	return Op_new(&OpFillPreserve_isa);
}

OpIsa OpSetAutoFill_isa = {
		.name="SetAutoFill",
		.size=sizeof(Op),
		.init = (void(*)(Op*))Op_init,
		.terminate = (void(*)(Op*))Op_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpSetAutoFill_execute,
		.check_args = NULL,
};

int OpSetAutoFill_execute(Op *self, OpCanvaContext *ctx)
{
	CanvaCtx_set_default_stroke_mode(ctx->Canva, Fill);
	return 0;
}

Op *OpSetAutoFill_new(void)
{
	return Op_new(&OpSetAutoFill_isa);
}

OpIsa OpSetAutoFillStroke_isa = {
		.name="SetAutoFillStroke",
		.size=sizeof(Op),
		.init = (void(*)(Op*))Op_init,
		.terminate = (void(*)(Op*))Op_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpSetAutoFillStroke_execute,
		.check_args = NULL,
};

int OpSetAutoFillStroke_execute(Op *self, OpCanvaContext *ctx)
{
	CanvaCtx_set_default_stroke_mode(ctx->Canva, FillStroke);
	return 0;
}

Op *OpSetAutoFillStroke_new(void)
{
	return Op_new(&OpSetAutoFillStroke_isa);
}

OpIsa OpSetAutoStroke_isa = {
		.name="SetAutoStroke",
		.size=sizeof(Op),
		.init = (void(*)(Op*))Op_init,
		.terminate = (void(*)(Op*))Op_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpSetAutoStroke_execute,
		.check_args = NULL,
};

int OpSetAutoStroke_execute(Op *self, OpCanvaContext *ctx)
{
	CanvaCtx_set_default_stroke_mode(ctx->Canva, Stroke);
	return 0;
}

Op *OpSetAutoStroke_new(void)
{
	return Op_new(&OpSetAutoStroke_isa);
}

OpIsa OpSetAutoStrokeFill_isa = {
		.name="SetAutoStrokeFill",
		.size=sizeof(Op),
		.init = (void(*)(Op*))Op_init,
		.terminate = (void(*)(Op*))Op_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpSetAutoStrokeFill_execute,
		.check_args = NULL,
};

int OpSetAutoStrokeFill_execute(Op *self, OpCanvaContext *ctx)
{
	CanvaCtx_set_default_stroke_mode(ctx->Canva, StrokeFill);
	return 0;
}

Op *OpSetAutoStrokeFill_new(void)
{
	return Op_new(&OpSetAutoStrokeFill_isa);
}


OpIsa OpSetOutputSize_isa = {
		.name="SetOutputSize",
		.size=sizeof(OpSetOutputSize),
		.init = (void(*)(Op*))OpSetOutputSize_init,
		.terminate = (void(*)(Op*))OpSetOutputSize_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpSetOutputSize_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpSetOutputSize_execute,
		.check_args = (int(*)(Op*, OpContext*))OpSetOutputSize_check_args,
};

void OpSetOutputSize_init(OpSetOutputSize *self)
{
	Op_init(&self->super);
	self->width = NULL;
	self->height = NULL;
}

void OpSetOutputSize_terminate(OpSetOutputSize *self)
{
	Op_terminate(&self->super);
	_OpSetOutputSize_set_width(self, NULL);
	_OpSetOutputSize_set_height(self, NULL);
}

void _OpSetOutputSize_set_width(OpSetOutputSize *self, Op *v)
{
	OP_SET_OPERANDE(self, width, v);
}

void _OpSetOutputSize_set_height(OpSetOutputSize *self, Op *v)
{
	OP_SET_OPERANDE(self, height, v);
}

#define OPSETOUTPUTSIZE_WIDTH 0
#define OPSETOUTPUTSIZE_HEIGHT 1

void OpSetOutputSize_set_width(OpSetOutputSize *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPSETOUTPUTSIZE_WIDTH);
}

void OpSetOutputSize_set_height(OpSetOutputSize *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPSETOUTPUTSIZE_HEIGHT);
}

int OpSetOutputSize_fix_operandes(OpSetOutputSize *self, OpCanvaContext *canvactx)
{
	int ret = -1;
	if(self->super.nb_ops >= 2)
	{
		ret = 0;
		_OpSetOutputSize_set_width(self, self->super.operandes[OPSETOUTPUTSIZE_WIDTH]);
		_OpSetOutputSize_set_height(self, self->super.operandes[OPSETOUTPUTSIZE_HEIGHT]);
	}
	return ret;
}

int OpSetOutputSize_check_args(OpSetOutputSize *self, OpCanvaContext *canvactx)
{
	if(self->width != NULL && self->height != NULL)
		return 0;
	return -1;
}

int OpSetOutputSize_execute(OpSetOutputSize *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double w = NAN, h = NAN;
	ret = Op_execute_get_double(self->width, (Op*)self, ctx, &w);
	ret = Op_execute_get_double(self->height, (Op*)self, ctx, &h);

	if(ret == 0)
	{
		fprintf(stderr, "Op Set Output Size Width Height %f %f\n", w, h);
		OpCanvaContext_set_width(canvactx, (int)w);
		OpCanvaContext_set_height(canvactx, (int)h);
	}
	return ret;
}

Op *OpSetOutputSize_new(void)
{
	return Op_new(&OpSetOutputSize_isa);
}


OpIsa OpSetLineWidth_isa = {
		.name="SetLineWidth",
		.size=sizeof(OpSetLineWidth),
		.init = (void(*)(Op*))OpSetLineWidth_init,
		.terminate = (void(*)(Op*))OpSetLineWidth_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpSetLineWidth_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpSetLineWidth_execute,
		.check_args = (int(*)(Op*, OpContext*))OpSetLineWidth_check_args,
};

void OpSetLineWidth_init(OpSetLineWidth *self)
{
	Op_init(&self->super);
	self->width = NULL;
}

void OpSetLineWidth_terminate(OpSetLineWidth *self)
{
	Op_terminate(&self->super);
	_OpSetLineWidth_set_width(self, NULL);
}

void _OpSetLineWidth_set_width(OpSetLineWidth *self, Op *v)
{
	OP_SET_OPERANDE(self, width, v);
}

#define OPSETLINEWIDTH_WIDTH 0

void OpSetLineWidth_set_width(OpSetLineWidth *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPSETLINEWIDTH_WIDTH);
}

int OpSetLineWidth_fix_operandes(OpSetLineWidth *self, OpCanvaContext *canvactx)
{
	int ret = -1;
	if(self->super.nb_ops >= 1)
	{
		ret = 0;
		_OpSetLineWidth_set_width(self, self->super.operandes[OPSETLINEWIDTH_WIDTH]);
	}
	return ret;
}

int OpSetLineWidth_check_args(OpSetLineWidth *self, OpCanvaContext *canvactx)
{
	if(self->width != NULL)
		return 0;
	return -1;
}

int OpSetLineWidth_execute(OpSetLineWidth *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double w = NAN;
	ret = Op_execute_get_double(self->width, (Op*)self, ctx, &w);

	if(ret == 0)
	{
		fprintf(stderr, "Op Set Line Width %f\n", w);
		CanvaCtx_set_line_width(canvactx->Canva, w);
	}
	return ret;
}

Op *OpSetLineWidth_new(void)
{
	return Op_new(&OpSetLineWidth_isa);
}


OpIsa OpGetLineWidth_isa = {
		.name="GetLineWidth",
		.size=sizeof(OpGetLineWidth),
		.init = (void(*)(Op*))OpGetLineWidth_init,
		.terminate = (void(*)(Op*))OpGetLineWidth_terminate,
		.fix_operandes = NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpGetLineWidth_execute,
		.check_args = NULL,//Pas de child
};

void OpGetLineWidth_init(OpGetLineWidth *self)
{
	Op_init(&self->super);
}

void OpGetLineWidth_terminate(OpGetLineWidth *self)
{
	Op_terminate(&self->super);
}

int OpGetLineWidth_execute(OpGetLineWidth *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double w  = CanvaCtx_get_line_width(canvactx->Canva);
	fprintf(stderr, "Op Get Line Width : %f\n", w);

	OpContext_set_current_value_double(ctx, w);
	return ret;
}

Op *OpGetLineWidth_new(void)
{
	return Op_new(&OpGetLineWidth_isa);
}


OpIsa OpRotate_isa = {
		.name="Rotate",
		.size=sizeof(OpRotate),
		.init = (void(*)(Op*))OpRotate_init,
		.terminate = (void(*)(Op*))OpRotate_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpRotate_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpRotate_execute,
		.check_args = (int(*)(Op*, OpContext*))OpRotate_check_args,
};

void OpRotate_init(OpRotate *self)
{
	Op_init(&self->super);
	self->angle = NULL;
}

void OpRotate_terminate(OpRotate *self)
{
	Op_terminate(&self->super);
	_OpRotate_set_angle(self, NULL);
}

#define OPROTATE_ANGLE 0

void OpRotate_set_angle(OpRotate *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPROTATE_ANGLE);
}

void _OpRotate_set_angle(OpRotate *self, Op *v)
{
	OP_SET_OPERANDE(self, angle, v);
}

int OpRotate_fix_operandes(OpRotate *self, OpCanvaContext *canvactx)
{
	int ret = -1;
	if(self->super.nb_ops >= 1)
	{
		ret = 0;
		_OpRotate_set_angle(self, self->super.operandes[OPROTATE_ANGLE]);
	}
	return ret;
}

int OpRotate_check_args(OpRotate *self, OpCanvaContext *canvactx)
{
	if(self->angle != NULL)
		return 0;
	return -1;
}

int OpRotate_execute(OpRotate *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double a = NAN;
	ret = Op_execute_get_double(self->angle, (Op*)self, ctx, &a);

	if(ret == 0)
	{
		fprintf(stderr, "Op Rotate %f\n", a);
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
		.fix_operandes = (int(*)(Op*, OpContext*))OpTranslate_fix_operandes,
		.check_args = (int(*)(Op*, OpContext*))OpTranslate_check_args,
};

void OpTranslate_init(OpTranslate *self)
{
	Op_init(&self->super);
	self->x = NULL;
	self->y= NULL;
	self->params = NULL;
}

void OpTranslate_terminate(OpTranslate *self)
{
	Op_terminate(&self->super);
	_OpTranslate_set_x(self, NULL);
	_OpTranslate_set_y(self, NULL);
	_OpTranslate_set_params(self, NULL);
}

#define OPTRANSLATE_X 0
#define OPTRANSLATE_Y 1
#define OPTRANSLATE_PARAMS 2

void OpTranslate_set_x(OpTranslate *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPTRANSLATE_X);
}

void OpTranslate_set_y(OpTranslate *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPTRANSLATE_Y);
}

void OpTranslate_set_params(OpTranslate *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPTRANSLATE_PARAMS);
}

void _OpTranslate_set_x(OpTranslate *self, Op *v)
{
	OP_SET_OPERANDE(self, x, v);
}

void _OpTranslate_set_y(OpTranslate *self, Op *v)
{
	OP_SET_OPERANDE(self, y, v);
}

void _OpTranslate_set_params(OpTranslate *self, Op *v)
{
	OP_SET_OPERANDE(self, params, v);
}


int OpTranslate_fix_operandes(OpTranslate *self, OpCanvaContext *canvactx)
{
	int ret = -1;
	if(self->super.nb_ops >=2)
	{
		Op *x, *y, *params = NULL;
		x = self->super.operandes[OPTRANSLATE_X];
		y = self->super.operandes[OPTRANSLATE_Y];
		if(self->super.nb_ops >= 3)
			params = self->super.operandes[OPTRANSLATE_PARAMS];

		if(x != NULL && y != NULL)
		{
			ret = 0;
			_OpTranslate_set_x(self, x);
			_OpTranslate_set_y(self, y);
		}
		else if(params != NULL)
		{
			ret = 0;
			_OpTranslate_set_params(self, params);
		}
	}
	return ret;
}

int OpTranslate_check_args(OpTranslate *self, OpCanvaContext *canvactx)
{
	if(self->params != NULL || (self->x != NULL && self->y != NULL))
		return 0;
	return -1;
}

int OpTranslate_execute(OpTranslate *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double x = NAN, y = NAN;
	if(self->params != NULL)
	{
		double *d;
		size_t size;
		ret = Op_execute_get_doubles(self->params, (Op*)self, ctx, &d, &size, 2);
		if(ret == 0)
		{
			x=d[0];
			y=d[1];
		}
	}
	else if(self->x != NULL)
	{
		ret = Op_execute_get_double(self->x, (Op*)self, ctx, &x);
		if(ret == 0)
			ret = Op_execute_get_double(self->y, (Op*)self, ctx, &y);

	}
	if(ret == 0)
	{
		fprintf(stderr, "Op Translate %f %f\n", x, y);
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
		.fix_operandes = (int(*)(Op*, OpContext*))OpMoveTo_fix_operandes,
		.check_args = (int(*)(Op*, OpContext*))OpMoveTo_check_args,
};

void OpMoveTo_init(OpMoveTo *self)
{
	Op_init(&self->super);
	self->x = NULL;
	self->y= NULL;
	self->params = NULL;
}

void OpMoveTo_terminate(OpMoveTo *self)
{
	Op_terminate(&self->super);
	_OpMoveTo_set_x(self, NULL);
	_OpMoveTo_set_y(self, NULL);
	_OpMoveTo_set_params(self, NULL);
}

#define OPMOVETO_X 0
#define OPMOVETO_Y 1
#define OPMOVETO_PARAMS 2

void OpMoveTo_set_x(OpMoveTo *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPMOVETO_X);
}

void OpMoveTo_set_y(OpMoveTo *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPMOVETO_Y);
}

void OpMoveTo_set_params(OpMoveTo *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPMOVETO_PARAMS);
}

void _OpMoveTo_set_x(OpMoveTo *self, Op *v)
{
	OP_SET_OPERANDE(self, x, v);
}

void _OpMoveTo_set_y(OpMoveTo *self, Op *v)
{
	OP_SET_OPERANDE(self, y, v);
}

void _OpMoveTo_set_params(OpMoveTo *self, Op *v)
{
	OP_SET_OPERANDE(self, params, v);
}

int OpMoveTo_fix_operandes(OpMoveTo *self, OpCanvaContext *canvactx)
{
	int ret = -1;
	if(self->super.nb_ops >=2)
	{
		Op *x, *y, *params = NULL;
		x = self->super.operandes[OPMOVETO_X];
		y = self->super.operandes[OPMOVETO_Y];
		if(self->super.nb_ops >= 3)
			params = self->super.operandes[OPMOVETO_PARAMS];

		if(x != NULL && y != NULL)
		{
			ret = 0;
			_OpMoveTo_set_x(self, x);
			_OpMoveTo_set_y(self, y);
		}
		else if(params != NULL)
		{
			ret = 0;
			_OpMoveTo_set_params(self, params);
		}
	}
	return ret;
}

int OpMoveTo_check_args(OpMoveTo *self, OpCanvaContext *canvactx)
{
	if(self->params != NULL || (self->x != NULL && self->y != NULL))
		return 0;
	return -1;
}

int OpMoveTo_execute(OpMoveTo *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double x = NAN, y = NAN;
	if(self->params != NULL)
	{
		double *d;
		size_t size;
		ret = Op_execute_get_doubles(self->params, (Op*)self, ctx, &d, &size, 2);
		if(ret == 0)
		{
			x=d[0];
			y=d[1];
		}
	}
	else if(self->x != NULL)
	{
		ret = Op_execute_get_double(self->x, (Op*)self, ctx, &x);
		if(ret == 0)
			ret = Op_execute_get_double(self->y, (Op*)self, ctx, &y);

	}
	if(ret == 0)
	{
		fprintf(stderr, "Op Move To %f %f\n", x, y);
		CanvaCtx_move_to(canvactx->Canva, x, y);
	}
	return ret;
}

Op *OpMoveTo_new(void)
{
	return Op_new(&OpMoveTo_isa);
}


OpIsa OpScale_isa = {
		.name="Scale",
		.size=sizeof(OpScale),
		.init = (void(*)(Op*))OpScale_init,
		.terminate = (void(*)(Op*))OpScale_terminate,
		.execute = (int(*)(Op*, OpContext*))OpScale_execute,
		.fix_operandes = (int(*)(Op*, OpContext*))OpScale_fix_operandes,
		.check_args = (int(*)(Op*, OpContext*))OpScale_check_args,
};

void OpScale_init(OpScale *self)
{
	Op_init(&self->super);
	self->x = NULL;
	self->y= NULL;
	self->params = NULL;
}

void OpScale_terminate(OpScale *self)
{
	Op_terminate(&self->super);
	_OpScale_set_x(self, NULL);
	_OpScale_set_y(self, NULL);
	_OpScale_set_params(self, NULL);
}

#define OPSCALE_X 0
#define OPSCALE_Y 1
#define OPSCALE_PARAMS 2

void OpScale_set_x(OpScale *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPSCALE_X);
}

void OpScale_set_y(OpScale *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPSCALE_Y);
}

void OpScale_set_params(OpScale *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPSCALE_PARAMS);
}

void _OpScale_set_x(OpScale *self, Op *v)
{
	OP_SET_OPERANDE(self, x, v);
}

void _OpScale_set_y(OpScale *self, Op *v)
{
	OP_SET_OPERANDE(self, y, v);
}

void _OpScale_set_params(OpScale *self, Op *v)
{
	OP_SET_OPERANDE(self, params, v);
}

int OpScale_fix_operandes(OpScale *self, OpCanvaContext *canvactx)
{
	int ret = -1;
	if(self->super.nb_ops >=2)
	{
		Op *x, *y, *params = NULL;
		x = self->super.operandes[OPSCALE_X];
		y = self->super.operandes[OPSCALE_Y];
		if(self->super.nb_ops >= 3)
			params = self->super.operandes[OPSCALE_PARAMS];

		if(x != NULL && y != NULL)
		{
			ret = 0;
			_OpScale_set_x(self, x);
			_OpScale_set_y(self, y);
		}
		else if(params != NULL)
		{
			ret = 0;
			_OpScale_set_params(self, params);
		}
	}
	return ret;
}

int OpScale_check_args(OpScale *self, OpCanvaContext *canvactx)
{
	if(self->params != NULL || (self->x != NULL && self->y != NULL))
		return 0;
	return -1;
}

int OpScale_execute(OpScale *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double x = NAN, y = NAN;
	if(self->params != NULL)
	{
		double *d;
		size_t size;
		ret = Op_execute_get_doubles(self->params, (Op*)self, ctx, &d, &size, 2);
		if(ret == 0)
		{
			x=d[0];
			y=d[1];
		}
	}
	else if(self->x != NULL)
	{
		ret = Op_execute_get_double(self->x, (Op*)self, ctx, &x);
		if(ret == 0)
			ret = Op_execute_get_double(self->y, (Op*)self, ctx, &y);

	}
	if(ret == 0)
	{
		fprintf(stderr, "Op Scale %f %f\n", x, y);
		CanvaCtx_scale(canvactx->Canva, x, y);
	}
	return ret;
}

Op *OpScale_new(void)
{
	return Op_new(&OpScale_isa);
}



OpIsa OpDrawLineTo_isa = {
		.name="DrawLineTo",
		.size=sizeof(OpDrawLineTo),
		.init = (void(*)(Op*))OpDrawLineTo_init,
		.terminate = (void(*)(Op*))OpDrawLineTo_terminate,
		.execute = (int(*)(Op*, OpContext*))OpDrawLineTo_execute,
		.fix_operandes = (int(*)(Op*, OpContext*))OpDrawLineTo_fix_operandes,
		.check_args = (int(*)(Op*, OpContext*))OpDrawLineTo_check_args,
};

void OpDrawLineTo_init(OpDrawLineTo *self)
{
	Op_init(&self->super);
	self->x = NULL;
	self->y= NULL;
	self->params = NULL;
}

void OpDrawLineTo_terminate(OpDrawLineTo *self)
{
	Op_terminate(&self->super);
	_OpDrawLineTo_set_x(self, NULL);
	_OpDrawLineTo_set_y(self, NULL);
	_OpDrawLineTo_set_params(self, NULL);
}

#define OPDAWLINETO_X 0
#define OPDAWLINETO_Y 1
#define OPDAWLINETO_PARAMS 2

void OpDrawLineTo_set_x(OpDrawLineTo *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPDAWLINETO_X);
}

void OpDrawLineTo_set_y(OpDrawLineTo *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPDAWLINETO_Y);
}

void OpDrawLineTo_set_params(OpDrawLineTo *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, OPDAWLINETO_PARAMS);
}

void _OpDrawLineTo_set_x(OpDrawLineTo *self, Op *v)
{
	OP_SET_OPERANDE(self, x, v);
}

void _OpDrawLineTo_set_y(OpDrawLineTo *self, Op *v)
{
	OP_SET_OPERANDE(self, y, v);
}

void _OpDrawLineTo_set_params(OpDrawLineTo *self, Op *v)
{
	OP_SET_OPERANDE(self, params, v);
}

int OpDrawLineTo_fix_operandes(OpDrawLineTo *self, OpCanvaContext *canvactx)
{
	int ret = -1;
	if(self->super.nb_ops >=2)
	{
		Op *x, *y, *params = NULL;
		x = self->super.operandes[OPDAWLINETO_X];
		y = self->super.operandes[OPDAWLINETO_Y];
		if(self->super.nb_ops >= 3)
			params = self->super.operandes[OPDAWLINETO_PARAMS];

		if(x != NULL && y != NULL)
		{
			ret = 0;
			_OpDrawLineTo_set_x(self, x);
			_OpDrawLineTo_set_y(self, y);
		}
		else if(params != NULL)
		{
			ret = 0;
			_OpDrawLineTo_set_params(self, params);
		}
	}
	return ret;
}

int OpDrawLineTo_check_args(OpDrawLineTo *self, OpCanvaContext *canvactx)
{
	if(self->params != NULL || (self->x != NULL && self->y != NULL))
		return 0;
	return -1;
}

int OpDrawLineTo_execute(OpDrawLineTo *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	double x = NAN, y = NAN;
	if(self->params != NULL)
	{
		double *d;
		size_t size;
		ret = Op_execute_get_doubles(self->params, (Op*)self, ctx, &d, &size, 2);
		if(ret == 0)
		{
			x=d[0];
			y=d[1];
		}
	}
	else if(self->x != NULL)
	{
		ret = Op_execute_get_double(self->x, (Op*)self, ctx, &x);
		if(ret == 0)
			ret = Op_execute_get_double(self->y, (Op*)self, ctx, &y);

	}
	if(ret == 0)
	{
		fprintf(stderr, "Op Draw Line To %f %f\n", x, y);
		CanvaCtx_draw_line_to(canvactx->Canva, x, y);
	}
	return ret;
}

Op *OpDrawLineTo_new(void)
{
	return Op_new(&OpDrawLineTo_isa);
}


OpIsa OpCanvaBloc_isa = {
		.name="CanvaBloc",
		.size=sizeof(OpCanvaBloc),
		.init = (void(*)(Op*))OpCanvaBloc_init,
		.terminate = (void(*)(Op*))OpCanvaBloc_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpBloc_fix_operandes,
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

int OpCanvaBloc_execute(OpCanvaBloc *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpContext *ctx = (OpContext*)canvactx;
	OpRunningState state = OpContext_get_running_state(ctx);
	if(state == PreRun)//PreRun ? Musn't play Canva part !
		return OpBloc_execute(&self->super, (OpContext*)ctx);

	fprintf(stderr, "Canva %p Save\n", canvactx->Canva);
	CanvaCtx_save(canvactx->Canva);
	ret = OpBloc_execute(&self->super, (OpContext*)ctx);
	if(ret == 0)
	{
		if(self->auto_stroke == true)
			CanvaCtx_auto_stroke(canvactx->Canva);

		fprintf(stderr, "Canva %p Restore\n", canvactx->Canva);
		CanvaCtx_restore(canvactx->Canva);
	}
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
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas d'op child
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
		.fix_operandes = (int(*)(Op*, OpContext*))OpDrawText_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpDrawText_execute,
		.check_args = (int(*)(Op*, OpContext*))OpDrawText_check_args,
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
	_OpDrawText_set_text(self, NULL);
}

void _OpDrawText_set_text(OpDrawText *self, Op *s)
{
	OP_SET_OPERANDE(self, text, s);
}

#define OPDRAWTEXT_TEXT 0

void OpDrawText_set_text(OpDrawText *self, Op *s)
{
	OP_ADD_OPERANDE(self, s, OPDRAWTEXT_TEXT);
}

void OpDrawText_set_text_mode(OpDrawText *self, TextMode m)
{
	self->mode = m;
}

int OpDrawText_fix_operandes(OpDrawText *self, OpCanvaContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >= 1)
	{
		ret = 0;
		_OpDrawText_set_text(self, self->super.operandes[OPDRAWTEXT_TEXT]);
	}
	return ret;
}

int OpDrawText_check_args(OpDrawText *self, OpCanvaContext *ctx)
{
	if(self->text != NULL)
		return 0;
	return -1;
}

int OpDrawText_execute(OpDrawText *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpVariable *c;
	OpContext *ctx = (OpContext*)canvactx;
	const char *text = NULL;

	ret = Op_execute_get_string(self->text, (Op*)self, ctx, &text);

	if(ret == 0)
	{
		switch(self->mode)
		{
		case show : CanvaCtx_draw_text(canvactx->Canva, text);
					break;
		case path : CanvaCtx_draw_text_path(canvactx->Canva, text);
					break;
		}
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
		.fix_operandes = (int(*)(Op*, OpContext*))OpSetFontSize_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpSetFontSize_execute,
		.check_args = (int(*)(Op*, OpContext*))OpSetFontSize_check_args,
};

void OpSetFontSize_init(OpSetFontSize *self)
{
	Op_init(&self->super);
	self->size = NULL;
}

void OpSetFontSize_terminate(OpSetFontSize *self)
{
	Op_terminate(&self->super);
	_OpSetFontSize_set_size(self, NULL);
}

void _OpSetFontSize_set_size(OpSetFontSize *self, Op *s)
{
	OP_SET_OPERANDE(self, size, s);
}

#define OPSETFONTSIZE_SIZE 0

void OpSetFontSize_set_size(OpSetFontSize *self, Op *s)
{
	OP_ADD_OPERANDE(self, s, OPSETFONTSIZE_SIZE);
}

int OpSetFontSize_fix_operandes(OpSetFontSize *self, OpCanvaContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >= 1)
	{
		ret = 0;
		_OpSetFontSize_set_size(self, self->super.operandes[OPSETFONTSIZE_SIZE]);
	}
	return ret;
}

int OpSetFontSize_check_args(OpSetFontSize *self, OpCanvaContext *ctx)
{
	if(self->size != NULL)
		return 0;
	return -1;
}

int OpSetFontSize_execute(OpSetFontSize *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	double s = NAN;
	OpContext *ctx = (OpContext*)canvactx;

	ret = Op_execute_get_double(self->size, (Op*)self, ctx, &s);

	if(ret == 0)
	{
		CanvaCtx_set_font_size(canvactx->Canva, s);
		fprintf(stderr, "Op Set Font Size %f\n", s);
	}
	return ret;
}

Op *OpSetFontSize_new(void)
{
	return Op_new(&OpSetFontSize_isa);
}



OpIsa OpGetTextExtents_isa = {
		.name="GetTextExtents",
		.size=sizeof(OpGetTextExtents),
		.init = (void(*)(Op*))OpGetTextExtents_init,
		.terminate = (void(*)(Op*))OpGetTextExtents_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpGetTextExtents_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpGetTextExtents_execute,
		.check_args = (int(*)(Op*, OpContext*))OpGetTextExtents_check_args,
};

void OpGetTextExtents_init(OpGetTextExtents *self)
{
	Op_init(&self->super);
	self->text = NULL;
}

void OpGetTextExtents_terminate(OpGetTextExtents *self)
{
	Op_terminate(&self->super);
	_OpGetTextExtents_set_text(self, NULL);
}

void _OpGetTextExtents_set_text(OpGetTextExtents *self, Op *s)
{
	if(self->text != NULL)
		Op_free(self->text);
	self->text = s;
}

#define OPGETTEXTEXTENTS_TEXT 0

void OpGetTextExtents_set_text(OpGetTextExtents *self, Op *s)
{
	Op_set_nb_ops(&self->super, OPGETTEXTEXTENTS_TEXT + 1);
	self->super.operandes[OPGETTEXTEXTENTS_TEXT] = s;
}

int OpGetTextExtents_fix_operandes(OpGetTextExtents *self, OpCanvaContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >= 1)
	{
		ret = 0;
		_OpGetTextExtents_set_text(self, self->super.operandes[OPGETTEXTEXTENTS_TEXT]);
	}
	return ret;
}

int OpGetTextExtents_check_args(OpGetTextExtents *self, OpCanvaContext *ctx)
{
	if(self->text != NULL)
		return 0;
	return -1;
}

int OpGetTextExtents_execute(OpGetTextExtents *self, OpCanvaContext *canvactx)
{
	int ret = 0;
	OpVariable *c;
	OpContext *ctx = (OpContext*)canvactx;
	const char *text;

	ret = Op_execute_get_string(self->text, (Op*)self, ctx, &text);
	if(ret == 0)
	{
			CanvaCtxTextExtent e;
			CanvaCtx_get_text_extents(canvactx->Canva, text, &e);
			OpVariable v;
			OpVariable_init(&v);
			OpVariable_append_double(&v, e.x_bearing);
			OpVariable_append_double(&v, e.y_bearing);
			OpVariable_append_double(&v, e.width);
			OpVariable_append_double(&v, e.height);
			OpVariable_append_double(&v, e.x_advance);
			OpVariable_append_double(&v, e.y_advance);
			OpContext_copy_variable_to_current_value(ctx, &v);

			String s;
			String_init(&s);
			OpVariable_to_string(&v, &s);
			fprintf(stderr, "Get Text Extents : %s\n", String_get_char_string(&s));
			String_finalize(&s);
			OpVariable_terminate(&v);
	}
	return ret;
}

Op *OpGetTextExtents_new(void)
{
	return Op_new(&OpGetTextExtents_isa);
}



OpIsa OpGetFontExtents_isa = {
		.name="GetFontExtents",
		.size=sizeof(OpGetFontExtents),
		.init = (void(*)(Op*))OpGetFontExtents_init,
		.terminate = (void(*)(Op*))OpGetFontExtents_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpGetFontExtents_execute,
		.check_args = NULL,
};

void OpGetFontExtents_init(OpGetFontExtents *self)
{
	Op_init(&self->super);
}

void OpGetFontExtents_terminate(OpGetFontExtents *self)
{
	Op_terminate(&self->super);
}

int OpGetFontExtents_execute(OpGetFontExtents *self, OpCanvaContext *ctx)
{
	CanvaCtxFontExtent e;
	CanvaCtx_get_font_extents(ctx->Canva, &e);
	OpVariable v;
	OpVariable_init(&v);
	OpVariable_append_double(&v, e.ascent);
	OpVariable_append_double(&v, e.descent);
	OpVariable_append_double(&v, e.height);
	OpVariable_append_double(&v, e.max_x_advance);
	OpVariable_append_double(&v, e.max_y_advance);
	OpContext_copy_variable_to_current_value((OpContext*)ctx, &v);

	String s;
	String_init(&s);
	OpVariable_to_string(&v, &s);
	fprintf(stderr, "Get Font Extents : %s\n", String_get_char_string(&s));
	String_finalize(&s);
	OpVariable_terminate(&v);

	return 0;
}

Op *OpGetFontExtents_new(void)
{
	return Op_new(&OpGetFontExtents_isa);
}


OpIsa OpGetOutputSize_isa = {
		.name="GetOutputSize",
		.size=sizeof(OpGetOutputSize),
		.init = (void(*)(Op*))OpGetOutputSize_init,
		.terminate = (void(*)(Op*))OpGetOutputSize_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpGetOutputSize_execute,
		.check_args = NULL,
};

void OpGetOutputSize_init(OpGetOutputSize *self)
{
	Op_init(&self->super);
}

void OpGetOutputSize_terminate(OpGetOutputSize *self)
{
	Op_terminate(&self->super);
}

int OpGetOutputSize_execute(OpGetOutputSize *self, OpCanvaContext *ctx)
{
	int w, h;
	w = OpCanvaContext_get_width(ctx);
	h = OpCanvaContext_get_height(ctx);
	OpVariable v;
	OpVariable_init(&v);
	OpVariable_append_double(&v, w);
	OpVariable_append_double(&v, h);

	OpContext_copy_variable_to_current_value((OpContext*)ctx, &v);

	String s;
	String_init(&s);
	OpVariable_to_string(&v, &s);
	fprintf(stderr, "Get Output Size : %s\n", String_get_char_string(&s));
	String_finalize(&s);
	OpVariable_terminate(&v);

	return 0;
}

Op *OpGetOutputSize_new(void)
{
	return Op_new(&OpGetOutputSize_isa);
}


OpIsa OpGetRedColor_isa = {
		.name="GetRedColor",
		.size=sizeof(OpGetRedColor),
		.init = (void(*)(Op*))OpGetRedColor_init,
		.terminate = (void(*)(Op*))OpGetRedColor_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpGetRedColor_execute,
		.check_args = NULL,
};

void OpGetRedColor_init(OpGetRedColor *self)
{
	Op_init(&self->super);
}

void OpGetRedColor_terminate(OpGetRedColor *self)
{
	Op_terminate(&self->super);
}

int OpGetRedColor_execute(OpGetRedColor *self, OpCanvaContext *ctx)
{
	OpVariable v;
	OpVariable_init(&v);
	OpVariable_append_double(&v, 255.0);
	OpVariable_append_double(&v, 0);
	OpVariable_append_double(&v, 0);

	OpContext_copy_variable_to_current_value((OpContext*)ctx, &v);

	String s;
	String_init(&s);
	OpVariable_to_string(&v, &s);
	fprintf(stderr, "Get Output Size : %s\n", String_get_char_string(&s));
	String_finalize(&s);
	OpVariable_terminate(&v);

	return 0;
}

Op *OpGetRedColor_new(void)
{
	return Op_new(&OpGetRedColor_isa);
}

OpIsa OpGetGreenColor_isa = {
		.name="GetGreenColor",
		.size=sizeof(OpGetGreenColor),
		.init = (void(*)(Op*))OpGetGreenColor_init,
		.terminate = (void(*)(Op*))OpGetGreenColor_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpGetGreenColor_execute,
		.check_args = NULL,
};

void OpGetGreenColor_init(OpGetGreenColor *self)
{
	Op_init(&self->super);
}

void OpGetGreenColor_terminate(OpGetGreenColor *self)
{
	Op_terminate(&self->super);
}

int OpGetGreenColor_execute(OpGetGreenColor *self, OpCanvaContext *ctx)
{
	OpVariable v;
	OpVariable_init(&v);
	OpVariable_append_double(&v, 0);
	OpVariable_append_double(&v, 255.0);
	OpVariable_append_double(&v, 0);

	OpContext_copy_variable_to_current_value((OpContext*)ctx, &v);

	String s;
	String_init(&s);
	OpVariable_to_string(&v, &s);
	fprintf(stderr, "Get Output Size : %s\n", String_get_char_string(&s));
	String_finalize(&s);
	OpVariable_terminate(&v);

	return 0;
}

Op *OpGetGreenColor_new(void)
{
	return Op_new(&OpGetGreenColor_isa);
}

OpIsa OpGetBlueColor_isa = {
		.name="GetBlueColor",
		.size=sizeof(OpGetBlueColor),
		.init = (void(*)(Op*))OpGetBlueColor_init,
		.terminate = (void(*)(Op*))OpGetBlueColor_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpGetBlueColor_execute,
		.check_args = NULL,
};

void OpGetBlueColor_init(OpGetBlueColor *self)
{
	Op_init(&self->super);
}

void OpGetBlueColor_terminate(OpGetBlueColor *self)
{
	Op_terminate(&self->super);
}

int OpGetBlueColor_execute(OpGetBlueColor *self, OpCanvaContext *ctx)
{
	OpVariable v;
	OpVariable_init(&v);
	OpVariable_append_double(&v, 0);
	OpVariable_append_double(&v, 0);
	OpVariable_append_double(&v, 255.0);

	OpContext_copy_variable_to_current_value((OpContext*)ctx, &v);

	String s;
	String_init(&s);
	OpVariable_to_string(&v, &s);
	fprintf(stderr, "Get Output Size : %s\n", String_get_char_string(&s));
	String_finalize(&s);
	OpVariable_terminate(&v);

	return 0;
}

Op *OpGetBlueColor_new(void)
{
	return Op_new(&OpGetBlueColor_isa);
}

OpIsa OpGetBlackColor_isa = {
		.name="GetBlackColor",
		.size=sizeof(OpGetBlackColor),
		.init = (void(*)(Op*))OpGetBlackColor_init,
		.terminate = (void(*)(Op*))OpGetBlackColor_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpGetBlackColor_execute,
		.check_args = NULL,
};

void OpGetBlackColor_init(OpGetBlackColor *self)
{
	Op_init(&self->super);
}

void OpGetBlackColor_terminate(OpGetBlackColor *self)
{
	Op_terminate(&self->super);
}

int OpGetBlackColor_execute(OpGetBlackColor *self, OpCanvaContext *ctx)
{
	OpVariable v;
	OpVariable_init(&v);
	OpVariable_append_double(&v, 0);
	OpVariable_append_double(&v, 0);
	OpVariable_append_double(&v, 0);

	OpContext_copy_variable_to_current_value((OpContext*)ctx, &v);

	String s;
	String_init(&s);
	OpVariable_to_string(&v, &s);
	fprintf(stderr, "Get Output Size : %s\n", String_get_char_string(&s));
	String_finalize(&s);
	OpVariable_terminate(&v);

	return 0;
}

Op *OpGetBlackColor_new(void)
{
	return Op_new(&OpGetBlackColor_isa);
}

OpIsa OpGetWhiteColor_isa = {
		.name="GetWhiteColor",
		.size=sizeof(OpGetWhiteColor),
		.init = (void(*)(Op*))OpGetWhiteColor_init,
		.terminate = (void(*)(Op*))OpGetWhiteColor_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,//Pas de child
		.execute = (int(*)(Op*, OpContext*))OpGetWhiteColor_execute,
		.check_args = NULL,
};

void OpGetWhiteColor_init(OpGetWhiteColor *self)
{
	Op_init(&self->super);
}

void OpGetWhiteColor_terminate(OpGetWhiteColor *self)
{
	Op_terminate(&self->super);
}

int OpGetWhiteColor_execute(OpGetWhiteColor *self, OpCanvaContext *ctx)
{
	OpVariable v;
	OpVariable_init(&v);
	OpVariable_append_double(&v, 255.0);
	OpVariable_append_double(&v, 255.0);
	OpVariable_append_double(&v, 255.0);

	OpContext_copy_variable_to_current_value((OpContext*)ctx, &v);

	String s;
	String_init(&s);
	OpVariable_to_string(&v, &s);
	fprintf(stderr, "Get Output Size : %s\n", String_get_char_string(&s));
	String_finalize(&s);
	OpVariable_terminate(&v);

	return 0;
}

Op *OpGetWhiteColor_new(void)
{
	return Op_new(&OpGetWhiteColor_isa);
}

OpIsa OpSetOutputPNG_isa = {
		.name="SetOutputPNG",
		.size=sizeof(OpSetOutputPNG),
		.init = (void(*)(Op*))OpSetOutputPNG_init,
		.terminate = (void(*)(Op*))OpSetOutputPNG_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,
		.execute = (int(*)(Op*, OpContext*))OpSetOutputPNG_execute,
		.check_args = NULL
};

void OpSetOutputPNG_init(OpSetOutputPNG *self)
{
	Op_init(&self->super);
}

void OpSetOutputPNG_terminate(OpSetOutputPNG *self)
{
	Op_terminate(&self->super);
}

int OpSetOutputPNG_execute(OpSetOutputPNG *self, OpCanvaContext *ctx)
{
	OpCanvaContext_set_output_png(ctx);
	return 0;
}

Op *OpSetOutputPNG_new(void)
{
	return Op_new(&OpSetOutputPNG_isa);
}

OpIsa OpSetOutputSVG_isa = {
		.name="SetOutputSVG",
		.size=sizeof(OpSetOutputSVG),
		.init = (void(*)(Op*))OpSetOutputSVG_init,
		.terminate = (void(*)(Op*))OpSetOutputSVG_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,
		.execute = (int(*)(Op*, OpContext*))OpSetOutputSVG_execute,
		.check_args = NULL
};

void OpSetOutputSVG_init(OpSetOutputSVG *self)
{
	Op_init(&self->super);
}

void OpSetOutputSVG_terminate(OpSetOutputSVG *self)
{
	Op_terminate(&self->super);
}

int OpSetOutputSVG_execute(OpSetOutputSVG *self, OpCanvaContext *ctx)
{
	OpCanvaContext_set_output_svg(ctx);
	return 0;
}

Op *OpSetOutputSVG_new(void)
{
	return Op_new(&OpSetOutputSVG_isa);
}
