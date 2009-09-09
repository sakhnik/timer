#include "clock.h"
#include "digit.h"

struct _SamClockPrivate
{
    GtkWidget *m1;
    GtkWidget *m2;
    GtkWidget *s1;
    GtkWidget *s2;

    int secs;
    guint timer;
};

G_DEFINE_TYPE (SamClock, sam_clock, GTK_TYPE_HBOX)

#define SAM_CLOCK_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), SAM_TYPE_CLOCK, SamClockPrivate))

static void
sam_clock_dispose (GObject *obj)
{
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (obj);
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

static gboolean
sam_clock_on_timer (gpointer data)
{
    int secs, mins;
    SamClock *clock = (SamClock *) data;
    SamClockPrivate *priv = SAM_CLOCK_GET_PRIVATE (clock);
    ++priv->secs;
    secs = priv->secs % 60;
    mins = priv->secs / 60;
    sam_digit_set_bits (SAM_DIGIT(priv->s2),
                        sam_clock_digit_to_bits(secs % 10));
    sam_digit_set_bits (SAM_DIGIT(priv->s1),
                        sam_clock_digit_to_bits(secs / 10));
    sam_digit_set_bits (SAM_DIGIT(priv->m2),
                        sam_clock_digit_to_bits(mins % 10));
    sam_digit_set_bits (SAM_DIGIT(priv->m1),
                        sam_clock_digit_to_bits(mins / 10));
    gtk_widget_queue_draw (GTK_WIDGET (clock));
    return TRUE;
}

static void
sam_clock_init (SamClock *clock)
{
    GtkWidget *sep;

    SamClockPrivate* priv = SAM_CLOCK_GET_PRIVATE (clock);
    priv->m1 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX(clock), priv->m1, TRUE, TRUE, 0);
    priv->m2 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX(clock), priv->m2, TRUE, TRUE, 0);

    sep = gtk_vseparator_new ();
    gtk_box_pack_start (GTK_BOX(clock), sep, TRUE, TRUE, 0);

    priv->s1 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX(clock), priv->s1, TRUE, TRUE, 0);
    priv->s2 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX(clock), priv->s2, TRUE, TRUE, 0);

    priv->secs = 0;

    priv->timer = g_timeout_add (1000, sam_clock_on_timer, clock);
}

GtkWidget*
sam_clock_new (void)
{
    return g_object_new (SAM_TYPE_CLOCK, NULL);
}
