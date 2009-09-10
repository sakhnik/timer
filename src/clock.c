#include "clock.h"
#include "digit.h"

struct _SamClockPrivate
{
    // Minutes
    GtkWidget *m3;
    GtkWidget *m2;
    GtkWidget *m1;
    // Seconds
    GtkWidget *s2;
    GtkWidget *s1;
    // Buttons
    GtkWidget *beep;
    GtkWidget *start;
    GtkWidget *minpp;
    GtkWidget *minmm;
    GtkWidget *secpp;
    GtkWidget *secmm;

    int secs;
    guint timer;
};

G_DEFINE_TYPE (SamClock, sam_clock, GTK_TYPE_TABLE)

#define SAM_CLOCK_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), SAM_TYPE_CLOCK, SamClockPrivate))

static void
sam_clock_dispose (GObject *obj)
{
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (obj);
    if (priv->timer)
        g_source_remove (priv->timer);
}

static void
sam_clock_class_init (SamClockClass *class)
{
    g_type_class_add_private (G_OBJECT_CLASS(class), sizeof(SamClockPrivate));
    G_OBJECT_CLASS(class)->dispose = sam_clock_dispose;
}

#define DIGIT_LEG(i) (1 << (i))

static int
sam_clock_digit_to_bits(int digit)
{
    switch (digit)
    {
    case 0:
        return DIGIT_LEG(0) | DIGIT_LEG(1) | DIGIT_LEG(2) | DIGIT_LEG(4) |
               DIGIT_LEG(5) | DIGIT_LEG(6);
    case 1:
        return DIGIT_LEG(2) | DIGIT_LEG(5);
    case 2:
        return DIGIT_LEG(0) | DIGIT_LEG(2) | DIGIT_LEG(3) | DIGIT_LEG(4) |
               DIGIT_LEG(6);
    case 3:
        return DIGIT_LEG(0) | DIGIT_LEG(2) | DIGIT_LEG(3) | DIGIT_LEG(5) |
               DIGIT_LEG(6);
    case 4:
        return DIGIT_LEG(1) | DIGIT_LEG(2) | DIGIT_LEG(3) | DIGIT_LEG(5);
    case 5:
        return DIGIT_LEG(0) | DIGIT_LEG(1) | DIGIT_LEG(3) | DIGIT_LEG(5) |
               DIGIT_LEG(6);
    case 6:
        return DIGIT_LEG(1) | DIGIT_LEG(3) | DIGIT_LEG(4) | DIGIT_LEG(5) |
               DIGIT_LEG(6);
    case 7:
        return DIGIT_LEG(0) | DIGIT_LEG(2) | DIGIT_LEG(5);
    case 8:
        return DIGIT_LEG(0) | DIGIT_LEG(1) | DIGIT_LEG(2) | DIGIT_LEG(3) |
               DIGIT_LEG(4) | DIGIT_LEG(5) | DIGIT_LEG(6);
    case 9:
        return DIGIT_LEG(0) | DIGIT_LEG(1) | DIGIT_LEG(2) | DIGIT_LEG(3) |
               DIGIT_LEG(5);
    default:
        return 0;
    }
}

static void* sam_clock_do_beep0 (void* arg)
{
    system ("paplay beep0.wav");
    return NULL;
}

static void* sam_clock_do_beep1 (void* arg)
{
    system ("paplay beep1.wav");
    return NULL;
}

static void
sam_clock_check_beep (SamClock *clock)
{
    int secs = SAM_CLOCK_GET_PRIVATE (clock)->secs % 60;
    if (!secs)
    {
        pthread_t pt;
        pthread_create (&pt, NULL,
                        &sam_clock_do_beep0, NULL);
        return;
    }
    if (secs >= 55)
    {
        pthread_t pt;
        pthread_create (&pt, NULL,
                        &sam_clock_do_beep1, NULL);
        return;
    }
}

static int
sam_clock_add_second (SamClock *clock, int dt)
{
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    const int MAX_SEC = 1000 * 60;
    priv->secs += dt;
    if (priv->secs < 0)
        priv->secs += MAX_SEC;
    if (priv->secs >= MAX_SEC)
        priv->secs -= MAX_SEC;
    return priv->secs;
}

static void
update_digits (SamClock *clock)
{
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    gint secs = priv->secs % 60;
    gint mins = priv->secs / 60;
    sam_digit_set_bits (SAM_DIGIT(priv->s1),
                        sam_clock_digit_to_bits(secs % 10));
    sam_digit_set_bits (SAM_DIGIT(priv->s2),
                        sam_clock_digit_to_bits(secs / 10));
    sam_digit_set_bits (SAM_DIGIT(priv->m1),
                        sam_clock_digit_to_bits(mins % 10));
    sam_digit_set_bits (SAM_DIGIT(priv->m2),
                        sam_clock_digit_to_bits(mins / 10 % 10));
    sam_digit_set_bits (SAM_DIGIT(priv->m3),
                        sam_clock_digit_to_bits(mins / 100));

    gtk_widget_queue_draw (GTK_WIDGET (clock));
}

static gboolean
sam_clock_on_timer (gpointer data)
{
    int secs, mins;
    SamClock *clock = (SamClock *) data;
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    sam_clock_add_second (clock, 1);

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(priv->beep)))
        sam_clock_check_beep (clock);

    update_digits (clock);

    return TRUE;
}

static void
on_start_clicked (GtkToggleButton *start, gpointer data)
{
    SamClock *clock = (SamClock *) data;
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    gboolean started = gtk_toggle_button_get_active (start);
    gtk_widget_set_sensitive (GTK_WIDGET (priv->minpp), !started);
    gtk_widget_set_sensitive (GTK_WIDGET (priv->minmm), !started);
    gtk_widget_set_sensitive (GTK_WIDGET (priv->secpp), !started);
    gtk_widget_set_sensitive (GTK_WIDGET (priv->secmm), !started);
    if (priv->timer)
        g_source_remove (priv->timer);
    if (started)
        priv->timer = g_timeout_add (1000, sam_clock_on_timer, clock);
}

static void
on_min_clicked (GtkButton *min, gpointer data)
{
    SamClock *clock = (SamClock *) data;
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    int dt = 60;
    if (GTK_WIDGET(min) == priv->minmm)
        dt = -dt;
    sam_clock_add_second (clock, dt);
    update_digits (clock);
}

static void
on_sec_clicked (GtkButton *sec, gpointer data)
{
    SamClock *clock = (SamClock *) data;
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    int dt = 1;
    if (GTK_WIDGET(sec) == priv->secmm)
        dt = -dt;
    sam_clock_add_second (clock, dt);
    update_digits (clock);
}

static void
sam_clock_init (SamClock *clock)
{
    GtkWidget *sep;
    GtkWidget *tmp;
    SamClockPrivate* priv = SAM_CLOCK_GET_PRIVATE (clock);

    gtk_table_resize (GTK_TABLE (clock), 3, 2);

    // Minutes
    tmp = gtk_hbox_new (FALSE, 0);
    gtk_table_attach (GTK_TABLE (clock), tmp, 0, 1, 0, 1,
                      GTK_EXPAND, GTK_EXPAND, 0, 0);

    priv->m3 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX(tmp), priv->m3, TRUE, TRUE, 0);
    priv->m2 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX(tmp), priv->m2, TRUE, TRUE, 0);
    priv->m1 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX(tmp), priv->m1, TRUE, TRUE, 0);

    sep = gtk_vseparator_new ();
    gtk_box_pack_start (GTK_BOX(tmp), sep, TRUE, TRUE, 0);

    // Seconds
    tmp = gtk_hbox_new (FALSE, 0);
    gtk_table_attach (GTK_TABLE (clock), tmp, 1, 2, 0, 1,
                      GTK_EXPAND, GTK_EXPAND, 0, 0);
    priv->s2 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX(tmp), priv->s2, TRUE, TRUE, 0);
    priv->s1 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX(tmp), priv->s1, TRUE, TRUE, 0);

    // Beep + restart
    tmp = gtk_vbox_new (FALSE, 0);
    gtk_table_attach (GTK_TABLE (clock), tmp, 2, 3, 0, 1,
                      GTK_EXPAND, GTK_EXPAND, 0, 0);
    priv->beep = gtk_toggle_button_new_with_label ("Beep");
    gtk_box_pack_start (GTK_BOX(tmp), priv->beep, TRUE, TRUE, 0);
    priv->start = gtk_toggle_button_new_with_label ("Start");
    gtk_box_pack_start (GTK_BOX(tmp), priv->start, TRUE, TRUE, 0);
    g_signal_connect (priv->start, "toggled",
                      G_CALLBACK(&on_start_clicked), clock);

    // Adjust minutes
    tmp = gtk_vbox_new (FALSE, 0);
    gtk_table_attach (GTK_TABLE (clock), tmp, 0, 1, 1, 2,
                      GTK_EXPAND, GTK_EXPAND, 0, 0);
    priv->minpp = gtk_button_new_with_label ("++MIN");
    gtk_box_pack_start (GTK_BOX(tmp), priv->minpp, TRUE, TRUE, 0);
    g_signal_connect (priv->minpp, "clicked",
                      G_CALLBACK(&on_min_clicked), clock);
    priv->minmm = gtk_button_new_with_label ("--MIN");
    gtk_box_pack_start (GTK_BOX(tmp), priv->minmm, TRUE, TRUE, 0);
    g_signal_connect (priv->minmm, "clicked",
                      G_CALLBACK(&on_min_clicked), clock);

    // Adjust seconds
    tmp = gtk_vbox_new (FALSE, 0);
    gtk_table_attach (GTK_TABLE (clock), tmp, 1, 2, 1, 2,
                      GTK_EXPAND, GTK_EXPAND, 0, 0);
    priv->secpp = gtk_button_new_with_label ("++SEC");
    gtk_box_pack_start (GTK_BOX(tmp), priv->secpp, TRUE, TRUE, 0);
    g_signal_connect (priv->secpp, "clicked",
                      G_CALLBACK(&on_sec_clicked), clock);
    priv->secmm = gtk_button_new_with_label ("--SEC");
    gtk_box_pack_start (GTK_BOX(tmp), priv->secmm, TRUE, TRUE, 0);
    g_signal_connect (priv->secmm, "clicked",
                      G_CALLBACK(&on_sec_clicked), clock);

    priv->secs = 999*60;
}

GtkWidget*
sam_clock_new (void)
{
    return g_object_new (SAM_TYPE_CLOCK, NULL);
}
