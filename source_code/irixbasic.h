/* Let's add our own definition of itoa if this is compiled on
 * either IRIX / LINUX / Mac OS X  
*/

#ifndef _WIN32
void itoa(int n, char s[], int c);
void reverse(char s[]);
#endif

#ifdef _WIN32
#include <windows.h>
#endif

void GetArguments(int argc, char *argv[]);
void InitGFX();
void PrintVersion();
void LoadScript(void);
void RenderScreen();
void DisplayTitle();
void GetInput();
void DisplayReady();
int  GetTextYPos(int row);
void PrintTextLine(char textstring[]);
void PrintCenteredTextLine(char textstring[]);
void moveto(int x1, int y1);
void drawlineto(int x1, int y1);
void drawline( int x1, int y1, int x2, int y2);
void drawpoint( int x1, int y1);
void forecolor(int red, int green, int blue);
void backcolor(int red, int green, int blue);
void clearscreen(void);
void drawcircle(int x1, int y1, int radius);
void drawfilledcircle(int x1, int y1, int radius);
int GetScreenWidth(void);
int GetScreenHeight(void);
int GetMouseButton(void);
int GetMouseX(void);
int GetMouseY(void);

typedef struct {
	int r;
	int g;
	int b;
} rgb_colors;


/* drawing colors */
rgb_colors foreground_color;
rgb_colors background_color;

/* drawing location */
int previous_x = 0;
int previous_y = 0;

/* The screen surface */
SDL_Surface *screen = NULL;
SDL_Rect screen_rect;
SDL_Rect display_area_rect;

Uint32 screen_color;
Uint32 frame_color;
Uint32 debug_color;


TTF_Font *fntc64; /* C64 Font */
int text_row = 0; /* current text row */

