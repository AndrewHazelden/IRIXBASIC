/*
 * Copyright (c) 2006, Adam Dunkels
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#define DEBUG 0

#if DEBUG
#define DEBUG_PRINTF(...)  printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

#include "ubasic.h"
#include "tokenizer.h"

#include <stdio.h> /* For printf() */
#include <stdlib.h> /* For exit(), rand() and srand()*/
#include <string.h> /* For strcpy */
#include <math.h> /* For cos and sin */
#include <time.h> /* For time(), used with srand(). */


extern int screen_width;
extern int screen_height;

static char *program_ptr;
#define MAX_STRINGLEN 40
static char string[MAX_STRINGLEN];

#define MAX_GOSUB_STACK_DEPTH 10
static int gosub_stack[MAX_GOSUB_STACK_DEPTH];
static int gosub_stack_ptr;

struct for_state {
	int line_after_for;
	int for_variable;
	int to;
	int step;
};
#define MAX_FOR_STACK_DEPTH 4
static struct for_state for_stack[MAX_FOR_STACK_DEPTH];
static int for_stack_ptr;

#define MAX_VARNUM 26
static char variables[MAX_VARNUM];

static int ended;

static int expr(void);
static void line_statement(void);
static void statement(void);

/*---------------------------------------------------------------------------*/
void
ubasic_init(char *program)
{
	program_ptr = program;
	for_stack_ptr = gosub_stack_ptr = 0;
	tokenizer_init(program);
	ended = 0;
}
/*---------------------------------------------------------------------------*/
static void
accept(int token)
{
	if(token != tokenizer_token()) {
		DEBUG_PRINTF("Token not what was expected (expected %d, got %d)\n",
					 token, tokenizer_token());
		tokenizer_error_print();
		exit(1);
	}
	DEBUG_PRINTF("Expected %d, got it\n", token);
	tokenizer_next();
}
/*---------------------------------------------------------------------------*/
static int
varfactor(void)
{
	int r;
	DEBUG_PRINTF("varfactor: obtaining %d from variable %d\n", variables[tokenizer_variable_num()], tokenizer_variable_num());
	r = ubasic_get_variable(tokenizer_variable_num());
	accept(TOKENIZER_VARIABLE);
	return r;
}
/*---------------------------------------------------------------------------*/
static int
factor(void)
{
	int r, t;
	float resultf;
	
	DEBUG_PRINTF("factor: token %d\n", tokenizer_token());
	switch(tokenizer_token()) {
		case TOKENIZER_NUMBER:
			r = tokenizer_num();
			DEBUG_PRINTF("factor: number %d\n", r);
			accept(TOKENIZER_NUMBER);
			break;
			
		case TOKENIZER_LEFTPAREN:
			accept(TOKENIZER_LEFTPAREN);
			r = expr();
			accept(TOKENIZER_RIGHTPAREN);
			break;
			
		case TOKENIZER_RANDOM:
			accept(TOKENIZER_RANDOM);
			r = rand()%127;
			break;
			
		case TOKENIZER_TIMER:
			accept(TOKENIZER_TIMER);
			r = (long int)time(NULL) - (long int)start_time;
			DEBUG_PRINTF("Start time: %ld\n", start_time);
			DEBUG_PRINTF("current time: %ld\n", time(NULL));
			DEBUG_PRINTF("uptime: %d\n", r);
			break;

		case TOKENIZER_SCREENWIDTH:
    	accept(TOKENIZER_SCREENWIDTH);
   		r = screenwidth_get_statement();
			DEBUG_PRINTF("Screen Width: %d\n", r);
			break;
			
		case TOKENIZER_SCREENHEIGHT:
			accept(TOKENIZER_SCREENHEIGHT);
			r = screenheight_get_statement();
			DEBUG_PRINTF("Screen Height: %d\n", r);
			break;
				
		case TOKENIZER_ISBUTTON:
			accept(TOKENIZER_ISBUTTON);
			GetInput();
			r=isbutton_down;
			if(r){
				printf("Mouse Button Clicked\n");
			}
			DEBUG_PRINTF("Mouse Button State: %d\n", r);
			break;


		case TOKENIZER_COS:
			accept(TOKENIZER_COS);				
			t = expr();
			resultf = cos(t)*(float)127;
			r = (int)resultf;
			DEBUG_PRINTF("7-bit COSINE of %d is %d\n", t, r);
			break;
				
		case TOKENIZER_SIN:
			accept(TOKENIZER_SIN);				
			t = expr();
			resultf = sin(t)*(float)127;
			r = (int)resultf;
			DEBUG_PRINTF("7-bit SINE of %d is %d\n", t, r);
			break;		
				
		case TOKENIZER_CHR:
			accept(TOKENIZER_CHR);
			accept(TOKENIZER_LEFTPAREN);
			r = expr();
			accept(TOKENIZER_RIGHTPAREN);
			DEBUG_PRINTF("ASCII Character: %c\n", r);	
			break;
			
		default:
			r = varfactor();
			break;
	}
	return r;
}
/*---------------------------------------------------------------------------*/
static int
term(void)
{
	int f1, f2;
	int op;
	
	f1 = factor();
	op = tokenizer_token();
	DEBUG_PRINTF("term: token %d\n", op);
	while(op == TOKENIZER_ASTR ||
		  op == TOKENIZER_SLASH ||
		  op == TOKENIZER_MOD) {
		tokenizer_next();
		f2 = factor();
		DEBUG_PRINTF("term: %d %d %d\n", f1, op, f2);
		switch(op) {
			case TOKENIZER_ASTR:
				f1 = f1 * f2;
				break;
			case TOKENIZER_SLASH:
				f1 = f1 / f2;
				break;
			case TOKENIZER_MOD:
				f1 = f1 % f2;
				break;
		}
		op = tokenizer_token();
	}
	DEBUG_PRINTF("term: %d\n", f1);
	return f1;
}
/*---------------------------------------------------------------------------*/
static int
expr(void)
{
	int t1, t2;
	int op;
	
	t1 = term();
	op = tokenizer_token();
	DEBUG_PRINTF("expr: token %d\n", op);
	while(op == TOKENIZER_PLUS ||
		  op == TOKENIZER_MINUS ||
		  op == TOKENIZER_AND ||
		  op == TOKENIZER_OR) {
		tokenizer_next();
		t2 = term();
		DEBUG_PRINTF("expr: %d %d %d\n", t1, op, t2);
		switch(op) {
			case TOKENIZER_PLUS:
				t1 = t1 + t2;
				break;
			case TOKENIZER_MINUS:
				t1 = t1 - t2;
				break;
			case TOKENIZER_AND:
				t1 = t1 & t2;
				break;
			case TOKENIZER_OR:
				t1 = t1 | t2;
				break;
		}
		op = tokenizer_token();
	}
	DEBUG_PRINTF("expr: %d\n", t1);
	return t1;
}
/*---------------------------------------------------------------------------*/
static int
relation(void)
{
	int r1, r2;
	int op;
	
	r1 = expr();
	op = tokenizer_token();
	DEBUG_PRINTF("relation: token %d\n", op);
	while(op == TOKENIZER_LT ||
		  op == TOKENIZER_GT ||
		  op == TOKENIZER_EQ) {
		tokenizer_next();
		r2 = expr();
		DEBUG_PRINTF("relation: %d %d %d\n", r1, op, r2);
		switch(op) {
			case TOKENIZER_LT:
				r1 = r1 < r2;
				break;
			case TOKENIZER_GT:
				r1 = r1 > r2;
				break;
			case TOKENIZER_EQ:
				r1 = r1 == r2;
				break;
		}
		op = tokenizer_token();
	}
	return r1;
}
/*---------------------------------------------------------------------------*/
static void
jump_linenum(int linenum)
{
	tokenizer_init(program_ptr);
	while(tokenizer_num() != linenum) {
		do {
			do {
				tokenizer_next();
			} while(tokenizer_token() != TOKENIZER_CR &&
					tokenizer_token() != TOKENIZER_ENDOFINPUT);
			if(tokenizer_token() == TOKENIZER_CR) {
				tokenizer_next();
			}
		} while(tokenizer_token() != TOKENIZER_NUMBER);
		DEBUG_PRINTF("jump_linenum: Found line %d\n", tokenizer_num());
	}
}
/*---------------------------------------------------------------------------*/
static void
goto_statement(void)
{
	accept(TOKENIZER_GOTO);
	jump_linenum(tokenizer_num());
}
/*---------------------------------------------------------------------------*/
static void
print_statement(void)
{
	
	char one_line[1024];
	char textChunk[255];
	int i;
	int temp_value = 0;
	
	/* Clear one_line */
	for(i=0;i<=1024;i++){
		one_line[i]=0;
	}
	
	/* Clear textChunk */
	for(i=0;i<=255;i++){
		textChunk[i]=0;
	}
	
	accept(TOKENIZER_PRINT);
	do {
		DEBUG_PRINTF("Print loop\n");
		if(tokenizer_token() == TOKENIZER_STRING) {
			tokenizer_string(string, sizeof(string));
			
			printf("%s", string);
			
			/* Copy text to onscreen display buffer*/
			sprintf(textChunk,"%s", string);
			strcat(one_line, textChunk);
			
			tokenizer_next();
		} else if(tokenizer_token() == TOKENIZER_COMMA) {
			printf(" ");
			
			/* Copy text to onscreen display buffer*/
			sprintf(textChunk," ", string);
			strcat(one_line, textChunk);
			
			tokenizer_next();
		} else if(tokenizer_token() == TOKENIZER_SEMICOLON) {
			tokenizer_next();
		} else if(tokenizer_token() == TOKENIZER_VARIABLE ||
				  tokenizer_token() == TOKENIZER_NUMBER) {
			
			temp_value = expr();
			printf("%d", temp_value);
			
			/* Copy text to onscreen display buffer*/
			sprintf(textChunk,"%d", temp_value);
			strcat(one_line, textChunk);
			
		} else {
			break;
		}
	} while(tokenizer_token() != TOKENIZER_CR &&
			tokenizer_token() != TOKENIZER_ENDOFINPUT);
	printf("\n");
	DEBUG_PRINTF("End of print\n");
	
	/* Copy text to onscreen display */
	PrintTextLine(one_line);
	
	tokenizer_next();
}

/*---------------------------------------------------------------------------*/
static void 
list_statement(void)
{
	accept(TOKENIZER_LIST);
	DEBUG_PRINTF("List source\n");
	
	printf("%s\n", program_ptr );
	
	printf("\n");
	DEBUG_PRINTF("End of source\n");
	accept(TOKENIZER_CR);
	
}

/*---------------------------------------------------------------------------*/

static void 
cls_statement(void)
{   
	
	accept(TOKENIZER_CLS);
	DEBUG_PRINTF("Clear Screen\n");
	
	clearscreen();
	RenderScreen();
	accept(TOKENIZER_CR);
	
}
/*---------------------------------------------------------------------------*/
static void
rem_statement(void)
{
	accept(TOKENIZER_REM);
	DEBUG_PRINTF("rem_statement\n");
	accept(TOKENIZER_CR);
}

/*---------------------------------------------------------------------------*/

static void
sleep_statement(void)
{
	int val;
	accept(TOKENIZER_SLEEP);
	
	val = expr();
	SDL_Delay(val);
	
	DEBUG_PRINTF("End of sleep:%d ms\n", val);
	accept(TOKENIZER_CR);
}

/*---------------------------------------------------------------------------*/

/* SDL Graphics Commands */

static void
drawline_statement(void)
{
	int valx1, valy1, valx2, valy2;
	accept(TOKENIZER_DRAWLINE);
	
	valx1 = expr();
	valy1 = expr();
	valx2 = expr();
	valy2 = expr();
	
	if(valx1<0)
	{
		valx1=0;
	}
	if(valy1<0)
	{
		valy1=0;
	}
	if(valx2<0)
	{
		valx2=0;
	}
	if(valy2<0)
	{
		valy2=0;
	}

	
	drawline(valx1, valy1, valx2, valy2);
	RenderScreen();
	DEBUG_PRINTF("Draw line:%d, %d, %d, %d\n", valx1, valy1, valx2, valy2);
	accept(TOKENIZER_CR);
}

static void
moveto_statement(void)
{
	int valx1, valy1;
	accept(TOKENIZER_MOVETO);
	
	valx1 = expr();
	tokenizer_next();
	if(tokenizer_token() == TOKENIZER_COMMA) {
		tokenizer_next();
	}
	else if(tokenizer_token() == TOKENIZER_VARIABLE || tokenizer_token() == TOKENIZER_NUMBER) 
	{
		valy1 = expr();
	}

  if(valx1<0)
	{
		valx1=0;
	}
	if(valy1<0)
	{
		valy1=0;
	}

	moveto(valx1, valy1);
		
	DEBUG_PRINTF("Move to:%d, %d\n", valx1, valy1);
	accept(TOKENIZER_CR);
}

static void
drawlineto_statement(void)
{
	int valx1, valy1;
	accept(TOKENIZER_DRAWLINETO);
	
	valx1 = expr();
	tokenizer_next();
	if(tokenizer_token() == TOKENIZER_COMMA) {
		tokenizer_next();
	}
	else if(tokenizer_token() == TOKENIZER_VARIABLE || tokenizer_token() == TOKENIZER_NUMBER) 
	{
		valy1 = expr();
	}
	
	if(valx1<0)
	{
		valx1= 0;
	}
	if(valy1<0)
	{
		valy1= 0;
	}
	
	drawlineto(valx1, valy1);
	RenderScreen();
	DEBUG_PRINTF("Draw lineto:%d, %d\n", valx1, valy1);
	
	accept(TOKENIZER_CR);
}

static void
drawpoint_statement(void)
{
	int valx1, valy1;
	accept(TOKENIZER_DRAWPOINT);
	
	valx1 = expr();
	tokenizer_next();
	if(tokenizer_token() == TOKENIZER_COMMA) {
		tokenizer_next();
	}
	else if(tokenizer_token() == TOKENIZER_VARIABLE || tokenizer_token() == TOKENIZER_NUMBER) 
	{
		valy1 = expr();
	}
	
	if(valx1<0)
	{
		valx1= 0;
	}
	if(valy1<0)
	{
		valy1= 0;
	}
	
	
	drawpoint(valx1, valy1);
	RenderScreen();
	DEBUG_PRINTF("Draw point:%d, %d\n", valx1, valy1);
	accept(TOKENIZER_CR);
}


static void
forecolor_statement(void)
{
	int r, g, b;
	accept(TOKENIZER_FORECOLOR);
	
	r = expr();
	
	tokenizer_next();
	if(tokenizer_token() == TOKENIZER_COMMA) {
		tokenizer_next();
	}
	
	if(tokenizer_token() == TOKENIZER_VARIABLE || tokenizer_token() == TOKENIZER_NUMBER) 
	{
		g = expr();
	}
	
	tokenizer_next();
	if(tokenizer_token() == TOKENIZER_VARIABLE || tokenizer_token() == TOKENIZER_NUMBER) 
	{
		b = expr();
	}
		
	if(r<0)
	{
		r=0;
	}
	if(g<0)
	{
		g=0;
	}
	if(b<0)
	{
		b=0;
	}	
		
	forecolor(r, g, b);
	
	DEBUG_PRINTF("forecolor:%d, %d, %d\n", r, g, b);
	accept(TOKENIZER_CR);
}


static void
backcolor_statement(void)
{
	int r, g, b;
	accept(TOKENIZER_BACKCOLOR);
	

	r = expr();
	
	tokenizer_next();
	if(tokenizer_token() == TOKENIZER_COMMA) {
		tokenizer_next();
	}
	
	if(tokenizer_token() == TOKENIZER_VARIABLE || tokenizer_token() == TOKENIZER_NUMBER) 
	{
		g = expr();
	}
	
	tokenizer_next();
	if(tokenizer_token() == TOKENIZER_VARIABLE || tokenizer_token() == TOKENIZER_NUMBER) 
	{
		b = expr();
	}
	
	if(r<0)
	{
		r=0;
	}
	if(g<0)
	{
		g=0;
	}
	if(b<0)
	{
		b=0;
	}	
	
	backcolor(r, g, b);
	
	DEBUG_PRINTF("backcolor:%d, %d, %d\n", r, g, b);
	accept(TOKENIZER_CR);
}


int screenwidth_get_statement(void)
{
	return GetScreenWidth();
}

int screenheight_get_statement(void)
{
	return GetScreenHeight(); 
}


int cos_get_statement(void)
{
	int t1, r1;
	float resultf;
	accept(TOKENIZER_COS);
	
	t1 = expr();
	resultf = cos(t1)*(float)127;
	r1 = (int)resultf;
	
	DEBUG_PRINTF("7-bit COSINE of %d is %d\n", t1, r1);
	accept(TOKENIZER_CR);
	
	return r1;
}


int sin_get_statement(void)
{
	int t1, r1;
	float resultf;
	accept(TOKENIZER_COS);
	
	t1 = expr();
	resultf = sin(t1)*(float)127;
	r1 = (int)resultf;
	
	DEBUG_PRINTF("7-bit SINE of %d is %d\n", t1, r1);
	accept(TOKENIZER_CR);
	
	return r1;
}



static void
circle_statement(void)
{
	int x1, y1, radius;
	accept(TOKENIZER_CIRCLE);
	
	
	x1 = expr();
	
	tokenizer_next();
	if(tokenizer_token() == TOKENIZER_COMMA) {
		tokenizer_next();
	}
	
	if(tokenizer_token() == TOKENIZER_VARIABLE || tokenizer_token() == TOKENIZER_NUMBER) 
	{
		y1 = expr();
	}
	
	tokenizer_next();
	if(tokenizer_token() == TOKENIZER_VARIABLE || tokenizer_token() == TOKENIZER_NUMBER) 
	{
	 radius = expr();
	}
	
	
	drawcircle(x1, y1, radius);
	RenderScreen();
	DEBUG_PRINTF("circle:%d, %d, radius:%d\n", x1, y1, radius);
	accept(TOKENIZER_CR);
}

static void
filled_circle_statement(void)
{
	int x1, y1, radius;
	accept(TOKENIZER_FCIRCLE);
	
	x1 = expr();
	
	tokenizer_next();
	if(tokenizer_token() == TOKENIZER_COMMA) {
		tokenizer_next();
	}
	
	if(tokenizer_token() == TOKENIZER_VARIABLE || tokenizer_token() == TOKENIZER_NUMBER) 
	{
		y1 = expr();
	}
	
	tokenizer_next();
	if(tokenizer_token() == TOKENIZER_VARIABLE || tokenizer_token() == TOKENIZER_NUMBER) 
	{
	 radius = expr();
	}
	
	
	drawfilledcircle(x1, y1, radius);
	RenderScreen();
	DEBUG_PRINTF("filled circle:%d, %d, radius:%d\n", x1, y1, radius);
	accept(TOKENIZER_CR);
}

static int
random_statement(void)
{
	int range, r1;
	accept(TOKENIZER_RANDOM);
	
	/* range = expr();*/
	/* r1 = rand() % range; */
	/*r1 = rand()%65535;*/
	
	DEBUG_PRINTF("Random Number: %d\n", r1);
	accept(TOKENIZER_CR);
	
	return rand() % 127;
	
}


static void
srand_statement(void)
{
	int seed;
	accept(TOKENIZER_SRAND);
	seed = expr();
	srand(seed);
	
	DEBUG_PRINTF("Randomized seed: %d\n", seed);
	accept(TOKENIZER_CR);
	
}


/* timer_statement can't return a number over 127!! */
static int
timer_statement(void)
{
	int r1;
	accept(TOKENIZER_TIMER);
	
	r1 = (long int)time(NULL) - (long int)start_time;
	
	DEBUG_PRINTF("Timer: %d\n", r1);
	accept(TOKENIZER_CR);
	
	return r1;
	
}


/* chr_statement Needs more work */
static int
chr_statement(void)
{
	int r1;
	accept(TOKENIZER_CHR);

	r1 = expr();
	DEBUG_PRINTF("ASCII Character: %c\n", r1);
	accept(TOKENIZER_CR);

	return r1;
	
}



/*---------------------------------------------------------------------------*/
static void
if_statement(void)
{
	int r;
	
	accept(TOKENIZER_IF);
	
	r = relation();
	DEBUG_PRINTF("if_statement: relation %d\n", r);
	accept(TOKENIZER_THEN);
	if(r) {
		statement();
	} else {
		do {
			tokenizer_next();
		} while(tokenizer_token() != TOKENIZER_ELSE &&
				tokenizer_token() != TOKENIZER_CR &&
				tokenizer_token() != TOKENIZER_ENDOFINPUT);
		if(tokenizer_token() == TOKENIZER_ELSE) {
			tokenizer_next();
			statement();
		} else if(tokenizer_token() == TOKENIZER_CR) {
			tokenizer_next();
		}
	}
}
/*---------------------------------------------------------------------------*/
static void
let_statement(void)
{
	int var;
	
	var = tokenizer_variable_num();
	accept(TOKENIZER_VARIABLE);
	accept(TOKENIZER_EQ);
	ubasic_set_variable(var, expr());
	DEBUG_PRINTF("let_statement: assign %d to %d\n", variables[var], var);
	accept(TOKENIZER_CR);
	
}
/*---------------------------------------------------------------------------*/
static void
gosub_statement(void)
{
	int linenum;
	accept(TOKENIZER_GOSUB);
	linenum = tokenizer_num();
	accept(TOKENIZER_NUMBER);
	accept(TOKENIZER_CR);
	if(gosub_stack_ptr < MAX_GOSUB_STACK_DEPTH) {
		gosub_stack[gosub_stack_ptr] = tokenizer_num();
		gosub_stack_ptr++;
		jump_linenum(linenum);
	} else {
		DEBUG_PRINTF("gosub_statement: gosub stack exhausted\n");
	}
}
/*---------------------------------------------------------------------------*/
static void
return_statement(void)
{
	accept(TOKENIZER_RETURN);
	if(gosub_stack_ptr > 0) {
		gosub_stack_ptr--;
		jump_linenum(gosub_stack[gosub_stack_ptr]);
	} else {
		DEBUG_PRINTF("return_statement: non-matching return\n");
	}
}
/*---------------------------------------------------------------------------*/
static void
next_statement(void)
{
	int var, value;
	
	accept(TOKENIZER_NEXT);
	var = tokenizer_variable_num();
	accept(TOKENIZER_VARIABLE);
	if(for_stack_ptr > 0 &&
	   var == for_stack[for_stack_ptr - 1].for_variable) {
		value = ubasic_get_variable(var) + for_stack[for_stack_ptr - 1].step;
		ubasic_set_variable(var, value);
		
		if(((for_stack[for_stack_ptr - 1].step > 0) && (value <= for_stack[for_stack_ptr - 1].to)) ||
		   ((for_stack[for_stack_ptr - 1].step < 0) && (value >= for_stack[for_stack_ptr - 1].to)))
			jump_linenum(for_stack[for_stack_ptr - 1].line_after_for); 
		else {
			for_stack_ptr--;
			accept(TOKENIZER_CR);
		}
	} else {
		DEBUG_PRINTF("next_statement: non-matching next (expected %d, found %d)\n", for_stack[for_stack_ptr - 1].for_variable, var);
		ended = 1;
	}
	
}
/*---------------------------------------------------------------------------*/
static void
for_statement(void)
{
	int for_variable, to, step;
	
	accept(TOKENIZER_FOR);
	for_variable = tokenizer_variable_num();
	accept(TOKENIZER_VARIABLE);
	accept(TOKENIZER_EQ);
	ubasic_set_variable(for_variable, expr());
	accept(TOKENIZER_TO);
	to = expr();                     
	step = 1;
	if (tokenizer_token() != TOKENIZER_CR) {
		accept(TOKENIZER_STEP);
		step = expr();         
	}
	accept(TOKENIZER_CR);
	
	if(for_stack_ptr < MAX_FOR_STACK_DEPTH) {
		for_stack[for_stack_ptr].line_after_for = tokenizer_num();
		for_stack[for_stack_ptr].for_variable = for_variable;
		for_stack[for_stack_ptr].to = to;
		for_stack[for_stack_ptr].step = step;
		DEBUG_PRINTF("for_statement: new for, var %d to %d\n",
					 for_stack[for_stack_ptr].for_variable,
					 for_stack[for_stack_ptr].to);
		
		for_stack_ptr++;
	} else {
		DEBUG_PRINTF("for_statement: for stack depth exceeded\n");
		ended = 1;
	}
}
/*---------------------------------------------------------------------------*/
static void
end_statement(void)
{
	accept(TOKENIZER_END);
	ended = 1;
}
/*---------------------------------------------------------------------------*/
static void
statement(void)
{
	int token;
	
	token = tokenizer_token();
	
	switch(token) {
		case TOKENIZER_PRINT:
			print_statement();
			break;
		case TOKENIZER_LIST:
			list_statement();
			break;
		case TOKENIZER_CLS:
			cls_statement();
			break;
		case TOKENIZER_SLEEP:
			sleep_statement();
			break;			
		case TOKENIZER_COS:
			get_int_var_statement(token, cos_get_statement);
			break;
		case TOKENIZER_SIN:
			get_int_var_statement(token, sin_get_statement);
			break;
		case TOKENIZER_CHR:
			chr_statement();
			break;
		case TOKENIZER_MOVETO:
			moveto_statement();
			break;	
		case TOKENIZER_DRAWLINETO:
			drawlineto_statement();
			break;
		case TOKENIZER_DRAWLINE:
			drawline_statement();
			break;
		case TOKENIZER_DRAWPOINT:
			drawpoint_statement();
			break;
		case TOKENIZER_FORECOLOR:
			forecolor_statement();
			break;
		case TOKENIZER_BACKCOLOR:
			backcolor_statement();
			break;
		case TOKENIZER_SCREENWIDTH:
			get_int_var_statement(token, screenwidth_get_statement);
			break;
		case TOKENIZER_SCREENHEIGHT:
			get_int_var_statement(token, screenheight_get_statement);
			break;				
		case TOKENIZER_CIRCLE:
			circle_statement();
			break;
		case TOKENIZER_FCIRCLE:
			filled_circle_statement();
			break;
		case TOKENIZER_RANDOM:
			random_statement();
			break;
		case TOKENIZER_SRAND:
			srand_statement();
			break;	
		case TOKENIZER_TIMER:
			timer_statement();
			break;
		case TOKENIZER_REM:
			rem_statement();
			break;
		case TOKENIZER_IF:
			if_statement();
			break;
		case TOKENIZER_GOTO:
			goto_statement();
			break;
		case TOKENIZER_GOSUB:
			gosub_statement();
			break;
		case TOKENIZER_RETURN:
			return_statement();
			break;
		case TOKENIZER_FOR:
			for_statement();
			break;
		case TOKENIZER_NEXT:
			next_statement();
			break;
		case TOKENIZER_END:
			end_statement();
			break;
		case TOKENIZER_LET:
			accept(TOKENIZER_LET);
			/* Fall through. */
		case TOKENIZER_VARIABLE:
			let_statement();
			break;
		default:
			DEBUG_PRINTF("ubasic.c: statement(): not implemented %d\n", token);
			exit(1);
	}
}
/*---------------------------------------------------------------------------*/
static void
line_statement(void)
{
	int r=0;
	
	DEBUG_PRINTF("----------- Line number %d ---------\n", tokenizer_num());
	/*    current_linenum = tokenizer_num();*/
	accept(TOKENIZER_NUMBER);
	
	statement();
	return;
}

/*---------------------------------------------------------------------------*/
void
ubasic_run(void)
{
	if(tokenizer_finished()) {
		DEBUG_PRINTF("uBASIC program finished\n");
		return;
	}
	
	GetInput();   /* Read the inputs */
	line_statement();
}
/*---------------------------------------------------------------------------*/
int
ubasic_finished(void)
{
	return ended || tokenizer_finished();
}
/*---------------------------------------------------------------------------*/
void
ubasic_set_variable(int varnum, int value)
{
	if(varnum > 0 && varnum <= MAX_VARNUM) {
		variables[varnum] = value;
	}
}
/*---------------------------------------------------------------------------*/
int
ubasic_get_variable(int varnum)
{
	if(varnum > 0 && varnum <= MAX_VARNUM) {
		return variables[varnum];
	}
	return 0;
}
/*---------------------------------------------------------------------------*/

static void get_int_var_statement(int token, int (*func)(void))
{
    int var;
    accept(token);
    var = tokenizer_variable_num();
    accept(TOKENIZER_VARIABLE);
    ubasic_set_variable(var, func());
    accept(TOKENIZER_CR);
}

