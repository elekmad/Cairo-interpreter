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
    OpVariable *var;
}

%parse-param { Op **root }
%parse-param { OpContext *Ctx }

%token <number> NUMBER
%token <string> IDENTIFIER
%token <string> TEXTCONTENT

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
%token MOVETO
%token IF
%token ELSE
%token IFX //Lower than else
%token FORLOOP
%token WHILETRUE
%token EQ
%token NEQ
%token LEQ
%token GEQ
%token FONTSIZE
%token FONT
%token TEXT
%token TEXTPATH
%token PI
%token RADIANS
%token DEGREES
%token COS
%token ACOS
%token SIN
%token ASIN
%token TAN
%token ATAN
%token FLOOR
%token CEIL
%token POWER
%token SQRT
%token MESSAGE

%type <node> program block expression statements statement
%type <var> number_list string_list

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
    | WHILETRUE '(' expression ')' block
    {
    	OpWhile *op = (OpWhile*)OpWhile_new();
    	$$ = (Op*)op;
    	OpWhile_set_condition(op, $3);
    	OpWhile_set_bloc(op, $5);
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
      	| MOVETO '(' expression ','
      				   expression ')' ';'
      {
      	OpMoveTo *op = (OpMoveTo*)OpMoveTo_new();
      	$$ = (Op*)op;
		OpMoveTo_set_x(op, $3);
		OpMoveTo_set_y(op, $5);
	  }
      	| FONTSIZE '(' expression ')' ';'
      {
      	OpSetFontSize *op = (OpSetFontSize*)OpSetFontSize_new();
      	$$ = (Op*)op;
		OpSetFontSize_set_size(op, $3);
	  }
      	| FONT '(' TEXTCONTENT ',' TEXTCONTENT ',' TEXTCONTENT ')' ';'
      {
		OpFontSelector *op = (OpFontSelector*)OpFontSelector_new();
      	$$ = (Op*)op;
		OpFontSelector_set_font_name(op, $3);
		OpFontSelector_set_font_slant(op, $5);
		OpFontSelector_set_font_weight(op, $7);
	  }
      	| TEXT '(' TEXTCONTENT ')' ';'
      {
		OpDrawText *op = (OpDrawText*)OpDrawText_new();
      	$$ = (Op*)op;
		OpDrawText_set_text(op, $3);
	  }
      	| TEXTPATH '(' TEXTCONTENT ')' ';'
      {
		OpDrawText *op = (OpDrawText*)OpDrawText_new();
      	$$ = (Op*)op;
		OpDrawText_set_text(op, $3);
		OpDrawText_set_text_mode(op, path);
	  }
      	| MESSAGE '(' TEXTCONTENT ',' expression ')' ';'
      {
		OpMessage *op = (OpMessage*)OpMessage_new();
      	$$ = (Op*)op;
		OpMessage_set_message(op, $3);
		OpMessage_set_value(op, $5);
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

number_list:
	NUMBER
	{
		OpVariable *var = OpVariable_new();
		OpVariable_append_double(var, $1);
      	$$ = var;
	}
	| number_list ',' NUMBER
	{
		OpVariable_append_double((OpVariable*)$$, $3);
	}
;

string_list:
	TEXTCONTENT
	{
		OpVariable *var = OpVariable_new();
		OpVariable_append_string(var, $1);
      	$$ = var;
	}
	| string_list ',' TEXTCONTENT
	{
		OpVariable_append_string((OpVariable*)$$, $3);
	}
;

expression:

      NUMBER
      {
      	OpGetValue *op = (OpGetValue*)OpGetValue_new();
      	$$ = (Op*)op;
      	OpGetValue_set_value(op, $1);
      }
      
      | TEXTCONTENT
      {
      	OpGetValue *op = (OpGetValue*)OpGetValue_new();
      	$$ = (Op*)op;
      	OpGetValue_set_value_string(op, $1);
      }
		| PI '('  ')'
      {
      	$$ = OpPi_new();
      }
      | '[' number_list ']'
      {
      	OpGetValue *op = (OpGetValue*)OpGetValue_new();
      	$$ = (Op*)op;
      	OpGetValue_copy_variable(op, $2);
      	OpVariable_free($2);
      }
      | '[' string_list ']'
      {
      	OpGetValue *op = (OpGetValue*)OpGetValue_new();
      	$$ = (Op*)op;
      	OpGetValue_copy_variable(op, $2);
      	OpVariable_free($2);
      }

    | IDENTIFIER
      {
  		size_t var_num = OpContext_get_variable_number(Ctx, $1);
      	OpGetVariable *op = (OpGetVariable*)OpGetVariable_new();
      	$$ = (Op*)op;
      	OpGetVariable_set_variable_number(op, var_num);
      }

    | IDENTIFIER '[' expression ']'
      {
    	Op2 *op = (Op2*)OpCrochets_new();
      	$$ = (Op*)op;
      	Op2_set_operande2(op, $3);
      	
  		size_t var_num = OpContext_get_variable_number(Ctx, $1);
      	OpGetVariable *opg = (OpGetVariable*)OpGetVariable_new();
      	OpGetVariable_set_variable_number(opg, var_num);
      	
      	Op2_set_operande1(op, (Op*)opg);
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

    | POWER '(' expression ',' expression ')'
      {
    	Op2 *op = (Op2*)OpPower_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $3);
      	Op2_set_operande2(op, $5);
      }

    | '(' expression ')'
      {
          $$ = $2;
      }

    | '-' expression %prec UMINUS
      {
      	Op1 *op = (Op1*)OpNegValue_new();
      	$$ = (Op*)op;
      	Op1_set_operande(op, $2);
      }

    | '!' expression
      {
      	Op1 *op = (Op1*)OpLogicalNegValue_new();
      	$$ = (Op*)op;
      	Op1_set_operande(op, $2);
      }
		| RADIANS '(' expression ')'
      {
      	Op1 *op = (Op1*)OpRadians_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
      }
		| DEGREES '(' expression ')'
      {
      	Op1 *op = (Op1*)OpDegrees_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
      }
		| COS '(' expression ')'
      {
      	Op1 *op = (Op1*)OpCos_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
      }
		| ACOS '(' expression ')'
      {
      	Op1 *op = (Op1*)OpAcos_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
      }
		| SIN '(' expression ')'
      {
      	Op1 *op = (Op1*)OpSin_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
      }
		| ASIN '(' expression ')'
      {
      	Op1 *op = (Op1*)OpAsin_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
      }
		| TAN '(' expression ')'
      {
      	Op1 *op = (Op1*)OpTan_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
      }
		| ATAN '(' expression ')'
      {
      	Op1 *op = (Op1*)OpAtan_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
      }
		| FLOOR '(' expression ')'
      {
      	Op1 *op = (Op1*)OpFloor_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
      }
		| CEIL '(' expression ')'
      {
      	Op1 *op = (Op1*)OpCeil_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
      }
		| SQRT '(' expression ')'
      {
      	Op1 *op = (Op1*)OpSqrt_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
      }

;

%%