#include <OpMaths.h>
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <string.h>

#define D1 42.0
#define D2 43.0

int main(int argc, char **argv)
{
    int ret = 0;
    OpVariable v1, v2, res;
    double *dres;
    const char *sres;
    const char *const *sress;
    String err;
    OpVariable_init(&v1);
    OpVariable_init(&v2);
    OpVariable_init(&res);

    String_init(&err);

    //Check Add DOUBLE < DOUBLE
    OpVariable_set_double(&v1, D1);
    OpVariable_set_double(&v2, D2);
    if(check_args_inf(&v1, &v2) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLE < DOUBLE should work\n");
    }
    if(compute_inf(&res, &v1, &v2, &err) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "compute DOUBLE < DOUBLE should work\n");
    }
    if(OpVariable_get_type(&res) != DOUBLE)
    {
    	ret = -1;
    	fprintf(stderr, "compute DOUBLE < DOUBLE should give DOUBLE\n");
    }
    if(double_eq(OpVariable_get_double(&res), 1) == false)
    {
    	ret = -1;
    	fprintf(stderr, "result DOUBLE < DOUBLE should work : %f != %d\n", OpVariable_get_double(&res), 1);
    }
    //Check not <
    OpVariable_set_double(&v2, D1);
    OpVariable_set_type(&res, NONE);
    compute_inf(&res, &v1, &v2, &err);
    if(double_eq(OpVariable_get_double(&res), 0) == false)
	{
		ret = -1;
		fprintf(stderr, "result DOUBLE < DOUBLE with not < should work : %f != %d\n", OpVariable_get_double(&res), 0);
	}

    //Check Add DOUBLE < DOUBLES

    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);

    OpVariable_set_type(&v1, DOUBLE);
    OpVariable_set_type(&v2, DOUBLES);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLE < DOUBLES shouldn't work\n");
    }


    //Check Add DOUBLES < DOUBLE

    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);

    OpVariable_set_type(&v1, DOUBLES);
    OpVariable_set_type(&v2, DOUBLE);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLES < DOUBLE should work\n");
    }

    //Check Add DOUBLES < DOUBLES

    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);

    OpVariable_set_type(&v1, DOUBLES);
    OpVariable_set_type(&v2, DOUBLES);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLES < DOUBLES shouldn't work\n");
    }


    //Check Add STRING < STRING


    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);

    OpVariable_set_type(&v1, STRING);
    OpVariable_set_type(&v2, STRING);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check STRING < STRING identicals shouldn't work\n");
    }

    //Check Add STRING < STRINGS

    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_set_type(&v1, STRING);
    OpVariable_set_type(&v2, STRINGS);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check STRING < STRINGS shouldn't work\n");
    }

    //Check Add STRINGS < STRING

    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_set_type(&v1, STRINGS);
    OpVariable_set_type(&v2, STRING);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check STRINGS < STRING shouldn't work\n");
    }

    //Check Add STRINGS < STRINGS

    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);

    OpVariable_set_type(&v1, STRINGS);
    OpVariable_set_type(&v2, STRINGS);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check STRINGS < STRINGS identicals shouldn't work\n");
    }

    //Check Add DOUBLE < STRING
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_set_type(&v1, DOUBLE);
    OpVariable_set_type(&v2, STRING);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLE < STRING shouldn't work\n");
    }

    //Check Add DOUBLE < STRINGS
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_set_type(&v1, DOUBLE);
    OpVariable_set_type(&v2, STRINGS);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLE < STRINGS shouldn't work\n");
    }


    //Check Add DOUBLES < STRING
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_set_type(&v1, DOUBLES);
    OpVariable_set_type(&v2, STRING);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLES < STRING shouldn't work\n");
    }

    //Check Add DOUBLES < STRINGS
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_set_type(&v1, DOUBLES);
    OpVariable_set_type(&v2, STRINGS);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLES < STRINGS shouldn't work\n");
    }

    //Check Add STRING < DOUBLE
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_set_type(&v1, STRING);
    OpVariable_set_type(&v2, DOUBLE);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check STRING < DOUBLE shouldn't work\n");
    }

    //Check Add STRING < DOUBLES
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_set_type(&v1, STRING);
    OpVariable_set_type(&v2, DOUBLES);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check STRING < DOUBLES shouldn't work\n");
    }

    //Check Add STRINGS < DOUBLE
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_set_type(&v1, STRINGS);
    OpVariable_set_type(&v2, DOUBLE);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check STRINGS < DOUBLE shouldn't work\n");
    }

    //Check Add STRINGS < DOUBLES
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_set_type(&v1, STRINGS);
    OpVariable_set_type(&v2, DOUBLES);
    if(check_args_inf(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check STRINGS < DOUBLES shouldn't work\n");
    }



    String_finalize(&err);

    OpVariable_terminate(&v1);
    OpVariable_terminate(&v2);
    OpVariable_terminate(&res);
    if(ret == 0)
        exit(EXIT_SUCCESS);
    else
        exit(EXIT_FAILURE);

}
