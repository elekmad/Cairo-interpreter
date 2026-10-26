/*
 * opcs.c
 *
 *  Created on: 10 juil. 2026
 *      Author: damien
 */


#include <OpProgram.h>
#include <parser.tab.h>
#include <lexer.h>
#include <String.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <log.h>


OpProgram *OpProgram_new(void)
{
	OpProgram *self = malloc(sizeof(OpProgram));
	if(self != NULL)
		OpProgram_init(self);
	return self;
}

void OpProgram_init(OpProgram *self)
{
	self->modules = NULL;
	self->number_of_modules = 0;
	self->imports = NULL;
	self->number_of_imports = 0;
	self->root = NULL;
	self->ctx = NULL;
}

void OpProgram_terminate(OpProgram *self)
{
	if(self->root != NULL)
		Op_free(self->root);
	if(self->number_of_modules > 0)
	{
		if(self->modules != NULL)
		{
			int i;
			for(i = 0; i < self->number_of_modules; i++)
			{
				if(self->modules[i] != NULL)
					OpModule_free(self->modules[i]);
			}
			free(self->modules);
			self->modules = NULL;
		}
	}
	if(self->number_of_imports > 0)
	{
		if(self->imports != NULL)
		{
			int i;
			for(i = 0; i < self->number_of_imports; i++)
			{
				if(self->imports[i] != NULL)
					OpImport_free(self->imports[i]);
			}
			free(self->imports);
			self->imports = NULL;
		}
	}
}

void OpProgram_free(OpProgram *self)
{
	if(self != NULL)
	{
		OpProgram_terminate(self);
		free(self);
	}
}

int OpProgram_fix_operandes(OpProgram *self)
{
	int ret = 0;
	if(self->root != NULL)
		ret = Op_fix_operandes(self->root, self->ctx);
	if(ret == 0)
	{
		size_t i;
		for(i = 0; i < self->number_of_modules; i++)
		{
			OpModule *m = self->modules[i];
			if(m != NULL)
			{
				if((ret = OpModule_fix_operandes(m)) != 0)
					break;
			}
		}
		for(i = 0; i < self->number_of_imports; i++)
		{
			OpImport *p = self->imports[i];
			if(p != NULL)
			{
				if((ret = OpImport_fix_operandes(p, self->ctx)) != 0)
					break;
			}
		}
	}
	return ret;
}

void OpProgram_set_Canva(OpProgram *self, CanvaCtx *c)
{
	size_t i;
	for(i = 0; i < self->number_of_modules; i++)
	{
		OpModule *m = self->modules[i];
		if(m != NULL)
			OpModule_set_Canva(m, c);
	}
}

void OpProgram_prerun(OpProgram *self)
{
	Op_prerun(self->root, self->ctx);
}

void OpProgram_run(OpProgram *self)
{
	Op_launch(self->root, self->ctx);
}

ssize_t OpProgram_check_module_number(OpProgram *self, const char *name)
{
	if(self->number_of_modules > 0)
	{
		size_t i;
		int found = 0;
		for(i = 0; i < self->number_of_modules; i++)
		{
			OpModule *m = self->modules[i];
			if(String_compare_with_char_string(OpModule_get_name(m), name) == 0)
			{
				found = 1;
				return (ssize_t)i;
			}
		}
	}
	return -1;
}

size_t OpProgram_get_module_number(OpProgram *self, const char *name)
{
	ssize_t num = OpProgram_check_module_number(self, name);
	if(num >= 0)
		return (size_t)num;
	size_t place;
	place = self->number_of_modules;
	self->number_of_modules++;
	self->modules = realloc(self->modules, self->number_of_modules * sizeof(OpModule*));
	OpModule *m = OpModule_new();
	OpModule_set_name(m, name);
	OpModule_duplicate_context(m, (OpCanvaContext*)self->ctx);
	self->modules[place] = m;
	return place;
}

OpModule *OpProgram_get_module(OpProgram *self, size_t num)
{
	OpModule *ret = NULL;
	if(num < self->number_of_modules)
		ret = self->modules[num];
	return ret;
}

size_t OpProgram_add_import(OpProgram *self, OpImport *import)
{
	self->number_of_imports++;
	self->imports = realloc(self->imports, self->number_of_imports * sizeof(OpImport*));
	self->imports[self->number_of_imports - 1] = import;
	return self->number_of_imports - 1;
}

OpImport *OpProgram_get_import(OpProgram *self, size_t num)
{
	if(num < self->number_of_imports)
		return self->imports[num];
	return NULL;
}

OpContext *OpProgram_get_context(OpProgram *self)
{
	return self->ctx;
}

void OpProgram_export_to_xml(OpProgram *self, String *xml, bool with_output)
{
	String_append_char_string(xml, "<xml>");
	OpCanvaContext_export_messages_to_xml((OpCanvaContext*)self->ctx, xml, with_output);
	String_append_char_string(xml, "<msgs>");
	OpContext_export_messages_to_xml(self->ctx, xml);
	size_t i;
	for(i = 0; i < self->number_of_modules; i++)
	{
		OpModule *m = self->modules[i];
		if(m == NULL)
			continue;
		OpModule_export_messages_to_xml(m, xml);
	}
	String_append_char_string(xml, "</msgs>");
	String_append_char_string(xml, "</xml>");
}

void OpProgram_set_context(OpProgram *self, OpContext *ctx)
{
	self->ctx = ctx;
}

void OpProgram_set_root(OpProgram *self, Op *root)
{
#ifdef DEBUG_FREE
	log_trace("Program set root %p\n", root);
#endif
	self->root = root;
}

OpParser *OpParser_new(void)
{
	OpParser *self = malloc(sizeof(OpParser));
	if(self != NULL)
		OpParser_init(self);
	return self;
}

void OpParser_init(OpParser *self)
{
	self->files = NULL;
	self->files_size = 0;
	self->depth = 0;
	String_init(&self->filename_prefix);
	LinkedList_init(&self->filenames);
	self->inside_module = 0;
	self->inside_loop = 0;
}

void OpParser_set_filename_prefix(OpParser *self, String *prefix)
{
	if(String_get_length(prefix) == 0)
		String_set_char_string(&self->filename_prefix, "./");
	else
	{
		String_cpy(&self->filename_prefix, prefix);
		if(String_get_char_at(&self->filename_prefix, String_get_length(&self->filename_prefix) - 1) != '/')
			String_append_char(&self->filename_prefix, '/');
	}
	log_trace("Filename Prefix : '%s'\n", String_get_char_string(&self->filename_prefix));
}

int OpParser_parse(OpParser *self, String *s)
{
	int fd = open("/tmp/foo", O_CREAT|O_TRUNC|O_WRONLY, 0666);
	write(fd, String_get_char_string(s), String_get_length(s));
	close(fd);
	OpParserFile *f = OpParserFile_new();
	f->state = yy_scan_bytes(String_get_data(s), String_get_length(s));
    f->file_content = s;
    OpParser_push_stream(self, f);

    Op *root = NULL;
    if(yyparse(&root, self) == 0)
    {
		OpProgram_set_root(self->program, root);
		if(OpProgram_fix_operandes(self->program) == 0)
			return 0;
		else
			return -1;
    }
    else
    	return -1;
    return 0;
}

int OpParser_check_if_include_exist(OpParser *self, const char *name)
{
	int ret = 0;
	LinkedListIterator ite;
	LinkedListIterator_init(&ite, &self->filenames);
    while(!LinkedListIterator_is_eol(&ite))
    {
    	LinkedListIterator_go_to_next(&ite);
    	String *f = LinkedListIterator_get_current_value(&ite);
    	if(f != NULL && String_compare_with_char_string(f, name) == 0)
    	{
    		ret = 1;
    		break;
    	}
    }
    LinkedListIterator_finalize(&ite);
	return ret;
}

void OpParser_parse_include(OpParser *self, const char *name)
{
	if(OpParser_check_if_include_exist(self, name) == 1)
		return;
	String *Sname = String_new();
	String_append_char_string(Sname, name);
	LinkedList_append(&self->filenames, Sname);
	String filename;
	String_init(&filename);
	String_append_String(&filename, &self->filename_prefix);
	String_append_String(&filename, Sname);
	String *new_stream = String_new();
	log_trace("Import include : %s => %s\n", name, String_get_char_string(&filename));
	char buf[100];
	int fd = open(String_get_char_string(&filename), O_RDONLY);
	ssize_t readed;
    while((readed = read(fd, buf, 100)) > 0)
    {
    	String_append_data(new_stream, readed, (const void*)buf);
    }
    close(fd);

    YY_BUFFER_STATE state = yy_scan_bytes(String_get_data(new_stream), String_get_length(new_stream));

    OpParserFile *f = OpParserFile_new();
    f->file_content = new_stream;
    f->state = state;
    String_finalize(&filename);

    OpParser_push_stream(self, f);
}

void OpParser_push_stream(OpParser *self, OpParserFile *s)
{
	self->depth++;
	if(self->depth > self->files_size)
	{
		self->files = realloc(self->files, self->depth * sizeof(OpParserFile*));
		self->files_size = self->depth;
	}
	self->files[self->depth - 1] = s;
	log_trace("Parser push : depth = %d\n", self->depth);
}

int OpParser_pop_stream(OpParser *self)
{
	OpParserFile *f = self->files[self->depth - 1];
	if(f != NULL)
	{
		OpParserFile_free(f);
		self->files[self->depth - 1] = NULL;
	}
	self->depth--;
	if(self->depth > 0)
		yy_switch_to_buffer(self->files[self->depth - 1]->state);
	log_trace("Parser pop : depth = %d\n", self->depth);
	return self->depth;
}

void OpParser_terminate(OpParser *self)
{
	int i;
	for(i = 0; i < self->files_size; i++)
	{
		if(self->files[i] != NULL)
			OpParserFile_free(self->files[i]);
	}
	free(self->files);
	String_finalize(&self->filename_prefix);
	LinkedList_do_to_all(&self->filenames, (void(*)(void*,void*))String_free, NULL);
	LinkedList_finalize(&self->filenames);
}

void OpParser_free(OpParser *self)
{
	if(self != NULL)
	{
		OpParser_terminate(self);
		free(self);
	}
}

OpContext *OpParser_get_current_context(OpParser *self)
{
	return self->current_c;
}

void OpParser_set_current_context(OpParser *self, OpContext *ctx)
{
	self->current_c = ctx;
}

OpModule *OpParser_get_current_module(OpParser *self)
{
	return self->current_m;
}

void OpParser_set_current_module(OpParser *self, OpModule *m)
{
	self->current_m = m;
}

int OpParser_get_inside_module(OpParser *self)
{
	return self->inside_module;
}

void OpParser_set_inside_module(OpParser *self)
{
	self->inside_module++;
}

void OpParser_unset_inside_module(OpParser *self)
{
	self->inside_module--;
}

int OpParser_get_inside_loop(OpParser *self)
{
	return self->inside_loop;
}

void OpParser_set_inside_loop(OpParser *self)
{
	self->inside_loop++;
}

void OpParser_unset_inside_loop(OpParser *self)
{
	self->inside_loop--;
}

OpProgram *OpParser_get_program(OpParser *self)
{
	return self->program;
}

void OpParser_set_program(OpParser *self, OpProgram *p)
{
	self->program = p;
}

int OpParseImport_import_svg(OpParseImport *self, OpImport *import, OpContext *ctx, const char *name)
{
	OpBloc *CanvaBloc = (OpBloc*)OpCanvaBloc_new();
	String filename;
	String_init(&filename);
	if(name[0] != '/')//Absolute path
		String_append_String(&filename, &self->prefix);
	String_append_char_string(&filename, name);

	xmlTextReaderPtr reader = xmlReaderForFile(String_get_char_string(&filename), NULL, 0);

	String_finalize(&filename);

	if(reader == NULL)
		return -1;

	while (xmlTextReaderRead(reader) == 1)
	{
		int append = 0;

	    if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT)
	        continue;

	    const xmlChar *name =
	        xmlTextReaderConstName(reader);


	    OpBloc *bloc = (OpBloc*)OpCanvaBloc_new();

		_OpParseImport_import_parse_transform_section(self, reader, bloc);

	    if (xmlStrEqual(name, BAD_CAST "svg")) {

	    	xmlChar *attr = xmlTextReaderGetAttribute(reader, BAD_CAST "viewBox");
	    	if(attr != NULL)
	    	{
	    		size_t x, y, w, h;
	    		if(sscanf((char*)attr, "%zu %zu %zu %zu", &x, &y, &w, &h) == 4)
	    		{
	    			import->x = x;
	    			import->y = y;
	    			import->w = w;
	    			import->h = h;
	    		}
	    		xmlFree(attr);
	    	}

	    	log_trace("svg\n");
	        //parse_svg(reader);
	    }
	    else if (xmlStrEqual(name, BAD_CAST "g")) {
	    	append = OpParseImport_import_parser_group(self, reader, bloc);
	    }
	    else if (xmlStrEqual(name, BAD_CAST "rect")) {
	    	append = 1;
	    	OpParseImport_import_parser_rect(self, reader, bloc);
	    }
	    else if (xmlStrEqual(name, BAD_CAST "circle")) {
	    	append = 1;
	    	OpParseImport_import_parser_circle(self, reader, bloc);
	    }
	    else if (xmlStrEqual(name, BAD_CAST "ellipse")) {
	    	append = 1;
	    	OpParseImport_import_parser_ellipse(self, reader, bloc);
	    }
	    else if (xmlStrEqual(name, BAD_CAST "path")) {
	    	append = 1;
	    	OpParseImport_import_parser_path(self, reader, bloc);
	    }
	    else if (xmlStrEqual(name, BAD_CAST "line")) {
	    	append = 1;
	    	OpParseImport_import_parser_line(self, reader, bloc);
	    }
	    else if (xmlStrEqual(name, BAD_CAST "polyline")) {
	    	append = 1;
	    	OpParseImport_import_parser_polyline(self, reader, bloc, 0);
	    }
	    else if (xmlStrEqual(name, BAD_CAST "polygon")) {
	    	append = 1;
	    	OpParseImport_import_parser_polyline(self, reader, bloc, 1);
	    }

	    if(append == 1)
	    {
			OpParseImport_import_parse_color_section(reader, bloc);

			OpBloc_append_Op(CanvaBloc, (Op*)bloc);
	    }
	    else
	    	Op_free((Op*)bloc);
	}

	xmlFreeTextReader(reader);
	Op_fix_operandes((Op*)CanvaBloc, ctx);
	OpImport_set_root(import, (Op*)CanvaBloc);
	return 0;
}

void _OpParseImport_import_parse_transform_section(OpParseImport *self, xmlTextReaderPtr reader, OpBloc *ret)
{
	xmlChar *attr = xmlTextReaderGetAttribute(reader, BAD_CAST "transform");
	if(attr != NULL)
	{
		char *p = (char*)attr;
		double x, y, a, m1, m2, m3, m4, m5, m6;
		while(*p != '\0')
		{
			if(strncmp(p, "translate(", 10) == 0)
			{
				if(sscanf(p+10, "%lf,%lf)", &x, &y) == 2)
				{
					Op *Tr = OpTranslate_new();
					Op *g = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)g, x);
					OpTranslate_set_x((OpTranslate*)Tr, g);
					g = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)g, y);
					OpTranslate_set_y((OpTranslate*)Tr, g);
					OpBloc_append_Op(ret, Tr);
					log_trace("Apply Translate (%f, %f) to bloc %p\n", x, y, ret);
				}
				else if(sscanf(p+10, "%lf)", &x) == 1)
				{
					Op *Tr = OpTranslate_new();
					Op *g = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)g, x);
					OpTranslate_set_x((OpTranslate*)Tr, g);
					g = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)g, x);
					OpTranslate_set_y((OpTranslate*)Tr, g);
					OpBloc_append_Op(ret, Tr);
					log_trace("Apply Translate (%f) to bloc %p\n", x, ret);
				}
			}
			else if(strncmp(p, "rotate(", 7) == 0)
			{
				if(sscanf(p+7, "%lf,%lf,%lf)", &a, &x, &y) == 3)
				{
					Op *Tr = OpTranslate_new();
					Op *g = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)g, x);
					OpTranslate_set_x((OpTranslate*)Tr, g);
					g = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)g, y);
					OpTranslate_set_y((OpTranslate*)Tr, g);
					OpBloc_append_Op(ret, Tr);
					Op *Rt = OpRotate_new();
					g = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)g, a * M_PI / 180);
					OpRotate_set_angle((OpRotate*)Rt, g);
					OpBloc_append_Op(ret, Rt);
					Tr = OpTranslate_new();
					g = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)g, -x);
					OpTranslate_set_x((OpTranslate*)Tr, g);
					g = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)g, -y);
					OpTranslate_set_y((OpTranslate*)Tr, g);
					OpBloc_append_Op(ret, Tr);
					log_trace("Apply Rotate Around Point to bloc %p\n", ret);
				}
				else if(sscanf(p+7, "%lf)", &a) == 1)
				{
					Op *Rt = OpRotate_new();
					Op *g = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)g, a * M_PI / 180);
					OpRotate_set_angle((OpRotate*)Rt, g);
					OpBloc_append_Op(ret, Rt);
					log_trace("Apply Rotate to bloc %p\n", ret);
				}
			}
			else if(strncmp(p, "scale(", 6) == 0)
			{
				if(sscanf(p+6, "%lf,%lf)", &x, &y) == 2)
				{
					Op *Sc = OpScale_new();
					Op *g = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)g, x);
					OpScale_set_x((OpScale*)Sc, g);
					g = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)g, y);
					OpScale_set_y((OpScale*)Sc, g);
					OpBloc_append_Op(ret, Sc);
					log_trace("Apply Scale to bloc %p\n", ret);
				}
				else if(sscanf(p+6, "%lf)", &x) == 1)
				{
					Op *Sc = OpScale_new();
					Op *g = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)g, x);
					OpScale_set_x((OpScale*)Sc, g);
					g = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)g, x);
					OpScale_set_y((OpScale*)Sc, g);
					OpBloc_append_Op(ret, Sc);
					log_trace("Apply Scale to bloc %p\n", ret);
				}
			}
			else if(sscanf(p, "matrix(%lf,%lf,%lf,%lf,%lf,%lf)", &m1, &m2, &m3, &m4, &m5, &m6) == 6)
			{
				Op *M = OpMatrix_new();
				Op *g = OpGetValue_new();
				OpGetValue_set_value((OpGetValue*)g, m1);
				OpMatrix_set_m1((OpMatrix*)M, g);

				g = OpGetValue_new();
				OpGetValue_set_value((OpGetValue*)g, m2);
				OpMatrix_set_m2((OpMatrix*)M, g);

				g = OpGetValue_new();
				OpGetValue_set_value((OpGetValue*)g, m3);
				OpMatrix_set_m3((OpMatrix*)M, g);

				g = OpGetValue_new();
				OpGetValue_set_value((OpGetValue*)g, m4);
				OpMatrix_set_m4((OpMatrix*)M, g);

				g = OpGetValue_new();
				OpGetValue_set_value((OpGetValue*)g, m5);
				OpMatrix_set_m5((OpMatrix*)M, g);

				g = OpGetValue_new();
				OpGetValue_set_value((OpGetValue*)g, m6);
				OpMatrix_set_m6((OpMatrix*)M, g);

				OpBloc_append_Op(ret, M);
				log_trace("Apply Matrix to bloc %p\n", ret);
			}
			else if(strncmp(p, "skew", 4) == 0)
			{
				if(strncmp(p+4, "X(", 2) == 0)
				{
					if(sscanf(p+6, "%lf)", &a) == 1)
					{
						Op *Sk = OpSkewX_new();
						Op *g = OpGetValue_new();
						OpGetValue_set_value((OpGetValue*)g, a * M_PI / 180);
						OpSkewX_set_angle((OpSkewX*)Sk, g);
						OpBloc_append_Op(ret, Sk);
						log_trace("Apply SkewX %f rad (%f deg) to bloc %p\n", a*M_PI/180, a, ret);
					}
				}
				else if(strncmp(p+4, "Y(", 2) == 0)
				{
					if(sscanf(p+6, "%lf)", &a) == 1)
					{
						Op *Sk = OpSkewY_new();
						Op *g = OpGetValue_new();
						OpGetValue_set_value((OpGetValue*)g, a * M_PI / 180);
						OpSkewY_set_angle((OpSkewY*)Sk, g);
						OpBloc_append_Op(ret, Sk);
						log_trace("Apply SkewY %f rad (%f deg) to bloc %p\n", a*M_PI/180, a, ret);
					}
				}
			}
			p = strchrnul(p, ')');
			while(*p != '\0' && !((*p >= 'a' && *p <='z') || (*p >= 'A' && *p <= 'Z')))
				p++;
		}
		xmlFree(attr);
	}
}


int OpParseImport_import_parser_group(OpParseImport *self, xmlTextReaderPtr reader, OpBloc *ret)
{
	log_trace("Parse SVG Group %p\n", ret);

	if(xmlTextReaderIsEmptyElement(reader))
	{
		log_trace("Empty Group %p\n", ret);
		return -1;
	}

	int depth = xmlTextReaderDepth(reader);
	while (xmlTextReaderRead(reader) == 1)
	{
		int append = 0;
	    if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT && xmlTextReaderDepth(reader) == depth)//End of group
	        break;
	    else if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT)
	        continue;

	    const xmlChar *name =
	        xmlTextReaderConstName(reader);


	    OpBloc *bloc = (OpBloc*)OpCanvaBloc_new();

		_OpParseImport_import_parse_transform_section(self, reader, bloc);

	    if (xmlStrEqual(name, BAD_CAST "g"))
	    {
			append = OpParseImport_import_parser_group(self, reader, bloc);
		}
	    else if (xmlStrEqual(name, BAD_CAST "rect")) {
	    	append = 1;
	    	OpParseImport_import_parser_rect(self, reader, bloc);
	    }
	    else if (xmlStrEqual(name, BAD_CAST "circle")) {
	    	append = 1;
	    	OpParseImport_import_parser_circle(self, reader, bloc);
	    }
	    else if (xmlStrEqual(name, BAD_CAST "ellipse")) {
	    	append = 1;
	    	OpParseImport_import_parser_ellipse(self, reader, bloc);
	    }
	    else if (xmlStrEqual(name, BAD_CAST "path")) {
	    	append = 1;
	    	OpParseImport_import_parser_path(self, reader, bloc);
	    }
	    else if (xmlStrEqual(name, BAD_CAST "line")) {
	    	append = 1;
	    	OpParseImport_import_parser_line(self, reader, bloc);
	    }
	    else if (xmlStrEqual(name, BAD_CAST "polyline")) {
	    	append = 1;
	    	OpParseImport_import_parser_polyline(self, reader, bloc, 0);
	    }
	    else if (xmlStrEqual(name, BAD_CAST "polygon")) {
	    	append = 1;
	    	OpParseImport_import_parser_polyline(self, reader, bloc, 1);
	    }

	    if(append == 1)
	    {
			OpParseImport_import_parse_color_section(reader, bloc);

			OpBloc_append_Op(ret, (Op*)bloc);
	    }
	    else
	    	Op_free((Op*)bloc);
	}
	log_trace("End of Group %p\n", ret);
	return 1;
}


void OpParseImport_import_parser_rect(OpParseImport *self, xmlTextReaderPtr reader, OpBloc *ret)
{
	log_trace("Parse SVG RECT\n");
	double x = NAN, y = NAN, w = NAN, h = NAN;
	xmlChar *attr = xmlTextReaderGetAttribute(reader, BAD_CAST "x");
	if(attr != NULL)
	{
		x = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	attr = xmlTextReaderGetAttribute(reader, BAD_CAST "y");
	if(attr != NULL)
	{
		y = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	attr = xmlTextReaderGetAttribute(reader, BAD_CAST "width");
	if(attr != NULL)
	{
		w = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	attr = xmlTextReaderGetAttribute(reader, BAD_CAST "height");
	if(attr != NULL)
	{
		h = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	if(!isnan(x) && !isnan(y) && !isnan(w) && !isnan(h))
	{
		OpRectangle *r = (OpRectangle*)OpRectangle_new();

		Op *g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, x);
		OpRectangle_set_x(r, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, y);
		OpRectangle_set_y(r, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, w);
		OpRectangle_set_w(r, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, h);
		OpRectangle_set_h(r, g);

		OpBloc_append_Op(ret, (Op*)r);
	}
}


void OpParseImport_import_parser_line(OpParseImport *self, xmlTextReaderPtr reader, OpBloc *ret)
{
	log_trace("Parse SVG LINE\n");
	double x1 = NAN, y1 = NAN, x2 = NAN, y2 = NAN;
	xmlChar *attr = xmlTextReaderGetAttribute(reader, BAD_CAST "x1");
	if(attr != NULL)
	{
		x1 = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	attr = xmlTextReaderGetAttribute(reader, BAD_CAST "y1");
	if(attr != NULL)
	{
		y1 = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	attr = xmlTextReaderGetAttribute(reader, BAD_CAST "x2");
	if(attr != NULL)
	{
		x2 = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	attr = xmlTextReaderGetAttribute(reader, BAD_CAST "y2");
	if(attr != NULL)
	{
		y2 = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	if(!isnan(x1) && !isnan(y1) && !isnan(x2) && !isnan(y2))
	{
		Op *p = OpNewPath_new();
		OpBloc_append_Op(ret, p);

		OpMoveTo *m = (OpMoveTo*)OpMoveTo_new();

		Op *g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, x1);
		OpMoveTo_set_x(m, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, y1);
		OpMoveTo_set_y(m, g);

		OpBloc_append_Op(ret, (Op*)m);

		OpDrawLineTo *l = (OpDrawLineTo*)OpDrawLineTo_new();

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, x2);
		OpDrawLineTo_set_x(l, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, y2);
		OpDrawLineTo_set_y(l, g);

		OpBloc_append_Op(ret, (Op*)l);
	}
}


void OpParseImport_import_parser_polyline(OpParseImport *self, xmlTextReaderPtr reader, OpBloc *ret, bool is_polygone)
{
	log_trace("Parse SVG POLYLINE/GONE (%d)\n", is_polygone);
	xmlChar *attr = xmlTextReaderGetAttribute(reader, BAD_CAST "points");
	if(attr != NULL)
	{
		double x, y;
		char *p = (char*)attr;
		OpVariable v;
		OpVariable_init(&v);
		while(*p != '\0')
		{
			x = NAN;
			y = NAN;
			x = strtod(p, &p);
			if(*p == ',')
				y = strtod(p+1, &p);
			if(!isnan(x) && !isnan(y))
			{
				OpVariable_append_double(&v, x);
				OpVariable_append_double(&v, y);
			}
			if(*p != '\0')
				p++;
		}
		if(OpVariable_get_type(&v) == DOUBLES && OpVariable_get_number_elements(&v) > 0)
		{
			Op *P = OpPolyline_new(), *g;
			g = OpGetValue_new();
			OpGetValue_copy_variable((OpGetValue*)g, &v);
			OpPolyline_set_params((OpPolyline*)P, g);
			if(is_polygone == 1)
				OpPolyline_set_polygon((OpPolyline*)P);
			OpBloc_append_Op(ret, P);
		}
		OpVariable_terminate(&v);
	}
}


void OpParseImport_import_parser_circle(OpParseImport *self, xmlTextReaderPtr reader, OpBloc *ret)
{
	log_trace("Parse SVG CIRCLE\n");
	double cx = NAN, cy = NAN, r = NAN;
	xmlChar *attr = xmlTextReaderGetAttribute(reader, BAD_CAST "cx");
	if(attr != NULL)
	{
		cx = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	attr = xmlTextReaderGetAttribute(reader, BAD_CAST "cy");
	if(attr != NULL)
	{
		cy = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	attr = xmlTextReaderGetAttribute(reader, BAD_CAST "r");
	if(attr != NULL)
	{
		r = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	if(!isnan(cx) && !isnan(cy) && !isnan(r))
	{
		Op *c = OpCircle_new();

		Op *g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, cx);
		OpCircle_set_x((OpCircle*)c, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, cy);
		OpCircle_set_y((OpCircle*)c, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, r);
		OpCircle_set_r((OpCircle*)c, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, 0);
		OpCircle_set_a1((OpCircle*)c, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, 2*M_PI);
		OpCircle_set_a2((OpCircle*)c, g);

		OpBloc_append_Op(ret, c);
	}
}


void OpParseImport_import_parser_ellipse(OpParseImport *self, xmlTextReaderPtr reader, OpBloc *ret)
{
	log_trace("Parse SVG CIRCLE\n");
	double cx = NAN, cy = NAN, rx = NAN, ry = NAN;
	xmlChar *attr = xmlTextReaderGetAttribute(reader, BAD_CAST "cx");
	if(attr != NULL)
	{
		cx = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	attr = xmlTextReaderGetAttribute(reader, BAD_CAST "cy");
	if(attr != NULL)
	{
		cy = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	attr = xmlTextReaderGetAttribute(reader, BAD_CAST "rx");
	if(attr != NULL)
	{
		rx = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	attr = xmlTextReaderGetAttribute(reader, BAD_CAST "ry");
	if(attr != NULL)
	{
		ry = strtod((char*)attr, NULL);
		xmlFree(attr);
	}
	if(!isnan(cx) && !isnan(cy) && !isnan(rx) && !isnan(ry))
	{
		OpBloc *cb = (OpBloc*)OpCanvaBloc_new();
		OpBloc_append_Op(ret, (Op*)cb);

		Op *t = OpTranslate_new();

		Op *g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, cx);
		OpTranslate_set_x((OpTranslate*)t, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, cy);
		OpTranslate_set_y((OpTranslate*)t, g);

		OpBloc_append_Op(cb, t);

		Op *sc = OpScale_new();

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, rx);
		OpScale_set_x((OpScale*)sc, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, ry);
		OpScale_set_y((OpScale*)sc, g);

		OpBloc_append_Op(cb, sc);

		Op *c = OpCircle_new();

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, 0);
		OpCircle_set_x((OpCircle*)c, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, 0);
		OpCircle_set_y((OpCircle*)c, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, 1.0);
		OpCircle_set_r((OpCircle*)c, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, 0);
		OpCircle_set_a1((OpCircle*)c, g);

		g = OpGetValue_new();
		OpGetValue_set_value((OpGetValue*)g, 2*M_PI);
		OpCircle_set_a2((OpCircle*)c, g);

		OpBloc_append_Op(cb, c);
	}
}


void OpParseImport_import_parse_color_section(xmlTextReaderPtr reader, OpBloc *ret)
{
	OpColor *fill_color = NULL, *stroke_color = NULL;
	int fill_present = 0, stroke_present = 0, line_width_present = 0;
	Op *fill_draw = NULL, *stroke_draw = NULL, *style = NULL;
	_OpParseImport_import_parse_style_section(reader, &fill_color, &fill_present, &stroke_color, &stroke_present, &style, &line_width_present);

	if(stroke_color == NULL && stroke_present == 0)
		_OpParseImport_import_parse_color_section(reader, &stroke_color, "stroke");
	if(stroke_color != NULL && line_width_present == 0)
	{
		xmlChar *attr = xmlTextReaderGetAttribute(reader, BAD_CAST "stroke-width");
		if(attr != NULL)
		{
			double lw = strtod((char*)attr, NULL);
			if(!isnan(lw))
			{
				Op *slw, *gv = OpGetValue_new();
				slw = OpSetLineWidth_new();
				OpGetValue_set_value((OpGetValue*)gv, lw);
				OpSetLineWidth_set_width((OpSetLineWidth*)slw, gv);

				if(style == NULL)
					style = OpBloc_new();
				OpBloc_append_Op((OpBloc*)style, slw);
			}
			xmlFree(attr);
		}
	}

	if(style != NULL)
		OpBloc_append_Op(ret, style);

	if(fill_color == NULL && fill_present == 0)
		_OpParseImport_import_parse_color_section(reader, &fill_color, "fill");
	if(fill_color != NULL)
	{
		OpBloc_append_Op(ret, (Op*)fill_color);
		if(stroke_color == NULL)
			fill_draw = OpFill_new();
		else
			fill_draw = OpFillPreserve_new();
		OpBloc_append_Op(ret, fill_draw);
	}
	if(stroke_color != NULL)
	{
		stroke_draw = OpStroke_new();
		OpBloc_append_Op(ret, (Op*)stroke_color);
		OpBloc_append_Op(ret, stroke_draw);
	}
}

void _OpParseImport_import_parse_style_section(xmlTextReaderPtr reader, OpColor **fill, int *fill_present, OpColor **stroke, int *stroke_present, Op **style, int *linewidth_present)
{
	xmlChar *attr = xmlTextReaderGetAttribute(reader, BAD_CAST "style");
	if(attr != NULL)
	{
		OpParserHashElt *elts = NULL;
		String name, value;
		char *p = (char*)attr;
		String_init(&name);
		String_init(&value);
		while(*p != '\0')
		{
			char *sep;
			if((sep = strchr(p, ':')) != NULL)
			{
				String_empty(&name);
				String_append_data(&name, sep - p, p);
				p = sep + 1;
				sep = strchrnul(p, ';');

				String_empty(&value);
				String_append_data(&value, sep - p, p);
				if(*sep != '\0')
					p = sep + 1;
				else
					p = sep;
				OpParserHashElt *e = malloc(sizeof(OpParserHashElt));
				strcpy(e->name, String_get_char_string(&name));
				strcpy(e->value, String_get_char_string(&value));
				HASH_ADD_STR(elts, name, e);

				log_trace("Name '%s' : value '%s'\n", String_get_char_string(&name), String_get_char_string(&value));
			}
		}
		String_finalize(&name);
		String_finalize(&value);
		//Use HasHMap

		//Search fill:color and fill-opacity:opacity
		char *col_name = "fill", *opacity_name = "fill-opacity";

		OpParserHashElt *color, *opacity;
		HASH_FIND_STR(elts, col_name, color);
		if(color)
		{
			*fill_present = 1;
			HASH_FIND_STR(elts, opacity_name, opacity);
			_OpParseImport_import_get_rgba_color(color->value, opacity != NULL ? opacity->value : NULL, fill);
		}

		col_name = "stroke";
		opacity_name = "stroke-opacity";

		HASH_FIND_STR(elts, col_name, color);
		if(color)
		{
			*stroke_present = 1;
			HASH_FIND_STR(elts, opacity_name, opacity);
			_OpParseImport_import_get_rgba_color(color->value, opacity != NULL ? opacity->value : NULL, stroke);

			OpParserHashElt *line_width;
			HASH_FIND_STR(elts, "stroke-width", line_width);
			if(line_width != NULL)
			{
				double lw = strtod(line_width->value, NULL);
				if(!isnan(lw))
				{
					*linewidth_present = 1;
					if(*style == NULL)
						*style = OpBloc_new();
					Op *sw = OpSetLineWidth_new(), *gv = OpGetValue_new();
					OpGetValue_set_value((OpGetValue*)gv, lw);
					OpSetLineWidth_set_width((OpSetLineWidth*)sw, gv);
					OpBloc_append_Op((OpBloc*)*style, sw);
				}
			}
		}

		//Free HashMap
		OpParserHashElt *cur, *tmp;
		HASH_ITER(hh, elts, cur, tmp)
		{
			HASH_DEL(elts, cur);
			free(cur);
		}
		xmlFree(attr);
	}
}

unsigned int _OpParseImport_import_hex_to_int(const char *e, int size)
{
	unsigned int r = 0;
	for(int i = 0; i < size; i++)
	{
		char c = e[i];
		if(c >= '0' && c <= '9')
			r = r * 16 + c - '0';
		else if(c >= 'a' && c <= 'f')
			r = r * 16 + c - 'a' + 10;
		else if(c >= 'A' && c <= 'F')
			r = r * 16 + c - 'A' + 10;
	}
	if(size == 1)
		r = r * 16 + r;
	return r;
}

void _OpParseImport_import_get_rgba_color(const char *color, const char *opacity, OpColor **op)
{
	double r = NAN, g = NAN, b = NAN, a = NAN;
	if(*color == '\0')
		return;
	if(opacity != NULL)
	{
		char *p;
		a = strtod(opacity, &p);
		if(*p == '%')
			a /= 100;

		if(double_eq(a, 0.0))
			return;
	}

	if(strcmp(color, "none") == 0)
		return;
	if(*color == '#')
	{
		if(strlen(color+1) == 3)
		{
			r = (double)_OpParseImport_import_hex_to_int(color + 1, 1);
			g = (double)_OpParseImport_import_hex_to_int(color + 2, 1);
			b = (double)_OpParseImport_import_hex_to_int(color + 3, 1);
		}
		else
		{
			r = (double)_OpParseImport_import_hex_to_int(color + 1, 2);
			g = (double)_OpParseImport_import_hex_to_int(color + 3, 2);
			b = (double)_OpParseImport_import_hex_to_int(color + 5, 2);
		}
	}
	else if(strncmp(color, "rgb(", 4) == 0)
	{
		char *p;
		r = strtod(color + 4, &p);
		if(*p == '%')
		{
			r *= 2.55;
			p++;
		}
		if(*p == ',')
			p++;
		while(*p != '\0' && !(*p >= '0' && *p <= '9'))
			p++;

		g = strtod(p, &p);
		if(*p == '%')
		{
			g *= 2.55;
			p++;
		}
		if(*p == ',')
			p++;
		while(*p != '\0' && !(*p >= '0' && *p <= '9'))
			p++;
		b = strtod(p, &p);
		if(*p == '%')
			b *= 2.55;
	}
	else
	{
		const char *rgb = getrgb(color);
		if(rgb != NULL)
		{
			r = (double)_OpParseImport_import_hex_to_int(rgb, 2);
			g = (double)_OpParseImport_import_hex_to_int(rgb + 2, 2);
			b = (double)_OpParseImport_import_hex_to_int(rgb + 4, 2);
		}
	}
	if(!isnan(r) && !isnan(g) && !isnan(b))
	{
		OpVariable v;
		OpVariable_init(&v);
		OpVariable_append_double(&v, r);
		OpVariable_append_double(&v, g);
		OpVariable_append_double(&v, b);
		if(!isnan(a))
			OpVariable_append_double(&v, a);
		*op = (OpColor*)OpColor_new();
		Op *gv = OpGetValue_new();
		OpGetValue_copy_variable((OpGetValue*)gv, &v);
		OpVariable_terminate(&v);
		OpColor_set_params((OpColor*)*op, gv);
	}
}

void _OpParseImport_import_parse_color_section(xmlTextReaderPtr reader, OpColor **op, const char *name)
{
	xmlChar *attr = xmlTextReaderGetAttribute(reader, BAD_CAST name);
	if(attr != NULL)
	{
		char *color = (char*)attr, *opacity = NULL;
		String opacity_name;
		String_init(&opacity_name);
		String_append_char_string(&opacity_name, name);
		String_append_char_string(&opacity_name, "-opacity");
		xmlChar *attr2 = xmlTextReaderGetAttribute(reader, BAD_CAST String_get_char_string(&opacity_name));
		if(attr2 != NULL)
			opacity = (char*)attr2;

		_OpParseImport_import_get_rgba_color(color, opacity, op);

		if(attr2 != NULL)
			xmlFree(attr2);

		xmlFree(attr);
	}
}


void OpParseImport_import_parser_path(OpParseImport *self, xmlTextReaderPtr reader, OpBloc *ret)
{
	log_trace("Parse SVG PATH\n");
	double lastx = 0, lasty = 0;
	xmlChar *attr = xmlTextReaderGetAttribute(reader, BAD_CAST "d");
	if(attr != NULL)
	{
		char *p = (char*)attr, c, old_c = '\0';
		double last_command_x, last_command_y;
		Op *o, *o2;
		o = OpNewPath_new();
		OpBloc_append_Op(ret, o);

		while(*p != '\0' && !((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z')))//Set p to beginning.
			p++;

		while(*p != '\0')
		{
			if((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z'))//New command, else continue same command
			{
				old_c = c;
				c = *p;
			}
			else
				p -= 2;//Move backward to parse as same as a command

			log_trace("Parsing Path '%c'\n", c);
			switch(c)
			{
			case 'M' :	lastx = 0;
						lasty = 0;
			case 'm' :	{
							double x, y;
							p+=2;
							x = strtod(p, &p) + lastx;
							p++;
							y = strtod(p, &p) + lasty;
							o = OpMoveTo_new();

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, x);
							OpMoveTo_set_x((OpMoveTo*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, y);
							OpMoveTo_set_y((OpMoveTo*)o, o2);

							OpBloc_append_Op(ret, o);
							log_trace("Append Move Path %p to bloc %p\n", ret, o);
							lastx = x;
							lasty = y;
							break;
						}
			case 'L' :	lastx = 0;
						lasty = 0;
			case 'l' :	{
							double x, y;
							p+=2;
							x = strtod(p, &p) + lastx;
							p++;
							y = strtod(p, &p) + lasty;
							o = OpDrawLineTo_new();

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, x);
							OpDrawLineTo_set_x((OpDrawLineTo*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, y);
							OpDrawLineTo_set_y((OpDrawLineTo*)o, o2);

							OpBloc_append_Op(ret, o);
							log_trace("Append Line Path %p to bloc %p\n", ret, o);

							lastx = x;
							lasty = y;
							break;
						}

			case 'H' :	lastx = 0;
			case 'h' :	{
							double x;
							p+=2;
							x = strtod(p, &p) + lastx;
							o = OpDrawLineTo_new();

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, x);
							OpDrawLineTo_set_x((OpDrawLineTo*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, lasty);
							OpDrawLineTo_set_y((OpDrawLineTo*)o, o2);

							OpBloc_append_Op(ret, o);
							log_trace("Append Line Path %p to bloc %p\n", ret, o);

							lastx = x;
							break;
						}

			case 'V' :	lasty = 0;
			case 'v' :	{
							double y;
							p+=2;
							y = strtod(p, &p) + lasty;
							o = OpDrawLineTo_new();

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, lastx);
							OpDrawLineTo_set_x((OpDrawLineTo*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, y);
							OpDrawLineTo_set_y((OpDrawLineTo*)o, o2);

							OpBloc_append_Op(ret, o);
							log_trace("Append Line Path %p to bloc %p\n", ret, o);

							lasty = y;
							break;
						}

			case 'C' :	lastx = 0;
						lasty = 0;
			case 'c' :	{
							double m1x, m1y, m2x, m2y, x, y;
							p+=2;
							m1x = strtod(p, &p);
							p++;
							m1y = strtod(p, &p);
							p++;
							o = OpCurve_new();

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m1x+lastx);
							OpCurve_set_m1x((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m1y+lasty);
							OpCurve_set_m1y((OpCurve*)o, o2);

							m2x = strtod(p, &p);
							p++;
							m2y = strtod(p, &p);
							p++;

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m2x+lastx);
							OpCurve_set_m2x((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m2y+lasty);
							OpCurve_set_m2y((OpCurve*)o, o2);

							x = strtod(p, &p);
							p++;
							y = strtod(p, &p);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, x+lastx);
							OpCurve_set_x((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, y+lasty);
							OpCurve_set_y((OpCurve*)o, o2);

							OpBloc_append_Op(ret, o);
							log_trace("Append Curve Path %p to bloc %p\n", ret, o);

							log_trace("Parsing Curve (from %f %f) %f %f %f %f %f %f\n", lastx, lasty, m1x, m1y, m2x, m2y, x, y);

							last_command_x = m2x + lastx;
							last_command_y = m2y + lasty;

							lastx = x+lastx;
							lasty = y+lasty;

							break;
						}


			case 'S' :
			case 's' :	{
							double m1x, m1y, m2x, m2y, x, y;
							p+=2;

							m2x = strtod(p, &p);
							p++;
							m2y = strtod(p, &p);
							p++;

							x = strtod(p, &p);
							p++;
							y = strtod(p, &p);

							if(old_c == 's' || old_c == 'S' || old_c == 'C' || old_c == 'c')
							{
								m1x = 2.0 * lastx - last_command_x;
								m1y = 2.0 * lasty - last_command_y;
							}

							if(c == 's')
							{
								m2x += lastx;
								m2y += lasty;
								x += lastx;
								y += lasty;
							}
							log_trace("Parsing Smoothed Curve Path %c m2 (%f, %f) xy (%f, %f) (m1 (%f, %f) lastxy (%f, %f) lastcommand(%f, %f))\n", c, m2x, m2y, x, y, m1x, m1y, lastx, lasty, last_command_x, last_command_y);

							o = OpCurve_new();

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m1x);
							OpCurve_set_m1x((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m1y);
							OpCurve_set_m1y((OpCurve*)o, o2);
							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m2x);
							OpCurve_set_m2x((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m2y);
							OpCurve_set_m2y((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, x);
							OpCurve_set_x((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, y);
							OpCurve_set_y((OpCurve*)o, o2);

							OpBloc_append_Op(ret, o);
							log_trace("Append Curve Path %p to bloc %p\n", ret, o);

							last_command_x = m2x;
							last_command_y = m2y;

							lastx = x;
							lasty = y;

							break;
						}
			case 'q' :
			case 'Q' :	{
							double mx, my, m1x, m1y, m2x, m2y, x, y;
							p+=2;
							mx = strtod(p, &p);
							p++;
							my = strtod(p, &p);

							p++;
							x = strtod(p, &p);
							p++;
							y = strtod(p, &p);
							if(c == 'q')
							{
								mx += lastx;
								my += lasty;
								x += lastx;
								y += lasty;
							}

							m1x = lastx + 2.0/3.0 * (mx - lastx);
							m1y = lasty + 2.0/3.0 * (my - lasty);

							m2x = x + 2.0/3.0 * (mx - x);
							m2y = y + 2.0/3.0 * (my - y);

							o = OpCurve_new();

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m1x);
							OpCurve_set_m1x((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m1y);
							OpCurve_set_m1y((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m2x);
							OpCurve_set_m2x((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m2y);
							OpCurve_set_m2y((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, x);
							OpCurve_set_x((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, y);
							OpCurve_set_y((OpCurve*)o, o2);

							OpBloc_append_Op(ret, o);
							log_trace("Append Curve Path %p to bloc %p\n", ret, o);

							last_command_x = mx;
							last_command_y = my;

							lastx = x;
							lasty = y;
							break;
						}
			case 't' :
			case 'T' :	{
							double mx, my, m1x, m1y, m2x, m2y, x, y;
							p+=2;

							x = strtod(p, &p);
							p++;
							y = strtod(p, &p);

							if(old_c == 'Q' || old_c == 'q' || old_c == 't' || old_c == 'T')
							{
								mx = 2.0 * lastx - last_command_x;
								my = 2.0 * lasty - last_command_y;
							}
							else
							{
								mx = lastx;
								my = lasty;
							}

							if(c == 't')
							{
								x += lastx;
								y += lasty;
							}

							m1x = lastx + 2.0/3.0 * (mx - lastx);
							m1y = lasty + 2.0/3.0 * (my - lasty);

							m2x = x + 2.0/3.0 * (mx - x);
							m2y = y + 2.0/3.0 * (my - y);

							o = OpCurve_new();

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m1x);
							OpCurve_set_m1x((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m1y);
							OpCurve_set_m1y((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m2x);
							OpCurve_set_m2x((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, m2y);
							OpCurve_set_m2y((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, x);
							OpCurve_set_x((OpCurve*)o, o2);

							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, y);
							OpCurve_set_y((OpCurve*)o, o2);

							OpBloc_append_Op(ret, o);
							log_trace("Append Curve Path %p to bloc %p\n", ret, o);

							last_command_x = mx;
							last_command_y = my;

							lastx = x;
							lasty = y;
							break;
						}
			case 'a' :
			case 'A' :	{
							double rx, ry, rotation, large_arc, sweep, x, y;
							p+=2;

							rx = strtod(p, &p);
							p++;
							ry = strtod(p, &p);

							p++;
							rotation = strtod(p, &p);

							p++;
							large_arc = strtod(p, &p);

							p++;
							sweep = strtod(p, &p);

							p++;
							x = strtod(p, &p);
							p++;
							y = strtod(p, &p);


							if(c == 'a')
							{
								x += lastx;
								y += lasty;
							}

							rx = fabs(rx);
							ry = fabs(ry);

							double phi, cos_phi, sin_phi;//Angles of elipse

							phi = rotation * M_PI / 180.0;

							cos_phi = cos(phi);
							sin_phi = sin(phi);

							double dx = (lastx - x) / 2.0;
							double dy = (lasty - y) / 2.0;

							double xp =  cos_phi * dx + sin_phi * dy;
							double yp = -sin_phi * dx + cos_phi * dy;

							double lambda =
							    (xp * xp) / (rx * rx) +
							    (yp * yp) / (ry * ry);

							if (lambda > 1.0) {
							    double scale = sqrt(lambda);
							    rx *= scale;
							    ry *= scale;
							}

							double numerator =
							    rx*rx * ry*ry
							    - rx*rx * yp*yp
							    - ry*ry * xp*xp;

							double denominator =
							    rx*rx * yp*yp
							    + ry*ry * xp*xp;

							double coef = sqrt(fmax(0.0, numerator / denominator));

							if (large_arc == sweep)
							    coef = -coef;

							double cxp = coef * (rx * yp / ry);
							double cyp = coef * (-ry * xp / rx);

							double cx =
							    cos_phi * cxp
							    - sin_phi * cyp
							    + (lastx + x) / 2.0;

							double cy =
							    sin_phi * cxp
							    + cos_phi * cyp
							    + (lasty +y) / 2.0;

							double ux = (xp - cxp) / rx;
							double uy = (yp - cyp) / ry;

							double angle1 = atan2(uy, ux);

							double vx = (-xp - cxp) / rx;
							double vy = (-yp - cyp) / ry;

							double angle2 = atan2(
							    ux * vy - uy * vx,
							    ux * vx + uy * vy
							);

							if (!sweep && angle2 > 0)
							    angle2 -= 2 * M_PI;

							if (sweep && angle2 < 0)
							    angle2 += 2 * M_PI;

							o = OpSave_new();
							OpBloc_append_Op(ret, o);

							o = OpTranslate_new();
							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, cx);
							OpTranslate_set_x((OpTranslate*)o, o2);
							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, cy);
							OpTranslate_set_y((OpTranslate*)o, o2);
							OpBloc_append_Op(ret, o);

							o = OpRotate_new();
							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, phi);
							OpRotate_set_angle((OpRotate*)o, o2);
							OpBloc_append_Op(ret, o);

							o = OpScale_new();
							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, rx);
							OpScale_set_x((OpScale*)o, o2);
							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, ry);
							OpScale_set_y((OpScale*)o, o2);
							OpBloc_append_Op(ret, o);

							o = OpCircle_new();
							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, 0);
							OpCircle_set_x((OpCircle*)o, o2);
							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, 0);
							OpCircle_set_y((OpCircle*)o, o2);
							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, 1);
							OpCircle_set_r((OpCircle*)o, o2);
							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, angle1);
							OpCircle_set_a1((OpCircle*)o, o2);
							o2 = OpGetValue_new();
							OpGetValue_set_value((OpGetValue*)o2, angle1 + angle2);
							OpCircle_set_a2((OpCircle*)o, o2);
							OpBloc_append_Op(ret, o);

							o = OpRestore_new();
							OpBloc_append_Op(ret, o);

							lastx = x;
							lasty = y;
							break;
						}
			case 'z' :
			case 'Z' :	{
							p+=1;
							o = OpClosePath_new();
							OpBloc_append_Op(ret, o);
							log_trace("Append Close Path %p to bloc %p\n", ret, o);
							break;
						}
			default :	do
							p++;
						while(*p != '\0' && !((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z')));

			}
			while(*p != '\0' && *p == ' ')//Go forward to reach next ' ' or next letter.
				p++;
		}

		xmlFree(attr);
	}
}

void OpParserFile_init(OpParserFile *self)
{
	self->state = NULL;
	self->file_content = NULL;
}

OpParserFile *OpParserFile_new(void)
{
	OpParserFile *self = malloc(sizeof(OpParserFile));
	if(self != NULL)
		OpParserFile_init(self);
	return self;
}

void OpParserFile_terminate(OpParserFile *self)
{
	if(self->state != NULL)
		yy_delete_buffer(self->state);
	if(self->file_content != NULL)
		String_free(self->file_content);
}

void OpParserFile_free(OpParserFile *self)
{
	OpParserFile_terminate(self);
	free(self);
}

void OpImport_init(OpImport *self)
{
	self->root = NULL;
	self->parser = NULL;
}

OpImport *OpImport_new(void)
{
	OpImport *self = malloc(sizeof(OpImport));
	if(self != NULL)
		OpImport_init(self);
	return self;
}

void OpImport_set_root(OpImport *self, Op *root)
{
	if(self->root != NULL)
		Op_free(self->root);
	self->root = root;
}

void OpImport_set_parser(OpImport *self, Op *p)
{
	if(self->parser != NULL)
		Op_free(self->parser);
	self->parser = p;
}

int OpImport_fix_operandes(OpImport *self, OpContext *ctx)
{
	if(self->root != NULL)
		return Op_fix_operandes(self->root, ctx);
	return 0;
}

int OpImport_run(OpImport *self, OpContext *ctx)
{
	int ret = 0;

	if(self->root == NULL)
	{
		ret = OpImport_parse(self, ctx);
	}
	if(ret == 0)
		return Op_execute(self->root, ctx);
	return ret;
}

int OpImport_parse(OpImport *self, OpContext *ctx)
{
	if(self->parser != NULL)
	{
		if(Op_fix_operandes(self->parser, ctx) == -1)
			return -1;
		return Op_execute(self->parser, ctx);
	}
	return 0;
}

void OpImport_terminate(OpImport *self)
{
	OpImport_set_root(self, NULL);
	OpImport_set_parser(self, NULL);
}

void OpImport_free(OpImport *self)
{
	OpImport_terminate(self);
	free(self);
}

OpIsa OpParseImport_isa = {
		.name="ParseImport",
		.size=sizeof(OpParseImport),
		.init = (void(*)(Op*))OpParseImport_init,
		.terminate = (void(*)(Op*))OpParseImport_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpParseImport_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpParseImport_execute,
		.check_args = (int(*)(Op*, OpContext*))OpParseImport_check_args
};

void OpParseImport_init(OpParseImport *self)
{
	Op_init(&self->super);
	String_init(&self->prefix);
	self->name = NULL;
}

void OpParseImport_terminate(OpParseImport *self)
{
	Op_terminate(&self->super);
	String_finalize(&self->prefix);
	_OpParseImport_set_name(self, NULL);
}

void OpParseImport_set_prefix(OpParseImport *self, String *prefix)
{
	String_cpy(&self->prefix, prefix);
}

void OpParseImport_set_program(OpParseImport *self, OpProgram *p)
{
	self->program = p;
}

#define OPPARSEIMPORT_NAME 0

void OpParseImport_set_name(OpParseImport *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OPPARSEIMPORT_NAME);
}

void _OpParseImport_set_name(OpParseImport *self, Op *op)
{
	OP_SET_OPERANDE(self, name, op);
}

int OpParseImport_fix_operandes(OpParseImport *self, OpContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >= 1)
	{
		ret = 0;
		_OpParseImport_set_name(self, self->super.operandes[OPPARSEIMPORT_NAME]);
	}
	return ret;
}

int OpParseImport_check_args(OpParseImport *self, OpContext *ctx)
{
	if(self->name == NULL)
		return -1;
	return 0;
}

int OpParseImport_execute(OpParseImport *self, OpContext *ctx)
{
	int ret = 0;
	const char *v;
	ret = Op_execute_get_string(self->name, (Op*)self, ctx, &v);
	if(ret == 0)
	{
		size_t num;
		OpImport *import = OpImport_new();
		num = OpProgram_add_import(self->program, import);
		ret = OpParseImport_import_svg(self, import, ctx, v);
		if(ret < 0)
		{
			String err_msg;
			String_init(&err_msg);
			String_append_printf(&err_msg, "Error during import of '%s'", v);
			OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&err_msg));
			String_finalize(&err_msg);
		}
		else
		{
			OpContext_set_current_value_double(ctx, (double)num);
		}
	}
	return ret;
}

Op *OpParseImport_new(void)
{
	return Op_new(&OpParseImport_isa);
}

OpIsa OpDrawImport_isa = {
		.name="DrawImport",
		.size=sizeof(OpDrawImport),
		.init = (void(*)(Op*))OpDrawImport_init,
		.terminate = (void(*)(Op*))OpDrawImport_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpDrawImport_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpDrawImport_execute,
		.check_args = (int(*)(Op*, OpContext*))OpDrawImport_check_args
};

void OpDrawImport_init(OpDrawImport *self)
{
	Op_init(&self->super);
	self->number = NULL;
}

void OpDrawImport_terminate(OpDrawImport *self)
{
	Op_terminate(&self->super);
	_OpDrawImport_set_number(self, NULL);
}

void OpDrawImport_set_program(OpDrawImport *self, OpProgram *p)
{
	self->program = p;
}

#define OPDRAWIMPORT_NUMBER 0

void OpDrawImport_set_number(OpDrawImport *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OPDRAWIMPORT_NUMBER);
}

void _OpDrawImport_set_number(OpDrawImport *self, Op *op)
{
	OP_SET_OPERANDE(self, number, op);
}

int OpDrawImport_fix_operandes(OpDrawImport *self, OpContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >= 1)
	{
		ret = 0;
		_OpDrawImport_set_number(self, self->super.operandes[OPDRAWIMPORT_NUMBER]);
	}
	return ret;
}

int OpDrawImport_check_args(OpDrawImport *self, OpContext *ctx)
{
	if(self->number == NULL)
		return -1;
	return 0;
}

int OpDrawImport_execute(OpDrawImport *self, OpContext *ctx)
{
	int ret = 0;
	double v;
	ret = Op_execute_get_double(self->number, (Op*)self, ctx, &v);
	if(ret == 0)
	{
		OpImport *import = OpProgram_get_import(self->program, (size_t)v);
		if(import != NULL)
			ret = OpImport_run(import, ctx);
		else
		{
			ret = -1;
			String msg;
			String_init(&msg);
			String_append_printf(&msg, "Import number %zu does not exist", (size_t)v);
			OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&msg));
			String_finalize(&msg);
		}
	}
	return ret;
}

Op *OpDrawImport_new(void)
{
	return Op_new(&OpDrawImport_isa);
}

OpIsa OpGetImportExtents_isa = {
		.name="GetImportExtents",
		.size=sizeof(OpGetImportExtents),
		.init = (void(*)(Op*))OpGetImportExtents_init,
		.terminate = (void(*)(Op*))OpGetImportExtents_terminate,
		.fix_operandes = (int(*)(Op*, OpContext*))OpGetImportExtents_fix_operandes,
		.execute = (int(*)(Op*, OpContext*))OpGetImportExtents_execute,
		.check_args = (int(*)(Op*, OpContext*))OpGetImportExtents_check_args
};

void OpGetImportExtents_init(OpGetImportExtents *self)
{
	Op_init(&self->super);
	self->number = NULL;
}

void OpGetImportExtents_terminate(OpGetImportExtents *self)
{
	Op_terminate(&self->super);
	_OpGetImportExtents_set_number(self, NULL);
}

void OpGetImportExtents_set_program(OpGetImportExtents *self, OpProgram *p)
{
	self->program = p;
}

#define OPGETIMPORTSEXTENTS_NUMBER 0

void OpGetImportExtents_set_number(OpGetImportExtents *self, Op *op)
{
	OP_ADD_OPERANDE(self, op, OPGETIMPORTSEXTENTS_NUMBER);
}

void _OpGetImportExtents_set_number(OpGetImportExtents *self, Op *op)
{
	OP_SET_OPERANDE(self, number, op);
}

int OpGetImportExtents_fix_operandes(OpGetImportExtents *self, OpContext *ctx)
{
	int ret = -1;
	if(self->super.nb_ops >= 1)
	{
		ret = 0;
		_OpGetImportExtents_set_number(self, self->super.operandes[OPGETIMPORTSEXTENTS_NUMBER]);
	}
	return ret;
}

int OpGetImportExtents_check_args(OpGetImportExtents *self, OpContext *ctx)
{
	if(self->number == NULL)
		return -1;
	return 0;
}

int OpGetImportExtents_execute(OpGetImportExtents *self, OpContext *ctx)
{
	int ret = 0;
	double v;
	ret = Op_execute_get_double(self->number, (Op*)self, ctx, &v);
	if(ret == 0)
	{
		OpImport *import = OpProgram_get_import(self->program, (size_t)v);
		if(import != NULL)
		{
			OpVariable ex;
			OpVariable_init(&ex);
			OpVariable_append_double(&ex, (double)import->x);
			OpVariable_append_double(&ex, (double)import->y);
			OpVariable_append_double(&ex, (double)import->w);
			OpVariable_append_double(&ex, (double)import->h);
			OpContext_copy_variable_to_current_value(ctx, &ex);
			OpVariable_terminate(&ex);
		}
		else
		{
			ret = -1;
			String msg;
			String_init(&msg);
			String_append_printf(&msg, "Import number %zu does not exist", (size_t)v);
			OpContext_set_running_state(ctx, (Op*)self, Error, String_get_char_string(&msg));
			String_finalize(&msg);
		}
	}
	return ret;
}

Op *OpGetImportExtents_new(void)
{
	return Op_new(&OpGetImportExtents_isa);
}
