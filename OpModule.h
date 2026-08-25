/*
 * ops.h
 *
 *  Created on: 10 juil. 2026
 *      Author: damien
 */

#ifndef OPMODULE_H_
#define OPMODULE_H_

#include <OpCanva.h>
#include <String.h>

typedef struct op_launch_module OpLaunchModule;


typedef struct op_module
{
	String name;
	OpCanvaContext *ctx;
	OpCanvaBloc *root;
	size_t number_of_arguments;
	OpLaunchModule *current_launcher;
}OpModule;

typedef struct op_launch_module
{
	Op super;//Must be First
	OpModule *module;
	OpCanvaContext *parent;
	Op **call_args;
	Op *childs;
	size_t number_of_call_args;
} OpLaunchModule;

typedef struct op_launch_module_childs
{
	Op super;//Must be First
	OpModule *module;
} OpLaunchModuleChilds;


#include <OpModule.proto.h>

#endif /* OPMODULE_H_ */
