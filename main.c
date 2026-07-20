/*
 * main.c
 *
 *  Created on: 6 juil. 2026
 *      Author: damien
 */

#include <math.h>


#include <stdlib.h>
#include <stdio.h>
#include <CanvaCtx.h>
#include <OpCanva.h>
#include <parser.tab.h>

#define SPEED_RATE 1



CanvaCtx Canva;

SDL_Renderer *renderer;
SDL_Texture *texture;
unsigned char *pixels;
SDL_Window *window;
int x=0, xspeed=0, y=0, yspeed=0;
SDL_Event event;
int width = 640, height = 480;

int one_iter(void)
{
    if(x + xspeed < 640 && x + xspeed >= 0)
        x += xspeed;
    if(y + yspeed < 480 && y + yspeed >= 0)
        y += yspeed;

    // Check for messages
    if (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                SDL_Quit();
    	    return -1;
                break;
    	case SDL_KEYUP:
    	case SDL_KEYDOWN:
    		printf("keydown %d\n", event.key.keysym.sym);
    		switch(event.key.keysym.sym)
    		{
                        case SDLK_DOWN:
                            yspeed = (event.type == SDL_KEYDOWN ? SPEED_RATE : 0);
                            break;
                        case SDLK_UP:
                            yspeed = (event.type == SDL_KEYDOWN ? -SPEED_RATE : 0);
                            break;
                        case SDLK_RIGHT:
                            xspeed = (event.type == SDL_KEYDOWN ? SPEED_RATE : 0);
                            break;
                        case SDLK_LEFT:
                            xspeed = (event.type == SDL_KEYDOWN ? -SPEED_RATE : 0);
                            break;
                        case SDLK_ESCAPE:
                            SDL_Quit();
                            return -1;
                            break;
    		}
    	        printf("xspeed %d yspeed %d \n", xspeed, yspeed);
    	        break;
        }
    }
    //Update the display
    CanvaCtx_update_display(&Canva);
    // affichage
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    return 0;

}



int main( int argc, char *argv[ ] )
{
    if( SDL_Init( SDL_INIT_VIDEO ) == -1 )
    {
        printf( "Can't init SDL:  %s\n", SDL_GetError( ) );
        return EXIT_FAILURE;
    }

    atexit( SDL_Quit );
    window = SDL_CreateWindow("Ma fenêtre de jeu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0 );

    renderer =
        SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED);

    SDL_Surface *sdl_surface = SDL_GetWindowSurface(window);

    if( sdl_surface == NULL )
    {
        printf( "Can't set video mode: %s\n", SDL_GetError( ) );
        return EXIT_FAILURE;
    }
    texture = SDL_CreateTexture(renderer,
    	    SDL_PIXELFORMAT_ARGB8888,
    	    SDL_TEXTUREACCESS_STREAMING,
    	    width,
    	    height
			);
    CanvaCtx_init(&Canva,
    		sdl_surface->w,
	        sdl_surface->h,
	        sdl_surface->pitch);
    CanvaCtx_set_texture(&Canva, texture);
    // Main loop

/*
    Op *op = OpBloc_new();
    Op *op_arc, *c;


    /////////////////////// Circle
    op_arc = OpCircle_new();

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 160);
    OpCircle_set_x((OpCircle*)op_arc, c);

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 160);
    OpCircle_set_y((OpCircle*)op_arc, c);

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 30);
    OpCircle_set_r((OpCircle*)op_arc, c);

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 0);
    OpCircle_set_a1((OpCircle*)op_arc, c);

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 2*M_PI);
    OpCircle_set_a2((OpCircle*)op_arc, c);

    OpBloc_append_Op((OpBloc*)op, op_arc);
    //////////////////////////////////////////// Fin Circle
    arc(160, 160, 30, 0, 6.28)

    op_arc = OpStroke_new();
    OpBloc_append_Op((OpBloc*)op, op_arc);

    /////////////////////////////// Circle
    op_arc = OpCircle_new();

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 500);
    OpCircle_set_x((OpCircle*)op_arc, c);

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 500);
    OpCircle_set_y((OpCircle*)op_arc, c);

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 30);
    OpCircle_set_r((OpCircle*)op_arc, c);

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 0);
    OpCircle_set_a1((OpCircle*)op_arc, c);

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 2*M_PI);
    OpCircle_set_a2((OpCircle*)op_arc, c);

    OpBloc_append_Op((OpBloc*)op, op_arc);
    ///////////////////////////////// Fin Circle

    op_arc = OpColor_new();

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 255);
    OpColor_set_red((OpColor*)op_arc, c);

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 102);
    OpColor_set_geen((OpColor*)op_arc, c);

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 70);
    OpColor_set_blue((OpColor*)op_arc, c);

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 255);
    OpColor_set_alpha((OpColor*)op_arc, c);
    OpBloc_append_Op((OpBloc*)op, op_arc);

///////////////////////////   Rectangle
    op_arc = OpRectangle_new();

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 10);
    OpRectangle_set_x((OpRectangle*)op_arc, c);

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 10);
    OpRectangle_set_y((OpRectangle*)op_arc, c);

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 400);
    OpRectangle_set_w((OpRectangle*)op_arc, c);

    c = OpGetValue_new();
    OpGetValue_set_value((OpGetValue*)c, 400);
    OpRectangle_set_h((OpRectangle*)op_arc, c);

    OpBloc_append_Op((OpBloc*)op, op_arc);
    //////////////////////Fin Rectangle
    */


    extern FILE *yyin;
    yyin = fopen(argv[1], "r");
	OpCanvaContext Ctx;
	OpCanvaContext_init(&Ctx);
    Op *root = NULL;
    extern int yydebug, yy_flex_debug;
    yydebug = 1;
    yy_flex_debug = 1;

    if(yyparse(&root, (OpContext*)&Ctx) == 0 && root != NULL)
    {
		Op *op_arc = OpStroke_new();
		OpBloc_append_Op((OpBloc*)root, op_arc);


		CanvaCtx_set_color(&Canva, 255, 255, 255, 255);
		CanvaCtx_draw_rectangle(&Canva, 0, 0, Canva.width, Canva.height);
		CanvaCtx_fill(&Canva);

		CanvaCtx_set_line_width (&Canva, 10.0);
		CanvaCtx_set_color(&Canva, 0, 0, 0, 255);

		OpCanvaContext_set_Canva(&Ctx, &Canva);

		Op_launch(root, (OpContext*)&Ctx);
		Op_free(root);
		while (one_iter() == 0)
		{
			// Delay to keep frame rate constant (using SDL)
			SDL_Delay(25);
		}
    }
    OpCanvaContext_terminate(&Ctx);

    SDL_Quit();

    CanvaCtx_terminate(&Canva);

    return EXIT_SUCCESS;
}
