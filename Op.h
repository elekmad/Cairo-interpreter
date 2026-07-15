/*
 * ops.h
 *
 *  Created on: 10 juil. 2026
 *      Author: damien
 */

#ifndef OP_H_
#define OP_H_

#include <LinkedList.h>
#include <stddef.h>

typedef struct op_context
{
	double current_value;
	double *variables;
	char **variables_names;
	size_t number_of_variables;
}OpContext;

typedef struct op Op;
typedef struct op_isa
{
	const char *name;
	size_t size;
	void(*init)(Op*);
	void(*terminate)(Op*);
	void (*execute)(Op*, OpContext *);
}OpIsa;

typedef struct op
{
	OpIsa *isa;
} Op;

typedef struct op_isa_2_operandes
{
	OpIsa super;//Must be first
	double (*compute)(double op1, double op2);
}OpIsaTwoOp;

typedef struct op_bloc
{
	Op super;//Must be First
	LinkedList ops;
} OpBloc;


typedef struct op_get_value
{
	Op super;
	double value;

}OpGetValue;


typedef struct op_neg_value
{
	Op super;
	Op *value;

}OpNegValue;


typedef struct op_logical_neg_value
{
	Op super;
	Op *value;

}OpLogicalNegValue;


typedef struct op_if
{
	Op super;
	Op *condition;
	Op *true_branch;
	Op *false_branch;

}OpIf;

typedef struct op_for_loop
{
	Op super;
	size_t variable_number;
	double start;
	double condition;
	double step;
	Op *loop;

}OpForLoop;


typedef struct op_get_variable
{
	Op super;
	size_t variable_number;

}OpGetVariable;


typedef struct op_set_variable
{
	Op super;
	size_t variable_number;
	Op *value;

}OpSetVariable;

typedef struct op_2_operandes
{
	Op super;
	Op *operande1;
	Op *operande2;

}Op2;


#include <Op.proto.h>

#endif /* OP_H_ */
