#include "messagebox.h"

#ifdef __linux__

#include <iostream>
#include <gtk/gtk.h>
#include <dlfcn.h>

namespace TankGame
{
	void* gtkLibrary;
	bool gtkLoaded = false;
	
#define GTK_FUNC(name) decltype(&::name) name;
#include "gtkfunctions.inl"
#undef GTK_FUNC
	
	void ShowErrorMessage(const std::string& message, const std::string& title)
	{
		if (!gtkLoaded)
		{
			gtkLibrary = dlopen("libgtk-3.so", RTLD_LAZY);
			gtkLoaded = true;
			
			if (gtkLibrary != nullptr)
			{
#define GTK_FUNC(name) name = reinterpret_cast<decltype(name)>(dlsym(gtkLibrary, #name));
#include "gtkfunctions.inl"
#undef GTK_FUNC
				
				gtk_init(nullptr, nullptr);
			}
		}
		
		if (gtkLibrary == nullptr)
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
