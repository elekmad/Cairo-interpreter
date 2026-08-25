/*
 * opcs.c
 *
 *  Created on: 10 juil. 2026
 *      Author: damien
 */


#include <OpModule.h>

void OpModule_init(OpModule *self)
{
	self->ctx = NULL;
	self->root = (OpCanvaBloc*)OpCanvaBloc_new();
	String_init(&self->name);
	self->arguments = NULL;
	self->number_of_arguments = 0;
	self->current_launcher = NULL;
}

OpModule *OpModule_new(void)
{
	OpModule *self = malloc(sizeof(OpModule));
	if(self != NULL)
		OpModule_init(self);
	return self;
}

void OpModule_set_context(OpModule *self, OpCanvaContext *ctx)
{
	if(self->ctx != NULL)
	{
		OpCanvaContext_free(self->ctx);
	}
	self->ctx = ctx;
}

void OpModule_set_Canva(OpModule *self, CanvaCtx *c)
{
	OpCanvaContext_set_Canva(self->ctx, c);
}

void OpModule_set_current_launcher(OpModule *self, OpLaunchModule *l)
{
	self->current_launcher = l;
}

OpLaunchModule *OpModule_get_current_launcher(OpModule *self)
{
	return self->current_launcher;
}

void _OpModule_add_arg_name(const char *name, OpModule *self)
{
	size_t num = OpContext_get_variable_number((OpContext*)self->ctx, name);
	OpVariable *var = OpContext_get_variable((OpContext*)self->ctx, num);
	OpModule_add_argument(self, var);
}

void OpModule_add_arguments(OpModule *self, LinkedList *list)
{
	LinkedList_do_to_all(list, (void(*)(void*, void*))_OpModule_add_arg_name, self);
}

void OpModule_add_argument(OpModule *self, OpVariable *arg)
{
	self->number_of_arguments++;
	self->arguments = realloc(self->arguments, sizeof(OpVariable*) * self->number_of_arguments);
	self->arguments[self->number_of_arguments - 1] = arg;
}

void OpModule_add_to_root(OpModule *self, Op *op)
{
	OpBloc_append_Op((OpBloc*)self->root, op);
}

void OpModule_duplicate_context(OpModule *self, OpCanvaContext *ctx)
{
	OpCanvaContext *newc = OpCanvaContext_new_from_other(ctx);
	OpModule_set_context(self, newc);
}

OpCanvaContext *OpModule_get_context(OpModule *self)
{
	return self->ctx;
}

OpCanvaBloc *OpModule_get_root(OpModule *self)
{
	return self->root;
}

void OpModule_fix_operandes(OpModule *self)
{
	Op_fix_operandes((Op*)self->root, (OpContext*)self->ctx);
}

void OpModule_set_name(OpModule *self, const char *name)
{
	String_set_char_string(&self->name, name);
}

const String *OpModule_get_name(OpModule *self)
{
	return &self->name;
}

void OpModule_terminate(OpModule *self)
{
	OpModule_set_context(self, NULL);
	Op_free((Op*)self->root);
	String_finalize(&self->name);
	if(self->number_of_arguments > 0)
		free(self->arguments);
}

void OpModule_free(OpModule *self)
{
	if(self != NULL)
	{
		OpModule_terminate(self);
		free(self);
	}
}

OpIsa OpLaunchModule_isa = {
		.name="LaunchModule",
		.size=sizeof(OpLaunchModule),
		.init = (void(*)(Op*))OpLaunchModule_init,
		.terminate = (void(*)(Op*))OpLaunchModule_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpLaunchModule_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpLaunchModule_execute,
		.check_args = (int(*)(Op*, OpContext*))OpLaunchModule_check_args,
};

void OpLaunchModule_init(OpLaunchModule *self)
{
	Op_init(&self->super);
	self->module = NULL;
	self->parent = NULL;
	self->call_args = NULL;
	self->number_of_call_args = 0;
	self->childs = NULL;
}

void OpLaunchModule_set_parent(OpLaunchModule *self, OpCanvaContext *p)
{
	self->parent = p;
}

OpCanvaContext *OpLaunchModule_get_parent(OpLaunchModule *self)
{
	return self->parent;
}

#define OPLAUNCHMODULE_CHILDS 0

void OpLaunchModule_set_childs(OpLaunchModule *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OPLAUNCHMODULE_CHILDS);
}

void _OpLaunchModule_set_childs(OpLaunchModule *self, Op *op)
{
	OP_SET_OPERANDE(self, childs, op);
}

Op *OpLaunchModule_get_childs(OpLaunchModule *self)
{
	return self->childs;
}

int OpLaunchModule_fix_operandes(OpLaunchModule *self, OpContext *ctx)
{
	size_t i;

	if(self->super.nb_ops > OPLAUNCHMODULE_CHILDS)
	{
		_OpLaunchModule_set_childs(self, self->super.operandes[OPLAUNCHMODULE_CHILDS]);
	}
	for(i = 0; i < self->number_of_call_args; i++)
	{
		Op *arg = self->call_args[i];
		if(arg != NULL)
			Op_fix_operandes(arg, ctx);
	}
	return 0;
}

int OpLaunchModule_check_args(OpLaunchModule *self, OpContext *ctx)
{
	int ret = 0;

	if(self->number_of_call_args != self->module->number_of_arguments)
		return -1;

	return ret;
}

void _OpLaunchModule_add_call_arg(Op *op, OpLaunchModule *self)
{
	OpLaunchModule_add_call_arg(self, op);
}

void OpLaunchModule_set_call_arguments(OpLaunchModule *self, LinkedList *list)
{
	LinkedList_do_to_all(list, (void(*)(void*,void*))_OpLaunchModule_add_call_arg, (void*)self);
}

void OpLaunchModule_add_call_arg(OpLaunchModule *self, Op *arg)
{
	self->number_of_call_args++;
	self->call_args = realloc(self->call_args, sizeof(Op*) * self->number_of_call_args);
	self->call_args[self->number_of_call_args - 1] = arg;
}

int OpLaunchModule_execute(OpLaunchModule *self, OpContext *ctx)
{
	int ret = 0;
	size_t i;
	OpModule_set_current_launcher(self->module, self);
	OpContext_reset_variables((OpContext*)OpModule_get_context(self->module));

	for(i = 0; i < self->number_of_call_args; i++)
	{
		Op *call_arg;
		OpVariable *arg;
		call_arg = self->call_args[i];
		arg = self->module->arguments[i];
		Op_execute(call_arg, ctx);
		OpVariable_copy(arg, OpContext_get_current_value(ctx));
	}
	OpContext_set_running_state((OpContext*)OpModule_get_context(self->module), (Op*)self, Run, "Ready to run module");
	ret = Op_execute((Op*)OpModule_get_root(self->module), (OpContext*)OpModule_get_context(self->module));
	if(ret != 0 || OpContext_get_running_state((OpContext*)OpModule_get_context(self->module)) == Error)
	{
		String msg;
		String_init(&msg);
		String_append_printf(&msg, "Error during execution of module '%s'", String_get_char_string(OpModule_get_name(self->module)));
		OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&msg));
		String_finalize(&msg);
	}
	OpModule_set_current_launcher(self->module, NULL);

	return ret;
}

void OpLaunchModule_set_module(OpLaunchModule *self, OpModule *m)
{
	self->module = m;
}

void OpLaunchModule_terminate(OpLaunchModule *self)
{
	Op_terminate(&self->super);
	if(self->number_of_call_args > 0)
	{
		size_t i;
		for(i = 0; i < self->number_of_call_args; i++)
		{
			Op *o = self->call_args[i];
			if(o != NULL)
				Op_free(o);
		}
		free(self->call_args);
	}
	_OpLaunchModule_set_childs(self, NULL);
}

Op *OpLaunchModule_new(void)
{
	return Op_new(&OpLaunchModule_isa);
}

OpIsa OpLaunchModuleChilds_isa = {
		.name="LaunchModuleChilds",
		.size=sizeof(OpLaunchModuleChilds),
		.init = (void(*)(Op*))OpLaunchModuleChilds_init,
		.terminate = (void(*)(Op*))OpLaunchModuleChilds_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,
		.execute = (int(*)(Op*, OpContext*))OpLaunchModuleChilds_execute,
		.check_args = (int(*)(Op*, OpContext*))NULL,
};

void OpLaunchModuleChilds_init(OpLaunchModuleChilds *self)
{
	Op_init(&self->super);
	self->module = NULL;
}

int OpLaunchModuleChilds_execute(OpLaunchModuleChilds *self, OpContext *ctx)
{
	int ret = 0;

	OpLaunchModule *l = OpModule_get_current_launcher(self->module);

	Op *childs = OpLaunchModule_get_childs(l);
	OpContext *parent = (OpContext*)OpLaunchModule_get_parent(l);

	ret = Op_execute(childs, parent);

	return ret;
}

void OpLaunchModuleChilds_set_module(OpLaunchModuleChilds *self, OpModule *m)
{
	self->module = m;
}

void OpLaunchModuleChilds_terminate(OpLaunchModuleChilds *self)
{
	Op_terminate(&self->super);
}

Op *OpLaunchModuleChilds_new(void)
{
	return Op_new(&OpLaunchModuleChilds_isa);
}
