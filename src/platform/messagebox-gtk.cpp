#include "messagebox.h"

#ifdef __linux__

#include <iostream>
#include <gtk/gtk.h>
#include <dlfcn.h>

namespace TankGame
{
	bool gtkLoadAttempted = false;
	
	decltype(&::gtk_message_dialog_new) gtk_message_dialog_new;
	decltype(&::gtk_window_set_title)   gtk_window_set_title;
	decltype(&::gtk_dialog_run)         gtk_dialog_run;
	decltype(&::gtk_widget_destroy)     gtk_widget_destroy;
	
	//glib-2.0
	decltype(&::g_main_context_iteration) g_main_context_iteration;
	
	void ShowErrorMessage(const std::string& message, const std::string& title)
	{
		if (!gtkLoadAttempted)
		{
			void* gtkLibrary = dlopen("libgtk-3.so", RTLD_LAZY);
			void* glibLibrary = dlopen("libglib-2.0.so", RTLD_LAZY);
			
			if (gtkLibrary != nullptr && glibLibrary != nullptr)
			{
				auto gtk_init = reinterpret_cast<decltype(&::gtk_init)>(dlsym(gtkLibrary, "gtk_init"));
				
				TankGame::gtk_message_dialog_new =
					reinterpret_cast<decltype(gtk_message_dialog_new)>(dlsym(gtkLibrary, "gtk_message_dialog_new"));
				TankGame::gtk_window_set_title =
					reinterpret_cast<decltype(gtk_window_set_title)>(dlsym(gtkLibrary, "gtk_window_set_title"));
				TankGame::gtk_dialog_run =
					reinterpret_cast<decltype(gtk_dialog_run)>(dlsym(gtkLibrary, "gtk_dialog_run"));
				TankGame::gtk_widget_destroy =
					reinterpret_cast<decltype(gtk_widget_destroy)>(dlsym(gtkLibrary, "gtk_widget_destroy"));
				
				TankGame::g_main_context_iteration =
					reinterpret_cast<decltype(g_main_context_iteration)>(dlsym(glibLibrary, "g_main_context_iteration"));
				
				gtk_init(nullptr, nullptr);
			}
			
			gtkLoadAttempted = true;
		}
		
		if (gtk_message_dialog_new == nullptr)
		{
			std::cerr << title << ": " << message << "\n";
			return;
		}
		
		GtkWidget* dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
		                                           GTK_BUTTONS_OK, "%s", message.c_str());
		
		gtk_window_set_title(reinterpret_cast<GtkWindow*>(dialog), title.c_str());
		gtk_dialog_run(reinterpret_cast<GtkDialog*>(dialog));
		
		gtk_widget_destroy(dialog);
		while (g_main_context_iteration(nullptr, false)) { }
	}
}

#endif
