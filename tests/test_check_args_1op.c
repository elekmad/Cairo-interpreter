#include <Op.h>
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <string.h>


int compteur_compute = 0;
int compteur_check = 0;

int compute_1op(OpVariable *res, OpVariable *v, String *err)
{
	compteur_compute++;
	return 0;
}

int check_args(OpVariable *v)
{
	compteur_check++;
	return -1;
}

OpIsaOneOp Test1Op = {
		.super.name="Test",
		.super.size=sizeof(Op1),
		.super.init = (void(*)(Op*))Op1_init,
		.super.terminate = (void(*)(Op*))Op1_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op1_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op1_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op1_check_args,
		.check_arg = check_args,
		.compute = compute_1op
};

int main(int argc, char **argv)
{
    int ret = 0;

    Op *o = Op_new(&Test1Op.super);

    OpContext c;
    OpContext_init(&c);

    Op_execute(o, &c);

    if(compteur_check != 0)
    {
    	ret = -1;
    	fprintf(stderr, "compteur check should be 0 Context just init : %d\n", compteur_check);
    }

    if(compteur_compute != 0)
    {
    	ret = -1;
    	fprintf(stderr, "compteur compute should be 0 Context just init : %d\n", compteur_compute);
    }

    Op_prerun(o, &c);
    int v;
    v = Op_launch(o, &c);
    if(v != -1)
    {
    	ret = -1;
    	fprintf(stderr, "Launch shound return -1 with check always -1 : %d\n", v);
    }

    if(compteur_check != 1)
    {
    	ret = -1;
    	fprintf(stderr, "compteur check should be 1 with launch, check return -1 should be executed : %d\n", compteur_check);
    }

    if(compteur_compute != 0)
    {
    	ret = -1;
    	fprintf(stderr, "compteur compute should be 0, check always return -1 : %d\n", compteur_compute);
    }
    OpContext_terminate(&c);
    Op_free(o);

    if(ret == 0)
        exit(EXIT_SUCCESS);
    else
        exit(EXIT_FAILURE);

}
