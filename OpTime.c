/*
 * opcs.c
 *
 *  Created on: 10 juil. 2026
 *      Author: damien
 */


#include <OpTime.h>
#include <time.h>

OpIsa OpGetTimeInSecs_isa = {
		.name="GetTimeInSecs",
		.size=sizeof(Op),
		.init = (void(*)(Op*))Op_init,
		.terminate = (void(*)(Op*))Op_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,
		.execute = (int(*)(Op*, OpContext*))OpGetTimeInSecs_execute,
		.check_args = NULL
};

int OpGetTimeInSecs_execute(Op *self, OpContext *ctx)
{
	OpContext_set_current_value_double(ctx, (double)time(NULL));
	return 0;
}

Op *OpGetTimeInSecs_new(void)
{
	return Op_new(&OpGetTimeInSecs_isa);
}

int compute_getgmttime(OpVariable *res, OpVariable *v)
{
	int ret = -1;
	switch(OpVariable_get_type(v))
	{
	case NONE :		break;
	case DOUBLE: 	{
						time_t tin = (time_t)OpVariable_get_double(v);
						struct tm tout, *r;
						r = gmtime_r(&tin, &tout);
						if(r != NULL)
						{
							/*
  int tm_sec;                    Seconds.     [0-60] (1 leap second)
  int tm_min;                    Minutes.     [0-59]
  int tm_hour;                   Hours.       [0-23]
  int tm_mday;                   Day.         [1-31]
  int tm_mon;                    Month.       [0-11]
  int tm_year;                   Year - 1900.
  int tm_wday;                   Day of week. [0-6]
  int tm_yday;                   Days in year.[0-365]
  int tm_isdst;                  DST.         [-1/0/1]
							 */
							ret = 0;
							OpVariable_append_double(res, (double)tout.tm_sec);
							OpVariable_append_double(res, (double)tout.tm_min);
							OpVariable_append_double(res, (double)tout.tm_hour);
							OpVariable_append_double(res, (double)tout.tm_mday);
							OpVariable_append_double(res, (double)tout.tm_mon);
							OpVariable_append_double(res, (double)tout.tm_year);
							OpVariable_append_double(res, (double)tout.tm_wday);
							OpVariable_append_double(res, (double)tout.tm_yday);
							OpVariable_append_double(res, (double)tout.tm_isdst);
						}
					}
					break;
	case DOUBLES :	break;
	case STRING :	break;
	case STRINGS :	break;
	}
	return ret;
}

int check_args_GetGMTTime(OpVariable *v)
{
	int ret = 0;
	OpVarType t = OpVariable_get_type(v);
	switch(t)
	{
	case NONE :		return -1;
					break;
	case DOUBLE :
					break;
	case DOUBLES :	return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	}
	return ret;
}

OpIsaOneOp OpGetGMTTime_isa = {
		.super.name="OpGetGMTTime_isa",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_GetGMTTime,
		.compute = compute_getgmttime
};

Op *OpGetGMTTime_new(void)
{
	return Op_new(&OpGetGMTTime_isa.super);
}

int compute_getlocaltime(OpVariable *res, OpVariable *v)
{
	int ret = -1;
	switch(OpVariable_get_type(v))
	{
	case NONE :		break;
	case DOUBLE: 	{
						time_t tin = (time_t)OpVariable_get_double(v);
						struct tm tout, *r;
						r = localtime_r(&tin, &tout);
						if(r != NULL)
						{
							/*
  int tm_sec;                    Seconds.     [0-60] (1 leap second)
  int tm_min;                    Minutes.     [0-59]
  int tm_hour;                   Hours.       [0-23]
  int tm_mday;                   Day.         [1-31]
  int tm_mon;                    Month.       [0-11]
  int tm_year;                   Year - 1900.
  int tm_wday;                   Day of week. [0-6]
  int tm_yday;                   Days in year.[0-365]
  int tm_isdst;                  DST.         [-1/0/1]
							 */
							ret = 0;
							OpVariable_append_double(res, (double)tout.tm_sec);
							OpVariable_append_double(res, (double)tout.tm_min);
							OpVariable_append_double(res, (double)tout.tm_hour);
							OpVariable_append_double(res, (double)tout.tm_mday);
							OpVariable_append_double(res, (double)tout.tm_mon);
							OpVariable_append_double(res, (double)tout.tm_year);
							OpVariable_append_double(res, (double)tout.tm_wday);
							OpVariable_append_double(res, (double)tout.tm_yday);
							OpVariable_append_double(res, (double)tout.tm_isdst);
						}
					}
					break;
	case DOUBLES :	break;
	case STRING :	break;
	case STRINGS :	break;
	}
	return ret;
}

int check_args_GetLocalTime(OpVariable *v)
{
	int ret = 0;
	OpVarType t = OpVariable_get_type(v);
	switch(t)
	{
	case NONE :		return -1;
					break;
	case DOUBLE :
					break;
	case DOUBLES :	return -1;
					break;
	case STRING :	return -1;
					break;
	case STRINGS :	return -1;
					break;
	}
	return ret;
}

OpIsaOneOp OpGetLocalTime_isa = {
		.super.name="OpGetLocalTime_isa",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args_GetLocalTime,
		.compute = compute_getlocaltime
};

Op *OpGetLocalTime_new(void)
{
	return Op_new(&OpGetLocalTime_isa.super);
}
