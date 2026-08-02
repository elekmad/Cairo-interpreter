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

#ifndef NOSDL
SDL_Renderer *renderer;
SDL_Texture *texture;
unsigned char *pixels;
SDL_Window *window;
int x=0, xspeed=0, y=0, yspeed=0;
SDL_Event event;
#endif
int width = 640, height = 480;

#ifndef NOSDL
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

#endif


int main( int argc, char *argv[ ] )
{

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
    	int w, h;

		Op_fix_operandes(root, (OpContext*)&Ctx);

		Op_prerun(root, (OpContext*)&Ctx);


		w = OpCanvaContext_get_width(&Ctx);
		h = OpCanvaContext_get_height(&Ctx);
		width = w != 0 ? w : width;
		height = h != 0 ? h : height;

		CanvaCtxOutputMode output_mode = OpCanvaContext_get_output_mode(&Ctx);

#ifndef NOSDL
		if(output_mode == SDL)
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



			CanvaCtx_set_line_width (&Canva, 10.0);
			CanvaCtx_set_color(&Canva, 0, 0, 0, 255);

			OpCanvaContext_set_Canva(&Ctx, &Canva);

			Op_launch(root, (OpContext*)&Ctx);
			// Main loop


			while (one_iter() == 0)
			{
				// Delay to keep frame rate constant (using SDL)
				SDL_Delay(25);
			}

			CanvaCtx_terminate(&Canva);

			SDL_Quit();
		}

#endif

		if(output_mode == PNG)
		{
			CanvaCtx_init_for_png(&Canva, width, height, OpCanvaContext_get_output_name(&Ctx));

			CanvaCtx_set_line_width (&Canva, 10.0);
			CanvaCtx_set_color(&Canva, 0, 0, 0, 255);

			OpCanvaContext_set_Canva(&Ctx, &Canva);

			Op_launch(root, (OpContext*)&Ctx);

			CanvaCtx_write_to_png(&Canva);

			CanvaCtx_terminate(&Canva);
		}

		if(output_mode == SVG)
		{
			CanvaCtx_init_for_svg(&Canva, width, height, OpCanvaContext_get_output_name(&Ctx));

			CanvaCtx_set_line_width (&Canva, 10.0);
			CanvaCtx_set_color(&Canva, 0, 0, 0, 255);

			OpCanvaContext_set_Canva(&Ctx, &Canva);

			Op_launch(root, (OpContext*)&Ctx);

			CanvaCtx_finish(&Canva);

			CanvaCtx_terminate(&Canva);
		}
	}

    OpCanvaContext_terminate(&Ctx);
	Op_free(root);

    return EXIT_SUCCESS;
}
