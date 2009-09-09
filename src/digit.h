#ifndef DIGIT_H_
#define DIGIT_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SAM_TYPE_DIGIT          (sam_digit_get_type())
#define SAM_DIGIT(obj)          \
        (G_TYPE_CHECK_INSTANCE_CAST ((obj), SAM_TYPE_DIGIT, SamDigit))
#define SAM_DIGIT_CLASS(obj)    \
        (G_TYPE_CHECK_CLASS_CAST ((obj), SAM_DIGIT, SamDigitClass))
#define SAM_IS_DIGIT(obj)       \
        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SAM_TYPE_DIGIT))
#define SAM_IS_DIGIT_CLASS(obj) \
        (G_TYPE_CHECK_CLASS_TYPE ((obj), SAM_TYPE_DIGIT))
#define SAM_DIGIT_GET_CLASS(obj) \
        (G_TYPE_INSTANCE_GET_CLASS ((obj), SAM_TYPE_DIGIT, SamDigitClass))

typedef struct _SamDigit        SamDigit;
typedef struct _SamDigitClass   SamDigitClass;
typedef struct _SamDigitPrivate SamDigitPrivate;

struct _SamDigit
{
    GtkDrawingArea parent;

    SamDigitPrivate *priv;
};

struct _SamDigitClass
{
    GtkDrawingAreaClass parent_class;
};

extern GtkWidget* sam_digit_new (void);

extern void sam_digit_set_bits (SamDigit* digit, int bits);

extern int sam_digit_get_bits (SamDigit* digit);

G_END_DECLS

#endif //DIGIT_H_
