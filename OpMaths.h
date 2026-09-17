/*
 * ops.h
 *
 *  Created on: 10 juil. 2026
 *      Author: damien
 */

#ifndef OPMATHS_H_
#define OPMATHS_H_

#include <Op.h>

typedef struct op_init_random
{
	Op super;
	Op *value;

}OpInitRandom;

typedef struct op_random
{
	Op super;
	Op *value;

}OpRandom;

#include <OpMaths.proto.h>

#endif /* OPMATHS_H_ */
