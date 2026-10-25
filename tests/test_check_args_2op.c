#include <Op.h>
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <string.h>


int compteur_compute = 0;
int compteur_check = 0;

int compute_2op(OpVariable *res, OpVariable *v1, OpVariable *v2, String *err)
{
	compteur_compute++;
	return 0;
}

int check_args(OpVariable *v1, OpVariable *v2)
{
	compteur_check++;
	return -1;
}

OpIsaTwoOp Test2Op = {
		.super.name="Test",
		.super.size=sizeof(Op2),
		.super.init = (void(*)(Op*))Op2_init,
		.super.terminate = (void(*)(Op*))Op2_terminate,
		.super.fix_operandes = (int(*)(Op*, OpContext*))Op2_fix_operandes,
		.super.execute = (int(*)(Op*, OpContext*))Op2_execute,
		.super.check_args = (int(*)(Op*,OpContext*))Op2_check_args,
		.check_args = check_args,
		.compute = compute_2op
};

int main(int argc, char **argv)
{
    int ret = 0;

    Op *o = Op_new(&Test2Op.super);

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
