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
%token TEXTEXTENTS
%token FONTEXTENTS
%token PI
%token PHI
%token RED
%token GREEN
%token BLUE
%token RADIANS
%token DEGREES
%token COS
%token ACOS
%token SIN
%token ASIN
%token CONCAT
%token TAN
%token ATAN
%token FLOOR
%token CEIL
%token POWER
%token SQRT
%token MESSAGE
%token PREMESSAGE
%token SETOUTPUTSIZE
%token GETOUTPUTSIZE
%token SETPNGOUTPUT
%token SETSVGOUTPUT

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
          Op_set_for_prerunning(*root);//Must be done before append because is recursive
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
      		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
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
      	| SETPNGOUTPUT '(' TEXTCONTENT ')' ';'
      {
		OpSetOutputPNG *op = (OpSetOutputPNG *)OpSetOutputPNG_new();
      	$$ = (Op*)op;
		OpSetOutputPNG_set_filename(op, $3);
      	Op_set_for_prerunning($$);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| SETSVGOUTPUT '(' TEXTCONTENT ')' ';'
      {
		OpSetOutputSVG *op = (OpSetOutputSVG *)OpSetOutputSVG_new();
      	$$ = (Op*)op;
		OpSetOutputSVG_set_filename(op, $3);
      	Op_set_for_prerunning($$);
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
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
    }
    | WHILETRUE '(' expression ')' block
    {
    	OpWhile *op = (OpWhile*)OpWhile_new();
    	$$ = (Op*)op;
    	OpWhile_set_condition(op, $3);
    	OpWhile_set_bloc(op, $5);
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
        OpCanvaBloc_set_auto_stroke((OpCanvaBloc*)op);
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
        OpCanvaBloc_set_auto_stroke((OpCanvaBloc*)op);
  		Op_set_source_pos($$, @5.first_line, @5.first_column, @5.last_line, @5.last_column);
  		Op_set_source_pos((Op*)opc, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
		| LINEWIDTH '(' expression ')' ';'
      {
      	OpSetLineWidth *op = (OpSetLineWidth*)OpSetLineWidth_new();
      	$$ = (Op*)op;
		OpSetLineWidth_set_width(op, $3);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
    	| LINEWIDTH '(' expression ')' block
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
		OpMessage *op = (OpMessage*)OpMessage_new();
      	$$ = (Op*)op;
		OpMessage_set_message(op, $3);
		OpMessage_set_value(op, $5);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }
      	| PREMESSAGE '(' TEXTCONTENT ',' expression ')' ';'
      {
		OpMessage *op = (OpMessage*)OpMessage_new();
      	$$ = (Op*)op;
		OpMessage_set_message(op, $3);
		OpMessage_set_value(op, $5);
      	Op_set_for_prerunning($$);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
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
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
      }
      
      | TEXTCONTENT
      {
      	OpGetValue *op = (OpGetValue*)OpGetValue_new();
      	$$ = (Op*)op;
      	OpGetValue_set_value_string(op, $1);
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
      
    | CONCAT '(' expression ',' expression ')'
    {
    	Op2 *op = (Op2*)OpConcat_new();
      	$$ = (Op*)op;
      	Op2_set_operande1(op, $3);
      	Op2_set_operande2(op, $5);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
    }
      	| GETOUTPUTSIZE '(' ')'
      {
		OpGetOutputSize *op = (OpGetOutputSize *)OpGetOutputSize_new();
      	$$ = (Op*)op;
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
	  }

    | IDENTIFIER
      {
  		size_t var_num = OpContext_get_variable_number(Ctx, $1);
      	OpGetVariable *op = (OpGetVariable*)OpGetVariable_new();
      	$$ = (Op*)op;
      	OpGetVariable_set_variable_number(op, var_num);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
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
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @1.last_line, @1.last_column);
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

    | '!' expression
      {
      	Op1 *op = (Op1*)OpLogicalNegValue_new();
      	$$ = (Op*)op;
      	Op1_set_operande(op, $2);
  		Op_set_source_pos($$, @1.first_line, @1.first_column, @2.last_line, @2.last_column);
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