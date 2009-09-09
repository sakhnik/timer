#include "digit.h"
#include <stdio.h>
#include <gtk/gtk.h>

int main(int argc, char* argv[])
{
    GtkWidget *window;
    GtkWidget *hbox;
    GtkWidget *digit1, *digit2, *digit3;

    gtk_init (&argc, &argv);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW(window), "Timer");
    gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect (window, "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);

    hbox = gtk_hbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER(window), hbox);

    digit1 = sam_digit_new ();
    sam_digit_set_bits (SAM_DIGIT(digit1), 15);
    gtk_box_pack_start (GTK_BOX(hbox), digit1,
                        TRUE, TRUE, 0);
    digit2 = sam_digit_new ();
    sam_digit_set_bits (SAM_DIGIT(digit2), 127);
    gtk_box_pack_start (GTK_BOX(hbox), digit2,
                        TRUE, TRUE, 0);

    digit3 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX(hbox), digit3,
                        TRUE, TRUE, 0);

    gtk_widget_show_all (window);

    gtk_main ();
    return 0;
}
