#ifndef __NUMBER_H__
#define __NUMBER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

GType sam_number_get_type (void);

#define SAM_TYPE_NUMBER         (sam_number_get_type())
#define SAM_NUMBER(obj)         \
        (G_TYPE_CHECK_INSTANCE_CAST ((obj), SAM_TYPE_NUMBER, SamNumber))

typedef struct _SamNumber        SamNumber;
typedef struct _SamNumberClass   SamNumberClass;
typedef struct _SamNumberPrivate SamNumberPrivate;

struct _SamNumber
{
    GtkHBox parent;
    SamNumberPrivate *priv;
};

struct _SamNumberClass
{
    GtkHBoxClass parent_class;
};

GtkWidget* sam_number_new (guint max_val);

void sam_number_set_value (SamNumber *number, guint val);
guint sam_number_get_value (SamNumber *number);

void sam_number_set_blink (SamNumber *number, gboolean blink);

void sam_number_cycle_values (SamNumber *number, gint delta);

G_END_DECLS

#endif //__NUMBER_H__
