/*
 * opcs.c
 *
 *  Created on: 10 juil. 2026
 *      Author: damien
 */


#include <OpMaths.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>


OpIsa OpInitRandom_isa = {
		.name="InitRandom",
		.size=sizeof(OpInitRandom),
		.init = (void(*)(Op*))OpInitRandom_init,
		.terminate = (void(*)(Op*))OpInitRandom_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpInitRandom_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpInitRandom_execute,
		.check_args = (int(*)(Op*, OpContext*))NULL,
};

void OpInitRandom_init(OpInitRandom *self)
{
	Op_init(&self->super);
	self->value = NULL;
}

#define OPINITRANDOM_VAUE 0

void OpInitRandom_set_value(OpInitRandom *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OPINITRANDOM_VAUE);
}

void _OpInitRandom_set_value(OpInitRandom *self, Op *op)
{
	OP_SET_OPERANDE(self, value, op);
}

int OpInitRandom_fix_operandes(OpInitRandom *self, OpContext *ctx)
{
	int ret = 0;
	if(self->super.nb_ops > OPINITRANDOM_VAUE)
	{
		Op *v = ((Op*)self)->operandes[OPINITRANDOM_VAUE];
		_OpInitRandom_set_value(self, v);
	}
	return ret;
}

int OpInitRandom_execute(OpInitRandom *self, OpContext *ctx)
{
	int ret = 0;
	double v;

	if(self->value != NULL)
	{
		ret = Op_execute_get_double(self->value, (Op*)self, ctx, &v);
		if(ret == 0)
		{
			unsigned int s = (unsigned int)v;
			srandom(s);
			fprintf(stderr, "Init Random Seed with %u\n", s);
		}
	}
	else
	{
		unsigned int s = (unsigned int)time(NULL) ^ (unsigned int)getpid();
		srandom(s);
		fprintf(stderr, "Init Random Seed with %u\n", s);
	}

	return ret;
}

void OpInitRandom_terminate(OpInitRandom *self)
{
	Op_terminate(&self->super);
	_OpInitRandom_set_value(self, NULL);
}

Op *OpInitRandom_new(void)
{
	return Op_new(&OpInitRandom_isa);
}

OpIsa OpRandom_isa = {
		.name="Random",
		.size=sizeof(OpRandom),
		.init = (void(*)(Op*))OpRandom_init,
		.terminate = (void(*)(Op*))OpRandom_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpRandom_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpRandom_execute,
		.check_args = (int(*)(Op*, OpContext*))NULL,
};

void OpRandom_init(OpRandom *self)
{
	Op_init(&self->super);
	self->value = NULL;
}

#define OPRANDOM_VAUE 0

void OpRandom_set_value(OpRandom *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OPRANDOM_VAUE);
}

void _OpRandom_set_value(OpRandom *self, Op *op)
{
	OP_SET_OPERANDE(self, value, op);
}

int OpRandom_fix_operandes(OpRandom *self, OpContext *ctx)
{
	int ret = 0;
	if(self->super.nb_ops > OPRANDOM_VAUE)
	{
		Op *v = ((Op*)self)->operandes[OPINITRANDOM_VAUE];
		_OpRandom_set_value(self, v);
	}
	return ret;
}

int OpRandom_execute(OpRandom *self, OpContext *ctx)
{
	int ret = 0;
	double max, v = NAN;

	if(self->value != NULL)
	{
		ret = Op_execute_get_double(self->value, (Op*)self, ctx, &max);
		if(ret == 0)
		{
			v = (double)(random() % 10000) * max /10000 ;
			fprintf(stderr, "Random value in [0; %f] : %f\n", max, v);
		}
	}
	else
	{
		v = (double)(random() % 10000) / 10000;
		fprintf(stderr, "Random value in [0; 1] : %f\n", v);
	}

	if(ret == 0 && isnan(v) == false)
	{
		OpContext_set_current_value_double(ctx, v);
	}

	return ret;
}

void OpRandom_terminate(OpRandom *self)
{
	Op_terminate(&self->super);
	_OpRandom_set_value(self, NULL);
}

Op *OpRandom_new(void)
{
	return Op_new(&OpRandom_isa);
}

OpIsa OpPi_isa = {
		.name="Pi",
		.size=sizeof(Op),
		.init = (void(*)(Op*))Op_init,
		.terminate = (void(*)(Op*))Op_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,
		.execute = (int(*)(Op*, OpContext*))OpPi_execute,
		.check_args = NULL
};

int OpPi_execute(Op *self, OpContext *ctx)
{
	OpContext_set_current_value_double(ctx, M_PI);
	return 0;
}

Op *OpPi_new(void)
{
	return Op_new(&OpPi_isa);
}

OpIsa OpPhi_isa = {
		.name="Phi",
		.size=sizeof(Op),
		.init = (void(*)(Op*))Op_init,
		.terminate = (void(*)(Op*))Op_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,
		.execute = (int(*)(Op*, OpContext*))OpPhi_execute,
		.check_args = NULL
};

int OpPhi_execute(Op *self, OpContext *ctx)
{
	OpContext_set_current_value_double(ctx, (1+sqrt(5))/2);
	return 0;
}

Op *OpPhi_new(void)
{
	return Op_new(&OpPhi_isa);
}

int compute_add(OpVariable *res, OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	if(t1 == DOUBLE)
	{
		if(t2 == DOUBLE)
		{
			OpVariable_set_type(res, DOUBLE);
			OpVariable_set_double(res, OpVariable_get_double(v1) + OpVariable_get_double(v2));
		}
		else
			ret = -1;
	}
	else if (t1 == DOUBLES)
	{
		if(t2 == DOUBLE)
		{
			size_t cmpt, nb;
			double *vs, v;
			vs = OpVariable_get_doubles(v1);
			v = OpVariable_get_double(v2);
			nb = OpVariable_get_number_elements(v1);
			OpVariable_set_type(res, DOUBLES);
			for(cmpt = 0; cmpt < nb; cmpt++)
			{
				OpVariable_append_double(res, vs[cmpt] + v);
			}
		}
		else if(t2 == DOUBLES)
		{
			size_t cmpt, nb;
			double *vs1, *vs2;
			vs1 = OpVariable_get_doubles(v1);
			vs2 = OpVariable_get_doubles(v2);
			nb = OpVariable_get_number_elements(v1);
			OpVariable_set_type(res, DOUBLES);
			for(cmpt = 0; cmpt < nb; cmpt++)
			{
				OpVariable_append_double(res, vs1[cmpt] + vs2[cmpt]);
			}
		}
		else
			ret = -1;
	}
	else if(t1 == STRING)
	{
		if(t2 == STRING)
		{
			size_t nb1 = OpVariable_get_number_elements(v1) - 1;
			size_t nb2 = OpVariable_get_number_elements(v2) - 1;
			char *s = malloc(sizeof(char) * (nb1+nb2 + 1));
			s[0] = '\0';
			strcat(s, OpVariable_get_string(v1));
			strcat(&s[nb1], OpVariable_get_string(v2));
			OpVariable_set_type(res, STRING);
			OpVariable_set_string(res, s);
			free(s);
		}
		else
			ret = -1;
	}
	else if(t1 == STRINGS)
	{
		if(t2 == STRING)
		{
			OpVariable_copy(res, v1);
			OpVariable_append_string(res, OpVariable_get_string(v2));
		}
		else if(t2 == STRING)
		{
			OpVariable_copy(res, v1);
			size_t nb2 = OpVariable_get_number_elements(v2), cmpt;
			const char * const * s = OpVariable_get_strings(v2);
			for(cmpt = 0; cmpt < nb2; cmpt++)
				OpVariable_append_string(res, s[cmpt]);
		}
	}
	else
		ret = -1;
	return ret;
}

int check_args_add(OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	switch(t1)
	{
	case NONE : 	ret = -1;
					break;
	case DOUBLE :	if(t2 != DOUBLE)
						ret = -1;
					break;
	case DOUBLES :	if(t2 != DOUBLE)
					{
						if(t2 == DOUBLES)
						{
							if(OpVariable_get_number_elements(v1) != OpVariable_get_number_elements(v2))
							{
								fprintf(stderr, "Differents size of DOUBLES : %zu <> %zu\n", OpVariable_get_number_elements(v1), OpVariable_get_number_elements(v2));
								ret = -1;
							}
						}
						else
						{
							fprintf(stderr, "Type 1 is %d, type 2 not : %d !\n", t1, t2);
							ret = -1;
						}
					}
					break;
	case STRING :	if(t2 != STRING)
						ret = -1;
					break;
	case STRINGS :	if(t2 != STRING && t2 != STRINGS)
						ret = -1;
					break;
	}
	return ret;
}

OpIsaTwoOp OpAdd_isa = {
		.super.name="Add",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args_add,
		.compute = compute_add
};

Op *OpAdd_new(void)
{
	return Op_new(&OpAdd_isa.super);
}

int compute_del(OpVariable *res, OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	if(t1 == DOUBLE)
	{
		if(t2 == DOUBLE)
		{
			OpVariable_set_type(res, DOUBLE);
			OpVariable_set_double(res, OpVariable_get_double(v1) - OpVariable_get_double(v2));
		}
		else if(t2 == DOUBLES)
		{
			size_t cmpt, nb;
			double *vs, v;
			v = OpVariable_get_double(v1);
			vs = OpVariable_get_doubles(v2);
			nb = OpVariable_get_number_elements(v2);
			OpVariable_set_type(res, DOUBLES);
			for(cmpt = 0; cmpt < nb; cmpt++)
			{
				OpVariable_append_double(res, v - vs[cmpt]);
			}
		}
		else
			ret = -1;
	} else if (t1 == DOUBLES)
	{
		if(t2 == DOUBLE)
		{
			size_t cmpt, nb;
			double *vs, v;
			vs = OpVariable_get_doubles(v1);
			v = OpVariable_get_double(v2);
			nb = OpVariable_get_number_elements(v1);
			OpVariable_set_type(res, DOUBLES);
			for(cmpt = 0; cmpt < nb; cmpt++)
			{
				OpVariable_append_double(res, vs[cmpt] - v);
			}
		}
		else if(t2 == DOUBLES)
		{
			size_t cmpt, nb;
			double *vs1, *vs2;
			vs1 = OpVariable_get_doubles(v1);
			vs2 = OpVariable_get_doubles(v2);
			nb = OpVariable_get_number_elements(v1);
			OpVariable_set_type(res, DOUBLES);
			for(cmpt = 0; cmpt < nb; cmpt++)
			{
				OpVariable_append_double(res, vs1[cmpt] - vs2[cmpt]);
			}
		}
		else
			ret = -1;
	}
	else
		ret = -1;
	return ret;
}

int check_args_del(OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	switch(t1)
	{
	case NONE : 	ret = -1;
					break;
	case DOUBLE :	if(t2 != DOUBLE && t2 != DOUBLES)
						ret = -1;
					break;
	case DOUBLES :	if(t2 != DOUBLE)
					{
						if(t2 == DOUBLES)
						{
							if(OpVariable_get_number_elements(v1) != OpVariable_get_number_elements(v2))
								ret = -1;
						}
						else
							ret = -1;
					}
					break;
	case STRING :	if(t2 != STRING)
						ret = -1;
					break;
	case STRINGS :	ret = -1;
					break;
	}
	return ret;
}

OpIsaTwoOp OpDel_isa = {
		.super.name="Del",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args_del,
		.compute = compute_del
};

Op *OpDel_new(void)
{
	return Op_new(&OpDel_isa.super);
}

int compute_mult(OpVariable *res, OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	if(t1 == DOUBLE)
	{
		if(t2 == DOUBLE)
		{
			OpVariable_set_type(res, DOUBLE);
			OpVariable_set_double(res, OpVariable_get_double(v1) * OpVariable_get_double(v2));
		}
		else if(t2 == DOUBLES)
		{
			size_t cmpt, nb;
			double *vs, v;
			v = OpVariable_get_double(v1);
			vs = OpVariable_get_doubles(v2);
			nb = OpVariable_get_number_elements(v2);
			OpVariable_set_type(res, DOUBLES);
			for(cmpt = 0; cmpt < nb; cmpt++)
			{
				OpVariable_append_double(res, v * vs[cmpt]);
			}
		}
		else
			ret = -1;
	} else if (t1 == DOUBLES)
	{
		if(t2 == DOUBLE)
		{
			size_t cmpt, nb;
			double *vs, v;
			vs = OpVariable_get_doubles(v1);
			v = OpVariable_get_double(v2);
			nb = OpVariable_get_number_elements(v1);
			OpVariable_set_type(res, DOUBLES);
			for(cmpt = 0; cmpt < nb; cmpt++)
			{
				OpVariable_append_double(res, vs[cmpt] * v);
			}
		}
		else if(t2 == DOUBLES)
		{
			size_t cmpt, nb;
			double *vs1, *vs2;
			vs1 = OpVariable_get_doubles(v1);
			vs2 = OpVariable_get_doubles(v2);
			nb = OpVariable_get_number_elements(v1);
			OpVariable_set_type(res, DOUBLES);
			for(cmpt = 0; cmpt < nb; cmpt++)
			{
				OpVariable_append_double(res, vs1[cmpt] * vs2[cmpt]);
			}
		}
		else
			ret = -1;
	}
	else
		ret = -1;
	return ret;
}

int check_args_mult(OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	switch(t1)
	{
	case NONE : 	ret = -1;
					break;
	case DOUBLE :	if(t2 != DOUBLE && t2 != DOUBLES)
						ret = -1;
					break;
	case DOUBLES :	if(t2 != DOUBLE)
					{
						if(t2 == DOUBLES)
						{
							if(OpVariable_get_number_elements(v1) != OpVariable_get_number_elements(v2))
								ret = -1;
						}
						else
							ret = -1;
					}
					break;
	case STRING :	if(t2 != STRING)
						ret = -1;
					break;
	case STRINGS :	ret = -1;
					break;
	}
	return ret;
}

OpIsaTwoOp OpMult_isa = {
		.super.name="Mult",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args_mult,
		.compute = compute_mult
};

Op *OpMult_new(void)
{
	return Op_new(&OpMult_isa.super);
}

OpIsaTwoOp OpDiv_isa = {
		.super.name="Div",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args_div,
		.compute = compute_div
};

Op *OpDiv_new(void)
{
	return Op_new(&OpDiv_isa.super);
}

int compute_div(OpVariable *res, OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	if(t1 == DOUBLE)
	{
		if(t2 == DOUBLE)
		{
			OpVariable_set_type(res, DOUBLE);
			OpVariable_set_double(res, OpVariable_get_double(v1) / OpVariable_get_double(v2));
		}
		else
			ret = -1;
	} else if (t1 == DOUBLES)
	{
		if(t2 == DOUBLE)
		{
			size_t cmpt, nb;
			double *vs, v;
			vs = OpVariable_get_doubles(v1);
			v = OpVariable_get_double(v2);
			nb = OpVariable_get_number_elements(v1);
			OpVariable_set_type(res, DOUBLES);
			for(cmpt = 0; cmpt < nb; cmpt++)
			{
				OpVariable_append_double(res, vs[cmpt] / v);
			}
		}
		else if(t2 == DOUBLES)
		{
			size_t cmpt, nb;
			double *vs1, *vs2;
			vs1 = OpVariable_get_doubles(v1);
			vs2 = OpVariable_get_doubles(v2);
			nb = OpVariable_get_number_elements(v1);
			OpVariable_set_type(res, DOUBLES);
			for(cmpt = 0; cmpt < nb; cmpt++)
			{
				OpVariable_append_double(res, vs1[cmpt] / vs2[cmpt]);
			}
		}
		else
			ret = -1;
	}
	else
		ret = -1;
	return ret;
}

int check_args_div(OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	switch(t1)
	{
	case NONE : 	ret = -1;
					break;
	case DOUBLE :	if(t2 != DOUBLE && t2 != DOUBLES)
						ret = -1;
					break;
	case DOUBLES :	if(t2 != DOUBLE)
					{
						if(t2 == DOUBLES)
						{
							if(OpVariable_get_number_elements(v1) != OpVariable_get_number_elements(v2))
								ret = -1;
						}
						else
							ret = -1;
					}
					break;
	case STRING :	if(t2 != STRING)
						ret = -1;
					break;
	case STRINGS :	ret = -1;
					break;
	}
	return ret;
}



int compute_equ(OpVariable *res, OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	bool eq = true;
	size_t cmpt, nb;
	switch(OpVariable_get_type(v1))
	{
	case DOUBLE :	eq = double_eq(OpVariable_get_double(v1), OpVariable_get_double(v2));
					break;
	case DOUBLES :	nb = OpVariable_get_number_elements(v1);
					double *d1 = OpVariable_get_doubles(v1), *d2 = OpVariable_get_doubles(v2);
					for(cmpt = 0; cmpt < nb; cmpt++)
					{
						if(double_eq(d1[cmpt], d2[cmpt]) == 0)
						{
							eq = false;
							break;
						}
					}
					break;
	case STRING :	eq = strcmp(OpVariable_get_string(v1), OpVariable_get_string(v2)) == 0 ? true : false;
					break;
	case STRINGS :	nb = OpVariable_get_number_elements(v1);
					const char * const *s1 = OpVariable_get_strings(v1);
					const char * const *s2 = OpVariable_get_strings(v2);
					for(cmpt = 0; cmpt < nb; cmpt++)
					{
						if(strcmp(s1[cmpt], s2[cmpt]) != 0)
						{
							eq = false;
							break;
						}
					}
					break;
	}
	OpVariable_set_double(res, eq == true ? 1.0 : 0.0);
	return ret;
}

int check_args_equ(OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	if(t1 != t2)
		return -1;
	if(OpVariable_get_number_elements(v1) != OpVariable_get_number_elements(v2))
		return -1;
	return ret;
}

OpIsaTwoOp OpEqu_isa = {
		.super.name="Equ",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args_equ,
		.compute = compute_equ
};

Op *OpEqu_new(void)
{
	return Op_new(&OpEqu_isa.super);
}

int compute_notequ(OpVariable *res, OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	bool eq = true;
	size_t cmpt, nb;
	//Recherche de la premiere difference
	switch(OpVariable_get_type(v1))
	{
	case DOUBLE :	eq = double_eq(OpVariable_get_double(v1), OpVariable_get_double(v2));
					break;
	case DOUBLES :	nb = OpVariable_get_number_elements(v1);
					double *d1 = OpVariable_get_doubles(v1), *d2 = OpVariable_get_doubles(v2);
					for(cmpt = 0; cmpt < nb; cmpt++)
					{
						if(double_eq(d1[cmpt], d2[cmpt]) == false)
						{
							eq = false;
							break;
						}
					}
					break;
	case STRING :	eq = strcmp(OpVariable_get_string(v1), OpVariable_get_string(v2)) == 0 ? true : false;
					break;
	case STRINGS :	nb = OpVariable_get_number_elements(v1);
					const char * const *s1 = OpVariable_get_strings(v1);
					const char * const *s2 = OpVariable_get_strings(v2);
					for(cmpt = 0; cmpt < nb; cmpt++)
					{
						if(strcmp(s1[cmpt], s2[cmpt]) != 0)
						{
							eq = false;
							break;
						}
					}
					break;
	}
	OpVariable_set_double(res, eq == true ? 0.0 : 1.0);
	return ret;
}

int check_args_notequ(OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	if(t1 != t2)
		return -1;
	if(OpVariable_get_number_elements(v1) != OpVariable_get_number_elements(v2))
		return -1;
	return ret;
}

OpIsaTwoOp OpNotEqu_isa = {
		.super.name="NotEqu",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args_notequ,
		.compute = compute_notequ
};

Op *OpNotEqu_new(void)
{
	return Op_new(&OpNotEqu_isa.super);
}

int compute_inf(OpVariable *res, OpVariable *v1, OpVariable *v2)
{
	if(OpVariable_get_type(v1) == DOUBLE)
	{
		if(OpVariable_get_type(v2) == DOUBLE)
		{
			bool bres = false;
			double d1 = OpVariable_get_double(v1), d2 = OpVariable_get_double(v2);
			if(d1 < d2)
				if(double_eq(d1, d2) == false)
					bres = true;
			OpVariable_set_double(res, bres == true ? 1.0 : 0.0);
			return 0;
		}
		else
			return -1;
	}
	else
		return -1;
}

int check_args_inf(OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	if(t1 != DOUBLE)
		return -1;
	if(t2 != DOUBLE)
		return -1;
	return ret;
}

OpIsaTwoOp OpInf_isa = {
		.super.name="Inf",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args_inf,
		.compute = compute_inf
};

Op *OpInf_new(void)
{
	return Op_new(&OpInf_isa.super);
}

int compute_infequ(OpVariable *res, OpVariable *v1, OpVariable *v2)
{
	if(OpVariable_get_type(v1) == DOUBLE)
	{
		if(OpVariable_get_type(v2) == DOUBLE)
		{
			bool bres = true;
			double d1 = OpVariable_get_double(v1), d2 = OpVariable_get_double(v2);
			if(double_eq(d1, d2) == false)
				if(d1 > d2)
					bres = false;
			OpVariable_set_double(res, bres == true ? 1.0 : 0.0);
			return 0;
		}
		else
			return -1;
	}
	else
		return -1;
}

int check_args_infequ(OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	if(t1 != DOUBLE)
		return -1;
	if(t2 != DOUBLE)
		return -1;
	return ret;
}

OpIsaTwoOp OpInfEqu_isa = {
		.super.name="InfEqu",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args_infequ,
		.compute = compute_infequ
};

Op *OpInfEqu_new(void)
{
	return Op_new(&OpInfEqu_isa.super);
}

int compute_sup(OpVariable *res, OpVariable *v1, OpVariable *v2)
{
	if(OpVariable_get_type(v1) == DOUBLE)
	{
		if(OpVariable_get_type(v2) == DOUBLE)
		{
			bool bres = false;
			double d1 = OpVariable_get_double(v1), d2 = OpVariable_get_double(v2);
			if(double_eq(d1, d2) == false)
				if(d1 > d2)
					bres = true;
			OpVariable_set_double(res, bres == true ? 1.0 : 0.0);
			return 0;
		}
		else
			return -1;
	}
	else
		return -1;
}

int check_args_sup(OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	if(t1 != DOUBLE)
		return -1;
	if(t2 != DOUBLE)
		return -1;
	return ret;
}

OpIsaTwoOp OpSup_isa = {
		.super.name="Sup",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args_sup,
		.compute = compute_sup
};

Op *OpSup_new(void)
{
	return Op_new(&OpSup_isa.super);
}

int compute_supequ(OpVariable *res, OpVariable *v1, OpVariable *v2)
{
	if(OpVariable_get_type(v1) == DOUBLE)
	{
		if(OpVariable_get_type(v2) == DOUBLE)
		{
			bool bres = true;
			double d1 = OpVariable_get_double(v1), d2 = OpVariable_get_double(v2);
			if(double_eq(d1, d2) == false)
				if(d1 < d2)
					bres = false;
			OpVariable_set_double(res, bres == true ? 1.0 : 0.0);
			return 0;
		}
		else
			return -1;
	}
	else
		return -1;
}

int check_args_supequ(OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	if(t1 != DOUBLE)
		return -1;
	if(t2 != DOUBLE)
		return -1;
	return ret;
}

OpIsaTwoOp OpSupEqu_isa = {
		.super.name="SupEqu",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args_supequ,
		.compute = compute_supequ
};

Op *OpSupEqu_new(void)
{
	return Op_new(&OpSupEqu_isa.super);
}

int compute_power(OpVariable *res, OpVariable *v1, OpVariable *v2)
{
	if(OpVariable_get_type(v1) == DOUBLE)
	{
		if(OpVariable_get_type(v2) == DOUBLE)
		{
			double d1 = OpVariable_get_double(v1), d2 = OpVariable_get_double(v2);
			OpVariable_set_double(res, pow(d1, d2));
			return 0;
		}
		else
			return -1;
	}
	else
		return -1;
}

int check_args_power(OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	if(t1 != DOUBLE)
		return -1;
	if(t2 != DOUBLE)
		return -1;
	return ret;
}

OpIsaTwoOp OpPower_isa = {
		.super.name="Power",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args_power,
		.compute = compute_power
};

Op *OpPower_new(void)
{
	return Op_new(&OpPower_isa.super);
}

int compute_modulo(OpVariable *res, OpVariable *v1, OpVariable *v2)
{
	if(OpVariable_get_type(v1) == DOUBLE)
	{
		if(OpVariable_get_type(v2) == DOUBLE)
		{
			double d1 = OpVariable_get_double(v1), d2 = OpVariable_get_double(v2), r;
			r = fmod(d1, d2);
			if(double_eq(r, 0) == 0 && r < 0)
				r += fabs(d2);
			OpVariable_set_double(res, r);
			return 0;
		}
		else
			return -1;
	}
	else
		return -1;
}

int check_args_modulo(OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	if(t1 != DOUBLE)
		return -1;
	if(t2 != DOUBLE)
		return -1;
	return ret;
}

OpIsaTwoOp OpModulo_isa = {
		.super.name="Modulo",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args_modulo,
		.compute = compute_modulo
};

Op *OpModulo_new(void)
{
	return Op_new(&OpModulo_isa.super);
}

int compute_radians(OpVariable *res, OpVariable *v)
{
	switch(OpVariable_get_type(v))
	{
	case NONE :		return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	case DOUBLE :	OpVariable_set_double(res, OpVariable_get_double(v) * M_PI / 180);
					return 0;
					break;
	case DOUBLES :
					{
						size_t cmpt, nb = OpVariable_get_number_elements(v);

						double *d = OpVariable_get_doubles(v);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_double(res, d[cmpt] * M_PI / 180);
						return 0;
					}
					break;
	}
	return 0;
}

int check_args_radians(OpVariable *v)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v);
	if(t1 != DOUBLE && t1 != DOUBLES)
		return -1;
	return ret;
}

OpIsaOneOp OpRadians_isa = {
		.super.name="Radians",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_radians,
		.compute = compute_radians
};

Op *OpRadians_new(void)
{
	return Op_new(&OpRadians_isa.super);
}

int compute_degrees(OpVariable *res, OpVariable *v)
{
	switch(OpVariable_get_type(v))
	{
	case NONE :		return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	case DOUBLE :	OpVariable_set_double(res, OpVariable_get_double(v) * 180 / M_PI);
					return 0;
					break;
	case DOUBLES :	{
						size_t cmpt, nb = OpVariable_get_number_elements(v);

						double *d = OpVariable_get_doubles(v);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_double(res, d[cmpt] * 180 / M_PI);
						return 0;
					}
					break;
	}
	return 0;
}

int check_args_degrees(OpVariable *v)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v);
	if(t1 != DOUBLE && t1 != DOUBLES)
		return -1;
	return ret;
}

OpIsaOneOp OpDegrees_isa = {
		.super.name="Degrees",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_degrees,
		.compute = compute_degrees
};

Op *OpDegrees_new(void)
{
	return Op_new(&OpDegrees_isa.super);
}

int compute_neg(OpVariable *res, OpVariable *v)
{
	switch(OpVariable_get_type(v))
	{
	case NONE :		return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	case DOUBLE :	OpVariable_set_double(res, OpVariable_get_double(v) * -1);
					return 0;
					break;
	case DOUBLES :	{
						size_t cmpt, nb = OpVariable_get_number_elements(v);

						double *d = OpVariable_get_doubles(v);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_double(res, d[cmpt] * -1);
						return 0;
					}
					break;
	}
	return 0;
}

int check_args_neg(OpVariable *v)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v);
	if(t1 != DOUBLE && t1 != DOUBLES)
		return -1;
	return ret;
}

OpIsaOneOp OpNegValue_isa = {
		.super.name="NegValue",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_neg,
		.compute = compute_neg
};

Op *OpNegValue_new(void)
{
	return Op_new(&OpNegValue_isa.super);
}

int compute_floor(OpVariable *res, OpVariable *v)
{
	switch(OpVariable_get_type(v))
	{
	case NONE :		return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	case DOUBLE :	OpVariable_set_double(res, floor(OpVariable_get_double(v)));
					return 0;
					break;
	case DOUBLES :	{
						size_t cmpt, nb = OpVariable_get_number_elements(v);

						double *d = OpVariable_get_doubles(v);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_double(res, floor(d[cmpt]));
						return 0;
					}
					break;
	}
	return 0;
}

int check_args_floor(OpVariable *v)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v);
	if(t1 != DOUBLE && t1 != DOUBLES)
		return -1;
	return ret;
}

OpIsaOneOp OpFloor_isa = {
		.super.name="Floor",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_floor,
		.compute = compute_floor
};

Op *OpFloor_new(void)
{
	return Op_new(&OpFloor_isa.super);
}

int compute_ceil(OpVariable *res, OpVariable *v)
{
	switch(OpVariable_get_type(v))
	{
	case NONE :		return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	case DOUBLE :	OpVariable_set_double(res, ceil(OpVariable_get_double(v)));
					return 0;
					break;
	case DOUBLES :	{
						size_t cmpt, nb = OpVariable_get_number_elements(v);

						double *d = OpVariable_get_doubles(v);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_double(res, ceil(d[cmpt]));
						return 0;
					}
					break;
	}
	return 0;
}

int check_args_ceil(OpVariable *v)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v);
	if(t1 != DOUBLE && t1 != DOUBLES)
		return -1;
	return ret;
}

OpIsaOneOp OpCeil_isa = {
		.super.name="Ceil",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_ceil,
		.compute = compute_ceil
};

Op *OpCeil_new(void)
{
	return Op_new(&OpCeil_isa.super);
}

int compute_cos(OpVariable *res, OpVariable *v)
{
	switch(OpVariable_get_type(v))
	{
	case NONE :		return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	case DOUBLE :	OpVariable_set_double(res, cos(OpVariable_get_double(v)));
					return 0;
					break;
	case DOUBLES :	{
						size_t cmpt, nb = OpVariable_get_number_elements(v);

						double *d = OpVariable_get_doubles(v);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_double(res, cos(d[cmpt]));
						return 0;
					}
					break;
	}
	return 0;
}

int check_args_cos(OpVariable *v)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v);
	if(t1 != DOUBLE && t1 != DOUBLES)
		return -1;
	return ret;
}

OpIsaOneOp OpCos_isa = {
		.super.name="Cos",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_cos,
		.compute = compute_cos
};

Op *OpCos_new(void)
{
	return Op_new(&OpCos_isa.super);
}

int compute_acos(OpVariable *res, OpVariable *v)
{
	switch(OpVariable_get_type(v))
	{
	case NONE :		return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	case DOUBLE :	OpVariable_set_double(res, acos(OpVariable_get_double(v)));
					return 0;
					break;
	case DOUBLES :	{
						size_t cmpt, nb = OpVariable_get_number_elements(v);

						double *d = OpVariable_get_doubles(v);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_double(res, acos(d[cmpt]));
						return 0;
					}
					break;
	}
	return 0;
}

int check_args_acos(OpVariable *v)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v);
	if(t1 != DOUBLE && t1 != DOUBLES)
		return -1;
	return ret;
}

OpIsaOneOp OpAcos_isa = {
		.super.name="Acos",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_acos,
		.compute = compute_acos
};

Op *OpAcos_new(void)
{
	return Op_new(&OpAcos_isa.super);
}

int compute_sin(OpVariable *res, OpVariable *v)
{
	switch(OpVariable_get_type(v))
	{
	case NONE :		return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	case DOUBLE :	OpVariable_set_double(res, sin(OpVariable_get_double(v)));
					return 0;
					break;
	case DOUBLES :	{
						size_t cmpt, nb = OpVariable_get_number_elements(v);

						double *d = OpVariable_get_doubles(v);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_double(res, sin(d[cmpt]));
						return 0;
					}
					break;
	}
	return 0;
}

int check_args_sin(OpVariable *v)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v);
	if(t1 != DOUBLE && t1 != DOUBLES)
		return -1;
	return ret;
}

OpIsaOneOp OpSin_isa = {
		.super.name="Sin",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_sin,
		.compute = compute_sin
};

Op *OpSin_new(void)
{
	return Op_new(&OpSin_isa.super);
}

int compute_asin(OpVariable *res, OpVariable *v)
{
	switch(OpVariable_get_type(v))
	{
	case NONE :		return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	case DOUBLE :	OpVariable_set_double(res, asin(OpVariable_get_double(v)));
					return 0;
					break;
	case DOUBLES :	{
						size_t cmpt, nb = OpVariable_get_number_elements(v);

						double *d = OpVariable_get_doubles(v);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_double(res, asin(d[cmpt]));
						return 0;
					}
					break;
	}
	return 0;
}

int check_args_asin(OpVariable *v)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v);
	if(t1 != DOUBLE && t1 != DOUBLES)
		return -1;
	return ret;
}

OpIsaOneOp OpAsin_isa = {
		.super.name="Asin",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_asin,
		.compute = compute_asin
};

Op *OpAsin_new(void)
{
	return Op_new(&OpAsin_isa.super);
}

int compute_tan(OpVariable *res, OpVariable *v)
{
	switch(OpVariable_get_type(v))
	{
	case NONE :		return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	case DOUBLE :	OpVariable_set_double(res, tan(OpVariable_get_double(v)));
					return 0;
					break;
	case DOUBLES :	{
						size_t cmpt, nb = OpVariable_get_number_elements(v);

						double *d = OpVariable_get_doubles(v);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_double(res, tan(d[cmpt]));
						return 0;
					}
					break;
	}
	return 0;
}

int check_args_tan(OpVariable *v)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v);
	if(t1 != DOUBLE && t1 != DOUBLES)
		return -1;
	return ret;
}

OpIsaOneOp OpTan_isa = {
		.super.name="Tan",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_tan,
		.compute = compute_tan
};

Op *OpTan_new(void)
{
	return Op_new(&OpTan_isa.super);
}

int compute_atan(OpVariable *res, OpVariable *v)
{
	switch(OpVariable_get_type(v))
	{
	case NONE :		return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	case DOUBLE :	OpVariable_set_double(res, atan(OpVariable_get_double(v)));
					return 0;
					break;
	case DOUBLES :	{
						size_t cmpt, nb = OpVariable_get_number_elements(v);

						double *d = OpVariable_get_doubles(v);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_double(res, atan(d[cmpt]));
						return 0;
					}
					break;
	}
	return 0;
}

int check_args_atan(OpVariable *v)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v);
	if(t1 != DOUBLE && t1 != DOUBLES)
		return -1;
	return ret;
}

OpIsaOneOp OpAtan_isa = {
		.super.name="Atan",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_atan,
		.compute = compute_atan
};

Op *OpAtan_new(void)
{
	return Op_new(&OpAtan_isa.super);
}

int compute_sqrt(OpVariable *res, OpVariable *v)
{
	switch(OpVariable_get_type(v))
	{
	case NONE :		return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	case DOUBLE :	OpVariable_set_double(res, sqrt(OpVariable_get_double(v)));
					return 0;
					break;
	case DOUBLES :	{
						size_t cmpt, nb = OpVariable_get_number_elements(v);

						double *d = OpVariable_get_doubles(v);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_double(res, sqrt(d[cmpt]));
						return 0;
					}
					break;
	}
	return 0;
}

int check_args_sqrt(OpVariable *v)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v);
	if(t1 != DOUBLE && t1 != DOUBLES)
		return -1;
	return ret;
}

OpIsaOneOp OpSqrt_isa = {
		.super.name="Sqrt",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_sqrt,
		.compute = compute_sqrt
};

Op *OpSqrt_new(void)
{
	return Op_new(&OpSqrt_isa.super);
}
