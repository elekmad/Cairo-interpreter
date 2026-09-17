#pragma once
#include <QString>
#include <QFileInfo>



extern "C"
{


#include <stdlib.h>
#include <stdio.h>
#include <CanvaCtx.h>
#include <OpCanva.h>
#include <OpProgram.h>
#include <parser.tab.h>
#include <lexer.h>
#include <unistd.h>


int cairo_parse(String *buffer, String *prefix, String *out, String *msgs, int *is_xml, int *width, int *height)
{
	CanvaCtx Canva;
	size_t readed;
	OpCanvaContext Ctx;
	OpProgram Prog;
	OpCanvaContext_init(&Ctx);
	OpProgram_init(&Prog);
	OpParser Parser;
	OpParser_init(&Parser);
	OpParser_set_filename_prefix(&Parser, prefix);
	OpProgram_set_context(&Prog, (OpContext*)&Ctx);
	OpParser_set_program(&Parser, &Prog);
	OpParser_set_current_context(&Parser, (OpContext*)&Ctx);
	Op *root = NULL;
	/*extern int yydebug, yy_flex_debug;
	yydebug = 1;
	yy_flex_debug = 1;*/
	fprintf(stderr, "Code String : %s\n", String_get_char_string(buffer));

	if(OpParser_parse(&Parser, buffer, &root) == 0)
	{
		int w, h;

		OpProgram_set_root(&Prog, root);
		OpProgram_fix_operandes(&Prog);
		OpProgram_prerun(&Prog);


		w = OpCanvaContext_get_width(&Ctx);
		h = OpCanvaContext_get_height(&Ctx);
		*width = (w != 0 ? w : 640);
		*height = (h != 0 ? h : 480);

		CanvaCtxOutputMode output_mode = OpCanvaContext_get_output_mode(&Ctx);

		if(output_mode == PNG)
		{
			CanvaCtx_init_for_png(&Canva, *width, *height);

			CanvaCtx_set_line_width (&Canva, 10.0);
			CanvaCtx_set_color(&Canva, 0, 0, 0, 255);

			OpCanvaContext_set_Canva(&Ctx, &Canva);
			OpProgram_set_Canva(&Prog, &Canva);

			OpProgram_run(&Prog);

			CanvaCtx_write_to_png(&Canva);
			OpProgram_export_to_xml(&Prog, msgs, false);
			CanvaCtx_fill_buffer_with_output(&Canva, out);
			*is_xml = 0;

			CanvaCtx_terminate(&Canva);
		}

		if(output_mode == SVG)
		{
			CanvaCtx_init_for_svg(&Canva, *width, *height);

			CanvaCtx_set_line_width (&Canva, 10.0);
			CanvaCtx_set_color(&Canva, 0, 0, 0, 255);

			OpCanvaContext_set_Canva(&Ctx, &Canva);
			OpProgram_set_Canva(&Prog, &Canva);

			OpProgram_run(&Prog);

			CanvaCtx_finish(&Canva);
			OpProgram_export_to_xml(&Prog, msgs, false);
			CanvaCtx_fill_buffer_with_output(&Canva, out);

			*is_xml = 1;
			CanvaCtx_terminate(&Canva);
		}
	}
	else
		OpCanvaContext_export_messages_to_xml(&Ctx, msgs, false);

	OpCanvaContext_terminate(&Ctx);
	OpProgram_terminate(&Prog);
	OpParser_terminate(&Parser);
	Op_free(root);
	return 0;
}
}

// Méthode simulée qui prend le code DSL et retourne le XML
int processlib(const QString& code, const QString &filepath, QString& msgs, QByteArray &output, bool &is_xml, int *width, int *height) {

	QByteArray ba = code.toUtf8();

	const char *c_str = ba.constData(); // ou ba.data()
	size_t length = static_cast<size_t>(ba.size());
	String *scode = String_new();
	String_append_data(scode, length, (const void*)c_str);
	String path;
	String_init(&path);
	QString dirNameAbs = QFileInfo(filepath).absolutePath();
	QByteArray bap = dirNameAbs.toUtf8();

	const char *p_str = bap.constData(); // ou ba.data()
	size_t p_length = static_cast<size_t>(bap.size());
	String_append_data(&path, p_length, (const void*)p_str);

	String out, Smsgs;
	// Remplace ce stub par l'appel effectif à ton moteur/bibliothèque.
	// Exemple de réponse XML retournée par la lib :
	String_init(&out);
	String_init(&Smsgs);
	qDebug() << "Code : " << code;
	qDebug() << "Code Array : " << ba;
	int is_xml_int;
	int ret = cairo_parse(scode, &path, &out, &Smsgs, &is_xml_int, width, height);

	if(ret == 0)
	{
		is_xml_int == 1 ? is_xml = true : is_xml = false;

		output = QByteArray(String_get_data(&out), static_cast<qsizetype>(String_get_length(&out)));
		msgs = QString::fromUtf8(String_get_char_string(&Smsgs), static_cast<qsizetype>(String_get_length(&Smsgs)));
	}
	String_finalize(&out);
	String_finalize(&Smsgs);
	String_finalize(&path);

	return ret;

}
