#include "digit.h"
#include <math.h>
#include <stdio.h>

struct _SamDigitPrivate
{
    int bits;
    gboolean visible;
};

G_DEFINE_TYPE (SamDigit, sam_digit, GTK_TYPE_DRAWING_AREA)

#define SAM_DIGIT_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), SAM_TYPE_DIGIT, SamDigitPrivate))

static void
sam_digit_draw (GtkWidget *digit, cairo_t *cr)
{
    SamDigitPrivate *priv = SAM_DIGIT_GET_PRIVATE (digit);
    int bits = priv->bits;

    // Centre
    double x = digit->allocation.width / 2;
    double y = digit->allocation.height / 2;
    // Leg length
    double a = MIN (digit->allocation.width * 2.0 / 3,
                    digit->allocation.height * 0.5) * 0.9;
    double w = a * 0.1;
    double b = w * 0.5;

    if (priv->visible)
        cairo_set_source_rgba (cr, 0, 0.5, 0, 1);
    else
        cairo_set_source_rgba (cr, 0, 0.5, 0, 0);
    cairo_set_line_width (cr, w);
    if (bits & (1 << 0))
    {
        cairo_move_to (cr, x - a/2 + b, y - a);
        cairo_line_to (cr, x + a/2 - b, y - a);
        cairo_stroke (cr);
    }
    if (bits & (1 << 1))
    {
        cairo_move_to (cr, x - a/2, y - a + b);
        cairo_line_to (cr, x - a/2, y - b);
        cairo_stroke (cr);
    }
    if (bits & (1 << 2))
    {
        cairo_move_to (cr, x + a/2, y - a + b);
        cairo_line_to (cr, x + a/2, y - b);
        cairo_stroke (cr);
    }
    if (bits & (1 << 3))
    {
        cairo_move_to (cr, x - a/2 + b, y);
        cairo_line_to (cr, x + a/2 - b, y);
        cairo_stroke (cr);
    }
    if (bits & (1 << 4))
    {
        cairo_move_to (cr, x - a/2, y + b);
        cairo_line_to (cr, x - a/2, y + a - b);
        cairo_stroke (cr);
    }
    if (bits & (1 << 5))
    {
        cairo_move_to (cr, x + a/2, y + b);
        cairo_line_to (cr, x + a/2, y + a - b);
        cairo_stroke (cr);
    }
    if (bits & (1 << 6))
    {
        cairo_move_to (cr, x - a/2 + b, y + a);
        cairo_line_to (cr, x + a/2 - b, y + a);
        cairo_stroke (cr);
    }
}

static gboolean
sam_digit_expose (GtkWidget *digit, GdkEventExpose *event)
{
    cairo_t *cr = gdk_cairo_create (digit->window);
    cairo_rectangle (cr,
                     event->area.x, event->area.y,
                     event->area.width, event->area.height);
    cairo_clip (cr);
    sam_digit_draw (digit, cr);
    cairo_destroy (cr);
    return FALSE;
}

static void
sam_digit_class_init (SamDigitClass *class)
{
    g_type_class_add_private (G_OBJECT_CLASS(class), sizeof(SamDigitPrivate));
    GTK_WIDGET_CLASS (class) -> expose_event = sam_digit_expose;
}

static void
sam_digit_init (SamDigit *digit)
{
    SAM_DIGIT_GET_PRIVATE(digit)->bits = 127;
    SAM_DIGIT_GET_PRIVATE(digit)->visible = TRUE;
}

GtkWidget*
sam_digit_new (void)
{
    GtkWidget *widget = g_object_new (SAM_TYPE_DIGIT, NULL);
    gtk_widget_set_size_request (widget, 100, 150);
    return widget;
}

void
sam_digit_set_bits (SamDigit *digit, int bits)
{
    SAM_DIGIT_GET_PRIVATE(digit)->bits = bits;
}

int
sam_digit_get_bits (SamDigit *digit)
{
    return SAM_DIGIT_GET_PRIVATE(digit)->bits;
}

void
sam_digit_set_visible (SamDigit *digit, gboolean visible)
{
    SAM_DIGIT_GET_PRIVATE(digit)->visible = visible;
    gtk_widget_queue_draw (GTK_WIDGET (digit));
}

gboolean
sam_digit_get_visible (SamDigit *digit)
{
    return SAM_DIGIT_GET_PRIVATE(digit)->visible;
}

#define DIGIT_LEG(i) (1 << (i))

void
sam_digit_set_digit (SamDigit *digit, int value)
{
    switch (value)
    {
    case 0: sam_digit_set_bits (digit, DIGIT_LEG(0) | DIGIT_LEG(1) |
                                       DIGIT_LEG(2) | DIGIT_LEG(4) |
                                       DIGIT_LEG(5) | DIGIT_LEG(6));
            return;
    case 1: sam_digit_set_bits (digit, DIGIT_LEG(2) | DIGIT_LEG(5));
            return;
    case 2: sam_digit_set_bits (digit, DIGIT_LEG(0) | DIGIT_LEG(2) |
                                       DIGIT_LEG(3) | DIGIT_LEG(4) |
                                       DIGIT_LEG(6));
            return;
    case 3: sam_digit_set_bits (digit, DIGIT_LEG(0) | DIGIT_LEG(2) |
                                       DIGIT_LEG(3) | DIGIT_LEG(5) |
                                       DIGIT_LEG(6));
            return;
    case 4: sam_digit_set_bits (digit, DIGIT_LEG(1) | DIGIT_LEG(2) |
                                       DIGIT_LEG(3) | DIGIT_LEG(5));
            return;
    case 5: sam_digit_set_bits (digit, DIGIT_LEG(0) | DIGIT_LEG(1) |
                                       DIGIT_LEG(3) | DIGIT_LEG(5) |
                                       DIGIT_LEG(6));
            return;
    case 6: sam_digit_set_bits (digit, DIGIT_LEG(1) | DIGIT_LEG(3) |
                                       DIGIT_LEG(4) | DIGIT_LEG(5) |
                                       DIGIT_LEG(6));
            return;
    case 7: sam_digit_set_bits (digit, DIGIT_LEG(0) | DIGIT_LEG(2) |
                                       DIGIT_LEG(5));
            return;
    case 8: sam_digit_set_bits (digit, DIGIT_LEG(0) | DIGIT_LEG(1) |
                                       DIGIT_LEG(2) | DIGIT_LEG(3) |
                                       DIGIT_LEG(4) | DIGIT_LEG(5) |
                                       DIGIT_LEG(6));
            return;
    case 9: sam_digit_set_bits (digit, DIGIT_LEG(0) | DIGIT_LEG(1) |
                                       DIGIT_LEG(2) | DIGIT_LEG(3) |
                                       DIGIT_LEG(5));
            return;
    default: sam_digit_set_bits (digit, 0);
    }
}

