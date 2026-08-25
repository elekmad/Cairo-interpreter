/*
 * ops.h
 *
 *  Created on: 10 juil. 2026
 *      Author: damien
 */

#ifndef OPPROGRAM_H_
#define OPPROGRAM_H_

#include <Op.h>
#include <OpModule.h>

typedef struct op_program
{
	OpContext *ctx;//Main context
	Op *root;//Main program
	OpModule **modules;
	size_t number_of_modules;
}OpProgram;

typedef struct op_parser
{
	OpContext *current_c;
	OpModule *current_m;
	OpProgram *program;
}OpParser;


#include <OpProgram.proto.h>

#endif /* OPPROGRAM_H_ */
