#include "process.h"
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *TITLE = "VNTyper";

void f(GtkWidget *combo_box){
	proc::config["CURRENT"] = std::string(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT (combo_box)));
}

int to_int(const char * c){
	int r = 0;
	for (int i=0; c[i]!='\0'; i++){
		r = r*10 + c[i]-48;
	}
	return r;
}

int main(int argc, char *argv[]){
	if (argc<=1 || strcmp(argv[1], "AREZ34ZX")!=0) {
		char *res = new char[100];
		sprintf(res, "%s AREZ34ZX | xclip -selection clipboard", argv[0]);
		system(res);
		system("xclip -selection clipboard -o | xclip -selection primary");
		delete[] res;
		return 0;
	}
	proc::init ();

	gtk_init (&argc, &argv);
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *text_view;
	GtkWidget *scrolledwindow;
	GtkWidget *combo_box;
	GtkTextBuffer *buffer;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), TITLE);
	gtk_window_set_default_size (GTK_WINDOW (window), to_int (proc::config["WIDTH"].data()), to_int (proc::config["HEIGHT"].data()));

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	
	text_view = gtk_text_view_new ();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW (text_view), GtkWrapMode (2));
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));

	combo_box = gtk_combo_box_text_new ();
	for (auto i = proc::method_name.begin(); i!=proc::method_name.end(); i++) {
		gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (combo_box), i->data(), i->data());
	}
	gtk_combo_box_set_active_id (GTK_COMBO_BOX (combo_box), proc::config["CURRENT"].data());

	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (proc::destroy), buffer);
	g_signal_connect (GTK_COMBO_BOX (combo_box), "changed", G_CALLBACK(f), combo_box);
	g_signal_connect (GTK_TEXT_VIEW (text_view), "key-press-event", G_CALLBACK (proc::keypress), buffer);

	gtk_container_add (GTK_CONTAINER (window), vbox);
	gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, 1, 1, 0);
	gtk_box_pack_start (GTK_BOX (vbox), combo_box, 0, 1, 0);
	gtk_container_add(GTK_CONTAINER (scrolledwindow), text_view);
	gtk_widget_show_all (window);
	gtk_main ();

	return 0; 
}

