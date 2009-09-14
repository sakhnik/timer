#include "number.h"
#include "digit.h"

struct _SamNumberPrivate
{
    GtkWidget *d2;
    GtkWidget *d1;

    guint val;
    guint max_val;
    guint timer; // to blink
};

G_DEFINE_TYPE (SamNumber, sam_number, GTK_TYPE_HBOX)

#define SAM_NUMBER_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), SAM_TYPE_NUMBER, SamNumberPrivate))

static void
sam_number_dispose (GObject *obj)
{
    SamNumberPrivate *priv = SAM_NUMBER_GET_PRIVATE (obj);
    if (priv->timer)
        g_source_remove (priv->timer);
}

static void
sam_number_class_init (SamNumberClass *class)
{
    g_type_class_add_private (G_OBJECT_CLASS(class), sizeof(SamNumberPrivate));
    G_OBJECT_CLASS(class)->dispose = sam_number_dispose;
}

static void
update_digits (SamNumber *number)
{
    SamNumberPrivate *priv = SAM_NUMBER_GET_PRIVATE (number);
    sam_digit_set_digit (SAM_DIGIT (priv->d1), priv->val % 10);
    sam_digit_set_digit (SAM_DIGIT (priv->d2), priv->val / 10 % 10);
    gtk_widget_queue_draw (GTK_WIDGET (number));
}

static void
sam_number_init (SamNumber *number)
{
    SamNumberPrivate *priv = SAM_NUMBER_GET_PRIVATE (number);

    priv->d2 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX (number), priv->d2, TRUE, TRUE, 0);
    priv->d1 = sam_digit_new ();
    gtk_box_pack_start (GTK_BOX (number), priv->d1, TRUE, TRUE, 0);

    priv->val = 0;
    priv->max_val = 100;
    update_digits (number);
}

GtkWidget*
sam_number_new (guint max_val)
{
    GtkWidget *number = g_object_new (SAM_TYPE_NUMBER, NULL);
    g_assert (max_val <= 100 && "max_val must be 100 at most");
    SAM_NUMBER_GET_PRIVATE (number)->max_val = max_val;
    return number;
}

void
sam_number_set_value (SamNumber *number, guint val)
{
    SamNumberPrivate *priv = SAM_NUMBER_GET_PRIVATE (number);
    g_assert (val < 100 && "val must be 2-digit at most");
    priv->val = val;
    sam_digit_set_digit (SAM_DIGIT (priv->d1), val % 10);
    sam_digit_set_digit (SAM_DIGIT (priv->d2), val / 10);
    gtk_widget_queue_draw (GTK_WIDGET (number));
}

guint sam_number_get_value (SamNumber *number)
{
    return SAM_NUMBER_GET_PRIVATE (number)->val;
}

static gboolean
on_blink (gpointer data)
{
    SamNumber *number = (SamNumber *) data;
    SamNumberPrivate *priv = SAM_NUMBER_GET_PRIVATE (number);

    if (priv->timer)
        g_source_remove (priv->timer);

    sam_digit_set_visible (SAM_DIGIT (priv->d2),
                           !sam_digit_get_visible (SAM_DIGIT (priv->d2)));
    sam_digit_set_visible (SAM_DIGIT (priv->d1),
                           !sam_digit_get_visible (SAM_DIGIT (priv->d1)));
    priv->timer = g_timeout_add (250, on_blink, number);
    gtk_widget_queue_draw (GTK_WIDGET (number));
    return TRUE;
}

void
sam_number_set_blink (SamNumber *number, gboolean blink)
{
    SamNumberPrivate *priv = SAM_NUMBER_GET_PRIVATE (number);

    if (blink)
    {
        on_blink (number);
        return;
    }

    if (priv->timer)
        g_source_remove (priv->timer);

    sam_digit_set_visible (SAM_DIGIT (priv->d2), TRUE);
    sam_digit_set_visible (SAM_DIGIT (priv->d1), TRUE);
}

void
sam_number_cycle_values (SamNumber *number)
{
    SamNumberPrivate *priv = SAM_NUMBER_GET_PRIVATE (number);
    guint val = priv->val + 1;
    if (val >= priv->max_val)
        val = 0;
    sam_number_set_value (number, val);
}
