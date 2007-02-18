#ifndef UTILS_BUTTONS_H
#define UTILS_BUTTONS_H

/* START_INCLUDE_H */
#include "./utils_buttons.h"
/* END_INCLUDE_H */


typedef enum GSB_BUTTON_STYLE {
    GSB_BUTTON_TEXT,
    GSB_BUTTON_ICON,
    GSB_BUTTON_BOTH,
} GsbButtonStyle;


/*START_DECLARATION*/
GtkWidget *cree_bouton_url ( const gchar *adr,
			     const gchar *inscription );
GtkWidget * new_image_label ( GsbButtonStyle style, const gchar * image_name, const gchar * name );
GtkWidget * new_stock_image_label ( GsbButtonStyle style, const gchar * stock_id, const gchar * name );
void set_popup_position (GtkMenu *menu, gint *x, gint *y, gboolean *push_in, gpointer user_data);
/*END_DECLARATION*/

#endif
