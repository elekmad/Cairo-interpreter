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
#include <String.h>

typedef enum op_variable_type
{
	NONE,
	DOUBLE,
	DOUBLES,
	STRING,
	STRINGS
}OpVarType;

typedef struct op_variable
{
	union
	{
		double v;
		double *vs;
		char *string;
		char **strings;
	};
	OpVarType type;
	char *name;
	size_t number_of_elements;
}OpVariable;

typedef enum op_running_state
{
	Init,
	Run,
	Finished,
	Error
}OpRunningState;

typedef struct op_context
{
	OpRunningState state;
	OpVariable current_value;
	OpVariable **variables;
	size_t number_of_variables;
}OpContext;

typedef struct op Op;
typedef struct op_isa
{
	const char *name;
	size_t size;
	void(*init)(Op*);
	void(*terminate)(Op*);
	int (*execute)(Op*, OpContext *);
	int (*check_args)(Op*, OpContext*);
}OpIsa;

typedef struct op
{
	OpIsa *isa;
} Op;

typedef struct op_isa_2_operandes
{
	OpIsa super;//Must be first
	int (*check_args)(OpVariable*, OpVariable*);
	int (*compute)(OpVariable *res, OpVariable *op1, OpVariable *op2);
}OpIsaTwoOp;

typedef struct op_isa_1_operande
{
	OpIsa super;//Must be first
	int (*check_arg)(OpVariable*);
	int (*compute)(OpVariable *res, OpVariable *op);
}OpIsaOneOp;

typedef struct op_isa_simple
{
	OpIsa upser;
	double (*compute)();
}OpIsaSimple;

typedef struct op_bloc
{
	Op super;//Must be First
	LinkedList ops;
} OpBloc;


typedef struct op_get_value
{
	Op super;
	OpVariable value;

}OpGetValue;


typedef struct op_message
{
	Op super;
	char *message;
	Op *value;

}OpMessage;

typedef struct op_pi
{
	Op super;

}OpPi;


typedef struct op_if
{
	Op super;
	Op *condition;
	Op *true_branch;
	Op *false_branch;

}OpIf;


typedef struct op_while
{
	Op super;
	Op *condition;
	Op *bloc;
}OpWhile;

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

typedef struct op_1_operandes
{
	Op super;
	Op *operande;

}Op1;


#include <Op.proto.h>

#endif /* OP_H_ */
