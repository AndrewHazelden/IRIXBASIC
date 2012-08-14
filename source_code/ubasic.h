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
#ifndef __UBASIC_H__
#define __UBASIC_H__

void ubasic_init(char *program);
void ubasic_run(void);
int ubasic_finished(void);

int ubasic_get_variable(int varnum);
void ubasic_set_variable(int varum, int value);
static void get_int_var_statement(int token, int (*func)(void));
#endif /* __UBASIC_H__ */


/* IRIX Basic externs */

extern void PrintTextLine(char textstring[]); /* Onscreen text printing function from irixbasic.c */
extern void moveto(int x1, int y1);
extern void drawlineto(int x1, int y1);
extern void drawline(int x1, int y1, int x2, int y2);
extern void drawpoint(int x1, int y1);
extern void forecolor(int red, int green, int blue);
extern void backcolor(int red, int green, int blue);
extern void clearscreen(void);
extern void drawcircle(int x1, int y1, int radius);
extern void drawfilledcircle(int x1, int y1, int radius);
extern int GetScreenWidth(void);
extern int GetScreenHeight(void);
extern int GetMouseButton(void);
extern void RenderScreen(void);

extern int isbutton_down;
extern int text_row;
extern long int start_time;

extern void itoa(int n, char s[], int c);
extern void reverse(char s[]);
/* End IRIX Basic externs */
