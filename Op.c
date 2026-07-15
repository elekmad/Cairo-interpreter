/*
 * opcs.c
 *
 *  Created on: 10 juil. 2026
 *      Author: damien
 */


#include <Op.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

bool double_eq(double a, double b)
{
    double diff = fabs(a - b);

    double norm =
        fmax(fabs(a), fabs(b));

    return diff <= norm * 1e-12;
}

void OpContext_init(OpContext *self)
{
	self->current_value = NAN;
	self->variables = NULL;
	self->variables_names = NULL;
	self->number_of_variables = 0;
}

void OpContext_terminate(OpContext *self)
{
	if(self->number_of_variables > 0)
	{
		if(self->variables != NULL)
		{
			free(self->variables);
			self->variables = NULL;
		}
		if(self->variables_names != NULL)
		{
			int i;
			for(i = 0; i < self->number_of_variables; i++)
			{
				if(self->variables_names[i] != NULL)
					free(self->variables_names[i]);
			}
			free(self->variables_names);
			self->variables_names = NULL;
		}
	}
}


void OpContext_set_current_value(OpContext *self, double v)
{
	self->current_value = v;
}

double OpContext_get_current_value(OpContext *self)
{
	return self->current_value;
}

size_t OpContext_get_variable_number(OpContext *self, const char *name)
{
	if(self->number_of_variables > 0)
	{
		size_t i;
		int found = 0;
		for(i = 0; i < self->number_of_variables; i++)
		{
			if(strcmp(self->variables_names[i], name) == 0)
			{
				found = 1;
				return i;
			}
		}
	}
	size_t place;
	place = self->number_of_variables;
	self->number_of_variables++;
	self->variables_names = realloc(self->variables_names, self->number_of_variables * sizeof(char*));
	self->variables_names[place] = strdup(name);
	return place;
}

void OpContext_freeze_number_of_variabes(OpContext *self)
{
	if(self->number_of_variables > 0)
	{
		int i;
		self->variables = realloc(self->variables, self->number_of_variables * sizeof(double));
		for(i = 0; i < self->number_of_variables; i++)
			self->variables[i] = NAN;
	}
}


int OpContext_set_variable_value(OpContext *self, size_t variable_number, double v)
{
	if(self->number_of_variables < variable_number + 1)
		return -1;
	self->variables[variable_number] = v;

	printf("Setting variable '%s' number %zu = %f\n", self->variables_names[variable_number], variable_number, v);
	return 0;
}

double OpContext_get_variable_value(OpContext *self, size_t variable_number)
{
	double val = NAN;
	if(self->number_of_variables >= variable_number + 1)
		val = self->variables[variable_number];
	printf("Getting variable '%s' number %zu = %f\n", self->variables_names[variable_number], variable_number, val);
	return val;
}

void Op_init(Op *self)
{
}

const char *Op_get_name(Op *self)
{
	if(self != NULL)
		return self->isa->name;
	return "op is null";
}

void Op_terminate(Op *self)
{
}

Op *Op_new(OpIsa *isa)
{
	Op *self = malloc(isa->size);
	if(self != NULL)
	{
		self->isa = isa;
		isa->init(self);
	}
	return self;
}

void Op_free(Op *self)
{
	if(self != NULL)
	{
		self->isa->terminate(self);
		free(self);
	}
}

void Op_execute(Op *self, OpContext *ctx)
{
	if(self != NULL && ctx != NULL)
	{
		printf("Op %p : %s\n", self, self->isa->name);
		self->isa->execute(self, ctx);
		printf("Fin Op %p : %s\n", self, self->isa->name);
	}
}

OpIsa OpBloc_isa = {
		.name="bloc",
		.size=sizeof(OpBloc),
		.init = (void(*)(Op*))OpBloc_init,
		.terminate = (void(*)(Op*))OpBloc_terminate,
		.execute = (void(*)(Op*, OpContext*))OpBloc_execute
};

void OpBloc_init(OpBloc *self)
{
	Op_init(&self->super);
	LinkedList_init(&self->ops);
}

void OpBloc_append_Op(OpBloc *self, Op *op)
{
	LinkedList_append(&self->ops, op);
}

void OpBloc_execute(OpBloc *self, OpContext *ctx)
{
	LinkedList_do_to_all(&self->ops, (void(*)(void*, void*))Op_execute, ctx);
}

void OpBloc_terminate(OpBloc *self)
{
	Op_terminate(&self->super);
	LinkedList_finalize(&self->ops);
}

Op *OpBloc_new(void)
{
	return Op_new(&OpBloc_isa);
}


OpIsa OpGetValue_isa = {
		.name="GetValue",
		.size=sizeof(OpGetValue),
		.init = (void(*)(Op*))OpGetValue_init,
		.terminate = (void(*)(Op*))OpGetValue_terminate,
		.execute = (void(*)(Op*, OpContext*))OpGetValue_execute
};

void OpGetValue_init(OpGetValue *self)
{
	Op_init(&self->super);
}

void OpGetValue_terminate(OpGetValue *self)
{
	Op_terminate(&self->super);
}

void OpGetValue_set_value(OpGetValue *self, double v)
{
	self->value = v;
}

void OpGetValue_execute(OpGetValue *self, OpContext *ctx)
{
	OpContext_set_current_value(ctx, self->value);
}

Op *OpGetValue_new(void)
{
	return Op_new(&OpGetValue_isa);
}

OpIsa OpNegValue_isa = {
		.name="NegValue",
		.size=sizeof(OpNegValue),
		.init = (void(*)(Op*))OpNegValue_init,
		.terminate = (void(*)(Op*))OpNegValue_terminate,
		.execute = (void(*)(Op*, OpContext*))OpNegValue_execute
};

void OpNegValue_init(OpNegValue *self)
{
	Op_init(&self->super);
	self->value = NULL;
}

void OpNegValue_terminate(OpNegValue *self)
{
	Op_terminate(&self->super);
	OpNegValue_set_value(self, NULL);
}

void OpNegValue_set_value(OpNegValue *self, Op *op)
{
	if(self->value != NULL)
		Op_free(self->value);
	self->value = op;
}

void OpNegValue_execute(OpNegValue *self, OpContext *ctx)
{
	double v = NAN;
	if(self->value != NULL)
	{
		Op_execute(self->value, ctx);
		v = -1 * OpContext_get_current_value(ctx);
	}
	OpContext_set_current_value(ctx, v);
}

Op *OpNegValue_new(void)
{
	return Op_new(&OpNegValue_isa);
}

OpIsa OpLogicalNegValue_isa = {
		.name="LogicalNegValue",
		.size=sizeof(OpLogicalNegValue),
		.init = (void(*)(Op*))OpLogicalNegValue_init,
		.terminate = (void(*)(Op*))OpLogicalNegValue_terminate,
		.execute = (void(*)(Op*, OpContext*))OpLogicalNegValue_execute
};

void OpLogicalNegValue_init(OpLogicalNegValue *self)
{
	Op_init(&self->super);
	self->value = NULL;
}

void OpLogicalNegValue_terminate(OpLogicalNegValue *self)
{
	Op_terminate(&self->super);
	OpLogicalNegValue_set_value(self, NULL);
}

void OpLogicalNegValue_set_value(OpLogicalNegValue *self, Op *op)
{
	if(self->value != NULL)
		Op_free(self->value);
	self->value = op;
}

void OpLogicalNegValue_execute(OpLogicalNegValue *self, OpContext *ctx)
{
	double v = NAN;
	if(self->value != NULL)
	{
		Op_execute(self->value, ctx);
		v = OpContext_get_current_value(ctx);
	}
	OpContext_set_current_value(ctx, double_eq(v, 0) ? 1.0 : 0.0);
}

Op *OpLogicalNegValue_new(void)
{
	return Op_new(&OpLogicalNegValue_isa);
}


OpIsa OpIf_isa = {
		.name="IfThenElse",
		.size=sizeof(OpIf),
		.init = (void(*)(Op*))OpIf_init,
		.terminate = (void(*)(Op*))OpIf_terminate,
		.execute = (void(*)(Op*, OpContext*))OpIf_execute
};

void OpIf_init(OpIf *self)
{
	Op_init(&self->super);
	self->condition = NULL;
	self->true_branch = NULL;
	self->false_branch = NULL;
}

void OpIf_terminate(OpIf *self)
{
	Op_terminate(&self->super);
	OpIf_set_condition(self, NULL);
	OpIf_set_true_branch(self, NULL);
	OpIf_set_false_branch(self, NULL);
}

void OpIf_set_condition(OpIf *self, Op *op)
{
	if(self->condition != NULL)
		Op_free(self->condition);
	self->condition = op;
}

void OpIf_set_true_branch(OpIf *self, Op *op)
{
	if(self->true_branch != NULL)
		Op_free(self->true_branch);
	self->true_branch = op;
}

void OpIf_set_false_branch(OpIf *self, Op *op)
{
	if(self->false_branch != NULL)
		Op_free(self->false_branch);
	self->false_branch = op;
}

void OpIf_execute(OpIf *self, OpContext *ctx)
{
	double c = NAN;
	printf("If %p Entered\n", self);
	if(self->condition != NULL)
	{
		Op_execute(self->condition, ctx);
		c = OpContext_get_current_value(ctx);
		printf("If %p Condition = %f\n", self, c);
	}
	if(c != 0)
	{
		if(self->true_branch != NULL)
		{
			printf("If %p Branch True\n", self);
			Op_execute(self->true_branch, ctx);
		}
	}
	else
	{
		if(self->false_branch != NULL)
		{
			printf("If %p Branch False\n", self);
			Op_execute(self->false_branch, ctx);
		}
	}
	printf("If %p Exited\n", self);
}

Op *OpIf_new(void)
{
	return Op_new(&OpIf_isa);
}


OpIsa OpForLoop_isa = {
		.name="ForLoop",
		.size=sizeof(OpForLoop),
		.init = (void(*)(Op*))OpForLoop_init,
		.terminate = (void(*)(Op*))OpForLoop_terminate,
		.execute = (void(*)(Op*, OpContext*))OpForLoop_execute
};

void OpForLoop_init(OpForLoop *self)
{
	Op_init(&self->super);
	self->condition = NAN;
	self->start = NAN;
	self->step = NAN;
	self->loop = NULL;
	self->variable_number = 0;
}

void OpForLoop_terminate(OpForLoop *self)
{
	Op_terminate(&self->super);
	OpForLoop_set_loop(self, NULL);
}


void OpForLoop_set_variable_number(OpForLoop *self, size_t v)
{
	self->variable_number = v;
}

void OpForLoop_set_start(OpForLoop *self, double v)
{
	self->start = v;
}

void OpForLoop_set_condition(OpForLoop *self, double v)
{
	self->condition = v;
}

void OpForLoop_set_step(OpForLoop *self, double v)
{
	self->step = v;
}

void OpForLoop_set_loop(OpForLoop *self, Op *op)
{
	if(self->loop != NULL)
		Op_free(self->loop);
	self->loop = op;
}

void OpForLoop_execute(OpForLoop *self, OpContext *ctx)
{
	if(!isnan(self->start) && !isnan(self->step) && !isnan(self->condition))
	{
		bool stop = false;
		double c = NAN, v;
		printf("ForLoop %p Start\n", self);
		v = self->start;
		printf("ForLoop %p start op : @%zu = %f\n", self, self->variable_number, v);
		OpContext_set_variable_value(ctx, self->variable_number, v);
		do
		{
			printf("ForLoop %p Condition\n", self);
			c = self->condition;
			v = OpContext_get_variable_value(ctx, self->variable_number);
			printf("ForLoop %p condition op : @%zu = %f\n", self, self->variable_number, v);
			stop = double_eq(v, c) ? false : v > c ? true : false;
			if(!stop)
			{
				printf("ForLoop %p Loop\n", self);
				Op_execute(self->loop, ctx);
				printf("ForLoop %p End Loop\n", self);
				printf("ForLoop %p Step\n", self);
				v = v + self->step;
				OpContext_set_variable_value(ctx, self->variable_number, v);
				printf("ForLoop %p step op : @%zu = %f\n", self, self->variable_number, v);
			}
		}
		while(!stop);
		printf("ForLoop %p Stopped\n", self);
	}
}

Op *OpForLoop_new(void)
{
	return Op_new(&OpForLoop_isa);
}

OpIsa OpGetVariable_isa = {
		.name="GetVariable",
		.size=sizeof(OpGetVariable),
		.init = (void(*)(Op*))OpGetVariable_init,
		.terminate = (void(*)(Op*))OpGetVariable_terminate,
		.execute = (void(*)(Op*, OpContext*))OpGetVariable_execute
};

void OpGetVariable_init(OpGetVariable *self)
{
	Op_init(&self->super);
}

void OpGetVariable_terminate(OpGetVariable *self)
{
	Op_terminate(&self->super);
}

void OpGetVariable_set_variable_number(OpGetVariable *self, size_t n)
{
	self->variable_number = n;
}

void OpGetVariable_execute(OpGetVariable *self, OpContext *ctx)
{
	double val = OpContext_get_variable_value(ctx, self->variable_number);
	printf("OP %p Getting variable number %zu = %f\n", self, self->variable_number, val);
	OpContext_set_current_value(ctx, val);
}

Op *OpGetVariable_new(void)
{
	return Op_new(&OpGetVariable_isa);
}

OpIsa OpSetVariable_isa = {
		.name="SetVariable",
		.size=sizeof(OpSetVariable),
		.init = (void(*)(Op*))OpSetVariable_init,
		.terminate = (void(*)(Op*))OpSetVariable_terminate,
		.execute = (void(*)(Op*, OpContext*))OpSetVariable_execute
};

void OpSetVariable_init(OpSetVariable *self)
{
	Op_init(&self->super);
	self->value = NULL;
}

void OpSetVariable_terminate(OpSetVariable *self)
{
	Op_terminate(&self->super);
	OpSetVariable_set_value(self, NULL);
}

void OpSetVariable_set_variable_number(OpSetVariable *self, size_t n)
{
	self->variable_number = n;
}

void OpSetVariable_set_value(OpSetVariable *self, Op *op)
{
	if(self->value != NULL)
		Op_free(self->value);
	self->value = op;
}

void OpSetVariable_execute(OpSetVariable *self, OpContext *ctx)
{
	double v = NAN;
	if(self->value != NULL)
	{
		Op_execute(self->value, ctx);
		v = OpContext_get_current_value(ctx);
	}
	OpContext_set_variable_value(ctx, self->variable_number, v);
}

Op *OpSetVariable_new(void)
{
	return Op_new(&OpSetVariable_isa);
}

OpIsaTwoOp Op2_isa = {
		.super.name="Op2",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.execute = (void(*)(Op*, OpContext*))Op2_execute
};

void Op2_init(Op2 *self)
{
	Op_init(&self->super);
	self->operande1 = NULL;
	self->operande2 = NULL;
}

void Op2_terminate(Op2 *self)
{
	Op_terminate(&self->super);
	Op2_set_operande1(self, NULL);
	Op2_set_operande2(self, NULL);
}

void Op2_set_operande1(Op2 *self, Op *o)
{
	if(self->operande1 != NULL)
		free(self->operande1);
	self->operande1 = o;
}

void Op2_set_operande2(Op2 *self, Op *o)
{
	if(self->operande2 != NULL)
		free(self->operande2);
	self->operande2 = o;
}

void Op2_execute(Op2 *self, OpContext *ctx)
{
	double v1 = NAN, v2 = NAN, res;
	if(self->operande1 != NULL)
	{
		Op_execute(self->operande1, ctx);
		v1 = OpContext_get_current_value(ctx);
	}
	if(self->operande2 != NULL)
	{
		Op_execute(self->operande2, ctx);
		v2 = OpContext_get_current_value(ctx);
	}
	res = ((OpIsaTwoOp*)((Op*)self)->isa)->compute(v1, v2);
	printf("Op %p 2 operandes '%s' %f, %f => %f\n", self, ((Op*)self)->isa->name, v1, v2, res);
	OpContext_set_current_value(ctx, res);
}

double compute_add(double v1, double v2) { return v1+v2; }

OpIsaTwoOp OpAdd_isa = {
		.super.name="Add",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.execute = (void(*)(Op*, OpContext*))Op2_execute,
		.compute = compute_add
};

Op *OpAdd_new(void)
{
	return Op_new(&OpAdd_isa.super);
}

double compute_del(double v1, double v2) { return v1-v2; }

OpIsaTwoOp OpDel_isa = {
		.super.name="Del",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.execute = (void(*)(Op*, OpContext*))Op2_execute,
		.compute = compute_del
};

Op *OpDel_new(void)
{
	return Op_new(&OpDel_isa.super);
}

double compute_mult(double v1, double v2) { return v1*v2; }

OpIsaTwoOp OpMult_isa = {
		.super.name="Mult",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.execute = (void(*)(Op*, OpContext*))Op2_execute,
		.compute = compute_mult
};

Op *OpMult_new(void)
{
	return Op_new(&OpMult_isa.super);
}

double compute_div(double v1, double v2) { return v1/v2; }

OpIsaTwoOp OpDiv_isa = {
		.super.name="Div",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.execute = (void(*)(Op*, OpContext*))Op2_execute,
		.compute = compute_div
};

Op *OpDiv_new(void)
{
	return Op_new(&OpDiv_isa.super);
}

double compute_equ(double v1, double v2) { return double_eq(v1, v2) == 1 ? 1.0 : 0.0; }

OpIsaTwoOp OpEqu_isa = {
		.super.name="Equ",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.execute = (void(*)(Op*, OpContext*))Op2_execute,
		.compute = compute_equ
};

Op *OpEqu_new(void)
{
	return Op_new(&OpEqu_isa.super);
}

double compute_notequ(double v1, double v2) { return double_eq(v1, v2) == 1 ? 0.0 : 1.0; }

OpIsaTwoOp OpNotEqu_isa = {
		.super.name="NotEqu",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.execute = (void(*)(Op*, OpContext*))Op2_execute,
		.compute = compute_notequ
};

Op *OpNotEqu_new(void)
{
	return Op_new(&OpNotEqu_isa.super);
}

double compute_inf(double v1, double v2) { return double_eq(v1, v2) == 1 ? 0.0 : (v1 < v2); }

OpIsaTwoOp OpInf_isa = {
		.super.name="Inf",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.execute = (void(*)(Op*, OpContext*))Op2_execute,
		.compute = compute_inf
};

Op *OpInf_new(void)
{
	return Op_new(&OpInf_isa.super);
}

double compute_infequ(double v1, double v2) { return double_eq(v1, v2) == 1 ? 1.0 : (v1 < v2); }

OpIsaTwoOp OpInfEqu_isa = {
		.super.name="InfEqu",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.execute = (void(*)(Op*, OpContext*))Op2_execute,
		.compute = compute_infequ
};

Op *OpInfEqu_new(void)
{
	return Op_new(&OpInfEqu_isa.super);
}

double compute_sup(double v1, double v2) { return double_eq(v1, v2) == 1 ? 0.0 : (v1 > v2); }

OpIsaTwoOp OpSup_isa = {
		.super.name="Sup",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.execute = (void(*)(Op*, OpContext*))Op2_execute,
		.compute = compute_sup
};

Op *OpSup_new(void)
{
	return Op_new(&OpSup_isa.super);
}

double compute_supequ(double v1, double v2) { return double_eq(v1, v2) == 1 ? 1.0 : (v1 > v2); }

OpIsaTwoOp OpSupEqu_isa = {
		.super.name="SupEqu",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.execute = (void(*)(Op*, OpContext*))Op2_execute,
		.compute = compute_supequ
};

Op *OpSupEqu_new(void)
{
	return Op_new(&OpSupEqu_isa.super);
}
