#include "messagebox.h"

#ifdef __linux__

#include <gtk/gtk.h>

namespace TankGame
{
	void ShowErrorMessage(const std::string& message, const std::string& title)
	{
		if (!gtk_init_check(0, nullptr))
			return;
		
		GtkWidget* dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
		                                           GTK_BUTTONS_OK, "%s", message.c_str());
		
		gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());
		gtk_dialog_run(GTK_DIALOG(dialog));
		
		gtk_widget_destroy(GTK_WIDGET(dialog));
		while (g_main_context_iteration(nullptr, false));
	}
}

#endif
