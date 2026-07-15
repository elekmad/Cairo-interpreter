%locations
%define parse.error verbose
%define parse.trace
%locations
%code requires
{
	#include <OpCanva.h>
}
%{

#include <stdio.h>
#include <OpCanva.h>

extern int yylex();
void yyerror(Op **root, OpContext *Ctx, const char *s)
{
	if(root != NULL && *root != NULL)
		printf("Problem during parsing %s %p : %s\n", Op_get_name(*root), *root, s);
	else
		printf("Problem during parsing with NULL op : %s\n", s);
}

Op *root;

%}

%union
{
    double number;
    char *string;
    Op *node;
}

%parse-param { Op **root }
%parse-param { OpContext *Ctx }

%token <number> NUMBER
%token <string> IDENTIFIER

%token RECTANGLE
%token ARC
%token STROKE
%token STROKEPRESERVE
%token COLOR
%token LINEWIDTH
%token FILL
%token FILLPRESERVE
%token ROTATE
%token TRANSLATE
%token IF
%token ELSE
%token IFX //Lower than else
%token FORLOOP
%token EQ
%token NEQ
%token LEQ
%token GEQ

%type <node> program block expression statements statement

%left EQ NEQ
%left '<' '>' LEQ GEQ
%left '+' '-'
%left '*' '/'
%right UMINUS
%right '!'

%nonassoc IFX
%nonassoc ELSE

%%
program:
		statement
		{
			OpBloc *op = (OpBloc*)OpBloc_new();
          *root = (Op*)op;
          $$ = (Op*)op;
          OpBloc_append_Op(op, $1);
		}
      | program statement
        {
        	$$ = $1;
          OpBloc_append_Op((OpBloc*)$$, $2);
      }
;

block:
		'{' statements '}'
		{
			OpBloc *op = (OpBloc*)OpBloc_new();
          $$ = (Op*)op;	
          OpBloc_append_Op(op, $2);	
		}
		| statement
		{
			OpBloc *op = (OpBloc*)OpBloc_new();
          $$ = (Op*)op;
          OpBloc_append_Op(op, $1);	
		}
;

statements:
		{
			OpBloc *op = (OpBloc*)OpBloc_new();
          $$ = (Op*)op;
		}
		| statements statement
		{
			$$ = $1;
			OpBloc_append_Op((OpBloc*)$$, $2);
		}

statement:

      IDENTIFIER '=' expression ';'
      {
      		size_t var_num = OpContext_get_variable_number(Ctx, $1);
      		OpSetVariable *op = (OpSetVariable*)OpSetVariable_new() ;
      		$$ = (Op*)op;
      		OpSetVariable_set_variable_number(op, var_num);
      		OpSetVariable_set_value(op, (Op*)$3);
      }
    | IF '(' expression ')' block %prec IFX
    {
    	OpIf *op = (OpIf*)OpIf_new();
    	$$ = (Op*)op;
    	OpIf_set_condition(op, $3);
    	OpIf_set_true_branch(op, $5);
    }
      
    | IF '(' expression ')' block ELSE block
    {
    	OpIf *op = (OpIf*)OpIf_new();
    	$$ = (Op*)op;
    	OpIf_set_condition(op, $3);
    	OpIf_set_true_branch(op, $5);
    	OpIf_set_false_branch(op, $7);
    }
    | FORLOOP '(' IDENTIFIER '=' '[' NUMBER ':' NUMBER ':' NUMBER ']' ')' block
    {
    	size_t var_num = OpContext_get_variable_number(Ctx, $3);
    	OpForLoop *op = (OpForLoop*)OpForLoop_new();
    	$$ = (Op*)op;
    	OpForLoop_set_start(op, $6);
    	OpForLoop_set_condition(op, $8);
    	OpForLoop_set_step(op, $10);
    	OpForLoop_set_loop(op, $13);
    	OpForLoop_set_variable_number(op, var_num);
    }

    | RECTANGLE '(' expression ','
                    expression ','
                    expression ','
                    expression ')' ';'
      {
      	OpRectangle *op = (OpRectangle*)OpRectangle_new();
      	$$ = (Op*)op;
      	OpRectangle_set_x(op, $3);
      	OpRectangle_set_y(op, $5);
      	OpRectangle_set_w(op, $7);
      	OpRectangle_set_h(op, $9);
      }

    | ARC '(' expression ','
                 expression ','
                 expression ','
                 expression ','
                 expression ')' ';'
      {
      	OpCircle *op = (OpCircle*)OpCircle_new();
      	$$ = (Op*)op;
		OpCircle_set_x(op, $3);
      	OpCircle_set_y(op, $5);
      	OpCircle_set_r(op, $7);
      	OpCircle_set_a1(op, $9);
      	OpCircle_set_a2(op, $11);
      }
      
      | COLOR '(' expression ','
                 expression ','
                 expression ','
                 expression ')' ';'
      {
      	OpColor *op = (OpColor*)OpColor_new();
      	$$ = (Op*)op;
		OpColor_set_red(op, $3);
      	OpColor_set_green(op, $5);
      	OpColor_set_blue(op, $7);
      	OpColor_set_alpha(op, $9);
      }
      
    	| COLOR '(' expression ','
                 expression ','
                 expression ','
                 expression ')' block
      {
      	OpBloc *op = (OpBloc*)OpCanvaBloc_new();
      	$$ = (Op*)op;
      	OpColor *opc = (OpColor*)OpColor_new();
		OpColor_set_red(opc, $3);
      	OpColor_set_green(opc, $5);
      	OpColor_set_blue(opc, $7);
      	OpColor_set_alpha(opc, $9);
        OpBloc_append_Op(op, (Op*)opc);
        OpBloc_append_Op(op, $11);
        OpCanvaBloc_set_auto_stroke((OpCanvaBloc*)op);
      }
		| LINEWIDTH '(' expression ')' ';'
      {
      	OpSetLineWidth *op = (OpSetLineWidth*)OpSetLineWidth_new();
      	$$ = (Op*)op;
		OpSetLineWidth_set_width(op, $3);
      }
    	| LINEWIDTH '(' expression ')' block
      {
      	OpBloc *op = (OpBloc*)OpCanvaBloc_new();
      	$$ = (Op*)op;
      	OpSetLineWidth *opl = (OpSetLineWidth*)OpSetLineWidth_new();
		OpSetLineWidth_set_width(opl, $3);
        OpBloc_append_Op(op, (Op*)opl);
        OpBloc_append_Op(op, $5);
      }
      	| ROTATE '(' expression ')' ';'
      {
      	OpRotate *op = (OpRotate*)OpRotate_new();
      	$$ = (Op*)op;
		OpRotate_set_angle(op, $3);
      }
    	| ROTATE '(' expression ')' block
      {
      	OpBloc *op = (OpBloc*)OpCanvaBloc_new();
      	$$ = (Op*)op;
      	OpRotate *opr = (OpRotate*)OpRotate_new();
		OpRotate_set_angle(opr, $3);
        OpBloc_append_Op(op, (Op*)opr);
        OpBloc_append_Op(op, $5);
      }
      	| TRANSLATE '(' expression ','
      				   expression ')' ';'
      {
      	OpTranslate *op = (OpTranslate*)OpTranslate_new();
      	$$ = (Op*)op;
		OpTranslate_set_x(op, $3);
		OpTranslate_set_y(op, $5);
      }
    	| TRANSLATE '(' expression ','
      				    expression ')' block
      {
      	OpBloc *op = (OpBloc*)OpCanvaBloc_new();
      	$$ = (Op*)op;
      	OpTranslate *opt = (OpTranslate*)OpTranslate_new();
		OpTranslate_set_x(opt, $3);
		OpTranslate_set_y(opt, $5);
        OpBloc_append_Op(op, (Op*)opt);
        OpBloc_append_Op(op, $7);
      }
      
      | FILLPRESERVE '(' ')' ';'
      {
      	$$ = OpFillPreserve_new();
      }
      
      | STROKEPRESERVE '(' ')' ';'
      {
      	$$ = OpStrokePreserve_new();
      }
      
      | FILL '(' ')' ';'
      {
      	$$ = OpFill_new();
      }
      
      | STROKE '(' ')' ';'
      {
      	$$ = OpStroke_new();
      }
;

expression:

      NUMBER
      {
      	OpGetValue *op = (OpGetValue*)OpGetValue_new();
      	$$ = (Op*)op;
      	OpGetValue_set_value(op, $1);
      }

    | IDENTIFIER
      {
  		size_t var_num = OpContext_get_variable_number(Ctx, $1);
      	OpGetVariable *op = (OpGetVariable*)OpGetVariable_new();
      	$$ = (Op*)op;
      	OpGetVariable_set_variable_number(op, var_num);
      }

    | expression EQ expression
      {
    	Op2 *op = (Op2*)OpEqu_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
      }

    | expression NEQ expression
      {
    	Op2 *op = (Op2*)OpNotEqu_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
      }

    | expression '<' expression
      {
    	Op2 *op = (Op2*)OpInf_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
      }
    | expression LEQ expression
      {
    	Op2 *op = (Op2*)OpInfEqu_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
      }

    | expression '>' expression
      {
    	Op2 *op = (Op2*)OpSup_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
      }
    | expression GEQ expression
      {
    	Op2 *op = (Op2*)OpSupEqu_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
      }

    | expression '+' expression
      {
    	Op2 *op = (Op2*)OpAdd_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
      }

    | expression '-' expression
      {
    	Op2 *op = (Op2*)OpDel_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
      }

    | expression '*' expression
      {
    	Op2 *op = (Op2*)OpMult_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
      }

    | expression '/' expression
      {
    	Op2 *op = (Op2*)OpDiv_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
      }

    | '(' expression ')'
      {
          $$ = $2;
      }

    | '-' expression %prec UMINUS
      {
      	OpNegValue *op = (OpNegValue*)OpNegValue_new();
      	$$ = (Op*)op;
      	OpNegValue_set_value(op, $2);
      }

    | '!' expression
      {
      	OpLogicalNegValue *op = (OpLogicalNegValue*)OpLogicalNegValue_new();
      	$$ = (Op*)op;
      	OpLogicalNegValue_set_value(op, $2);
      }

;

%%