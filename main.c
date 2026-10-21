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
#include <OpProgram.h>
#include <parser.tab.h>
#include <lexer.h>
#include <unistd.h>
#include <log.h>




CanvaCtx Canva;

#ifndef NOSDL
SDL_Renderer *renderer;
SDL_Texture *texture;
unsigned char *pixels;
SDL_Window *window;
SDL_Event event;
#endif
int width = 640, height = 480;

#ifndef NOSDL
int one_iter(void)
{
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
    		log_trace("keydown %d\n", event.key.keysym.sym);
    		switch(event.key.keysym.sym)
    		{
                        case SDLK_ESCAPE:
                            SDL_Quit();
                            return -1;
                            break;
    		}
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
	log_set_level(LOG_INFO);
	size_t readed;
	OpCanvaContext Ctx;
	OpProgram Prog;
	OpCanvaContext_init(&Ctx);
	OpProgram_init(&Prog);
	OpParser Parser;
	OpParser_init(&Parser);
	OpProgram_set_context(&Prog, (OpContext*)&Ctx);
	OpParser_set_program(&Parser, &Prog);
	OpParser_set_current_context(&Parser, (OpContext*)&Ctx);
    /*extern int yydebug, yy_flex_debug;
    yydebug = 1;
    yy_flex_debug = 1;*/
    String *buffer;
    buffer = String_new();
#ifdef CGIMODE
    char *cbuffer;
    char *length_env = getenv("CONTENT_LENGTH");
    if(length_env == NULL)
    {
    	log_error("No CONTENT_LENGTH found into env\n");
    	return -1;
    }
    size_t length = strtoul(length_env, NULL, 10);
    cbuffer = malloc(length);
    readed = read(0, cbuffer, length);
    if(readed != length)
    {
    	log_error("read %zu != %zu\n", readed, length);
    	return -1;
    }
    String_append_data(buffer, length, (void*)cbuffer);
#else

    char buf[100];
    while((readed = read(0, buf, 100)) > 0)
    {
    	String_append_data(buffer, readed, (const void*)buf);
    }
#endif

    if(OpParser_parse(&Parser, buffer) == 0)
    {
    	int w, h;

    	OpProgram_prerun(&Prog);


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
				log_error("Can't init SDL:  %s\n", SDL_GetError( ) );
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
				log_error("Can't set video mode: %s\n", SDL_GetError( ) );
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
			OpProgram_set_Canva(&Prog, &Canva);

			OpProgram_run(&Prog);
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
			OpProgram_set_Canva(&Prog, &Canva);

			OpProgram_run(&Prog);

			CanvaCtx_write_to_png(&Canva);
#ifdef CGIMODE
			String out;
			String_init(&out);
			String_append_char_string(&out, "Content-Type: application/xml; charset=utf-8\r\n");
			String_append_char_string(&out, "\r\n");
			OpProgram_export_to_xml(&Prog, &out, true);
			printf("%s", String_get_char_string(&out));
			String_finalize(&out);
#else
			String out;
			String_init(&out);
			OpProgram_export_to_xml(&Prog, &out, false);
			log_trace("%s", String_get_char_string(&out));
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
			OpProgram_set_Canva(&Prog, &Canva);

			OpProgram_run(&Prog);

			CanvaCtx_finish(&Canva);
#ifdef CGIMODE
			String out;
			String_init(&out);
			String_append_char_string(&out, "Content-Type: application/xml; charset=utf-8\r\n");
			String_append_char_string(&out, "\r\n");
			OpProgram_export_to_xml(&Prog, &out, true);
			printf("%s", String_get_char_string(&out));
			String_finalize(&out);
#else
			String out;
			String_init(&out);
			OpProgram_export_to_xml(&Prog, &out, false);
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
			OpProgram_export_to_xml(&Prog, &out, true);
			printf("%s", String_get_char_string(&out));
			String_finalize(&out);
#else
			String out;
			String_init(&out);
			OpProgram_export_to_xml(&Prog, &out, false);
			fprintf(stderr, "%s", String_get_char_string(&out));
			String_finalize(&out);
#endif
    }

#ifdef CGIMODE
    free(cbuffer);
#endif

    OpCanvaContext_terminate(&Ctx);
    OpProgram_terminate(&Prog);
    OpParser_terminate(&Parser);

    return EXIT_SUCCESS;
}
