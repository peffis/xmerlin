/***************************************************************************
 * graphics.c
 *
 * Copyright (c) 2000 Stefan Hellkvist, Ericsson Radio Systems AB 
 * Email: stefan@hellkvist.org
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 *****************************************************************************/

#include "graphics.h"
#include "pattern.h"
#include <string.h>
#include <stdio.h>


unsigned long getColor( Display *dsp, unsigned short red, unsigned green, unsigned blue );


Display *myDisplay;
Window myWindow;
GC myGC;
int myScreen;
int window_x, window_y, window_width, window_height;

extern Pattern *current;
XFontStruct *myFont;


void
assignDefault()
{
    window_x = window_y = 0;
    window_width = DEFAULT_WINDOW_WIDTH;
    window_height = DEFAULT_WINDOW_HEIGHT;
}

int
parseArgs( int argc, char *argv[])
{
    char xsign, ysign;
    if ( argc != 3 || strcmp( "-geometry", argv[1] ) )
	return 1;

    if ( sscanf( argv[2], "%dx%d%c%d%c%d\n", &window_width, 
		 &window_height, 
		 &xsign, &window_x, &ysign, &window_y ) != 6 ) 
    	return 1;
    
    if ( xsign == '-' )
    {
	int screen_width = WidthOfScreen( 
	    ScreenOfDisplay( myDisplay, myScreen ) );
	window_x = screen_width - window_width - window_x;
    }

    if ( ysign == '-' )
    {
	int screen_height = HeightOfScreen( 
	    ScreenOfDisplay( myDisplay, myScreen ) );
	window_y = screen_height - window_height - window_y;
    }
    return 0;
}


void 
initGraphics( int argc, char *argv[] )
{
    XSetWindowAttributes myWindowAttributes;
    int myDepth; 
    XGCValues myGCValues;
    unsigned long myWindowMask; 

    myDisplay = XOpenDisplay( "" );
    if (myDisplay == NULL)
    {
	exit (0);
    }

    myScreen = DefaultScreen (myDisplay);
    myDepth = DefaultDepth (myDisplay, myScreen);

    if ( parseArgs( argc, argv ) )
	assignDefault();

    myWindowAttributes.border_pixel = 
	BlackPixel (myDisplay, myScreen);
    myWindowAttributes.background_pixel = 
	getColor( myDisplay, 252, 234, 212 );
    myWindowAttributes.override_redirect = 0;

    myWindowMask = CWBackPixel | CWBorderPixel | CWOverrideRedirect;
    
    myWindow = XCreateWindow (myDisplay, 
			      RootWindow (myDisplay, myScreen), 
			      window_x, 
			      window_y, 
			      window_width, window_height, 
			      BORDER_WIDTH, myDepth, InputOutput, 
			      CopyFromParent, myWindowMask, 
			      &myWindowAttributes);

    myGC = XCreateGC (myDisplay, myWindow, (unsigned long) 0, &myGCValues);
    if (myGC == 0)
    {
	XDestroyWindow(myDisplay, myScreen);
	exit (0);
    }

    myFont = XLoadQueryFont (myDisplay, "9x15");
    if (myFont == (XFontStruct *)NULL)
    {
        fprintf(stderr,"Cannot get font:9x15.\n");
        myFont = NULL;
    }
    else
	XSetFont (myDisplay, myGC, myFont->fid);

    XSetForeground (myDisplay, myGC, BlackPixel( myDisplay, myScreen ) );
    XMapWindow (myDisplay, myWindow);
    XLowerWindow( myDisplay, myWindow );
    paint();
    XFlush( myDisplay );
}

unsigned long
getColor( Display *dsp, unsigned short red, unsigned green, unsigned blue )
{
    XColor col;
    Colormap cmap = DefaultColormapOfScreen( ScreenOfDisplay( dsp, 0 ) );
    col.red = (red + 1)*256 - 1;
    col.green = (green + 1)*256 - 1;
    col.blue = (blue + 1)*256 - 1;
    if (!XAllocColor( dsp, cmap, &col ) )
        printf( "Can't alloc color\n" );
    return col.pixel;
}


void
drawText()
{
    XDrawString( myDisplay, myWindow, myGC, 2+1, 15+1,
		 "abc", strlen("abc"));
    XDrawString( myDisplay, myWindow, myGC, window_width/2 - 13 + 1, 15 + 1,
		 "ABC", strlen("ABC"));
    XDrawString( myDisplay, myWindow, myGC, window_width - 3*9 - 2 + 1, 15 + 1,
		 "123", strlen("123"));

    XSetForeground (myDisplay, myGC, getColor( myDisplay, 60, 120, 200 ) );

    XDrawString( myDisplay, myWindow, myGC, 2, 15,
		 "abc", strlen("abc"));
    XDrawString( myDisplay, myWindow, myGC, window_width/2 - 13, 15,
		 "ABC", strlen("ABC"));
    XDrawString( myDisplay, myWindow, myGC, window_width - 3*9 - 2, 15,
		 "123", strlen("123"));

    XSetForeground (myDisplay, myGC, BlackPixel( myDisplay, myScreen ) );
}

void 
paintPattern()
{
    int i;
    if ( current != NULL && current->nPoints > 1 )
    {
	for ( i = 0; i < current->nPoints - 1; i++ )
	    if ( !current->pv[i].isLast ) 
		XDrawLine( myDisplay, myWindow, myGC, 
			   current->pv[i].x, current->pv[i].y,
			   current->pv[i+1].x, current->pv[i+1].y );
    }
}

void
paint( void )
{
    XClearWindow( myDisplay, myWindow );
    XSetLineAttributes( myDisplay, myGC, 1, LineOnOffDash, 
			CapNotLast, JoinMiter );
    XDrawLine( myDisplay, myWindow, myGC, window_width / 2,
	       20, window_width / 2, window_height );
    XSetLineAttributes( myDisplay, myGC, 1, LineSolid, 
			CapNotLast, JoinMiter );
    paintPattern();
    if ( myFont != NULL )
	drawText();
}
