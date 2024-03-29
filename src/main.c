#include "clock.h"
#include <stdio.h>
#include <gtk/gtk.h>

int main(int argc, char* argv[])
{
    GtkWidget *window;
    GtkWidget *clock;

    gtk_init (&argc, &argv);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW(window), "Timer");
    gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect (window, "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);

    clock = sam_clock_new ();
    gtk_container_add (GTK_CONTAINER(window), clock);

    gtk_widget_show_all (window);

    gtk_main ();

    return 0;
}
