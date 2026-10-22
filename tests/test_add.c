#include <OpMaths.h>
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <string.h>

#define D1 42.0
#define D11 42.5
#define D2 3.0
#define D21 4.3

#define S1 "foo"
#define S11 "ga"
#define S2 "bar"
#define S21 "bu"

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

    //Check Add DOUBLE + DOUBLE
    OpVariable_set_double(&v1, D1);
    OpVariable_set_double(&v2, D2);
    if(check_args_add(&v1, &v2) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLE + DOUBLE should work\n");
    }
    if(compute_add(&res, &v1, &v2, &err) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "compute DOUBLE + DOUBLE should work\n");
    }
    if(OpVariable_get_number_elements(&res) != 1)
    {
    	ret = -1;
    	fprintf(stderr, "compute DOUBLE + DOUBLE with same number of elements  nb res = %zu != 1\n", OpVariable_get_number_elements(&res));
    }
    if(double_eq(OpVariable_get_double(&res), D1+D2) == false)
    {
    	ret = -1;
    	fprintf(stderr, "result DOUBLE + DOUBLE should work : %f != %f\n", OpVariable_get_double(&res), D1+D2);
    }

    //Check Add DOUBLE + DOUBLES

    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);

    OpVariable_set_type(&v1, DOUBLE);
    OpVariable_set_double(&v1, D1);
    OpVariable_set_type(&v2, DOUBLES);
    OpVariable_append_double(&v2, D2);
    OpVariable_append_double(&v2, D21);
    if(check_args_add(&v1, &v2) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLE + DOUBLES should work\n");
    }
    if(compute_add(&res, &v1, &v2, &err) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "compute DOUBLE + DOUBLES should work\n");
    }
    if(OpVariable_get_number_elements(&res) != 2)
    {
    	ret = -1;
    	fprintf(stderr, "compute DOUBLE + DOUBLES with same number of elements  nb res = %zu != 2\n", OpVariable_get_number_elements(&res));
    }
    dres = OpVariable_get_doubles(&res);
    if(double_eq(dres[0], D1+D2) == false)
    {
    	ret = -1;
    	fprintf(stderr, "result DOUBLE + DOUBLES should work : %f[0] != %f\n", dres[0], D1+D2);
    }
    if(double_eq(dres[1], D1+D21) == false)
    {
    	ret = -1;
    	fprintf(stderr, "result DOUBLE + DOUBLES should work : %f[1] != %f\n", dres[1], D1+D21);
    }

    //Check Add DOUBLES + DOUBLE

    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);

    OpVariable_set_type(&v1, DOUBLES);
    OpVariable_append_double(&v1, D1);
    OpVariable_append_double(&v1, D11);
    OpVariable_set_type(&v2, DOUBLE);
    OpVariable_set_double(&v2, D2);
    if(check_args_add(&v1, &v2) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLES + DOUBLE should work\n");
    }
    if(compute_add(&res, &v1, &v2, &err) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "compute DOUBLES + DOUBLE should work\n");
    }
    if(OpVariable_get_number_elements(&res) != 2)
    {
    	ret = -1;
    	fprintf(stderr, "compute DOUBLES + DOUBLE with same number of elements  nb res = %zu != 2\n", OpVariable_get_number_elements(&res));
    }
    dres = OpVariable_get_doubles(&res);
    if(double_eq(dres[0], D1+D2) == false)
    {
    	ret = -1;
    	fprintf(stderr, "result DOUBLES + DOUBLE should work : %f[0] != %f\n", dres[0], D1+D2);
    }
    if(double_eq(dres[1], D11+D2) == false)
    {
    	ret = -1;
    	fprintf(stderr, "result DOUBLES + DOUBLE should work : %f[1] != %f\n", dres[1], D11+D2);
    }

    //Check Add DOUBLES + DOUBLES

    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);

    OpVariable_set_type(&v1, DOUBLES);
    OpVariable_append_double(&v1, D1);
    OpVariable_append_double(&v1, D11);
    OpVariable_set_type(&v2, DOUBLES);
    OpVariable_append_double(&v2, D2);
    OpVariable_append_double(&v2, D21);
    if(check_args_add(&v1, &v2) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLES + DOUBLES with same number of elements should work\n");
    }
    if(compute_add(&res, &v1, &v2, &err) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "compute DOUBLES + DOUBLES with same number of elements  should work\n");
    }
    if(OpVariable_get_number_elements(&res) != 2)
    {
    	ret = -1;
    	fprintf(stderr, "compute DOUBLES + DOUBLES with same number of elements  nb res = %zu != 2\n", OpVariable_get_number_elements(&res));
    }
    dres = OpVariable_get_doubles(&res);
    if(double_eq(dres[0], D1+D2) == false)
    {
    	ret = -1;
    	fprintf(stderr, "result DOUBLES + DOUBLES with same number of elements  should work : %f[0] != %f\n", dres[0], D1+D2);
    }
    if(double_eq(dres[1], D11+D21) == false)
    {
    	ret = -1;
    	fprintf(stderr, "result DOUBLES + DOUBLES with same number of elements  should work : %f[1] != %f\n", dres[1], D11+D21);
    }
    //Check Add DOUBLES + DOUBLES different sizes
    OpVariable_append_double(&v2, D21);
    if(check_args_add(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLES + DOUBLES with different sizes shouldn't work\n");
    }


    //Check Add STRING + STRING

    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);

    OpVariable_set_string(&v1, S1);
    OpVariable_set_string(&v2, S2);
    if(check_args_add(&v1, &v2) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "check STRING + STRING should work\n");
    }
    if(compute_add(&res, &v1, &v2, &err) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "compute STRING + STRING should work\n");
    }
    if(OpVariable_get_type(&res) != STRING)
    {
    	ret = -1;
    	fprintf(stderr, "compute STRING + STRING should give type STRING\n");
    }
    sres = OpVariable_get_string(&res);
    if(strcmp(sres, S1 S2) != 0)
    {
    	ret = -1;
    	fprintf(stderr, "result STRING + STRING should work : '%s' != '%s'\n", sres, S1 S2);
    }


    //Check Add STRING + STRINGS

    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);

    OpVariable_set_string(&v1, S1);
    OpVariable_append_string(&v2, S2);
    OpVariable_append_string(&v2, S21);
    if(check_args_add(&v1, &v2) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "check STRING + STRINGS should work\n");
    }
    if(compute_add(&res, &v1, &v2, &err) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "compute STRING + STRINGS should work\n");
    }
    if(OpVariable_get_number_elements(&res) != 2)
    {
    	ret = -1;
    	fprintf(stderr, "compute STRING + STRINGS nb res = %zu != 2\n", OpVariable_get_number_elements(&res));
    }
    sress = OpVariable_get_strings(&res);
    if(strcmp(sress[0], S1 S2) != 0)
    {
    	ret = -1;
    	fprintf(stderr, "result STRING + STRINGS should work : [0]'%s' != '%s'\n", sress[0], S1 S2);
    }
    if(strcmp(sress[1], S1 S21) != 0)
    {
    	ret = -1;
    	fprintf(stderr, "result STRING + STRINGS should work : [1]'%s' != '%s'\n", sress[1], S1 S21);
    }

    //Check Add STRINGS + STRING

    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);

    OpVariable_append_string(&v1, S1);
    OpVariable_append_string(&v1, S11);
    OpVariable_set_string(&v2, S2);
    if(check_args_add(&v1, &v2) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "check STRINGS + STRING should work\n");
    }
    if(compute_add(&res, &v1, &v2, &err) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "compute STRINGS + STRING should work\n");
    }
    if(OpVariable_get_number_elements(&res) != 2)
    {
    	ret = -1;
    	fprintf(stderr, "compute STRINGS + STRING nb res = %zu != 2\n", OpVariable_get_number_elements(&res));
    }
    sress = OpVariable_get_strings(&res);
    if(strcmp(sress[0], S1 S2) != 0)
    {
    	ret = -1;
    	fprintf(stderr, "result STRINGS + STRING should work : [0]'%s' != '%s'\n", sress[0], S1 S2);
    }
    if(strcmp(sress[1], S11 S2) != 0)
    {
    	ret = -1;
    	fprintf(stderr, "result STRINGS + STRING should work : [1]'%s' != '%s'\n", sress[1], S11 S2);
    }

    //Check Add STRINGS + STRINGS

    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);

    OpVariable_append_string(&v1, S1);
    OpVariable_append_string(&v1, S11);
    OpVariable_append_string(&v2, S2);
    OpVariable_append_string(&v2, S21);
    if(check_args_add(&v1, &v2) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "check STRINGS + STRINGS should work\n");
    }
    if(compute_add(&res, &v1, &v2, &err) == -1)
    {
    	ret = -1;
    	fprintf(stderr, "compute STRINGS + STRINGS should work\n");
    }
    if(OpVariable_get_number_elements(&res) != 2)
    {
    	ret = -1;
    	fprintf(stderr, "compute STRINGS + STRINGS nb res = %zu != 2\n", OpVariable_get_number_elements(&res));
    }
    sress = OpVariable_get_strings(&res);
    if(strcmp(sress[0], S1 S2) != 0)
    {
    	ret = -1;
    	fprintf(stderr, "result STRINGS + STRINGS should work : [0]'%s' != '%s'\n", sress[0], S1 S2);
    }
    if(strcmp(sress[1], S11 S21) != 0)
    {
    	ret = -1;
    	fprintf(stderr, "result STRINGS + STRINGS should work : [1]'%s' != '%s'\n", sress[1], S11 S21);
    }
    //Check Add STRINGS + STRINGS different sizes
    OpVariable_append_string(&v2, S21);
    if(check_args_add(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check STRINGS + STRINGS with different sizes shouldn't work\n");
    }

    //Check Add DOUBLE + STRING
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_set_double(&v1, D1);
    OpVariable_set_string(&v2, S11);
    if(check_args_add(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLE + STRING shouldn't work\n");
    }

    //Check Add DOUBLE + STRINGS
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_set_double(&v1, D1);
    OpVariable_append_string(&v2, S11);
    OpVariable_append_string(&v2, S11);
    if(check_args_add(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLE + STRINGS shouldn't work\n");
    }


    //Check Add DOUBLES + STRING
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_append_double(&v1, D1);
    OpVariable_set_string(&v2, S11);
    if(check_args_add(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLES + STRING shouldn't work\n");
    }

    //Check Add DOUBLES + STRINGS
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_append_double(&v1, D1);
    OpVariable_append_string(&v2, S11);
    OpVariable_append_string(&v2, S11);
    if(check_args_add(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check DOUBLES + STRINGS shouldn't work\n");
    }

    //Check Add STRING + DOUBLE
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_set_string(&v1, S1);
    OpVariable_set_double(&v2, D1);
    if(check_args_add(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check STRING + DOUBLE shouldn't work\n");
    }

    //Check Add STRING + DOUBLES
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_set_string(&v1, S1);
    OpVariable_append_double(&v2, D1);
    OpVariable_append_double(&v2, D1);
    if(check_args_add(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check STRING + DOUBLES shouldn't work\n");
    }

    //Check Add STRINGS + DOUBLE
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_append_string(&v1, S1);
    OpVariable_set_double(&v2, D1);
    if(check_args_add(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check STRINGS + DOUBLE shouldn't work\n");
    }

    //Check Add STRINGS + DOUBLES
    OpVariable_set_type(&v1, NONE);
    OpVariable_set_type(&v2, NONE);
    OpVariable_set_type(&res, NONE);
    OpVariable_append_string(&v1, S1);
    OpVariable_append_double(&v2, D1);
    if(check_args_add(&v1, &v2) == 0)
    {
    	ret = -1;
    	fprintf(stderr, "check STRINGS + DOUBLES shouldn't work\n");
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
