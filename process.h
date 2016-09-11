#ifndef _process_h
#define _process_h

#include <string>
#include <map>
#include <vector>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

namespace proc{
	typedef std::map<std::string, guint> _t_method;

	extern std::vector<std::string> method_name;
	extern std::map<std::string, _t_method> method;
	extern std::map<std::string, std::string> config;
	extern std::map<std::string, std::string> data;
	extern std::map<std::string, std::string> db0, db1;

	int init ();
	gboolean keypress (GtkWidget *, GdkEventKey *, GtkTextBuffer *);
	int print_and_close (GtkTextBuffer *);
	void destroy ();
} 

#endif
