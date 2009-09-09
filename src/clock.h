#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SAM_TYPE_CLOCK          (sam_clock_get_type())
#define SAM_CLOCK(obj)          \
        (G_TYPE_CHECK_INSTANCE_CAST ((obj), SAM_TYPE_CLOCK, SamClock))

typedef struct _SamClock        SamClock;
typedef struct _SamClockClass   SamClockClass;
typedef struct _SamClockPrivate SamClockPrivate;

struct _SamClock
{
    GtkHBox parent;
    SamClockPrivate *priv;
};

struct _SamClockClass
{
    GtkHBoxClass parent_class;
};

extern GtkWidget* sam_clock_new (void);

G_END_DECLS

#endif //__CLOCK_H__
