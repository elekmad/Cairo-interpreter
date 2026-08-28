// %locations
%define parse.error verbose
//%define parse.trace
%locations
%code requires
{
	#include <OpCanva.h>
	#include <OpProgram.h>
	#include <OpModule.h>
}
%{

#include <stdio.h>
#include <OpCanva.h>
#include <OpProgram.h>
#include <OpModule.h>

extern int yylex();

void yyerror(Op **root, OpParser *p, const char *s);


Op *root;

%}

%union
{
    double number;
    char *string;
    Op *node;
    OpVariable *var;
    LinkedList *list;
}

%parse-param { Op **root }
%parse-param { OpParser *p }
%lex-param { OpParser *p }

%token <number> NUMBER
%token <string> IDENTIFIER
%token <string> TEXTCONTENT

%token RECTANGLE
%token SETBGCOLOR
%token SETAUTOSTROKECOLOR
%token SETAUTOFILLCOLOR
%token SETTEXTCOLOR
%token SETAUTOFILL
%token SETAUTOSTROKE
%token SETAUTOFILLSTROKE
%token SETAUTOSTROKEFILL
%token ARC
%token ARCNEG
%token CURVE
%token STROKE
%token STROKEPRESERVE
%token COLOR
%token SETLINEWIDTH
%token GETLINEWIDTH
%token FILL
%token FILLPRESERVE
%token ROTATE
%token TRANSLATE
%token MOVETO
%token SCALE
%token LINETO
%token IF
%token ELSE
%token IFX //Lower than else
%token FORLOOP
%token FOREACH
%token IN
%token WHILETRUE
%token EQ
%token NEQ
%token LEQ
%token GEQ
%token FONTSIZE
%token FONT
%token TEXT
%token TEXTPATH
%token TEXTEXTENTS
%token FONTEXTENTS
%token GETFIRST
%token GETLAST
%token GETLEN
%token GETSIZE
%token PI
%token PHI
%token RED
%token GREEN
%token BLUE
%token BLACK
%token WHITE
%token RADIANS
%token DEGREES
%token AND
%token OR
%token XOR
%token NOT
%token COS
%token ACOS
%token SIN
%token ASIN
%token CONCAT
%token UNSET
%token STRTOD
%token DTOSTR
%token TAN
%token ATAN
%token FLOOR
%token CEIL
%token POWER
%token SQRT
%token RANDINIT
%token RAND
%token MESSAGE
%token PREMESSAGE
%token SETOUTPUTSIZE
%token GETOUTPUTSIZE
%token SETPNGOUTPUT
%token SETSVGOUTPUT
%token DEFMODULE
%token CHILDS
%token INCLUDE

%type <node> program block expression statements statement
%type <list> call_args def_args
%type <var> number_list string_list
%destructor { free($$); } <string>

%left OR
%left XOR
%left AND

%left EQ NEQ
%left '<' '>' LEQ GEQ
%left '+' '-'
%left '*' '/'
%right APPEND
%right UMINUS
%right NOT

%nonassoc IFX
%nonassoc ELSE

%%
program:
		statement
		{
			OpBloc *op = (OpBloc*)OpCanvaBloc_new();
          	*root = (Op*)op;
          	Op_set_for_prerunning(*root);//Must be done before append because is recursive
          	OpCanvaBloc_set_auto_draw((OpCanvaBloc*)op);
          	$$ = (Op*)op;
          	if($1 != NULL)
          		OpBloc_append_Op(op, $1);
  			Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
		}
      | program statement
        {
        	$$ = $1;
        	if($2 != NULL)
          		OpBloc_append_Op((OpBloc*)$$, $2);
      		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
;

call_args:
	expression
	{
		LinkedList *list = LinkedList_new();
		LinkedList_append(list, $1);
		$$ = list;
	}
	| call_args ',' expression
	{
		LinkedList_append($$, $3);
	}
;

def_args:
	IDENTIFIER
	{
		LinkedList *list = LinkedList_new();
		LinkedList_append(list, $1);
		$$ = list;
	}
	| def_args ',' IDENTIFIER
	{
		LinkedList_append($$, $3);
	}
;

block:
		'{' statements '}'
		{
			OpBloc *op = (OpBloc*)OpBloc_new();
          $$ = (Op*)op;	
          OpBloc_append_Op(op, $2);	
      		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
		}
		| statement
		{
			OpBloc *op = (OpBloc*)OpBloc_new();
          $$ = (Op*)op;
          OpBloc_append_Op(op, $1);	
      		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
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
			if($2 != NULL)
				OpBloc_append_Op((OpBloc*)$$, $2);
		}

statement:

      IDENTIFIER '=' expression ';'
      {
      		size_t var_num = OpContext_get_variable_number(OpParser_get_current_context(p), $1);
      		OpSetVariable *op = (OpSetVariable*)OpSetVariable_new() ;
      		$$ = (Op*)op;
      		OpSetVariable_set_variable_number(op, var_num);
      		OpSetVariable_set_value(op, (Op*)$3);
      		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      		free($1);
      }
      
      | IDENTIFIER '(' call_args ')' ';'
      {
      		OpLaunchModule *op = (OpLaunchModule*)OpLaunchModule_new();
      		$$ = (Op*)op;
      		ssize_t module_num = OpProgram_check_module_number(OpParser_get_program(p), $1);
      		if(module_num >= 0)
      		{
	      		OpModule *m = OpProgram_get_module(OpParser_get_program(p), module_num);
	      		OpLaunchModule_set_module(op, m);
	      		OpLaunchModule_set_call_arguments(op, $3);
	      		LinkedList_free($3);
      			free($1);
      		}
      		else
      		{
      			LinkedList_free($3);
      			String s;
      			String_init(&s);
      			String_append_printf(&s, "Module '%s' not found", $1);
      			yyerror(root, p, String_get_char_string(&s));
      			String_finalize(&s);
      			free($1);
      			YYERROR;
  			}
  			fprintf(stderr, "CALL MODULE WITHOUT BLOCK\n");
      		
      }
      	| CHILDS '(' ')' ';'
      {
		OpLaunchModuleChilds *op = (OpLaunchModuleChilds *)OpLaunchModuleChilds_new();
      	$$ = (Op*)op;
      	OpModule *current = OpParser_get_current_module(p);
      	if(current == NULL)
		{
	        yyerror(root, p, "Use of 'children' outside module");
	        YYERROR;
    	}
      	OpLaunchModuleChilds_set_module(op, current);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      
      | IDENTIFIER '(' call_args ')' block
      {
      		OpLaunchModule *op = (OpLaunchModule*)OpLaunchModule_new();
      		$$ = (Op*)op;
      		ssize_t module_num = OpProgram_check_module_number(OpParser_get_program(p), $1);
      		if(module_num >= 0)
      		{
	      		OpModule *m = OpProgram_get_module(OpParser_get_program(p), module_num);
	      		OpLaunchModule_set_module(op, m);
	      		OpLaunchModule_set_call_arguments(op, $3);
	      		OpLaunchModule_set_parent(op, (OpCanvaContext*)OpParser_get_current_context(p));
	      		OpLaunchModule_set_childs(op, $5);
	      		LinkedList_free($3);
	      		free($1);
      		}
      		else
      		{
      			LinkedList_free($3);
      			String s;
      			String_init(&s);
      			String_append_printf(&s, "Module '%s' not found", $1);
      			yyerror(root, p, String_get_char_string(&s));
      			String_finalize(&s);
      			free($1);
      			YYERROR;
  			}
  			fprintf(stderr, "CALL MODULE WITH BLOCK\n");
      }
      
      | DEFMODULE IDENTIFIER
      {
      		size_t module_num = OpProgram_get_module_number(OpParser_get_program(p), $2);
      		free($2);
      		OpModule *m = OpProgram_get_module(OpParser_get_program(p), module_num);
      		OpParser_set_current_module(p, m);
      		OpParser_set_current_context(p, (OpContext*)OpModule_get_context(m));
      }
      '(' def_args ')' block
      {
      		OpModule *m = OpParser_get_current_module(p);
      		OpModule_add_to_root(m, $7);
      		OpModule_add_arguments(m, $5);
      		LinkedList_do_to_all($5, (void(*)(void*, void*))free, NULL);
      		LinkedList_free($5);
      		OpParser_set_current_context(p, OpProgram_get_context(OpParser_get_program(p)));
      		OpParser_set_current_module(p, NULL);
      		$$ = (Op*)NULL;
      }
      | INCLUDE TEXTCONTENT ';'
      {
			OpParser_parse_include(p, $2);
			free($2);
			$$ = NULL;
      }

      | UNSET '(' IDENTIFIER ')' ';'
      {
      		size_t var_num = OpContext_get_variable_number(OpProgram_get_context(OpParser_get_program(p)), $3);
      		OpSetVariable *op = (OpSetVariable*)OpSetVariable_new() ;
      		$$ = (Op*)op;
      		OpGetValue *g = (OpGetValue*)OpGetValue_new();
      		OpVariable empty;
      		OpVariable_init(&empty);
      		OpGetValue_copy_variable(g, &empty);//Not really needed as Internal Variable of GetValue is init to empty, but cleaner !
      		OpVariable_terminate(&empty);
      		OpSetVariable_set_variable_number(op, var_num);
      		OpSetVariable_set_value(op, (Op*)g);
      		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      		free($3);
      }
      	| SETOUTPUTSIZE '(' expression ',' expression ')' ';'
      {
		OpSetOutputSize *op = (OpSetOutputSize *)OpSetOutputSize_new();
      	$$ = (Op*)op;
		OpSetOutputSize_set_width(op, $3);
		OpSetOutputSize_set_height(op, $5);
      	Op_set_for_prerunning($$);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| SETPNGOUTPUT '(' ')' ';'
      {
		OpSetOutputPNG *op = (OpSetOutputPNG *)OpSetOutputPNG_new();
      	$$ = (Op*)op;
      	Op_set_for_prerunning($$);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| SETSVGOUTPUT '(' ')' ';'
      {
		OpSetOutputSVG *op = (OpSetOutputSVG *)OpSetOutputSVG_new();
      	$$ = (Op*)op;
      	Op_set_for_prerunning($$);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| SETAUTOFILL '(' ')' ';'
      {
		Op *op = (Op *)OpSetAutoFill_new();
      	$$ = (Op*)op;
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| SETAUTOSTROKE '(' ')' ';'
      {
		Op *op = (Op *)OpSetAutoStroke_new();
      	$$ = (Op*)op;
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| SETAUTOFILLSTROKE '(' ')' ';'
      {
		Op *op = (Op *)OpSetAutoFillStroke_new();
      	$$ = (Op*)op;
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| SETAUTOSTROKEFILL '(' ')' ';'
      {
		Op *op = (Op *)OpSetAutoStrokeFill_new();
      	$$ = (Op*)op;
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
    | IF '(' expression ')' block %prec IFX
    {
    	OpIf *op = (OpIf*)OpIf_new();
    	$$ = (Op*)op;
    	OpIf_set_condition(op, $3);
    	OpIf_set_true_branch(op, $5);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
    }
      
    | IF '(' expression ')' block ELSE block
    {
    	OpIf *op = (OpIf*)OpIf_new();
    	$$ = (Op*)op;
    	OpIf_set_condition(op, $3);
    	OpIf_set_true_branch(op, $5);
    	OpIf_set_false_branch(op, $7);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
    }
    | FORLOOP '(' IDENTIFIER '=' '[' expression ':' expression ':' expression ']' ')' block
    {
    	size_t var_num = OpContext_get_variable_number(OpParser_get_current_context(p), $3);
    	OpForLoop *op = (OpForLoop*)OpForLoop_new();
    	$$ = (Op*)op;
    	OpForLoop_set_start(op, $6);
    	OpForLoop_set_condition(op, $8);
    	OpForLoop_set_step(op, $10);
    	OpForLoop_set_loop(op, $13);
    	OpForLoop_set_variable_number(op, var_num);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
  		free($3);
    }
    | FORLOOP '(' IDENTIFIER '=' '[' expression ':' expression ']' ')' block
    {
    	size_t var_num = OpContext_get_variable_number(OpParser_get_current_context(p), $3);
    	OpForLoop *op = (OpForLoop*)OpForLoop_new();
    	$$ = (Op*)op;
    	OpForLoop_set_start(op, $6);
    	OpForLoop_set_condition(op, $8);
    	OpForLoop_set_loop(op, $11);
    	OpForLoop_set_variable_number(op, var_num);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
  		free($3);
    }
    | FOREACH '(' IDENTIFIER IN expression ')' block
    {
    	size_t var_num = OpContext_get_variable_number(OpParser_get_current_context(p), $3);
    	OpForEach *op = (OpForEach*)OpForEach_new();
    	$$ = (Op*)op;
    	OpForEach_set_value(op, $5);
    	OpForEach_set_loop(op, $7);
    	OpForEach_set_variable_number(op, var_num);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
  		free($3);
    }
    | WHILETRUE '(' expression ')' block
    {
    	OpWhile *op = (OpWhile*)OpWhile_new();
    	$$ = (Op*)op;
    	OpWhile_set_condition(op, $3);
    	OpWhile_set_bloc(op, $5);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
    }
      	| RANDINIT '(' expression ')' ';'
      {
		OpInitRandom *op = (OpInitRandom *)OpInitRandom_new();
      	$$ = (Op*)op;
    	OpInitRandom_set_value(op, $3);
      	Op_set_for_prerunning($$);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| RANDINIT '(' ')' ';'
      {
		OpInitRandom *op = (OpInitRandom *)OpInitRandom_new();
      	$$ = (Op*)op;
      	Op_set_for_prerunning($$);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
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
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }

    | RECTANGLE '(' expression ')' ';'
      {
      	OpRectangle *op = (OpRectangle*)OpRectangle_new();
      	$$ = (Op*)op;
      	OpRectangle_set_params(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
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
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }

    | ARC '(' expression  ')' ';'
      {
      	OpCircle *op = (OpCircle*)OpCircle_new();
      	$$ = (Op*)op;
		OpCircle_set_params(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
    | ARCNEG '(' expression ','
                 expression ','
                 expression ','
                 expression ','
                 expression ')' ';'
      {
      	OpCircle *op = (OpCircle*)OpCircle_new();
      	$$ = (Op*)op;
      	OpCircle_set_negative(op);
		OpCircle_set_x(op, $3);
      	OpCircle_set_y(op, $5);
      	OpCircle_set_r(op, $7);
      	OpCircle_set_a1(op, $9);
      	OpCircle_set_a2(op, $11);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
    | ARCNEG '(' expression ')' ';'
      {
      	OpCircle *op = (OpCircle*)OpCircle_new();
      	$$ = (Op*)op;
      	OpCircle_set_negative(op);
		OpCircle_set_params(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
    | CURVE '(' expression ','
                 expression ','
                 expression ','
                 expression ','
                 expression ','
                 expression ')' ';'
      {
      	OpCurve *op = (OpCurve*)OpCurve_new();
      	$$ = (Op*)op;
		OpCurve_set_m1x(op, $3);
      	OpCurve_set_m1y(op, $5);
      	OpCurve_set_m2x(op, $7);
      	OpCurve_set_m2y(op, $9);
      	OpCurve_set_x(op, $11);
      	OpCurve_set_y(op, $13);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
    | CURVE '(' expression ')' ';'
      {
      	OpCurve *op = (OpCurve*)OpCurve_new();
      	$$ = (Op*)op;
		OpCurve_set_params(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
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
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
      | COLOR '(' expression ')' ';'
      {
      	OpColor *op = (OpColor*)OpColor_new();
      	$$ = (Op*)op;
		OpColor_set_params(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
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
        OpCanvaBloc_set_auto_draw((OpCanvaBloc*)op);
  		Op_set_source_pos($$, @11.first_line, @11.first_column, @11.last_line, @11.last_column);
  		Op_set_source_pos((Op*)opc, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
    	| COLOR '(' expression ')' block
      {
      	OpBloc *op = (OpBloc*)OpCanvaBloc_new();
      	$$ = (Op*)op;
      	OpColor *opc = (OpColor*)OpColor_new();
		OpColor_set_params(opc, $3);
        OpBloc_append_Op(op, (Op*)opc);
        OpBloc_append_Op(op, $5);
        OpCanvaBloc_set_auto_draw((OpCanvaBloc*)op);
  		Op_set_source_pos($$, @5.first_line, @5.first_column, @5.last_line, @5.last_column);
  		Op_set_source_pos((Op*)opc, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
      | SETBGCOLOR '(' expression ','
                 expression ','
                 expression ','
                 expression ')' ';'
      {
      	OpColor *op = (OpColor*)OpSetBGColor_new();
      	$$ = (Op*)op;
		OpColor_set_red(op, $3);
      	OpColor_set_green(op, $5);
      	OpColor_set_blue(op, $7);
      	OpColor_set_alpha(op, $9);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
      | SETBGCOLOR '(' expression ')' ';'
      {
      	OpColor *op = (OpColor*)OpSetBGColor_new();
      	$$ = (Op*)op;
		OpColor_set_params(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
      | SETAUTOSTROKECOLOR '(' expression ','
                 expression ','
                 expression ','
                 expression ')' ';'
      {
      	OpColor *op = (OpColor*)OpSetDefaultStrokeColor_new();
      	$$ = (Op*)op;
		OpColor_set_red(op, $3);
      	OpColor_set_green(op, $5);
      	OpColor_set_blue(op, $7);
      	OpColor_set_alpha(op, $9);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
      | SETAUTOSTROKECOLOR '(' expression ')' ';'
      {
      	OpColor *op = (OpColor*)OpSetDefaultStrokeColor_new();
      	$$ = (Op*)op;
		OpColor_set_params(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
      | SETAUTOFILLCOLOR '(' expression ','
                 expression ','
                 expression ','
                 expression ')' ';'
      {
      	OpColor *op = (OpColor*)OpSetDefaultFillColor_new();
      	$$ = (Op*)op;
		OpColor_set_red(op, $3);
      	OpColor_set_green(op, $5);
      	OpColor_set_blue(op, $7);
      	OpColor_set_alpha(op, $9);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
      | SETAUTOFILLCOLOR '(' expression ')' ';'
      {
      	OpColor *op = (OpColor*)OpSetDefaultFillColor_new();
      	$$ = (Op*)op;
		OpColor_set_params(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
      | SETTEXTCOLOR '(' expression ','
                 expression ','
                 expression ','
                 expression ')' ';'
      {
      	OpColor *op = (OpColor*)OpSetTextColor_new();
      	$$ = (Op*)op;
		OpColor_set_red(op, $3);
      	OpColor_set_green(op, $5);
      	OpColor_set_blue(op, $7);
      	OpColor_set_alpha(op, $9);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
      | SETTEXTCOLOR '(' expression ')' ';'
      {
      	OpColor *op = (OpColor*)OpSetTextColor_new();
      	$$ = (Op*)op;
		OpColor_set_params(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| SETLINEWIDTH '(' expression ')' ';'
      {
      	OpSetLineWidth *op = (OpSetLineWidth*)OpSetLineWidth_new();
      	$$ = (Op*)op;
		OpSetLineWidth_set_width(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
    	| SETLINEWIDTH '(' expression ')' block
      {
      	OpBloc *op = (OpBloc*)OpCanvaBloc_new();
      	$$ = (Op*)op;
      	OpSetLineWidth *opl = (OpSetLineWidth*)OpSetLineWidth_new();
		OpSetLineWidth_set_width(opl, $3);
        OpBloc_append_Op(op, (Op*)opl);
        OpBloc_append_Op(op, $5);
  		Op_set_source_pos((Op*)opl, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
  		Op_set_source_pos($$, @5.first_line, @5.first_column, @5.last_line, @5.last_column);
      }
      	| ROTATE '(' expression ')' ';'
      {
      	OpRotate *op = (OpRotate*)OpRotate_new();
      	$$ = (Op*)op;
		OpRotate_set_angle(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
    	| ROTATE '(' expression ')' block
      {
      	OpBloc *op = (OpBloc*)OpCanvaBloc_new();
      	$$ = (Op*)op;
      	OpRotate *opr = (OpRotate*)OpRotate_new();
		OpRotate_set_angle(opr, $3);
        OpBloc_append_Op(op, (Op*)opr);
        OpBloc_append_Op(op, $5);
  		Op_set_source_pos((Op*)opr, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
  		Op_set_source_pos($$, @5.first_line, @5.first_column, @5.last_line, @5.last_column);
      }
      	| TRANSLATE '(' expression ','
      				   expression ')' ';'
      {
      	OpTranslate *op = (OpTranslate*)OpTranslate_new();
      	$$ = (Op*)op;
		OpTranslate_set_x(op, $3);
		OpTranslate_set_y(op, $5);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
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
  		Op_set_source_pos((Op*)opt, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
  		Op_set_source_pos($$, @7.first_line, @7.first_column, @7.last_line, @7.last_column);
      }
      	| TRANSLATE '(' expression ')' ';'
      {
      	OpTranslate *op = (OpTranslate*)OpTranslate_new();
      	$$ = (Op*)op;
		OpTranslate_set_params(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
    	| TRANSLATE '(' expression ')' block
      {
      	OpBloc *op = (OpBloc*)OpCanvaBloc_new();
      	$$ = (Op*)op;
      	OpTranslate *opt = (OpTranslate*)OpTranslate_new();
		OpTranslate_set_params(opt, $3);
        OpBloc_append_Op(op, (Op*)opt);
        OpBloc_append_Op(op, $5);
  		Op_set_source_pos((Op*)opt, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
  		Op_set_source_pos($$, @5.first_line, @5.first_column, @5.last_line, @5.last_column);
      }
      	| SCALE '(' expression ','
      				   expression ')' ';'
      {
      	OpScale *op = (OpScale*)OpScale_new();
      	$$ = (Op*)op;
		OpScale_set_x(op, $3);
		OpScale_set_y(op, $5);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
    	| SCALE '(' expression ','
      				    expression ')' block
      {
      	OpBloc *op = (OpBloc*)OpCanvaBloc_new();
      	$$ = (Op*)op;
      	OpScale *opt = (OpScale*)OpScale_new();
		OpScale_set_x(opt, $3);
		OpScale_set_y(opt, $5);
        OpBloc_append_Op(op, (Op*)opt);
        OpBloc_append_Op(op, $7);
  		Op_set_source_pos((Op*)opt, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
  		Op_set_source_pos($$, @7.first_line, @7.first_column, @7.last_line, @7.last_column);
      }
      	| SCALE '(' expression ')' ';'
      {
      	OpScale *op = (OpScale*)OpScale_new();
      	$$ = (Op*)op;
		OpScale_set_params(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
    	| SCALE '(' expression ')' block
      {
      	OpBloc *op = (OpBloc*)OpCanvaBloc_new();
      	$$ = (Op*)op;
      	OpScale *opt = (OpScale*)OpScale_new();
		OpScale_set_params(opt, $3);
        OpBloc_append_Op(op, (Op*)opt);
        OpBloc_append_Op(op, $5);
  		Op_set_source_pos((Op*)opt, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
  		Op_set_source_pos($$, @5.first_line, @5.first_column, @5.last_line, @5.last_column);
      }
      	| MOVETO '(' expression ','
      				   expression ')' ';'
      {
      	OpMoveTo *op = (OpMoveTo*)OpMoveTo_new();
      	$$ = (Op*)op;
		OpMoveTo_set_x(op, $3);
		OpMoveTo_set_y(op, $5);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| MOVETO '(' expression ')' ';'
      {
      	OpMoveTo *op = (OpMoveTo*)OpMoveTo_new();
      	$$ = (Op*)op;
		OpMoveTo_set_params(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| LINETO '(' expression ','
      				   expression ')' ';'
      {
      	OpDrawLineTo *op = (OpDrawLineTo*)OpDrawLineTo_new();
      	$$ = (Op*)op;
		OpDrawLineTo_set_x(op, $3);
		OpDrawLineTo_set_y(op, $5);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| LINETO '(' expression ')' ';'
      {
      	OpDrawLineTo *op = (OpDrawLineTo*)OpDrawLineTo_new();
      	$$ = (Op*)op;
		OpDrawLineTo_set_params(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| FONTSIZE '(' expression ')' ';'
      {
      	OpSetFontSize *op = (OpSetFontSize*)OpSetFontSize_new();
      	$$ = (Op*)op;
		OpSetFontSize_set_size(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| FONT '(' TEXTCONTENT ',' TEXTCONTENT ',' TEXTCONTENT ')' ';'
      {
		OpFontSelector *op = (OpFontSelector*)OpFontSelector_new();
      	$$ = (Op*)op;
		OpFontSelector_set_font_name(op, $3);
		OpFontSelector_set_font_slant(op, $5);
		OpFontSelector_set_font_weight(op, $7);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
  		free($3);
  		free($5);
  		free($7);
	  }
      	| TEXT '(' expression ')' ';'
      {
		OpDrawText *op = (OpDrawText*)OpDrawText_new();
      	$$ = (Op*)op;
		OpDrawText_set_text(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| TEXTPATH '(' expression ')' ';'
      {
		OpDrawText *op = (OpDrawText*)OpDrawText_new();
      	$$ = (Op*)op;
		OpDrawText_set_text(op, $3);
		OpDrawText_set_text_mode(op, path);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| MESSAGE '(' TEXTCONTENT ',' expression ')' ';'
      {
		OpPrintMessage *op = (OpPrintMessage*)OpPrintMessage_new();
      	$$ = (Op*)op;
		OpPrintMessage_set_message(op, $3);
		OpPrintMessage_set_value(op, $5);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
  		free($3);
	  }
      	| PREMESSAGE '(' TEXTCONTENT ',' expression ')' ';'
      {
		OpPrintMessage *op = (OpPrintMessage*)OpPrintMessage_new();
      	$$ = (Op*)op;
		OpPrintMessage_set_message(op, $3);
		OpPrintMessage_set_value(op, $5);
      	Op_set_for_prerunning($$);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
  		free($3);
	  }
      
      | FILLPRESERVE '(' ')' ';'
      {
      	$$ = OpFillPreserve_new();
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
      | STROKEPRESERVE '(' ')' ';'
      {
      	$$ = OpStrokePreserve_new();
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
      | FILL '(' ')' ';'
      {
      	$$ = OpFill_new();
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
      | STROKE '(' ')' ';'
      {
      	$$ = OpStroke_new();
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
;

number_list:
	NUMBER
	{
		OpVariable *var = OpVariable_new();
		OpVariable_append_double(var, $1);
      	$$ = var;
	}
	| '-' NUMBER
	{
		OpVariable *var = OpVariable_new();
		OpVariable_append_double(var, -1 * $2);
      	$$ = var;
	}
	
	| number_list ',' NUMBER
	{
		OpVariable_append_double((OpVariable*)$$, $3);
	}
	
	| number_list ',' '-' NUMBER
	{
		OpVariable_append_double((OpVariable*)$$, -1 * $4);
	}
;

string_list:
	TEXTCONTENT
	{
		OpVariable *var = OpVariable_new();
		OpVariable_append_string(var, $1);
      	$$ = var;
      	free($1);
	}
	| string_list ',' TEXTCONTENT
	{
		OpVariable_append_string((OpVariable*)$$, $3);
		free($3);
	}
;

expression:

      NUMBER
      {
      	OpGetValue *op = (OpGetValue*)OpGetValue_new();
      	$$ = (Op*)op;
      	OpGetValue_set_value(op, $1);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      } 
      | RAND '(' expression ')'
      {
		OpRandom *op = (OpRandom *)OpRandom_new();
      	$$ = (Op*)op;
    	OpRandom_set_value(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| RAND '(' ')'
      {
		OpRandom *op = (OpRandom *)OpRandom_new();
      	$$ = (Op*)op;
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      
      | TEXTCONTENT
      {
      	OpGetValue *op = (OpGetValue*)OpGetValue_new();
      	$$ = (Op*)op;
      	OpGetValue_set_value_string(op, $1);
      	free($1);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      	| TEXTEXTENTS '(' expression ')'
      {
		OpGetTextExtents *op = (OpGetTextExtents*)OpGetTextExtents_new();
      	$$ = (Op*)op;
		OpGetTextExtents_set_text(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| FONTEXTENTS '('  ')'
      {
		OpGetFontExtents *op = (OpGetFontExtents*)OpGetFontExtents_new();
      	$$ = (Op*)op;
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
		| GETLINEWIDTH '(' ')'
      {
      	$$ = OpGetLineWidth_new();
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| PI '('  ')'
      {
      	$$ = OpPi_new();
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| PHI '('  ')'
      {
      	$$ = OpPhi_new();
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| RED '('  ')'
      {
      	$$ = OpGetRedColor_new();
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| GREEN '('  ')'
      {
      	$$ = OpGetGreenColor_new();
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| BLUE '('  ')'
      {
      	$$ = OpGetBlueColor_new();
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| BLACK '('  ')'
      {
      	$$ = OpGetBlackColor_new();
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| WHITE '('  ')'
      {
      	$$ = OpGetWhiteColor_new();
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      | '[' number_list ']'
      {
      	OpGetValue *op = (OpGetValue*)OpGetValue_new();
      	$$ = (Op*)op;
      	OpGetValue_copy_variable(op, $2);
      	OpVariable_free($2);
  		Op_set_source_pos($$, @2.first_line, @2.first_column, @2.last_line, @2.last_column);
      }
      | '[' string_list ']'
      {
      	OpGetValue *op = (OpGetValue*)OpGetValue_new();
      	$$ = (Op*)op;
      	OpGetValue_copy_variable(op, $2);
      	OpVariable_free($2);
  		Op_set_source_pos($$, @2.first_line, @2.first_column, @2.last_line, @2.last_column);
      }
      | GETFIRST '(' expression ')'
      {
      	Op1 *op = (Op1*)OpGetFirst_new();
      	$$ = (Op*)op;
      	Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @4.last_line, @4.last_column);
      }
      | GETLAST '(' expression ')'
      {
      	Op1 *op = (Op1*)OpGetLast_new();
      	$$ = (Op*)op;
      	Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @4.last_line, @4.last_column);
      }
      | GETLEN '(' expression ')'
      {
      	Op1 *op = (Op1*)OpGetLen_new();
      	$$ = (Op*)op;
      	Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @4.last_line, @4.last_column);
      }
      | GETSIZE '(' expression ')'
      {
      	Op1 *op = (Op1*)OpGetSize_new();
      	$$ = (Op*)op;
      	Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @4.last_line, @4.last_column);
      }
      | '[' expression ':' expression ':' expression ']'
      {
      	OpIntervalGen *op = (OpIntervalGen*)OpIntervalGen_new();
      	$$ = (Op*)op;
      	OpIntervalGen_set_min(op, $2);
      	OpIntervalGen_set_max(op, $4);
      	OpIntervalGen_set_step(op, $6);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @7.last_line, @7.last_column);
      }
      | '[' expression ':' expression ']'
      {
      	OpIntervalGen *op = (OpIntervalGen*)OpIntervalGen_new();
      	$$ = (Op*)op;
      	OpIntervalGen_set_min(op, $2);
      	OpIntervalGen_set_max(op, $4);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @5.last_line, @5.last_column);
      }
      
    | CONCAT '(' expression ',' expression ')'
    {
    	Op2 *op = (Op2*)OpConcat_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $3);
      	Op2_set_operande2(op, $5);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
    }
    | expression APPEND expression
    {
    	Op2 *op = (Op2*)OpConcat_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @3.last_line, @3.last_column);
    }
      	| GETOUTPUTSIZE '(' ')'
      {
		OpGetOutputSize *op = (OpGetOutputSize *)OpGetOutputSize_new();
      	$$ = (Op*)op;
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }

    | IDENTIFIER
      {
  		size_t var_num = OpContext_get_variable_number(OpParser_get_current_context(p), $1);
      	OpGetVariable *op = (OpGetVariable*)OpGetVariable_new();
      	$$ = (Op*)op;
      	OpGetVariable_set_variable_number(op, var_num);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
  		free($1);
      }

    | IDENTIFIER '[' expression ']'
      {
    	Op2 *op = (Op2*)OpCrochets_new();
      	$$ = (Op*)op;
      	Op2_set_operande2(op, $3);
      	
  		size_t var_num = OpContext_get_variable_number(OpParser_get_current_context(p), $1);
      	OpGetVariable *opg = (OpGetVariable*)OpGetVariable_new();
      	OpGetVariable_set_variable_number(opg, var_num);
      	
      	Op2_set_operande1(op, (Op*)opg);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
  		free($1);
      }

    | expression EQ expression
      {
    	Op2 *op = (Op2*)OpEqu_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
  		Op_set_source_pos($$, @2.first_line, @2.first_column, @2.last_line, @2.last_column);
      }

    | expression NEQ expression
      {
    	Op2 *op = (Op2*)OpNotEqu_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
  		Op_set_source_pos($$, @2.first_line, @2.first_column, @2.last_line, @2.last_column);
      }

    | expression '<' expression
      {
    	Op2 *op = (Op2*)OpInf_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
  		Op_set_source_pos($$, @2.first_line, @2.first_column, @2.last_line, @2.last_column);
      }
    | expression LEQ expression
      {
    	Op2 *op = (Op2*)OpInfEqu_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
  		Op_set_source_pos($$, @2.first_line, @2.first_column, @2.last_line, @2.last_column);
      }

    | expression '>' expression
      {
    	Op2 *op = (Op2*)OpSup_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
  		Op_set_source_pos($$, @2.first_line, @2.first_column, @2.last_line, @2.last_column);
      }
    | expression GEQ expression
      {
    	Op2 *op = (Op2*)OpSupEqu_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
  		Op_set_source_pos($$, @2.first_line, @2.first_column, @2.last_line, @2.last_column);
      }

    | expression '+' expression
      {
    	Op2 *op = (Op2*)OpAdd_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
  		Op_set_source_pos($$, @2.first_line, @2.first_column, @2.last_line, @2.last_column);
      }

    | expression '-' expression
      {
    	Op2 *op = (Op2*)OpDel_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
  		Op_set_source_pos($$, @2.first_line, @2.first_column, @2.last_line, @2.last_column);
      }

    | expression '*' expression
      {
    	Op2 *op = (Op2*)OpMult_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
  		Op_set_source_pos($$, @2.first_line, @2.first_column, @2.last_line, @2.last_column);
      }

    | expression '/' expression
      {
    	Op2 *op = (Op2*)OpDiv_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
  		Op_set_source_pos($$, @2.first_line, @2.first_column, @2.last_line, @2.last_column);
      }

    | POWER '(' expression ',' expression ')'
      {
    	Op2 *op = (Op2*)OpPower_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $3);
      	Op2_set_operande2(op, $5);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }

    | '(' expression ')'
      {
          $$ = $2;
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @3.last_line, @3.last_column);
      }

    | '-' expression %prec UMINUS
      {
      	Op1 *op = (Op1*)OpNegValue_new();
      	$$ = (Op*)op;
      	Op1_set_operande(op, $2);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @2.last_line, @2.last_column);
      }

    | expression AND expression
      {
      	Op2 *op = (Op2*)OpAnd_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @3.last_line, @3.last_column);
      }

    | expression OR expression
      {
      	Op2 *op = (Op2*)OpOr_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @3.last_line, @3.last_column);
      }

    | expression XOR expression
      {
      	Op2 *op = (Op2*)OpXor_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $1);
      	Op2_set_operande2(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @3.last_line, @3.last_column);
      }

    | NOT expression
      {
      	Op1 *op = (Op1*)OpNot_new();
      	$$ = (Op*)op;
      	Op1_set_operande(op, $2);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @2.last_line, @2.last_column);
      }
		| STRTOD '(' expression ')'
      {
      	Op1 *op = (Op1*)OpStrToDouble_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| DTOSTR '(' expression ')'
      {
      	Op1 *op = (Op1*)OpDoubleToStr_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| RADIANS '(' expression ')'
      {
      	Op1 *op = (Op1*)OpRadians_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| DEGREES '(' expression ')'
      {
      	Op1 *op = (Op1*)OpDegrees_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| COS '(' expression ')'
      {
      	Op1 *op = (Op1*)OpCos_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| ACOS '(' expression ')'
      {
      	Op1 *op = (Op1*)OpAcos_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| SIN '(' expression ')'
      {
      	Op1 *op = (Op1*)OpSin_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| ASIN '(' expression ')'
      {
      	Op1 *op = (Op1*)OpAsin_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| TAN '(' expression ')'
      {
      	Op1 *op = (Op1*)OpTan_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| ATAN '(' expression ')'
      {
      	Op1 *op = (Op1*)OpAtan_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| FLOOR '(' expression ')'
      {
      	Op1 *op = (Op1*)OpFloor_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| CEIL '(' expression ')'
      {
      	Op1 *op = (Op1*)OpCeil_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| SQRT '(' expression ')'
      {
      	Op1 *op = (Op1*)OpSqrt_new();
      	$$ = (Op*)op;
		Op1_set_operande(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }

;

%%

void yyerror(Op **root, OpParser *p, const char *s)
{
	if(p != NULL)
	{
		SourcePos Pos = {
						.first_line = yylloc.first_line,
						.first_column = yylloc.first_column,
						.last_line = yylloc.last_line,
						.last_column = yylloc.last_column
						};
		OpContext_report_parse_error(OpParser_get_current_context(p), s, &Pos);
	}
	if(root != NULL && *root != NULL)
		fprintf(stderr, "Problem during parsing %s %p : %s @%d:%d\n", Op_get_name(*root), *root, s, yylloc.first_line, yylloc.first_column);
	else
		fprintf(stderr, "Problem during parsing with NULL op : %s\n", s);
}
