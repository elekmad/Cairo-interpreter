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

void OpVariable_init(OpVariable *self)
{
	self->name = NULL;
	self->type = NONE;
	self->number_of_elements = 1;
	self->vs = NULL;
}

OpVariable *OpVariable_new(void)
{
	OpVariable *self = malloc(sizeof(OpVariable));
	if(self != NULL)
		OpVariable_init(self);
	return self;
}

void OpVariable_terminate(OpVariable *self)
{
	if(self->name != NULL)
	{
		free(self->name);
		self->name = NULL;
}
	OpVariables_empty(self);
}

void OpVariable_free(OpVariable *self)
{
	if(self != NULL)
	{
		OpVariable_terminate(self);
		free(self);
	}
}

void OpVariables_empty(OpVariable *self)
{
	switch(self->type)
	{
	case NONE :
					break;
	case DOUBLE :	self->v = NAN;
					break;
	case DOUBLES : 	if(self->vs != NULL)
						free(self->vs);
					self->vs = NULL;
					break;
	case STRING :	if(self->string != NULL)
						free(self->string);
					self->string = NULL;
					break;
	case STRINGS :	if(self->strings != NULL)
					{
						int cmpt;
						for(cmpt = 0; cmpt < self->number_of_elements; cmpt++)
						{
							if(self->strings[cmpt] != NULL)
								free(self->strings[cmpt]);
						}
					}
					free(self->strings);
					self->strings = NULL;
					break;
	}
}

int OpVariable_copy(OpVariable *self, OpVariable *other)
{
	if(self->type == NONE)
		OpVariable_set_type(self, other->type);
	if(self->type != other->type)
	{
		OpVariables_empty(self);
		OpVariable_set_type(self, other->type);
	}
	switch(self->type)
	{
	case NONE :		break;
	case DOUBLE :	self->v = other->v;
					break;
	case DOUBLES :	if(self->number_of_elements != other->number_of_elements)
					{
						self->vs = realloc(self->vs, sizeof(double) * other->number_of_elements);
						self->number_of_elements = other->number_of_elements;
					}
					memcpy(self->vs, other->vs, sizeof(double) * other->number_of_elements);
					break;
	case STRING :	if(self->number_of_elements != other->number_of_elements)
					{
						self->string = realloc(self->string, sizeof(char) * other->number_of_elements);
						self->number_of_elements = other->number_of_elements;
					}
					memcpy(self->string, other->string, sizeof(char) * other->number_of_elements);
					break;
	case STRINGS :	if(self->number_of_elements != other->number_of_elements)
					{
						OpVariables_empty(self);
						self->vs = malloc(sizeof(char*) * other->number_of_elements);
						self->number_of_elements = other->number_of_elements;
					}
					int cmpt;
					for(cmpt = 0; cmpt < self->number_of_elements; cmpt++)
					{
						self->strings[cmpt] = strdup(other->strings[cmpt]);
					}
					break;
	}
	return 0;
}

void OpVariable_print(OpVariable *self)
{
	int cmpt;
	printf("self %p name '%s' content :", self, self->name != NULL ? self->name : "null");
	switch(self->type)
	{
	case NONE : 	printf("empty type");
					break;
	case DOUBLE : 	printf("%f", self->v);
					break;
	case DOUBLES :	for(cmpt = 0; cmpt < self->number_of_elements; cmpt++)
						printf("%f, ", self->vs[cmpt]);
					break;
	case STRING :	printf("'%s'", self->string);
					break;
	case STRINGS :	for(cmpt = 0; cmpt < self->number_of_elements; cmpt++)
						printf("'%s', ", self->strings[cmpt]);
					break;
	}
	printf("\n");
}

void OpVariable_to_string(OpVariable *self, String *s)
{
	int cmpt;
	String_append_printf(s, "self %p name '%s' content : ", self, self->name != NULL ? self->name : "null");
	switch(self->type)
	{
	case NONE : 	String_append_printf(s, "empty type");
					break;
	case DOUBLE : 	String_append_printf(s, "%f", self->v);
					break;
	case DOUBLES :	for(cmpt = 0; cmpt < self->number_of_elements; cmpt++)
					{
						String_append_printf(s, "%f", self->vs[cmpt]);
						if(cmpt + 1 < self->number_of_elements)
							String_append_char_string(s, ", ");
					}
					break;
	case STRING :	String_append_printf(s, "'%s'", self->string);
					break;
	case STRINGS :	for(cmpt = 0; cmpt < self->number_of_elements; cmpt++)
					{
						String_append_printf(s, "'%s'", self->strings[cmpt]);
						if(cmpt + 1 < self->number_of_elements)
							String_append_char_string(s, ", ");
					}
					break;
	}
}

void OpVariable_set_type(OpVariable *self, OpVarType t)
{
	if(t != self->type)
	{
		OpVariables_empty(self);
		switch(t)
		{
		case NONE :		break;
		case DOUBLE :	self->v = NAN;
						self->number_of_elements = 1;
						break;
		case DOUBLES : 	self->vs = NULL;
						self->number_of_elements = 0;
						break;
		case STRING :	self->string = NULL;
						self->number_of_elements = 0;
						break;
		case STRINGS :	self->strings = NULL;
						self->number_of_elements = 0;
						break;
		}
		self->type = t;
	}
}

OpVarType OpVariable_get_type(OpVariable *self)
{
	return self->type;
}

size_t OpVariable_get_number_elements(OpVariable *self)
{
	return self->number_of_elements;
}

double OpVariable_get_double(OpVariable *self)
{
	if(self->type != DOUBLE)
		return NAN;
	return self->v;
}

double *OpVariable_get_doubles(OpVariable *self)
{
	if(self->type != DOUBLES)
		return NULL;
	return self->vs;
}

const char *OpVariable_get_string(OpVariable *self)
{
	if(self->type != STRING)
		return NULL;
	return (const char*)self->string;
}

const char * const *OpVariable_get_strings(OpVariable *self)
{
	if(self->type != STRINGS)
		return NULL;
	return (const char *const*)self->strings;
}

void OpVariable_set_name(OpVariable *self, const char *name)
{
	if(self->name != NULL)
		free(self->name);
	self->name = strdup(name);
}

const char *OpVariable_get_name(OpVariable *self)
{
	return self->name;
}

int OpVariable_set_double(OpVariable *self, double v)
{
	if(self->type == NONE)
		OpVariable_set_type(self, DOUBLE);
	if(self->type != DOUBLE)
		return -1;
	self->v = v;
	return 0;
}

int OpVariable_append_double(OpVariable *self, double v)
{
	if(self->type == NONE)
		OpVariable_set_type(self, DOUBLES);
	if(self->type != DOUBLES)
		return -1;
	self->vs = realloc(self->vs, sizeof(double) * (self->number_of_elements + 1));
	self->vs[self->number_of_elements] = v;
	self->number_of_elements++;
	return 0;
}

int OpVariable_set_string(OpVariable *self, const char *v)
{
	size_t size;
	if(self->type == NONE)
		OpVariable_set_type(self, STRING);
	if(self->type != STRING)
		return -1;
	size = strlen(v) + 1;
	if(self->number_of_elements < size)
		self->string = realloc(self->string, sizeof(char) * size);
	strcpy(self->string, v);
	self->number_of_elements = size;
	return 0;
}

int OpVariable_append_string(OpVariable *self, const char *v)
{
	if(self->type == NONE)
		OpVariable_set_type(self, STRINGS);
	if(self->type != STRINGS)
		return -1;
	self->strings = realloc(self->strings, sizeof(char*) * (self->number_of_elements + 1));
	self->strings[self->number_of_elements] = strdup(v);
	self->number_of_elements++;
	return 0;
}

bool double_eq(double a, double b)
{
    double diff = fabs(a - b);

    double norm =
        fmax(fabs(a), fabs(b));

    return diff <= norm * 1e-12;
}

void OpContext_init(OpContext *self)
{
	self->state = Init;
	OpVariable_init(&self->current_value);
	self->variables = NULL;
	self->number_of_variables = 0;
}

void OpContext_terminate(OpContext *self)
{
	if(self->number_of_variables > 0)
	{
		if(self->variables != NULL)
		{
			int i;
			for(i = 0; i < self->number_of_variables; i++)
			{
				if(self->variables[i] != NULL)
					OpVariable_free(self->variables[i]);
			}
			free(self->variables);
			self->variables = NULL;
		}
	}
}

void OpContext_set_running_state(OpContext *self, Op *sender, OpRunningState state, const char *message)
{
	printf("Context %p setting state %d '%s' @%d:%d\n", self, state, message, sender->pos.first_line, sender->pos.first_column);
	self->state = state;
}

OpRunningState OpContext_get_running_state(OpContext *self)
{
	return self->state;
}

void OpContext_empty_current_value(OpContext *self)
{
	OpVariables_empty(&self->current_value);
}


void OpContext_set_current_value_double(OpContext *self, double v)
{
	if(OpVariable_set_double(&self->current_value, v) == -1)
	{
		OpVariable_set_type(&self->current_value, DOUBLE);
		OpVariable_set_double(&self->current_value, v);
	}
}

double OpContext_get_current_value_double(OpContext *self)
{
	return OpVariable_get_double(&self->current_value);
}


void OpContext_set_current_value_string(OpContext *self, const char *v)
{
	if(OpVariable_set_string(&self->current_value, v) == -1)
	{
		OpVariable_set_type(&self->current_value, STRING);
		OpVariable_set_string(&self->current_value, v);
	}
}


void OpContext_set_current_value_doubles(OpContext *self, double *v, size_t n)
{
	int cmpt;
	OpVariable_set_type(&self->current_value, DOUBLES);
	for(cmpt = 0; cmpt < n; cmpt++)
		OpVariable_append_double(&self->current_value, v[cmpt]);
}

void OpContext_set_current_value_strings(OpContext *self, const char * const *v, size_t n)
{
	int cmpt;
	OpVariable_set_type(&self->current_value, STRINGS);
	for(cmpt = 0; cmpt < n; cmpt++)
		OpVariable_append_string(&self->current_value, v[cmpt]);
}


int OpContext_append_current_value_doubles(OpContext *self, double v)
{
	if(OpVariable_get_type(&self->current_value) != DOUBLES)
		return -1;
	OpVariable_set_type(&self->current_value, DOUBLES);
	OpVariable_append_double(&self->current_value, v);
	return 0;
}

int OpContext_append_current_value_strings(OpContext *self, const char *v)
{
	if(OpVariable_get_type(&self->current_value) != STRINGS)
		return -1;
	OpVariable_set_type(&self->current_value, STRINGS);
	OpVariable_append_string(&self->current_value, v);
	return 0;
}

size_t OpContext_get_variable_number(OpContext *self, const char *name)
{
	if(self->number_of_variables > 0)
	{
		size_t i;
		int found = 0;
		for(i = 0; i < self->number_of_variables; i++)
		{
			OpVariable *v = self->variables[i];
			if(strcmp(OpVariable_get_name(v), name) == 0)
			{
				found = 1;
				return i;
			}
		}
	}
	size_t place;
	place = self->number_of_variables;
	self->number_of_variables++;
	self->variables = realloc(self->variables, self->number_of_variables * sizeof(OpVariable*));
	OpVariable *v = OpVariable_new();
	OpVariable_set_name(v, name);
	self->variables[place] = v;
	return place;
}


int OpContext_set_variable_value_double(OpContext *self, size_t variable_number, double v)
{
	OpVariable *var;
	if(self->number_of_variables < variable_number + 1)
		return -1;
	var = self->variables[variable_number];
	if(OpVariable_set_double(var, v) == -1)
	{
		OpVariable_set_type(var, DOUBLE);
		OpVariable_set_double(var, v);
	}

	printf("Setting variable '%s' number %zu = %f\n", OpVariable_get_name(var), variable_number, v);
	return 0;
}

int OpContext_copy_current_to_variable(OpContext *self, size_t var_num)
{
	OpVariable *var;
	if(self->number_of_variables < var_num + 1)
		return -1;
	var = self->variables[var_num];

	printf("Copying Internal state to '%s'@%zu\n", OpVariable_get_name(var), var_num);
	OpVariable_print(&self->current_value);
	if(OpVariable_copy(var, &self->current_value) == -1)
	{
		printf("Error copy from Internal State to '%s'@%zu two different types\n", OpVariable_get_name(var), var_num);
		return -1;
	}
	OpVariable_print(var);
	return 0;
}

OpVariable *OpContext_get_current_value(OpContext *self)
{
	return &self->current_value;
}

void OpContext_copy_variable_to_current_value(OpContext *self, OpVariable *var)
{
	OpVariable_copy(&self->current_value, var);
}

OpVariable *OpContext_get_variable(OpContext *self, size_t variable_number)
{
	OpVariable *var;
	if(self->number_of_variables < variable_number + 1)
		return NULL;
	var = self->variables[variable_number];
	printf("Getting variable '%s' number %zu\n", OpVariable_get_name(var), variable_number);
	OpVariable_print(var);
	return var;
}

double OpContext_get_variable_value(OpContext *self, size_t variable_number)
{
	OpVariable *var;
	double val = NAN;
	if(self->number_of_variables < variable_number + 1)
		return NAN;
	var = self->variables[variable_number];
	if(OpVariable_get_type(var) != DOUBLE)
		return NAN;
	val = OpVariable_get_double(var);
	printf("Getting variable '%s' number %zu = %f\n", OpVariable_get_name(var), variable_number, val);
	return val;
}

void Op_init(Op *self)
{
	self->for_prerunning = false;
	self->operandes = NULL;
	self->nb_ops = 0;
}

void Op_set_nb_ops(Op *self, size_t nb)
{
	if(nb > self->nb_ops)
	{
		self->operandes = realloc(self->operandes, sizeof(Op*) * nb);
		while(self->nb_ops < nb)
		{
			self->operandes[self->nb_ops] = NULL;
			self->nb_ops++;
		}
	}
}

int Op_fix_operandes(Op *self, OpContext *ctx)
{
	int ret = 0;
	if(self->isa->fix_operandes != NULL)
		ret = self->isa->fix_operandes(self, ctx);
	if(ret == 0)
	{
		size_t cmpt;
		for(cmpt = 0; cmpt < self->nb_ops; cmpt++)
		{
			Op *o = self->operandes[cmpt];
			if(o != NULL)
			{
				ret = Op_fix_operandes(o, ctx);
				if(ret != 0)
					break;
			}
		}
	}
	return ret;
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

void Op_set_for_prerunning(Op *self)
{
	size_t cmpt;
	self->for_prerunning = true;
	for(cmpt = 0; cmpt < self->nb_ops; cmpt++)
		Op_set_for_prerunning(self->operandes[cmpt]);
}

void Op_set_source_pos(Op *self, int fl, int fc, int ll, int lc)
{
	self->pos.first_line = fl;
	self->pos.first_column = fc;
	self->pos.last_line = ll;
	self->pos.last_column = lc;
}

void Op_free(Op *self)
{
	if(self != NULL)
	{
		self->isa->terminate(self);
		free(self);
	}
}

int Op_prerun(Op *self, OpContext *ctx)
{
	int ret = -1;
	OpRunningState state = OpContext_get_running_state(ctx);
	if(state != Init)
	{
		printf("Prerun Op %p Ctx %p failed : Ctx bad state : %d\n", self, ctx, state);
		return ret;
	}
	String m;
	String_init(&m);
	String_append_printf(&m, "Prerunning Op %p", self);
	OpContext_set_running_state(ctx, self, PreRun, String_get_char_string(&m));

	ret = Op_execute(self, ctx);
	String_empty(&m);
	String_append_printf(&m, "Prerunning completed Op %p return %d", self, ret);
	OpContext_set_running_state(ctx, self, Ready, String_get_char_string(&m));
	String_finalize(&m);
	return ret;
}

int Op_launch(Op *self, OpContext *ctx)
{
	int ret = -1;
	OpRunningState state = OpContext_get_running_state(ctx);
	if(state != Ready)
	{
		printf("Launching Op %p Ctx %p failed : Ctx bad state : %d\n", self, ctx, state);
		return ret;
	}
	String m;
	String_init(&m);
	String_append_printf(&m, "Launching Op %p", self);
	OpContext_set_running_state(ctx, self, Run, String_get_char_string(&m));

	ret = Op_execute(self, ctx);
	String_empty(&m);
	String_append_printf(&m, "Execution completed Op %p return %d", self, ret);
	OpContext_set_running_state(ctx, self, Finished, String_get_char_string(&m));
	String_finalize(&m);
	return ret;
}

int Op_execute_get_double(Op *self, Op *sender, OpContext *ctx, double *d)
{
	int ret;
	OpVariable *c;
	OpVarType t;

	ret = Op_execute(self, ctx);
	if(ret == 0)
	{
		c = OpContext_get_current_value(ctx);
		t = OpVariable_get_type(c);
		if(t == DOUBLE)
			*d = OpVariable_get_double(c);
		else if(t == DOUBLES)
		{
			if(OpVariable_get_number_elements(c) > 0)
				*d = OpVariable_get_doubles(c)[0];
			else
			{
				String m;
				String_init(&m);
				String_append_printf(&m, "%s not enough objects in list", sender->isa->name);
				OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&m));
				String_finalize(&m);
				ret = -1;
			}
		}
		else
		{
			String m;
			String_init(&m);
			String_append_printf(&m, "%s bad operande type", sender->isa->name);
			OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&m));
			String_finalize(&m);
			ret = -1;
		}
	}
	return ret;
}

int Op_execute_get_doubles(Op *self, Op *sender, OpContext *ctx, double **d, size_t *nb, size_t expect)
{
	int ret;
	OpVariable *c;
	OpVarType t;

	ret = Op_execute(self, ctx);
	if(ret == 0)
	{
		c = OpContext_get_current_value(ctx);
		t = OpVariable_get_type(c);
		if(t == DOUBLES)
		{
			*nb = OpVariable_get_number_elements(c);
			if(*nb >= expect)
				*d = OpVariable_get_doubles(c);
			else
			{
				String m;
				String_init(&m);
				String_append_printf(&m, "Not enough objects for %s : %zu < %zu", sender->isa->name, *nb, expect);
				OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&m));
				String_finalize(&m);
				ret = -1;
			}
		}
		else
		{
			String m;
			String_init(&m);
			String_append_printf(&m, "%s bad operandes", sender->isa->name);
			OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&m));
			String_finalize(&m);
			ret = -1;
		}
	}
	return ret;
}

int Op_execute_get_string(Op *self, Op *sender, OpContext *ctx, const char **s)
{
	int ret;
	OpVariable *c;
	OpVarType t;

	ret = Op_execute(self, ctx);
	if(ret == 0)
	{
		c = OpContext_get_current_value(ctx);
		t = OpVariable_get_type(c);
		if(t == STRING)
			*s = OpVariable_get_string(c);
		else if(t == DOUBLES)
		{
			if(OpVariable_get_number_elements(c) > 0)
				*s = OpVariable_get_strings(c)[0];
			else
			{
				String m;
				String_init(&m);
				String_append_printf(&m, "%s not enough objects in list", sender->isa->name);
				OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&m));
				String_finalize(&m);
				ret = -1;
			}
		}
		else
		{
			String m;
			String_init(&m);
			String_append_printf(&m, "%s bad operande type", sender->isa->name);
			OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&m));
			String_finalize(&m);
			ret = -1;
		}
	}
	return ret;
}

int Op_execute_get_strings(Op *self, Op *sender, OpContext *ctx, const char * const **s, size_t *nb, size_t expect)
{
	int ret;
	OpVariable *c;
	OpVarType t;

	ret = Op_execute(self, ctx);
	if(ret == 0)
	{
		c = OpContext_get_current_value(ctx);
		t = OpVariable_get_type(c);
		if(t == STRINGS)
		{
			*nb = OpVariable_get_number_elements(c);
			if(*nb >= expect)
				*s = OpVariable_get_strings(c);
			else
			{
				String m;
				String_init(&m);
				String_append_printf(&m, "Not enough objects for %s : %zu < %zu", sender->isa->name, *nb, expect);
				OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&m));
				String_finalize(&m);
				ret = -1;
			}
		}
		else
		{
			String m;
			String_init(&m);
			String_append_printf(&m, "%s bad operandes", sender->isa->name);
			OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&m));
			String_finalize(&m);
			ret = -1;
		}
	}
	return ret;
}

int Op_execute(Op *self, OpContext *ctx)
{
	int ret = 0;
	if(self != NULL && ctx != NULL)
	{
		OpRunningState state = OpContext_get_running_state(ctx);
		printf("Op %p : %s\n", self, self->isa->name);
		if(state == Run)
		{
			if(self->isa->check_args != NULL)
			{
				ret = self->isa->check_args(self, ctx);
				printf("Checking Args of %p %p : %d\n", self, ctx, ret);
			}
			if(ret == 0)
				ret = self->isa->execute(self, ctx);
		}
		else if(state == PreRun && self->for_prerunning == true)
		{
			if(self->isa->check_args != NULL)
			{
				ret = self->isa->check_args(self, ctx);
				printf("Checking Args of %p %p : %d\n", self, ctx, ret);
			}
			if(ret == 0)
				ret = self->isa->execute(self, ctx);
		}
		else
			printf("Op %p Not executed because Ctx state %d\n", self, state);
		printf("Fin Op %p : %s\n", self, self->isa->name);
	}
	return ret;
}

OpIsa OpBloc_isa = {
		.name="bloc",
		.size=sizeof(OpBloc),
		.init = (void(*)(Op*))OpBloc_init,
		.terminate = (void(*)(Op*))OpBloc_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpBloc_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpBloc_execute,
		.check_args = NULL
};

void OpBloc_init(OpBloc *self)
{
	Op_init(&self->super);
	LinkedList_init(&self->ops);
}

void OpBloc_append_Op(OpBloc *self, Op *op)
{
	Op *s = (Op*)self;
	size_t n = s->nb_ops;
	Op_set_nb_ops(s, n + 1);
	s->operandes[n] = op;

}

int OpBloc_fix_operandes(OpBloc *self, OpContext *ctx)
{
	int ret = 0;
	size_t cmpt;
	for(cmpt = 0; cmpt < ((Op*)self)->nb_ops; cmpt++)
	{
		Op *op = ((Op*)self)->operandes[cmpt];
		LinkedList_append(&self->ops, op);
	}
	return ret;
}

int OpBloc_execute(OpBloc *self, OpContext *ctx)
{
	int ret = 0;

	//Remplacer par un iterateur à construire, c'est plus propre !
	LinkedList_do_to_all(&self->ops, (void(*)(void*, void*))Op_execute, ctx);
	if(OpContext_get_running_state(ctx) == Error)
		ret = -1;
	return ret;
}

void OpBloc_terminate(OpBloc *self)
{
	Op_terminate(&self->super);
	LinkedList_do_to_all(&self->ops, (void(*)(void*, void*))Op_free, NULL);
	LinkedList_finalize(&self->ops);
}

Op *OpBloc_new(void)
{
	return Op_new(&OpBloc_isa);
}

OpIsa OpMessage_isa = {
		.name="Message",
		.size=sizeof(OpMessage),
		.init = (void(*)(Op*))OpMessage_init,
		.terminate = (void(*)(Op*))OpMessage_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpMessage_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpMessage_execute,
		.check_args = NULL
};

void OpMessage_init(OpMessage *self)
{
	Op_init(&self->super);
	self->message = NULL;
	self->value = NULL;
}

#define OPMESSAGE_VAUE 0

void OpMessage_set_value(OpMessage *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OPMESSAGE_VAUE);
}

void _OpMessage_set_value(OpMessage *self, Op *op)
{
	OP_SET_OPERANDE(self, value, op);
}

int OpMessage_fix_operandes(OpMessage *self, OpContext *ctx)
{
	int ret = 0;
	Op *v = ((Op*)self)->operandes[OPMESSAGE_VAUE];
	_OpMessage_set_value(self, v);
	return ret;
}

void OpMessage_set_message(OpMessage *self, const char *m)
{
	if(self->message != NULL)
		free(self->message);
	if(m != NULL)
		self->message = strdup(m);
	else
		self->message = NULL;
}

int OpMessage_execute(OpMessage *self, OpContext *ctx)
{
	int ret = 0;
	OpVariable val;
	String s;

	if(self->value != NULL && self->message != NULL)
	{
		OpVariable_init(&val);
		String_init(&s);
		ret = Op_execute(self->value, ctx);
		if(ret == 0)
		{
			OpVariable_copy(&val, OpContext_get_current_value(ctx));
			String_append_printf(&s, "Message '%s' : ", self->message);
			OpVariable_to_string(&val, &s);
			printf("%s\n", String_get_char_string(&s));
		}
		String_finalize(&s);
		OpVariable_terminate(&val);
	}
	return ret;
}

void OpMessage_terminate(OpMessage *self)
{
	Op_terminate(&self->super);
	_OpMessage_set_value(self, NULL);
	OpMessage_set_message(self, NULL);
}

Op *OpMessage_new(void)
{
	return Op_new(&OpMessage_isa);
}


OpIsa OpGetValue_isa = {
		.name="GetValue",
		.size=sizeof(OpGetValue),
		.init = (void(*)(Op*))OpGetValue_init,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,
		.terminate = (void(*)(Op*))OpGetValue_terminate,
		.execute = (int(*)(Op*, OpContext*))OpGetValue_execute,
		.check_args = NULL
};

void OpGetValue_init(OpGetValue *self)
{
	Op_init(&self->super);
	OpVariable_init(&self->value);
}

void OpGetValue_terminate(OpGetValue *self)
{
	Op_terminate(&self->super);
	OpVariable_terminate(&self->value);
}

void OpGetValue_set_value(OpGetValue *self, double v)
{
	OpVariable_set_double(&self->value, v);
}


void OpGetValue_set_value_string(OpGetValue *self, const char * v)
{
	OpVariable_set_string(&self->value, v);
}

void OpGetValue_copy_variable(OpGetValue *self, OpVariable *v)
{
	OpVariable_copy(&self->value, v);
}

int OpGetValue_execute(OpGetValue *self, OpContext *ctx)
{
	OpContext_copy_variable_to_current_value(ctx, &self->value);
	return 0;
}

Op *OpGetValue_new(void)
{
	return Op_new(&OpGetValue_isa);
}

OpIsa OpPi_isa = {
		.name="Pi",
		.size=sizeof(OpPi),
		.init = (void(*)(Op*))OpPi_init,
		.terminate = (void(*)(Op*))OpPi_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,
		.execute = (int(*)(Op*, OpContext*))OpPi_execute,
		.check_args = NULL
};

void OpPi_init(OpPi *self)
{
	Op_init(&self->super);
}

void OpPi_terminate(OpPi *self)
{
	Op_terminate(&self->super);
}

int OpPi_execute(OpPi *self, OpContext *ctx)
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
		.size=sizeof(OpPhi),
		.init = (void(*)(Op*))OpPhi_init,
		.terminate = (void(*)(Op*))OpPhi_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,
		.execute = (int(*)(Op*, OpContext*))OpPhi_execute,
		.check_args = NULL
};

void OpPhi_init(OpPhi *self)
{
	Op_init(&self->super);
}

void OpPhi_terminate(OpPhi *self)
{
	Op_terminate(&self->super);
}

int OpPhi_execute(OpPhi *self, OpContext *ctx)
{
	OpContext_set_current_value_double(ctx, (1+sqrt(5))/2);
	return 0;
}

Op *OpPhi_new(void)
{
	return Op_new(&OpPhi_isa);
}

OpIsa OpIf_isa = {
		.name="IfThenElse",
		.size=sizeof(OpIf),
		.init = (void(*)(Op*))OpIf_init,
		.terminate = (void(*)(Op*))OpIf_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpIf_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpIf_execute,
		.check_args = (int(*)(Op*, OpContext*))OpIf_check_args
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
	_OpIf_set_condition(self, NULL);
	_OpIf_set_true_branch(self, NULL);
	_OpIf_set_false_branch(self, NULL);
}

#define OPIF_CONDITION 0
#define OP_IF_TRUE_BRANCH 1
#define OP_IF_FALSE_BRANCH 2

void OpIf_set_condition(OpIf *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OPIF_CONDITION);
}

void OpIf_set_true_branch(OpIf *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OP_IF_TRUE_BRANCH);
}

void OpIf_set_false_branch(OpIf *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OP_IF_FALSE_BRANCH);
}

void _OpIf_set_condition(OpIf *self, Op *op)
{
	OP_SET_OPERANDE(self, condition, op);
}

void _OpIf_set_true_branch(OpIf *self, Op *op)
{
	OP_SET_OPERANDE(self, true_branch, op);
}

void _OpIf_set_false_branch(OpIf *self, Op *op)
{
	OP_SET_OPERANDE(self, false_branch, op);
}

int OpIf_fix_operandes(OpIf *self, OpContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >=2)
	{
		ret = 0;
		_OpIf_set_condition(self, self->super.operandes[OPIF_CONDITION]);
		_OpIf_set_true_branch(self, self->super.operandes[OP_IF_TRUE_BRANCH]);
		if(self->super.nb_ops >= 3)
			_OpIf_set_false_branch(self, self->super.operandes[OP_IF_FALSE_BRANCH]);
	}
	return ret;
}

int OpIf_check_args(OpIf *self, OpContext *ctx)
{
	int ret = -1;
	if(self->condition != NULL && self->true_branch != NULL)
		return 0;
	return ret;
}

int OpIf_execute(OpIf *self, OpContext *ctx)
{
	int ret = 0;
	double c = NAN;
	printf("If %p Entered\n", self);

	ret = Op_execute_get_double(self->condition, (Op*)self, ctx, &c);
	if(ret == -1)
	{
		printf("Error during if condifion\n");
		return -1;

	}
	printf("If %p Condition = %f\n", self, c);

	if(c != 0)
	{
			printf("If %p Branch True\n", self);
			ret = Op_execute(self->true_branch, ctx);
	}
	else
	{
		if(self->false_branch != NULL)
		{
			printf("If %p Branch False\n", self);
			ret = Op_execute(self->false_branch, ctx);
		}
	}
	printf("If %p Exited\n", self);
	return ret;
}

Op *OpIf_new(void)
{
	return Op_new(&OpIf_isa);
}

OpIsa OpWhile_isa = {
		.name="While",
		.size=sizeof(OpWhile),
		.init = (void(*)(Op*))OpWhile_init,
		.terminate = (void(*)(Op*))OpWhile_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpWhile_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpWhile_execute,
		.check_args = (int(*)(Op*, OpContext*))OpWhile_check_args
};

void OpWhile_init(OpWhile *self)
{
	Op_init(&self->super);
	self->condition = NULL;
	self->bloc = NULL;
}

void OpWhile_terminate(OpWhile *self)
{
	Op_terminate(&self->super);
	_OpWhile_set_condition(self, NULL);
	_OpWhile_set_bloc(self, NULL);
}

#define OPWHILE_CONDITION 0
#define OPWHILE_BLOC 1

void OpWhile_set_condition(OpWhile *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OPWHILE_CONDITION);
}

void OpWhile_set_bloc(OpWhile *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OPWHILE_BLOC);
}

void _OpWhile_set_condition(OpWhile *self, Op *op)
{
	OP_SET_OPERANDE(self, condition, op);
}

void _OpWhile_set_bloc(OpWhile *self, Op *op)
{
	OP_SET_OPERANDE(self, bloc, op);
}

int OpWhile_check_args(OpWhile *self, OpContext *ctx)
{
	int ret = -1;
	if(self->bloc != NULL && self->condition != NULL)
		return 0;
	return ret;
}

int OpWhile_fix_operandes(OpWhile *self, OpContext *ctx)
{
	int ret = -1;

	if(self->super.nb_ops >= 2)
	{
		ret = 0;
		_OpWhile_set_condition(self, self->super.operandes[OPWHILE_CONDITION]);
		_OpWhile_set_bloc(self, self->super.operandes[OPWHILE_BLOC]);
	}
	return ret;
}

int OpWhile_execute(OpWhile *self, OpContext *ctx)
{
	int ret = 0;
	double c = NAN;
	printf("While %p Entered\n", self);
	if(self->condition != NULL && self->bloc != NULL)
	{
		ret = Op_execute(self->condition, ctx);
		c = OpContext_get_current_value_double(ctx);
		printf("While %p Condition = %f\n", self, c);
		while(double_eq(c, 0.0) != 1)
		{
			printf("While %p Execute bloc\n", self);
			ret = Op_execute(self->bloc, ctx);
			ret = Op_execute(self->condition, ctx);
			c = OpContext_get_current_value_double(ctx);
			printf("While %p Condition = %f\n", self, c);
		}
	}
	printf("While %p Exited\n", self);
	return ret;
}

Op *OpWhile_new(void)
{
	return Op_new(&OpWhile_isa);
}


OpIsa OpForLoop_isa = {
		.name="ForLoop",
		.size=sizeof(OpForLoop),
		.init = (void(*)(Op*))OpForLoop_init,
		.terminate = (void(*)(Op*))OpForLoop_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpForLoop_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpForLoop_execute,
		.check_args = (int(*)(Op*, OpContext*))OpForLoop_check_args
};

void OpForLoop_init(OpForLoop *self)
{
	Op_init(&self->super);
	self->condition = NULL;
	self->start = NULL;
	self->step = NULL;
	self->loop = NULL;
	self->variable_number = 0;
	Op_set_nb_ops(&self->super, 1);
}

void OpForLoop_terminate(OpForLoop *self)
{
	Op_terminate(&self->super);
	_OpForLoop_set_start(self, NULL);
	_OpForLoop_set_condition(self, NULL);
	_OpForLoop_set_step(self, NULL);
	_OpForLoop_set_loop(self, NULL);
}


#define FORLOOP_START 0
#define FORLOOP_CONDITION 1
#define FORLOOP_STEP 2
#define FORLOOP_LOOP 3


void OpForLoop_set_variable_number(OpForLoop *self, size_t v)
{
	self->variable_number = v;
}

void OpForLoop_set_start(OpForLoop *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, FORLOOP_START);
}

void _OpForLoop_set_start(OpForLoop *self, Op *op)
{
	OP_SET_OPERANDE(self, start, op);
}

void OpForLoop_set_condition(OpForLoop *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, FORLOOP_CONDITION);
}

void _OpForLoop_set_condition(OpForLoop *self, Op *op)
{
	OP_SET_OPERANDE(self, condition, op);
}

void OpForLoop_set_step(OpForLoop *self, Op *v)
{
	OP_ADD_OPERANDE(self, v, FORLOOP_STEP);
}

void _OpForLoop_set_step(OpForLoop *self, Op *op)
{
	OP_SET_OPERANDE(self, step, op);
}

void OpForLoop_set_loop(OpForLoop *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, FORLOOP_LOOP);
}

void _OpForLoop_set_loop(OpForLoop *self, Op *op)
{
	OP_SET_OPERANDE(self, loop, op);
}

int OpForLoop_fix_operandes(OpForLoop *self, OpContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >= FORLOOP_LOOP)
	{
		ret = 0;
		Op *o = self->super.operandes[FORLOOP_LOOP];
		_OpForLoop_set_loop(self, o);
		o = self->super.operandes[FORLOOP_START];
		_OpForLoop_set_start(self, o);
		o = self->super.operandes[FORLOOP_STEP];
		_OpForLoop_set_step(self, o);
		o = self->super.operandes[FORLOOP_CONDITION];
		_OpForLoop_set_condition(self, o);
	}
	return ret;
}

int OpForLoop_check_args(OpForLoop *self, OpContext *ctx)
{
	int ret = -1;
	if(self->start != NULL && self->condition != NULL && self->loop != NULL)//By default step = 1
		return 0;
	return ret;
}

int OpForLoop_execute(OpForLoop *self, OpContext *ctx)
{
	int ret = 0;
	double start, condition, step = 1.0;

	ret = Op_execute_get_double(self->start, (Op*)self, ctx, &start);
	if(ret == -1)
		return -1;

	ret = Op_execute_get_double(self->condition, (Op*)self, ctx, &condition);
	if(ret == -1)
		return -1;

	if(self->step != NULL)
	{
		ret = Op_execute_get_double(self->step, (Op*)self, ctx, &step);
		if(ret == -1)
			return -1;
	}

	bool stop = false;
	double c = NAN, v;
	printf("ForLoop %p Start\n", self);
	v = start;
	printf("ForLoop %p start op : @%zu = %f\n", self, self->variable_number, v);
	OpContext_set_variable_value_double(ctx, self->variable_number, v);
	do
	{
		printf("ForLoop %p Condition\n", self);
		c = condition;
		v = OpContext_get_variable_value(ctx, self->variable_number);
		printf("ForLoop %p condition op : @%zu = %f\n", self, self->variable_number, v);
		stop = double_eq(v, c) ? false : v > c ? true : false;
		if(!stop)
		{
			printf("ForLoop %p Loop\n", self);
			ret = Op_execute(self->loop, ctx);
			printf("ForLoop %p End Loop\n", self);
			printf("ForLoop %p Step\n", self);
			v = v + step;
			OpContext_set_variable_value_double(ctx, self->variable_number, v);
			printf("ForLoop %p step op : @%zu = %f\n", self, self->variable_number, v);
		}
	}
	while(!stop);
	printf("ForLoop %p Stopped\n", self);

	return ret;
}

Op *OpForLoop_new(void)
{
	return Op_new(&OpForLoop_isa);
}

OpIsa OpIntervalGen_isa = {
		.name="IntervalGen",
		.size=sizeof(OpIntervalGen),
		.init = (void(*)(Op*))OpIntervalGen_init,
		.terminate = (void(*)(Op*))OpIntervalGen_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpIntervalGen_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpIntervalGen_execute,
		.check_args = (int(*)(Op*, OpContext*))OpIntervalGen_check_args,
};

void OpIntervalGen_init(OpIntervalGen *self)
{
	Op_init(&self->super);
	self->min = NULL;
	self->max = NULL;
	self->step = NULL;
}

void OpIntervalGen_terminate(OpIntervalGen *self)
{
	Op_terminate(&self->super);
	_OpIntervalGen_set_min(self, NULL);
	_OpIntervalGen_set_max(self, NULL);
	_OpIntervalGen_set_step(self, NULL);
}

#define OPINTERVALGEN_MIN 0
#define OPINTERVALGEN_MAX 1
#define OPINTERVALGEN_STEP 2

void OpIntervalGen_set_min(OpIntervalGen *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OPINTERVALGEN_MIN);
}

void OpIntervalGen_set_max(OpIntervalGen *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OPINTERVALGEN_MAX);
}

void OpIntervalGen_set_step(OpIntervalGen *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OPINTERVALGEN_STEP);
}

void _OpIntervalGen_set_min(OpIntervalGen *self, Op *op)
{
	OP_SET_OPERANDE(self, min, op);
}

void _OpIntervalGen_set_max(OpIntervalGen *self, Op *op)
{
	OP_SET_OPERANDE(self, max, op);
}

void _OpIntervalGen_set_step(OpIntervalGen *self, Op *op)
{
	OP_SET_OPERANDE(self, step, op);
}

int OpIntervalGen_fix_operandes(OpIntervalGen *self, OpContext *ctx)
{
	int ret = -1;

	if(self->super.nb_ops >= 1)
	{
		ret = 0;
		_OpIntervalGen_set_min(self, self->super.operandes[OPINTERVALGEN_MIN]);
		if(self->super.nb_ops >= 2)
		{
			_OpIntervalGen_set_max(self, self->super.operandes[OPINTERVALGEN_MAX]);
			if(self->super.nb_ops >= 3)
				_OpIntervalGen_set_step(self, self->super.operandes[OPINTERVALGEN_STEP]);
		}
	}
	return ret;
}


int OpIntervalGen_check_args(OpIntervalGen *self, OpContext *ctx)
{
	int ret = -1;
	if(self->min != NULL && self->max != NULL)//[min : max] or [min : max : step]
		return 0;
	return ret;
}

int OpIntervalGen_execute(OpIntervalGen *self, OpContext *ctx)
{
	int ret = 0;
	double min, max = NAN, step = NAN;
	ret = Op_execute_get_double(self->min, (Op*)self, ctx, &min);
	if(ret == 0)
	{
		double max;
		ret = Op_execute_get_double(self->max, (Op*)self, ctx, &max);
		if(ret == 0)
		{
			if(self->step != NULL)
			{
				double step;
				ret = Op_execute_get_double(self->step, (Op*)self, ctx, &step);
				if(ret == 0)
				{
					OpVariable v;
					OpVariable_init(&v);
					while(min <= max)
					{
						OpVariable_append_double(&v, min);
						min += step;
					}
					OpContext_copy_variable_to_current_value(ctx, &v);
					OpVariable_terminate(&v);
				}
			}
			else
			{
				OpVariable v;
				OpVariable_init(&v);
				while(min <= max)
				{
					OpVariable_append_double(&v, min);
					min += 1.0;
				}
				OpContext_copy_variable_to_current_value(ctx, &v);
				OpVariable_terminate(&v);
			}
		}
	}
	return ret;
}

Op *OpIntervalGen_new(void)
{
	return Op_new(&OpIntervalGen_isa);
}

OpIsa OpGetVariable_isa = {
		.name="GetVariable",
		.size=sizeof(OpGetVariable),
		.init = (void(*)(Op*))OpGetVariable_init,
		.terminate = (void(*)(Op*))OpGetVariable_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,
		.execute = (int(*)(Op*, OpContext*))OpGetVariable_execute,
		.check_args = NULL
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

int OpGetVariable_execute(OpGetVariable *self, OpContext *ctx)
{
	int ret = 0;
	OpVariable *var = OpContext_get_variable(ctx, self->variable_number);
	printf("OP %p Getting variable number %zu\n", self, self->variable_number);
	OpContext_copy_variable_to_current_value(ctx, var);
	OpVariable_print(OpContext_get_current_value(ctx));
	return ret;
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
		.fix_operandes = (int(*)(Op*, OpContext*))OpSetVariable_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpSetVariable_execute,
		.check_args = NULL
};

void OpSetVariable_init(OpSetVariable *self)
{
	Op_init(&self->super);
	self->value = NULL;
}

void OpSetVariable_terminate(OpSetVariable *self)
{
	Op_terminate(&self->super);
	_OpSetVariable_set_value(self, NULL);
}

void OpSetVariable_set_variable_number(OpSetVariable *self, size_t n)
{
	self->variable_number = n;
}

#define OPSETVAR_VALUE 0

void OpSetVariable_set_value(OpSetVariable *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OPSETVAR_VALUE);
}

void _OpSetVariable_set_value(OpSetVariable *self, Op *op)
{
	OP_SET_OPERANDE(self, value, op);
}

int OpSetVariable_fix_operandes(OpSetVariable *self, OpContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >= 1)
	{
		ret = 0;
		_OpSetVariable_set_value(self, self->super.operandes[OPSETVAR_VALUE]);
	}
	return ret;
}

int OpSetVariable_execute(OpSetVariable *self, OpContext *ctx)
{
	int ret = 0;
	if(self->value != NULL)
		ret = Op_execute(self->value, ctx);
	if(ret == 0)
		ret = OpContext_copy_current_to_variable(ctx, self->variable_number);
	return ret;
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
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute
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
	_Op2_set_operande1(self, NULL);
	_Op2_set_operande2(self, NULL);
}

#define OP2_OP1 0
#define OP2_OP2 1



void Op2_set_operande1(Op2 *self, Op *o)
{
	OP_ADD_OPERANDE(self, o, OP2_OP1);
}

void Op2_set_operande2(Op2 *self, Op *o)
{
	OP_ADD_OPERANDE(self, o, OP2_OP2);
}

void _Op2_set_operande1(Op2 *self, Op *o)
{
	OP_SET_OPERANDE(self, operande1, o);
}

void _Op2_set_operande2(Op2 *self, Op *o)
{
	OP_SET_OPERANDE(self, operande2, o);
}

int Op2_fix_operandes(Op2 *self, OpContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >= 2)
	{
		ret = 0;
		_Op2_set_operande1(self, self->super.operandes[OP2_OP1]);
		_Op2_set_operande2(self, self->super.operandes[OP2_OP2]);
	}
	return ret;
}

int Op2_check_args(Op2 *self, OpContext *ctx)
{
	int ret = 0;
	if(self->operande1 == NULL || self->operande2 == NULL)
		return 0;

	return ret;
}

int Op2_execute(Op2 *self, OpContext *ctx)
{
	int ret = 0;
	OpVariable v1, v2, res;

	OpVariable_init(&v1);
	OpVariable_init(&v2);
	OpVariable_init(&res);

	if(ret == 0)
	{
		ret = Op_execute(self->operande1, ctx);
		OpVariable_copy(&v1, OpContext_get_current_value(ctx));
	}
	if(ret == 0)
	{
		ret = Op_execute(self->operande2, ctx);
		OpVariable_copy(&v2, OpContext_get_current_value(ctx));
	}
	if(ret == 0)
	{
		if(((OpIsaTwoOp*)((Op*)self)->isa)->check_args != NULL)
			ret = ((OpIsaTwoOp*)((Op*)self)->isa)->check_args(&v1, &v2);
		if(ret == 0)
		{
			ret = ((OpIsaTwoOp*)((Op*)self)->isa)->compute(&res, &v1, &v2);
			if(ret == 0)
			{
				printf("Op %p 2 operandes '%s'\n", self, ((Op*)self)->isa->name);
				OpVariable_print(&v1);
				printf("With\n");
				OpVariable_print(&v2);
				printf("Give\n");
				OpVariable_print(&res);
				OpVariable_copy(OpContext_get_current_value(ctx), &res);
			}
			else
			{
				String m;
				String_init(&m);
				String_append_printf(&m, "Error '%s' %p during compute", ((Op*)self)->isa->name, self);
				OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&m));
				String_finalize(&m);
			}
		}
		else
		{
			String m;
			String_init(&m);
			String_append_printf(&m, "Error '%s' %p operandes types", ((Op*)self)->isa->name, self);
			OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&m));
			String_finalize(&m);
		}
	}

	OpVariable_terminate(&v1);
	OpVariable_terminate(&v2);
	OpVariable_terminate(&res);
	return ret;
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
								ret = -1;
						}
						else
							ret = -1;
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

int compute_crochets(OpVariable *res, OpVariable *v1, OpVariable *v2)
{
	int ret = -1;
	switch(OpVariable_get_type(v1))
	{
	case NONE :		break;
	case DOUBLE: 	break;
	case DOUBLES :	if(OpVariable_get_type(v2) == DOUBLE)
					{
						double *d1 = OpVariable_get_doubles(v1), d2 = OpVariable_get_double(v2), dres = NAN;
						size_t nb = OpVariable_get_number_elements(v1), pos = (size_t)d2;
						if(nb > pos)
						{
							dres = d1[pos];
							ret = OpVariable_set_double(res, dres);
						}
						else
							ret = -1;
					}
					else
						ret =  -1;
					break;
	case STRING :	if(OpVariable_get_type(v2) == DOUBLE)
					{
						double d2 = OpVariable_get_double(v2);
						const char *s = OpVariable_get_string(v1);
						char cres[2];
						size_t nb = OpVariable_get_number_elements(v1), pos = (size_t)d2;
						if(nb > pos)
						{
							cres[0] = s[pos];
							cres[1] = '\0';
							ret = OpVariable_set_string(res, cres);
						}
						else
							ret = -1;
					}
					else
						ret =  -1;
					break;
	case STRINGS :	if(OpVariable_get_type(v2) == DOUBLE)
					{
						double d2 = OpVariable_get_double(v2);
						const char * const *s = OpVariable_get_strings(v1);
						const char *empty = "", *cres;
						size_t nb = OpVariable_get_number_elements(v1), pos = (size_t)d2;
						if(nb > pos)
						{
							cres = s[pos];
							ret = OpVariable_set_string(res, cres);
						}
						else
							ret = -1;
					}
					else
						ret =  -1;
					break;
	}
	return ret;
}

int check_args_crochets(OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	switch(t1)
	{
	case NONE :		return -1;
					break;
	case DOUBLE :	return -1;
					break;
	case DOUBLES :
					break;
	case STRING :
					break;
	case STRINGS :
					break;
	}
	if(t2 != DOUBLE)
		return -1;
	return ret;
}

OpIsaTwoOp OpCrochets_isa = {
		.super.name="[]",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args_crochets,
		.compute = compute_crochets
};

Op *OpCrochets_new(void)
{
	return Op_new(&OpCrochets_isa.super);
}

int compute_concat(OpVariable *res, OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	size_t cmpt, nb;
	switch(OpVariable_get_type(v1))
	{
	case NONE :		break;
	case DOUBLE: 	{
						double d = OpVariable_get_double(v1);
						OpVariable_append_double(res, d);
						break;
					}
	case DOUBLES :	{
						double *d = OpVariable_get_doubles(v1);
						nb = OpVariable_get_number_elements(v1);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_double(res, d[cmpt]);
						break;
					}
	case STRING :	{
						const char *s = OpVariable_get_string(v1);
						OpVariable_append_string(res, s);
						break;
					}
	case STRINGS :	{
						const char * const *s = OpVariable_get_strings(v1);
						nb = OpVariable_get_number_elements(v1);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_string(res, s[cmpt]);
						break;
					}
	}
	switch(OpVariable_get_type(v2))
	{
	case NONE :		break;
	case DOUBLE: 	{
						double d = OpVariable_get_double(v2);
						OpVariable_append_double(res, d);
						break;
					}
	case DOUBLES :	{
						double *d = OpVariable_get_doubles(v2);
						nb = OpVariable_get_number_elements(v2);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_double(res, d[cmpt]);
						break;
					}
	case STRING :	{
						const char *s = OpVariable_get_string(v2);
						OpVariable_append_string(res, s);
						break;
					}
	case STRINGS :	{
						const char * const *s = OpVariable_get_strings(v2);
						nb = OpVariable_get_number_elements(v2);
						for(cmpt = 0; cmpt < nb; cmpt++)
							OpVariable_append_string(res, s[cmpt]);
						break;
					}
	}
	return ret;
}

int check_args_concat(OpVariable *v1, OpVariable *v2)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v1), t2 = OpVariable_get_type(v2);
	switch(t1)
	{
	case NONE :		if(t2 == NONE)//Allowing starting a list by empty state, will be completed by type of t2
						return -1;
					break;
	case DOUBLE :	if(t2 != DOUBLE && t2 != DOUBLES)
						return -1;
					break;
	case DOUBLES :	if(t2 != DOUBLE && t2 != DOUBLES)
						return -1;
					break;
	case STRING :	if(t2 != STRING && t2 != STRINGS)
						return -1;
					break;
	case STRINGS :	if(t2 != STRING && t2 != STRINGS)
						return -1;
					break;
	}
	return ret;
}

OpIsaTwoOp OpConcat_isa = {
		.super.name="Concat",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args_concat,
		.compute = compute_concat
};

Op *OpConcat_new(void)
{
	return Op_new(&OpConcat_isa.super);
}

OpIsaOneOp Op1_isa = {
		.super.name="Op1",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute
};

void Op1_init(Op1 *self)
{
	Op_init(&self->super);
	self->operande = NULL;
}

void Op1_terminate(Op1 *self)
{
	Op_terminate(&self->super);
	_Op1_set_operande(self, NULL);
}

void _Op1_set_operande(Op1 *self, Op *o)
{
	OP_SET_OPERANDE(self, operande, o);
}

#define OP1_OP 0

void Op1_set_operande(Op1 *self, Op *o)
{
	OP_ADD_OPERANDE(self, o, OP1_OP);
}

int Op1_check_args(Op1 *self, OpContext *ctx)
{
	int ret = 0;
	if(self->operande)
		return 0;

	return ret;
}

int Op1_fix_operandes(Op1 *self, OpContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >= 1)
	{
		ret = 0;
		_Op1_set_operande(self, self->super.operandes[OP1_OP]);
	}
	return ret;
}

int Op1_execute(Op1 *self, OpContext *ctx)
{
	int ret = 0;
	OpVariable v, res;

	OpVariable_init(&v);
	OpVariable_init(&res);

	if(ret == 0)
	{
		ret = Op_execute(self->operande, ctx);
		OpVariable_copy(&v, OpContext_get_current_value(ctx));
	}
	if(ret == 0)
	{
		if(((OpIsaOneOp*)((Op*)self)->isa)->check_arg != NULL)
			ret = ((OpIsaOneOp*)((Op*)self)->isa)->check_arg(&v);
		if(ret == 0)
		{
			ret = ((OpIsaOneOp*)((Op*)self)->isa)->compute(&res, &v);
			if(ret == 0)
			{
				printf("Op %p 1 operandes '%s'\n", self, ((Op*)self)->isa->name);
				OpVariable_print(&v);
				printf("Give\n");
				OpVariable_print(&res);
				OpVariable_copy(OpContext_get_current_value(ctx), &res);
			}
			else
			{
				String m;
				String_init(&m);
				String_append_printf(&m, "Error '%s' %p during compute", ((Op*)self)->isa->name, self);
				OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&m));
				String_finalize(&m);
			}
		}
		else
		{
			String m;
			String_init(&m);
			String_append_printf(&m, "Error '%s' %p operande type", ((Op*)self)->isa->name, self);
			OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&m));
			String_finalize(&m);
		}
	}

	OpVariable_terminate(&v);
	OpVariable_terminate(&res);
	return ret;
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
	case DOUBLES :	size_t cmpt, nb = OpVariable_get_number_elements(v);
					double *d = OpVariable_get_doubles(v);
					for(cmpt = 0; cmpt < nb; cmpt++)
						OpVariable_append_double(res, d[cmpt] * M_PI / 180);
					return 0;
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
	case DOUBLES :	size_t cmpt, nb = OpVariable_get_number_elements(v);
					double *d = OpVariable_get_doubles(v);
					for(cmpt = 0; cmpt < nb; cmpt++)
						OpVariable_append_double(res, d[cmpt] * 180 / M_PI);
					return 0;
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
	case DOUBLES :	size_t cmpt, nb = OpVariable_get_number_elements(v);
					double *d = OpVariable_get_doubles(v);
					for(cmpt = 0; cmpt < nb; cmpt++)
						OpVariable_append_double(res, d[cmpt] * -1);
					return 0;
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

int compute_logical_neg(OpVariable *res, OpVariable *v)
{
	switch(OpVariable_get_type(v))
	{
	case NONE :		return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	case DOUBLE :	OpVariable_set_double(res, double_eq(OpVariable_get_double(v), 0) ? 1.0 : 0.0);
					return 0;
					break;
	case DOUBLES :	size_t cmpt, nb = OpVariable_get_number_elements(v);
					double *d = OpVariable_get_doubles(v);
					for(cmpt = 0; cmpt < nb; cmpt++)
						OpVariable_append_double(res, double_eq(d[cmpt], 0) ? 1.0 : 0.0);
					return 0;
					break;
	}
	return 0;
}

int check_args_logical_neg(OpVariable *v)
{
	int ret = 0;
	OpVarType t1 = OpVariable_get_type(v);
	if(t1 != DOUBLE && t1 != DOUBLES)
		return -1;
	return ret;
}

OpIsaOneOp OpLogicalNegValue_isa = {
		.super.name="LogicalNegValue",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_logical_neg,
		.compute = compute_logical_neg
};

Op *OpLogicalNegValue_new(void)
{
	return Op_new(&OpLogicalNegValue_isa.super);
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
	case DOUBLES :	size_t cmpt, nb = OpVariable_get_number_elements(v);
					double *d = OpVariable_get_doubles(v);
					for(cmpt = 0; cmpt < nb; cmpt++)
						OpVariable_append_double(res, floor(d[cmpt]));
					return 0;
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
	case DOUBLES :	size_t cmpt, nb = OpVariable_get_number_elements(v);
					double *d = OpVariable_get_doubles(v);
					for(cmpt = 0; cmpt < nb; cmpt++)
						OpVariable_append_double(res, ceil(d[cmpt]));
					return 0;
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
	case DOUBLES :	size_t cmpt, nb = OpVariable_get_number_elements(v);
					double *d = OpVariable_get_doubles(v);
					for(cmpt = 0; cmpt < nb; cmpt++)
						OpVariable_append_double(res, cos(d[cmpt]));
					return 0;
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
	case DOUBLES :	size_t cmpt, nb = OpVariable_get_number_elements(v);
					double *d = OpVariable_get_doubles(v);
					for(cmpt = 0; cmpt < nb; cmpt++)
						OpVariable_append_double(res, acos(d[cmpt]));
					return 0;
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
	case DOUBLES :	size_t cmpt, nb = OpVariable_get_number_elements(v);
					double *d = OpVariable_get_doubles(v);
					for(cmpt = 0; cmpt < nb; cmpt++)
						OpVariable_append_double(res, sin(d[cmpt]));
					return 0;
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
	case DOUBLES :	size_t cmpt, nb = OpVariable_get_number_elements(v);
					double *d = OpVariable_get_doubles(v);
					for(cmpt = 0; cmpt < nb; cmpt++)
						OpVariable_append_double(res, asin(d[cmpt]));
					return 0;
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
	case DOUBLES :	size_t cmpt, nb = OpVariable_get_number_elements(v);
					double *d = OpVariable_get_doubles(v);
					for(cmpt = 0; cmpt < nb; cmpt++)
						OpVariable_append_double(res, tan(d[cmpt]));
					return 0;
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
	case DOUBLES :	size_t cmpt, nb = OpVariable_get_number_elements(v);
					double *d = OpVariable_get_doubles(v);
					for(cmpt = 0; cmpt < nb; cmpt++)
						OpVariable_append_double(res, atan(d[cmpt]));
					return 0;
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
	case DOUBLES :	size_t cmpt, nb = OpVariable_get_number_elements(v);
					double *d = OpVariable_get_doubles(v);
					for(cmpt = 0; cmpt < nb; cmpt++)
						OpVariable_append_double(res, sqrt(d[cmpt]));
					return 0;
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

int compute_get_first(OpVariable *res, OpVariable *v)
{
	int ret = -1;
	switch(OpVariable_get_type(v))
	{
	case NONE :		break;
	case DOUBLE: 	break;
	case DOUBLES :	{
						double *d1 = OpVariable_get_doubles(v), dres = NAN;
						size_t nb = OpVariable_get_number_elements(v), pos = 0;
						if(nb >= pos)
							dres = d1[pos];
						ret = OpVariable_set_double(res, dres);
					}
					break;
	case STRING :	{
						const char *s = OpVariable_get_string(v);
						char cres[2];
						size_t nb = OpVariable_get_number_elements(v), pos = 0;
						if(nb >= pos)
							cres[0] = s[pos];
						else
							cres[0] = '\0';
						cres[1] = '\0';
						ret = OpVariable_set_string(res, cres);
					}
					break;
	case STRINGS :	{
						const char * const *s = OpVariable_get_strings(v);
						const char *empty = "", *cres;
						size_t nb = OpVariable_get_number_elements(v), pos = 0;
						if(nb >= pos)
							cres = s[pos];
						else
							cres = empty;
						ret = OpVariable_set_string(res, cres);
					}
					break;
	}
	return ret;
}

int check_args_get_first(OpVariable *v)
{
	int ret = 0;
	OpVarType t = OpVariable_get_type(v);
	switch(t)
	{
	case NONE :		return -1;
					break;
	case DOUBLE :	return -1;
					break;
	case DOUBLES :
					break;
	case STRING :
					break;
	case STRINGS :
					break;
	}
	return ret;
}

OpIsaOneOp OpGetFirst_isa = {
		.super.name="GetFirst",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_get_first,
		.compute = compute_get_first
};

Op *OpGetFirst_new(void)
{
	return Op_new(&OpGetFirst_isa.super);
}

int compute_get_last(OpVariable *res, OpVariable *v)
{
	int ret = -1;
	switch(OpVariable_get_type(v))
	{
	case NONE :		break;
	case DOUBLE: 	break;
	case DOUBLES :	{
						double *d1 = OpVariable_get_doubles(v), dres = NAN;
						size_t nb = OpVariable_get_number_elements(v);
						if(nb > 0)
							dres = d1[nb - 1];
						ret = OpVariable_set_double(res, dres);
					}
					break;
	case STRING :	{
						const char *s = OpVariable_get_string(v);
						char cres[2];
						size_t nb = OpVariable_get_number_elements(v);
						if(nb > 1) // size pas len
							cres[0] = s[nb - 2];
						else
							cres[0] = '\0';
						cres[1] = '\0';
						ret = OpVariable_set_string(res, cres);
					}
					break;
	case STRINGS :	{
						const char * const *s = OpVariable_get_strings(v);
						const char *empty = "", *cres;
						size_t nb = OpVariable_get_number_elements(v);
						if(nb > 0)
							cres = s[nb - 1];
						else
							cres = empty;
						ret = OpVariable_set_string(res, cres);
					}
					break;
	}
	return ret;
}

int check_args_get_last(OpVariable *v)
{
	int ret = 0;
	OpVarType t = OpVariable_get_type(v);
	switch(t)
	{
	case NONE :		return -1;
					break;
	case DOUBLE :	return -1;
					break;
	case DOUBLES :
					break;
	case STRING :
					break;
	case STRINGS :
					break;
	}
	return ret;
}

OpIsaOneOp OpGetLast_isa = {
		.super.name="GetLast",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_get_last,
		.compute = compute_get_last
};

Op *OpGetLast_new(void)
{
	return Op_new(&OpGetLast_isa.super);
}

int compute_get_len(OpVariable *res, OpVariable *v)
{
	int ret = -1;
	switch(OpVariable_get_type(v))
	{
	case NONE :		break;
	case DOUBLE: 	break;
	case DOUBLES :	break;
	case STRING :	{
						size_t len = OpVariable_get_number_elements(v) - 1;
						ret = OpVariable_set_double(res, (double)len);
					}
					break;
	case STRINGS :
					break;
	}
	return ret;
}

int check_args_get_len(OpVariable *v)
{
	int ret = 0;
	OpVarType t = OpVariable_get_type(v);
	switch(t)
	{
	case NONE :		return -1;
					break;
	case DOUBLE :	return -1;
					break;
	case DOUBLES :	return -1;
					break;
	case STRING :
					break;
	case STRINGS :	return -1;
					break;
	}
	return ret;
}

OpIsaOneOp OpGetLen_isa = {
		.super.name="GetLen",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_get_len,
		.compute = compute_get_len
};

Op *OpGetLen_new(void)
{
	return Op_new(&OpGetLen_isa.super);
}

int compute_get_size(OpVariable *res, OpVariable *v)
{
	int ret = -1;
	switch(OpVariable_get_type(v))
	{
	case NONE :		break;
	case DOUBLE: 	break;
	case DOUBLES :	{
						size_t size = OpVariable_get_number_elements(v);
						ret = OpVariable_set_double(res, (double)size);
					}
					break;
	case STRING :	{
						size_t size = OpVariable_get_number_elements(v);
						ret = OpVariable_set_double(res, (double)size);
					}
					break;
	case STRINGS :	{
						size_t size = OpVariable_get_number_elements(v);
						ret = OpVariable_set_double(res, (double)size);
					}
					break;
	}
	return ret;
}

int check_args_get_size(OpVariable *v)
{
	int ret = 0;
	OpVarType t = OpVariable_get_type(v);
	switch(t)
	{
	case NONE :		return -1;
					break;
	case DOUBLE :	return -1;
					break;
	case DOUBLES :
					break;
	case STRING :
					break;
	case STRINGS :
					break;
	}
	return ret;
}

OpIsaOneOp OpGetSize_isa = {
		.super.name="GetSize",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_get_size,
		.compute = compute_get_size
};

Op *OpGetSize_new(void)
{
	return Op_new(&OpGetSize_isa.super);
}
