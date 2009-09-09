#include "clock.h"
#include "digit.h"

struct _SamClockPrivate
{
    GtkWidget *d1;
    GtkWidget *d2;
};

G_DEFINE_TYPE (SamClock, sam_clock, GTK_TYPE_HBOX)

#define SAM_CLOCK_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), SAM_TYPE_CLOCK, SamClockPrivate))

static void
sam_clock_class_init (SamClockClass *class)
{
    g_type_class_add_private (G_OBJECT_CLASS(class), sizeof(SamClockPrivate));
}

static void
sam_clock_init (SamClock *clock)
{
    SAM_CLOCK_GET_PRIVATE(clock)->d1 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX(clock),
                        SAM_CLOCK_GET_PRIVATE(clock)->d1,
                        TRUE, TRUE, 0);
    SAM_CLOCK_GET_PRIVATE(clock)->d2 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX(clock),
                        SAM_CLOCK_GET_PRIVATE(clock)->d2,
                        TRUE, TRUE, 0);
}

GtkWidget*
sam_clock_new (void)
{
    return g_object_new (SAM_TYPE_CLOCK, NULL);
}
