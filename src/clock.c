#include "clock.h"
#include "number.h"
#include <stdlib.h>
#include <pthread.h>

typedef enum _SamClockMode
{
    SCM_NORMAL = 0,
    SCM_ADJUST_HR,
    SCM_ADJUST_MIN,
    SCM_ADJUST_SEC,

    SCM_LAST
} SamClockMode;

struct _SamClockPrivate
{
    // Numbers
    GtkWidget *hours;
    GtkWidget *minutes;
    GtkWidget *seconds;
    // Buttons
    GtkWidget *beep_button;
    GtkWidget *start_button;
    GtkWidget *mode_button;
    GtkWidget *plus_button;
    GtkWidget *minus_button;

    guint timer; // timer id
    SamClockMode mode; // Current operation mode
};

G_DEFINE_TYPE (SamClock, sam_clock, GTK_TYPE_HBOX)

#define SAM_CLOCK_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), SAM_TYPE_CLOCK, SamClockPrivate))

static void
sam_clock_dispose (GObject *obj)
{
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (obj);
    if (priv->timer)
        // Remove timer if any
        g_source_remove (priv->timer);
}

static void
sam_clock_class_init (SamClockClass *class)
{
    g_type_class_add_private (G_OBJECT_CLASS(class), sizeof(SamClockPrivate));
    G_OBJECT_CLASS(class)->dispose = sam_clock_dispose;
}

static void*
sam_clock_do_system (void* arg)
{
    int res = system ((char const *) arg);
    if (res);
    return NULL;
}

#ifdef WIN32
#  define PLAY "sndrec32 /play /close /embedding "
#else
#  define PLAY "aplay "
#endif

static void
sam_clock_check_beep (SamClock *clock)
{
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    guint secs = sam_number_get_value (SAM_NUMBER (priv->seconds));
    if (!secs)
    {
        pthread_t pt;
        pthread_create (&pt, NULL,
                        &sam_clock_do_system,
                        PLAY "beep0.wav");

        return;
    }
    if (secs >= 55)
    {
        pthread_t pt;
        pthread_create (&pt, NULL,
                        &sam_clock_do_system,
                        PLAY "beep1.wav");
        return;
    }
}

static gboolean
sam_clock_on_timer (gpointer data)
{
    SamClock *clock = (SamClock *) data;
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    guint remind = 0;

    guint tmp = sam_number_get_value (SAM_NUMBER (priv->seconds)) + 1;
    remind = tmp / 60;
    sam_number_set_value (SAM_NUMBER (priv->seconds), tmp % 60);
    if (remind)
    {
        tmp = sam_number_get_value (SAM_NUMBER (priv->minutes)) + remind;
        remind = tmp / 60;
        sam_number_set_value (SAM_NUMBER (priv->minutes), tmp % 60);
    }
    if (remind)
    {
        tmp = sam_number_get_value (SAM_NUMBER (priv->hours)) + remind;
        sam_number_set_value (SAM_NUMBER (priv->hours), tmp % 24);
    }

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(priv->beep_button)))
        sam_clock_check_beep (clock);

    return TRUE;
}

static GtkWidget*
get_active_number (SamClock *clock)
{
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    switch (priv->mode)
    {
    case SCM_ADJUST_HR: return priv->hours;
    case SCM_ADJUST_MIN: return priv->minutes;
    case SCM_ADJUST_SEC: return priv->seconds;
    default: return NULL;
    }
}

static void
select_mode (SamClock *clock, SamClockMode mode)
{
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    GtkWidget *active = get_active_number (clock);
    // Show previously selected number deliberately
    if (active)
        sam_number_set_blink (SAM_NUMBER (active), FALSE);
    // Set new mode
    if (mode >= SCM_LAST)
        mode = SCM_NORMAL;
    priv->mode = mode;

    // Show/hide the adjust button
    active = get_active_number (clock);
    gtk_widget_set_sensitive (priv->minus_button, !!active);
    gtk_widget_set_sensitive (priv->plus_button, !!active);
    if (active)
        sam_number_set_blink (SAM_NUMBER (active), TRUE);
}

static void
on_start_button_clicked (GtkToggleButton *start_button, gpointer data)
{
    SamClock *clock = (SamClock *) data;
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    gboolean started = gtk_toggle_button_get_active (start_button);

    select_mode (clock, SCM_NORMAL);

    gtk_widget_set_sensitive (GTK_WIDGET (priv->mode_button), !started);
    gtk_widget_set_sensitive (GTK_WIDGET (priv->minus_button), !started);
    gtk_widget_set_sensitive (GTK_WIDGET (priv->plus_button), !started);

    if (priv->timer)
        g_source_remove (priv->timer);
    if (started)
        priv->timer = g_timeout_add (1000, sam_clock_on_timer, clock);
}

static void
on_mode_button_clicked (GtkButton *min, gpointer data)
{
    SamClock *clock = (SamClock *) data;
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);

    select_mode (clock, priv->mode + 1);
}

static void
on_adjust_button_clicked (GtkButton *adjust, gpointer data)
{
    SamClock *clock = (SamClock *) data;
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    SamNumber *active = SAM_NUMBER (get_active_number (clock));
    gint delta = 1;
    if (GTK_WIDGET (adjust) == priv->minus_button)
        delta = -1;
    g_assert (active && "There must be active number");
    sam_number_cycle_values (active, delta);
    sam_number_set_blink (active, TRUE); // For nice visual effect
}

static void
sam_clock_init (SamClock *clock)
{
    GtkWidget *tmp;
    GtkWidget *vbox;
    SamClockPrivate* priv = SAM_CLOCK_GET_PRIVATE (clock);

    // Hours
    priv->hours = sam_number_new (24);
    gtk_box_pack_start (GTK_BOX (clock), priv->hours, TRUE, TRUE, 0);
    sam_number_set_value (SAM_NUMBER (priv->hours), 23);

    tmp = gtk_vseparator_new ();
    gtk_box_pack_start (GTK_BOX (clock), tmp, TRUE, TRUE, 0);

    // Minutes
    priv->minutes = sam_number_new (60);
    gtk_box_pack_start (GTK_BOX (clock), priv->minutes, TRUE, TRUE, 0);
    sam_number_set_value (SAM_NUMBER (priv->minutes), 59);

    tmp = gtk_vseparator_new ();
    gtk_box_pack_start (GTK_BOX (clock), tmp, TRUE, TRUE, 0);

    // Seconds
    priv->seconds = sam_number_new (60);
    gtk_box_pack_start (GTK_BOX (clock), priv->seconds, TRUE, TRUE, 0);
    sam_number_set_value (SAM_NUMBER (priv->seconds), 0);

    // Beep + restart + mode + adjust
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (clock), vbox, TRUE, TRUE, 0);

    priv->beep_button = gtk_toggle_button_new_with_label ("Beep");
    gtk_box_pack_start (GTK_BOX (vbox), priv->beep_button, TRUE, TRUE, 0);
    priv->start_button = gtk_toggle_button_new_with_label ("Start");
    gtk_box_pack_start (GTK_BOX (vbox), priv->start_button, TRUE, TRUE, 0);
    g_signal_connect (priv->start_button, "toggled",
                      G_CALLBACK (&on_start_button_clicked), clock);

    tmp = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (vbox), tmp, TRUE, TRUE, 0);

    // Mode
    priv->mode_button = gtk_button_new_with_label ("Mode");
    gtk_box_pack_start (GTK_BOX (vbox), priv->mode_button, TRUE, TRUE, 0);
    g_signal_connect (priv->mode_button, "clicked",
                      G_CALLBACK (&on_mode_button_clicked), clock);

    // Cycle number
    tmp = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), tmp, TRUE, TRUE, 0);

    priv->minus_button = gtk_button_new_with_label ("−");
    gtk_box_pack_start (GTK_BOX (tmp), priv->minus_button, TRUE, TRUE, 0);
    gtk_widget_set_sensitive (priv->minus_button, FALSE);
    g_signal_connect (priv->minus_button, "clicked",
                      G_CALLBACK (&on_adjust_button_clicked), clock);

    priv->plus_button = gtk_button_new_with_label ("+");
    gtk_box_pack_start (GTK_BOX (tmp), priv->plus_button, TRUE, TRUE, 0);
    gtk_widget_set_sensitive (priv->plus_button, FALSE);
    g_signal_connect (priv->plus_button, "clicked",
                      G_CALLBACK (&on_adjust_button_clicked), clock);

    priv->mode = SCM_NORMAL;
}

GtkWidget*
sam_clock_new (void)
{
    return g_object_new (SAM_TYPE_CLOCK, NULL);
}
