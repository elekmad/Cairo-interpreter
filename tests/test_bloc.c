#include <Op.h>
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <string.h>

#define NB_ITER 10

int compteur_compute = 0;

int execute(Op *v, OpContext *c)
{
	compteur_compute++;
	return 0;
}

OpIsa TestOp = {
		.name="test",
		.size=sizeof(Op),
		.init = (void(*)(Op*))Op_init,
		.terminate = (void(*)(Op*))Op_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))NULL,
		.execute = (int(*)(Op*, OpContext*))execute,
		.check_args = NULL
};

int main(int argc, char **argv)
{
    int ret = 0;

    Op *ob = OpBloc_new();

    int cmpt;
    for(cmpt = 0; cmpt < NB_ITER; cmpt++)
    {
    	Op *o = Op_new(&TestOp);
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
    	fprintf(stderr, "All ops, in bloc should be computed : %d != %d\n", compteur_compute, NB_ITER);
    }

    OpContext_terminate(&c);
    Op_free(ob);

    if(ret == 0)
        exit(EXIT_SUCCESS);
    else
        exit(EXIT_FAILURE);

}
