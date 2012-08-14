/* IRIXbasic Version 1.0
 * Created July 2012
 * 
 * Created by Andrew Hazelden
 * Email andrew@andrewhazelden.com
 *
 * Based upon Adam Dunkels uBasic software
 *
 * Required Graphics Libraries: 
 * SDL "Simple DirectMedia Layer"
 * http://www.libsdl.org/
 *  
 * SDL_TTF
 * http://www.libsdl.org/projects/SDL_ttf/
 *  
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>  /* For time(), used with srand(). */

/* #include <SDL/SDL.h>*/       /* Adds graphics support */
#include <SDL.h>
#include <SDL_ttf.h>            /* Adds font support */
#include <SDL_rotozoom.h>       /* SDL_gfx Rotozoom  */
#include <SDL_gfxPrimitives.h>	/* SDL_gfx Primitives */
#include <SDL_framerate.h>	/* SDL_gfx Framerate Manager */

#include "ubasic.h"
#include "irixbasic.h"

#define MAX_SCRIPT_LENGTH 100000

#define DEFAULT_BAS_SCRIPT "scripts/default.bas"


/* script path */
int script_found = 0; /* could the script be opened? */
char script_path[2048];

/* irixbasic Console size */ 
int screen_width = 640;
int screen_height = 480;
int enable_fullscreen = 0;

/* Console text size in points */
int fontsize = 16;  /* default font size is 16 points for 640x480 */

int character_with = 0;
int character_height = 0;
int total_text_rows = 0;
int total_text_cols = 0;


/* User input */
int isbutton_down = 0; /* read if the mouse button was pressed */
int mousex = 0; /* read the mousex position */
int mousey = 0; /* read the mousey position */



/* * Sample Basic Program 1: */
char program[MAX_SCRIPT_LENGTH];

/* SDL done flag */
int done;

/* Program uptime counter */
long int start_time = 0;

SDL_Surface *program_icon;


int main (int argc, char *argv[])
{
	int inc=0;
	
	start_time = time(NULL);
	
	GetArguments(argc, argv); /* Get the script filename */
	PrintVersion();  /* Print info */
	InitGFX();       /* Setup SDL  */	
	DisplayTitle();  /* Show the irixbasic titles */
	DisplayReady();  /* Display the ready text */
	
	LoadScript(); 
	
	SDL_Flip(screen);
	ubasic_init(program); /* Setup uBasic  */	
	
	
	/* Start the program when the user is ready */
	PrintTextLine("Click to Run.");
	SDL_Delay(200);
	do
	{
		GetInput();   /* Read the inputs */
	}while (!GetMouseButton());
	
	
	do
	{
		GetInput();   /* Read the inputs */
		ubasic_run(); /* Process a line of code*/
	}while (!ubasic_finished() && !done);
	
	PrintTextLine("");
	PrintTextLine("Click or Press ESC to quit.");
	SDL_Flip(screen);
	
	while(!done && !GetMouseButton()){
		GetInput();
	}
	
	TTF_CloseFont( fntc64 );
	
	return 0;
}


void forecolor(int red, int green, int blue)
{
	foreground_color.r = red;
	foreground_color.g = green;
	foreground_color.b = blue;
	
	frame_color = SDL_MapRGB(screen->format, foreground_color.r, foreground_color.g, foreground_color.b);
	
}

int GetScreenWidth(void)
{
	return screen_width;
}

int GetScreenHeight(void)
{
	return screen_height;
}


int GetMouseX(void)
{
	return mousex;
}

int GetMouseY(void)
{
	return mousey;
}

int GetMouseButton(void)
{
	if(isbutton_down)
	{
		printf("you clicked\n");
	}
	return isbutton_down;
}



void backcolor(int red, int green, int blue)
{
	background_color.r = red;
	background_color.g = green;
	background_color.b = blue;
	
	screen_color = SDL_MapRGB(screen->format, background_color.r, background_color.g, background_color.b);
	
}



void drawline(int x1, int y1, int x2, int y2)
{
	lineRGBA(screen, x1, y1, x2, y2, foreground_color.r, foreground_color.g, foreground_color.b, 255);
	
	/* save last draw position */
	previous_x = x2;
	previous_y = y2;
	/* SDL_Flip(screen); */
}

void drawlineto(int x1, int y1)
{
	lineRGBA(screen, previous_x, previous_y, x1, y1, foreground_color.r, foreground_color.g, foreground_color.b, 255);
	
	/* save last draw position */
	previous_x = x1;
	previous_y = y1;
	/* SDL_Flip(screen); */
}

void moveto(int x1, int y1)
{
	/* move to a new draw position */
	previous_x = x1;
	previous_y = y1;
	/* SDL_Flip(screen); */
}


void drawpoint(int x1, int y1)
{
	pixelRGBA(screen, x1, y1, foreground_color.r, foreground_color.g, foreground_color.b , 255);
	
	/* save last draw position */
	previous_x = x1;
	previous_y = y1;
	/* SDL_Flip(screen); */
}


void clearscreen(void){
	text_row = 0;
	
	/* Fill the screen with the background color */
	SDL_FillRect(screen, NULL, screen_color);
	
	/* Make sure everything is displayed on screen */
	/* SDL_Flip(screen); */
	
}


void drawcircle(int x1, int y1, int radius){
	circleRGBA(screen, x1, y1, radius, foreground_color.r, foreground_color.g, foreground_color.b , 255);
	
	/* SDL_Flip(screen); */
}

void drawfilledcircle(int x1, int y1, int radius){
	filledCircleRGBA(screen, x1, y1, radius, foreground_color.r, foreground_color.g, foreground_color.b , 255);
	
	/* SDL_Flip(screen); */
}


/* read the argv values */
void GetArguments(int argc, char *argv[])
{
	int scanner = 0;
	
	char option1[128];
	char option2[128];
	int tempwidth, tempheight;
	
	script_found = 0;
	
	printf("Usage: irixbasic <script.bas> [options]\n");
	printf("Options:\n");
	printf("  --window <width> <height> [-fullscreen]\n");
	printf("    Set the size of the main window\n");
	
	/*printf("Argv contents:\n");*/
	for(scanner=0;scanner<=argc;scanner++)
	{
		printf("[%d]%s\n",scanner, argv[scanner]);
	} /*end for scanner*/
	
	if(argc >=2)
	{
		/* Check for the script file name */
		if(argv[1] != NULL)
		{
			strcpy(script_path, argv[1]); 
			/*check if the .bas file is in the argv path*/
			if( strstr(script_path, ".bas") )
			{
				script_found = 1;
				
				/* check for the screen size argument */	
				if(argv[2] != NULL)
				{
					strcpy(option1, argv[2]); 
					/*check if the --window option is in the argv*/
					if( strstr(option1, "--window") )
					{
						/* you found the window option - this requires width and height */
						if((argv[3] != NULL) && (argv[4] != NULL))
						{ 
							tempwidth = atoi(argv[3]);
							tempheight = atoi(argv[4]);
							printf("Got a --window size of width=%d height=%d\n", tempwidth, tempheight);
							if (tempwidth >= 640) 
							{
								screen_width = tempwidth;
							}
							if(tempheight >= 400)
							{
								screen_height = tempheight;
							}
							
							/* Check for fullscreen option */
							if(argv[5] != NULL)
							{
								strcpy(option2, argv[5]); 
								/*check if the --window option is in the argv*/
								if( strstr(option2, "-fullscreen") )
								{
									enable_fullscreen=1;
									printf("Got a -fullscreen\n");
								}
								else
								{
									enable_fullscreen=0;
								}
								
							}
						}					
					} /*end argv null check*/
				}		
			}
		}
	}
}
/* Load a basic script */
void LoadScript(void){
	FILE *fp;
	char line[80];
	int i;
	char loadingString[255];
	char one_line[80];
	
	
	/* did GetArguments find a script? */		
	if(script_found)
	{
		printf("Located .bas script path: %s\n\n", script_path); 
	}
	else
	{
		strcpy(script_path, DEFAULT_BAS_SCRIPT);
		printf("No .bas script provided. Loading the default script.\n\n", DEFAULT_BAS_SCRIPT);
	}
	
	
	
	
	fp = fopen(script_path, "r");
	
	if(fp != NULL)
	{
		
		/* Display file loading info onscreen */
		sprintf(one_line, "Load \"%s\"", script_path);
		PrintTextLine(one_line);
		
		/* Clean out the old program */
		for(i=0;i<MAX_SCRIPT_LENGTH-1;i++)
		{
			program[i] = 0;
		}
		
		/* Read in the new program */
		while(fgets(line, sizeof(line), fp)){
			strcat(program, line); 
			/* printf("%s\n", line); */
			/* sprintf(one_line, "%s", line); */
			/* PrintTextLine(one_line); */ /* Display program line onscreen */
		}		
		printf("--------------------------------------------\n");
		printf("Loading Script: \"%s\"\n", script_path); 
		printf("--------------------------------------------\n");
		printf("%s",program);
		printf("--------------------------------------------\n");
	}
	else
	{
		printf("Error: Sorry, I can't open the script.\n");
		PrintTextLine("Sorry, I can't open the script.");
	}
	
	fclose(fp);
}



/* Game version info */
void PrintVersion(){
	printf("\nWelcome to the IRIXbasic Interpreter V1.0\n");
	printf("--------------------------------------------\n");
	printf("Created by Andrew Hazelden\n");
	
#ifdef _WIN32
	printf("Running on Windows\n");	
#elseif __sgi
	printf("Running on IRIX\n");	
#elseif __APPLE__
	printf("Running on Mac OS X\n");
#elseif __linux__
	printf("Running on Linux\n");
#endif
	
	printf("\n");
}




/* Setup SDL */
void InitGFX(){
	static int flags=0;

	/* Define the program icon */
#ifdef _WIN32
	program_icon = SDL_LoadBMP("irixbasic-icon-32px.bmp");
#else
	program_icon = SDL_LoadBMP("irixbasic-icon-128px.bmp");
#endif
	
	char *msg;
	
	/* Initialize SDL */
	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		sprintf (msg, "Couldn't initialize SDL: %s\n", SDL_GetError ());
		/*MessageBox (0, msg, "Error", program_icon);*/
		free (msg);
		exit (1);
	}
	atexit (SDL_Quit);
	
	/* Set video mode */
	screen = SDL_SetVideoMode (screen_width, screen_height, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
	
	if (screen == NULL)
	{
		sprintf (msg, "Couldn't set %dx%dx16 video mode: %s\n", screen_width, screen_height, SDL_GetError ());
		printf("%s",msg);
		free (msg);
		exit (2);
	}
	
	flags = screen->flags;
	
	/* Check if the enable fullscreen argument is turned on */
	if(enable_fullscreen){
		screen = SDL_SetVideoMode(0, 0, 0, screen->flags ^ SDL_FULLSCREEN);
		screen_width = screen->w;
		screen_height = screen->h;
		
		if(screen == NULL) 
		{
			screen = SDL_SetVideoMode(0, 0, 0, flags); /* If toggle FullScreen failed, then switch back */
		}
		if(screen == NULL)
		{
			exit(1); /* If you can't switch back for some reason, then fail */
		}
	}
	
	
	
	SDL_WM_SetIcon(program_icon, NULL);
	SDL_WM_SetCaption ("IRIXBASIC", "IRIXBASIC");
	
	/* Create background screen colors*/
	screen_color = SDL_MapRGB(screen->format, 66, 66, 231);
	frame_color = SDL_MapRGB(screen->format, 165, 165, 255);
	debug_color = SDL_MapRGB(screen->format, 126, 126, 126);
	
	
	/* light blue */
	foreground_color.r = 165;
	foreground_color.g = 165;
	foreground_color.b = 255;
	
	/* dark blue */
	background_color.r = 66;
	background_color.g = 66;
	background_color.b = 231;
	
	
	/* Setup a rect for the full screen */
	screen_rect.h = screen->h;
	screen_rect.w = screen->w; 
	
	/* frame border width 36px / height 45px */
	display_area_rect.x = 36;
	display_area_rect.y = 45;
	display_area_rect.w = screen->w-(36*2);
	display_area_rect.h = screen->h-(45*2);
	
	
	/* Set up the SDL_TTF */
	TTF_Init();
	atexit(TTF_Quit);
	/* TTF_Init() is like SDL_Init(), but with no parameters.  Basically, it initializes
	 SDL_TTF.  There's really not much to it.  Remember, when the program ends, we
	 have to call TTF_Quit().  atexit(TTF_Quit) ensures that when we call exit(), the
	 program calls TTF_Quit() for us. */
	
	fntc64 = TTF_OpenFont( "./C64_User_Mono_v1.0-STYLE.ttf", fontsize );
	
	/* Get the size of a character in pixels */
	if(TTF_SizeText(fntc64,"B",&character_with,&character_height)) {
		printf("SDL_TTF error: %s", TTF_GetError());
	} else {
		total_text_rows = (display_area_rect.h/character_height)-1;
		total_text_cols = (display_area_rect.w/character_with)-1;
		printf("Current Screen Size: width=%d height=%d\n", screen_width, screen_height);
		printf("Text Settings: %d=points rows=%d columns=%d \n", fontsize, total_text_rows, total_text_cols);
		printf("\n");
	}
	
	/*Wait a moment for the screen to switch*/
	if(enable_fullscreen)
	{
		SDL_Flip(screen);
		SDL_Delay(1500);
	}
	
	/*-------------------------------
	 * Fill the screen
	 * -------------------------------
	 */
	
	/* Fill the screen with the light blue frame */
	SDL_FillRect(screen, NULL, frame_color);
	
	/*Fill the center of the screen with dark blue */ 			
	SDL_FillRect(screen, &display_area_rect, screen_color);
	
	/* Make sure everything is displayed on screen */
	SDL_Flip(screen);
	
	
	isbutton_down = 0;
	
}


int	GetTextYPos(int row){
	int starting_height = (47+8);
	return starting_height+(row*(fontsize+2));
}



void PrintTextLine(char textstring[]){
	SDL_Color text_color;
	text_color.r = foreground_color.r;
	text_color.g = foreground_color.g;
	text_color.b = foreground_color.b;
	
	
	SDL_Surface *textSurface = TTF_RenderText_Blended(fntc64, textstring, text_color);
	
	SDL_Rect textDestRect;
	
	if (textSurface) {
		/* now center it horizontally and position it just below the logo */
		textDestRect.x = 38;
		textDestRect.y = GetTextYPos(text_row);
		textDestRect.w = textSurface->w;
		textDestRect.h = textSurface->h;
		SDL_BlitSurface(textSurface, NULL, screen, &textDestRect);
		SDL_FreeSurface(textSurface);
	}
	SDL_Flip(screen);
	
	text_row++; /* increment the text display row */

	/* Check if the text has hit the bottom of the screen */
	if(text_row> total_text_rows)
	{
		clearscreen();  /* reset the row to the 0 and clear the screen */
	}
	
}

void PrintCenteredTextLine(char textstring[]){
	SDL_Color text_color;
	text_color.r = foreground_color.r;
	text_color.g = foreground_color.g;
	text_color.b = foreground_color.b;
	
	SDL_Surface *textSurface = TTF_RenderText_Blended(fntc64, textstring, text_color);
	
	SDL_Rect textDestRect;
	
	if (textSurface) {
		/* now center it horizontally and position it just below the logo */
		textDestRect.x = (screen->w - textSurface->w) / 2;
		textDestRect.y = GetTextYPos(text_row);
		textDestRect.w = textSurface->w;
		textDestRect.h = textSurface->h;
		SDL_BlitSurface(textSurface, NULL, screen, &textDestRect);
		SDL_FreeSurface(textSurface);
	}
	SDL_Flip(screen);
	
	text_row++; /* increment the text display row */

	/* Check if the text has hit the bottom of the screen */
	if(text_row> total_text_rows)
	{
		clearscreen();  /* reset the row to the 0 and clear the screen */
	}

	
}


void DisplayTitle()
{
	PrintCenteredTextLine("   ****  IRIX BASIC V1 ****   ");
	PrintCenteredTextLine("Living in a Retro Wonderland");
	
	
}


void DisplayReady()
{
	PrintTextLine("Ready");
	/* Make sure everything is displayed on screen */
	SDL_Flip(screen);
}

void RenderScreen(){
	/* Make sure everything is displayed on screen */
	SDL_Flip(screen);
}



/* Check for user input */
void GetInput(){
	SDL_Event event;
	
	/*-------------------
	 *Check for events
	 *-------------------
	 */
	
	while (SDL_PollEvent (&event))
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
				switch( event.key.keysym.sym ){
					case SDLK_LEFT:
						break;
					case SDLK_RIGHT:
						break;
					case SDLK_UP:
						break;
					case SDLK_DOWN:
						break;
					default:
						break;
				}
				break;
			case SDL_KEYUP:
				switch( event.key.keysym.sym ){
					case SDLK_LEFT:
						break;
					case SDLK_RIGHT:
						break;
					case SDLK_UP:
						break;
					case SDLK_DOWN:
						break;                
					case SDLK_ESCAPE:
						done = 1;  /* Quit when a key is pressed */
						break;
				}
			case SDL_MOUSEBUTTONDOWN:
				isbutton_down=1;
				break;
			case SDL_MOUSEBUTTONUP:
				isbutton_down=0;
				break;			
			case SDL_QUIT:
				/* done = 1; */
				break;
			default:
				break;
		}
	} 
}





/* Let's add our own definition of itoa if this is compiled on
 * either IRIX / LINUX / Mac OS X */

#ifndef _WIN32
/* itoa:  convert n to characters in s */
void itoa(int n, char s[], int c)
{
	int i, sign;
	
	if ((sign = n) < 0)  /* record sign */
		n = -n;          /* make n positive */
	i = 0;
	do {       /* generate digits in reverse order */
		s[i++] = n % 10 + '0';   /* get next digit */
	} while ((n /= 10) > 0);     /* delete it */
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}


/* reverse:  reverse string s in place */
void reverse(char s[])
{
	int i, j;
	char c;
	
	for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}
#endif

