#include "clock.h"
#include "digit.h"

typedef enum _SamClockMode
{
    SCM_NORMAL = 0,
    SCM_ADJUST_M3,
    SCM_ADJUST_M2,
    SCM_ADJUST_M1,
    SCM_ADJUST_S2,
    SCM_ADJUST_S1,

    SCM_LAST
} SamClockMode;

const int MAX_SEC = 1000 * 60;

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
    GtkWidget *adjust;
    GtkWidget *cycle;

    int secs;    // seconds passed 0 <= secs <= MAX_SEC
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
    system ((char const *) arg);
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
                        &sam_clock_do_system, "paplay beep0.wav");
        return;
    }
    if (secs >= 55)
    {
        pthread_t pt;
        pthread_create (&pt, NULL,
                        &sam_clock_do_system, "paplay beep1.wav");
        return;
    }
}

static void
sam_clock_cycle_digit (SamClock *clock, int n)
{
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    guint mins = priv->secs / 60;
    guint secs = priv->secs % 60;
    char buf[24];
    snprintf(buf, sizeof(buf), "%03d%02d", mins, secs);
    if (++buf[n] > '9')
        buf[n] = '0';
    sscanf(buf, "%03d%02d", &mins, &secs);
    priv->secs = mins * 60 + secs;
}

static void
update_digits (SamClock *clock)
{
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    gint secs = priv->secs % 60;
    gint mins = priv->secs / 60;
    sam_digit_set_digit (SAM_DIGIT(priv->s1), secs % 10);
    sam_digit_set_digit (SAM_DIGIT(priv->s2), secs / 10);
    sam_digit_set_digit (SAM_DIGIT(priv->m1), mins % 10);
    sam_digit_set_digit (SAM_DIGIT(priv->m2), mins / 10 % 10);
    sam_digit_set_digit (SAM_DIGIT(priv->m3), mins / 100);

    gtk_widget_queue_draw (GTK_WIDGET (clock));
}

static gboolean
sam_clock_on_timer (gpointer data)
{
    int secs, mins;
    SamClock *clock = (SamClock *) data;
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);

    // Add next second
    ++priv->secs;
    if (priv->secs < 0)
        priv->secs += MAX_SEC;
    if (priv->secs >= MAX_SEC)
        priv->secs -= MAX_SEC;

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
    gtk_widget_set_sensitive (GTK_WIDGET (priv->adjust), !started);
    gtk_widget_set_sensitive (GTK_WIDGET (priv->cycle), !started);
    if (priv->timer)
        g_source_remove (priv->timer);
    if (started)
        priv->timer = g_timeout_add (1000, sam_clock_on_timer, clock);
}

static GtkWidget*
get_active_digit (SamClock *clock)
{
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    switch (priv->mode)
    {
    case SCM_ADJUST_M3: return priv->m3;
    case SCM_ADJUST_M2: return priv->m2;
    case SCM_ADJUST_M1: return priv->m1;
    case SCM_ADJUST_S2: return priv->s2;
    case SCM_ADJUST_S1: return priv->s1;
    }
    return NULL;
}

static gboolean
on_blink_digit (gpointer data)
{
    SamClock *clock = (SamClock *) data;
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    GtkWidget *active = get_active_digit (clock);

    if (priv->timer)
        g_source_remove (priv->timer);

    if (!active)
        return TRUE;

    if (sam_digit_get_visible (SAM_DIGIT (active)))
    {
        sam_digit_set_visible (SAM_DIGIT (active), FALSE);
        priv->timer = g_timeout_add (200, on_blink_digit, clock);
    }
    else
    {
        sam_digit_set_visible (SAM_DIGIT (active), TRUE);
        priv->timer = g_timeout_add (500, on_blink_digit, clock);
    }
    gtk_widget_queue_draw (active);
    return TRUE;
}

static void
on_adjust_clicked (GtkButton *min, gpointer data)
{
    SamClock *clock = (SamClock *) data;
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    GtkWidget *active = get_active_digit (clock);
    if (active)
    {
        sam_digit_set_visible (SAM_DIGIT (active), TRUE);
        gtk_widget_queue_draw (active);
    }
    if (++priv->mode == SCM_LAST)
        priv->mode = SCM_NORMAL;
    on_blink_digit (clock);
}

static void
on_cycle_clicked (GtkButton *sec, gpointer data)
{
    SamClock *clock = (SamClock *) data;
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    switch (priv->mode)
    {
    case SCM_ADJUST_M3: sam_clock_cycle_digit (clock, 0); break;
    case SCM_ADJUST_M2: sam_clock_cycle_digit (clock, 1); break;
    case SCM_ADJUST_M1: sam_clock_cycle_digit (clock, 2); break;
    case SCM_ADJUST_S2: sam_clock_cycle_digit (clock, 3); break;
    case SCM_ADJUST_S1: sam_clock_cycle_digit (clock, 4); break;
    }
    update_digits (clock);
}

static void
sam_clock_init (SamClock *clock)
{
    GtkWidget *tmp;
    GtkWidget *vbox;
    SamClockPrivate* priv = SAM_CLOCK_GET_PRIVATE (clock);

    // Minutes
    priv->m3 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX (clock), priv->m3, TRUE, TRUE, 0);
    priv->m2 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX (clock), priv->m2, TRUE, TRUE, 0);
    priv->m1 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX (clock), priv->m1, TRUE, TRUE, 0);

    tmp = gtk_vseparator_new ();
    gtk_box_pack_start (GTK_BOX (clock), tmp, TRUE, TRUE, 0);

    // Seconds
    priv->s2 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX (clock), priv->s2, TRUE, TRUE, 0);
    priv->s1 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX (clock), priv->s1, TRUE, TRUE, 0);

    // Beep + restart + adjust + cycle
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (clock), vbox, TRUE, TRUE, 0);

    priv->beep = gtk_toggle_button_new_with_label ("Beep");
    gtk_box_pack_start (GTK_BOX (vbox), priv->beep, TRUE, TRUE, 0);
    priv->start = gtk_toggle_button_new_with_label ("Start");
    gtk_box_pack_start (GTK_BOX (vbox), priv->start, TRUE, TRUE, 0);
    g_signal_connect (priv->start, "toggled",
                      G_CALLBACK (&on_start_clicked), clock);

    tmp = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (vbox), tmp, TRUE, TRUE, 0);

    // Adjust
    priv->adjust = gtk_button_new_with_label ("Adjust");
    gtk_box_pack_start (GTK_BOX (vbox), priv->adjust, TRUE, TRUE, 0);
    g_signal_connect (priv->adjust, "clicked",
                      G_CALLBACK (&on_adjust_clicked), clock);

    // Cycle digit
    priv->cycle = gtk_button_new_with_label ("Cycle");
    gtk_box_pack_start (GTK_BOX (vbox), priv->cycle, TRUE, TRUE, 0);
    g_signal_connect (priv->cycle, "clicked",
                      G_CALLBACK (&on_cycle_clicked), clock);

    priv->mode = SCM_NORMAL;
    priv->secs = 999*60;
    update_digits (clock);
}

GtkWidget*
sam_clock_new (void)
{
    return g_object_new (SAM_TYPE_CLOCK, NULL);
}
