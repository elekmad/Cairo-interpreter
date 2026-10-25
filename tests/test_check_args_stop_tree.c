#include <Op.h>
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <string.h>

#define NB_ITER 3

int compteur_compute = 0;
int compteur_check = 0;

int execute(Op *v, OpContext *c)
{
	compteur_compute++;
	return 0;
}

int check_argsok(Op *v, OpContext *c)
{
	compteur_check++;
	return 0;
}

int check_argsnok(Op *v, OpContext *c)
{
	compteur_check++;
	return -1;
}

OpIsa TestOpOK = {
		.name="test",
		.size=sizeof(Op),
		.init = (void(*)(Op*))Op_init,
		.terminate = (void(*)(Op*))Op_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,
		.execute = (int(*)(Op*, OpContext*))execute,
		.check_args = check_argsok
};

OpIsa TestOpNOK = {
		.name="test",
		.size=sizeof(Op),
		.init = (void(*)(Op*))Op_init,
		.terminate = (void(*)(Op*))Op_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,
		.execute = (int(*)(Op*, OpContext*))execute,
		.check_args = check_argsnok
};

int main(int argc, char **argv)
{
    int ret = 0;

    Op *ob = OpBloc_new();
    Op *o;

    int cmpt;
    for(cmpt = 0; cmpt < NB_ITER; cmpt++)
    {
    	o = Op_new(&TestOpOK);
    	OpBloc_append_Op((OpBloc*)ob, o);
    }
	o = Op_new(&TestOpNOK);
	OpBloc_append_Op((OpBloc*)ob, o);
    for(cmpt = 0; cmpt < NB_ITER; cmpt++)
    {
    	o = Op_new(&TestOpOK);
    	OpBloc_append_Op((OpBloc*)ob, o);
    }


    OpContext c;
    OpContext_init(&c);

    Op_fix_operandes(ob, &c);

    Op_prerun(ob, &c);
    Op_launch(ob, &c);

    if(compteur_compute != NB_ITER)
    {
    	ret = -1;
    	fprintf(stderr, "All first ops should be computed : %d != %d\n", compteur_compute, NB_ITER);
    }
    if(compteur_check != NB_ITER + 1)
    {
    	ret = -1;
    	fprintf(stderr, "Check args should work until return -1 : %d != %d\n", compteur_check, NB_ITER+1);
    }

    OpContext_terminate(&c);
    Op_free(ob);

    if(ret == 0)
        exit(EXIT_SUCCESS);
    else
        exit(EXIT_FAILURE);

}
