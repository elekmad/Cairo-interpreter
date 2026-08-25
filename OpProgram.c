/*
 * opcs.c
 *
 *  Created on: 10 juil. 2026
 *      Author: damien
 */


#include <OpProgram.h>


OpProgram *OpProgram_new(void)
{
	OpProgram *self = malloc(sizeof(OpProgram));
	if(self != NULL)
		OpProgram_init(self);
	return self;
}

void OpProgram_init(OpProgram *self)
{
	self->modules = NULL;
	self->number_of_modules = 0;
	self->root = NULL;
}

void OpProgram_terminate(OpProgram *self)
{
	if(self->number_of_modules > 0)
	{
		if(self->modules != NULL)
		{
			int i;
			for(i = 0; i < self->number_of_modules; i++)
			{
				if(self->modules[i] != NULL)
					OpModule_free(self->modules[i]);
			}
			free(self->modules);
			self->modules = NULL;
		}
	}
}

void OpProgram_free(OpProgram *self)
{
	if(self != NULL)
	{
		OpProgram_terminate(self);
		free(self);
	}
}

void OpProgram_fix_operandes(OpProgram *self)
{
	Op_fix_operandes(self->root, self->ctx);
	size_t i;
	for(i = 0; i < self->number_of_modules; i++)
	{
		OpModule *m = self->modules[i];
		if(m != NULL)
			OpModule_fix_operandes(m);
	}
}

void OpProgram_set_Canva(OpProgram *self, CanvaCtx *c)
{
	size_t i;
	for(i = 0; i < self->number_of_modules; i++)
	{
		OpModule *m = self->modules[i];
		if(m != NULL)
			OpModule_set_Canva(m, c);
	}
}

void OpProgram_prerun(OpProgram *self)
{
	Op_prerun(self->root, self->ctx);
}

void OpProgram_run(OpProgram *self)
{
	Op_launch(self->root, self->ctx);
}

ssize_t OpProgram_check_module_number(OpProgram *self, const char *name)
{
	if(self->number_of_modules > 0)
	{
		size_t i;
		int found = 0;
		for(i = 0; i < self->number_of_modules; i++)
		{
			OpModule *m = self->modules[i];
			if(String_compare_with_char_string(OpModule_get_name(m), name) == 0)
			{
				found = 1;
				return (ssize_t)i;
			}
		}
	}
	return -1;
}

size_t OpProgram_get_module_number(OpProgram *self, const char *name)
{
	ssize_t num = OpProgram_check_module_number(self, name);
	if(num >= 0)
		return (size_t)num;
	size_t place;
	place = self->number_of_modules;
	self->number_of_modules++;
	self->modules = realloc(self->modules, self->number_of_modules * sizeof(OpModule*));
	OpModule *m = OpModule_new();
	OpModule_set_name(m, name);
	OpModule_duplicate_context(m, (OpCanvaContext*)self->ctx);
	self->modules[place] = m;
	return place;
}

OpModule *OpProgram_get_module(OpProgram *self, size_t num)
{
	OpModule *ret = NULL;
	if(num < self->number_of_modules)
		ret = self->modules[num];
	return ret;
}

OpContext *OpProgram_get_context(OpProgram *self)
{
	return self->ctx;
}

void OpProgram_set_context(OpProgram *self, OpContext *ctx)
{
	self->ctx = ctx;
}

void OpProgram_set_root(OpProgram *self, Op *root)
{
	self->root = root;
}

OpParser *OpParser_new(void)
{
	OpParser *self = malloc(sizeof(OpParser));
	if(self != NULL)
		OpParser_init(self);
	return self;
}

void OpParser_init(OpParser *self)
{

}

void OpParser_terminate(OpParser *self)
{

}

void OpParser_free(OpParser *self)
{
	if(self != NULL)
	{
		OpParser_terminate(self);
		free(self);
	}
}

OpContext *OpParser_get_current_context(OpParser *self)
{
	return self->current_c;
}

void OpParser_set_current_context(OpParser *self, OpContext *ctx)
{
	self->current_c = ctx;
}

OpModule *OpParser_get_current_module(OpParser *self)
{
	return self->current_m;
}

void OpParser_set_current_module(OpParser *self, OpModule *m)
{
	self->current_m = m;
}

OpProgram *OpParser_get_program(OpParser *self)
{
	return self->program;
}

void OpParser_set_program(OpParser *self, OpProgram *p)
{
	self->program = p;
}
