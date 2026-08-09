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
#include <lexer.h>
#include <unistd.h>

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
    		fprintf(stderr, "keydown %d\n", event.key.keysym.sym);
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
    	        fprintf(stderr, "xspeed %d yspeed %d \n", xspeed, yspeed);
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
	size_t readed;
	OpCanvaContext Ctx;
	OpCanvaContext_init(&Ctx);
    Op *root = NULL;
    /*extern int yydebug, yy_flex_debug;
    yydebug = 1;
    yy_flex_debug = 1;*/
#ifdef CGIMODE
    char *buffer;
    char *length_env = getenv("CONTENT_LENGTH");
    if(length_env == NULL)
    {
    	fprintf(stderr, "No CONTENT_LENGTH found into env\n");
    	return -1;
    }
    size_t length = strtoul(length_env, NULL, 10);
    fprintf(stderr, "about to read %zu from stdin\n", length);
    buffer = malloc(length);
    readed = read(0, buffer, length);
    if(readed != length)
    {
    	fprintf(stderr, "read %zu != %zu\n", readed, length);
    	return -1;
    }

    fprintf(stderr, "stdin read, creating buffer\n");
    YY_BUFFER_STATE state = yy_scan_bytes(buffer, length);
    fprintf(stderr, "now parse\n");
#else
    String buffer;
    String_init(&buffer);
    char buf[100];
    while((readed = read(0, buf, 100)) > 0)
    {
    	String_append_data(&buffer, readed, (const void*)buf);
    }
    YY_BUFFER_STATE state = yy_scan_bytes(String_get_data(&buffer), String_get_length(&buffer));
#endif
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
				fprintf(stderr,  "Can't init SDL:  %s\n", SDL_GetError( ) );
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
				fprintf(stderr,  "Can't set video mode: %s\n", SDL_GetError( ) );
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
			CanvaCtx_init_for_png(&Canva, width, height);

			CanvaCtx_set_line_width (&Canva, 10.0);
			CanvaCtx_set_color(&Canva, 0, 0, 0, 255);

			OpCanvaContext_set_Canva(&Ctx, &Canva);

			Op_launch(root, (OpContext*)&Ctx);

			CanvaCtx_write_to_png(&Canva);
#ifdef CGIMODE
			String out;
			String_init(&out);
			String_append_char_string(&out, "Content-Type: application/xml; charset=utf-8\r\n");
			String_append_char_string(&out, "\r\n");
			OpCanvaContext_export_messages_to_xml(&Ctx, &out, true);
			printf("%s", String_get_char_string(&out));
			String_finalize(&out);
#else
			String out;
			String_init(&out);
			OpCanvaContext_export_messages_to_xml(&Ctx, &out, false);
			fprintf(stderr, "%s", String_get_char_string(&out));
			String_finalize(&out);
			CanvaCtx_write_to_fd(&Canva, 1);
#endif

			CanvaCtx_terminate(&Canva);
		}

		if(output_mode == SVG)
		{
			CanvaCtx_init_for_svg(&Canva, width, height);

			CanvaCtx_set_line_width (&Canva, 10.0);
			CanvaCtx_set_color(&Canva, 0, 0, 0, 255);

			OpCanvaContext_set_Canva(&Ctx, &Canva);

			Op_launch(root, (OpContext*)&Ctx);

			CanvaCtx_finish(&Canva);
#ifdef CGIMODE
			String out;
			String_init(&out);
			String_append_char_string(&out, "Content-Type: application/xml; charset=utf-8\r\n");
			String_append_char_string(&out, "\r\n");
			OpCanvaContext_export_messages_to_xml(&Ctx, &out, true);
			printf("%s", String_get_char_string(&out));
			String_finalize(&out);
#else
			String out;
			String_init(&out);
			OpCanvaContext_export_messages_to_xml(&Ctx, &out, false);
			fprintf(stderr, "%s", String_get_char_string(&out));
			String_finalize(&out);
			CanvaCtx_write_to_fd(&Canva, 1);
#endif
			CanvaCtx_terminate(&Canva);
		}
	}
    else
    {
#ifdef CGIMODE
			String out;
			String_init(&out);
			String_append_char_string(&out, "Content-Type: application/xml; charset=utf-8\r\n");
			String_append_char_string(&out, "\r\n");
			OpCanvaContext_export_messages_to_xml(&Ctx, &out, true);
			printf("%s", String_get_char_string(&out));
			String_finalize(&out);
#else
			String out;
			String_init(&out);
			OpCanvaContext_export_messages_to_xml(&Ctx, &out, false);
			fprintf(stderr, "%s", String_get_char_string(&out));
			String_finalize(&out);
#endif
    }
    yy_delete_buffer(state);

#ifndef CGIMODE
    String_finalize(&buffer);
#endif

    OpCanvaContext_terminate(&Ctx);
	Op_free(root);

    return EXIT_SUCCESS;
}
