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
#include <LinkedList.h>
#include <String.h>

typedef struct op_program
{
	OpContext *ctx;//Main context
	Op *root;//Main program
	OpModule **modules;
	size_t number_of_modules;
}OpProgram;

typedef struct yy_buffer_state *YY_BUFFER_STATE;

typedef struct op_parser_file
{
	YY_BUFFER_STATE state;
	String *file_content;
}OpParserFile;

typedef struct op_parser
{
	OpContext *current_c;
	OpModule *current_m;
	OpProgram *program;
	OpParserFile **files;
	size_t files_size;
	String filename_prefix;
	int depth;
}OpParser;


#include <OpProgram.proto.h>

#endif /* OPPROGRAM_H_ */
