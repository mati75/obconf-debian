#include "preview_update.h"
#include "main.h"

static gboolean restart_theme_preview_update = TRUE;

static GtkTreeView  *tree_view            = NULL;
static GtkListStore *list_store           = NULL;
static gchar        *title_layout         = NULL;
static RrFont       *active_window_font   = NULL;
static RrFont       *inactive_window_font = NULL;
static RrFont       *menu_title_font      = NULL;
static RrFont       *menu_item_font       = NULL;
static RrFont       *osd_font             = NULL;

static gboolean update_theme_preview_iterate(gpointer data);

void preview_update_all()
{
    if (!list_store) return;

    g_idle_remove_by_data(list_store);

    if (!(title_layout && active_window_font && inactive_window_font &&
          menu_title_font && menu_item_font && osd_font))
        return; /* not set up */

    restart_theme_preview_update = TRUE;
    g_idle_add_full(G_PRIORITY_LOW,
                    update_theme_preview_iterate,
                    list_store, NULL);
}

void preview_update_set_tree_view(GtkTreeView *tr, GtkListStore *ls)
{
    g_assert(!!tr == !!ls);

    if (list_store) g_idle_remove_by_data(list_store);

    tree_view = tr;
    list_store = ls;

    if (list_store) preview_update_all();
}

void preview_update_set_active_font(RrFont *f)
{
    RrFontClose(active_window_font);
    active_window_font = f;
    preview_update_all();
}

void preview_update_set_inactive_font(RrFont *f)
{
    RrFontClose(inactive_window_font);
    inactive_window_font = f;
    preview_update_all();
}

void preview_update_set_menu_header_font(RrFont *f)
{
    RrFontClose(menu_title_font);
    menu_title_font = f;
    preview_update_all();
}

void preview_update_set_menu_item_font(RrFont *f)
{
    RrFontClose(menu_item_font);
    menu_item_font = f;
    preview_update_all();
}

void preview_update_set_osd_font(RrFont *f)
{
    RrFontClose(osd_font);
    osd_font = f;
    preview_update_all();
}

void preview_update_set_title_layout(const gchar *layout)
{
    g_free(title_layout);
    title_layout = g_strdup(layout);
    preview_update_all();
}

static gboolean update_theme_preview_iterate(gpointer data)
{
    GtkListStore *ls = data;
    static GtkTreeIter iter;
    gchar *name;

    if (restart_theme_preview_update) {
        if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(ls), &iter))
            return FALSE;
        restart_theme_preview_update = FALSE;
    } else {
        if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(ls), &iter)) {
            GtkTreePath *path;

            restart_theme_preview_update = TRUE;

            gtk_tree_view_get_cursor(tree_view, &path, NULL);
            gtk_tree_view_scroll_to_cell(tree_view, path, NULL,
                                         FALSE, 0, 0);
            gtk_tree_path_free(path);

            obconf_show_main();

            return FALSE;
        }
    }

    gtk_tree_model_get(GTK_TREE_MODEL(ls), &iter, 0, &name, -1);

    gtk_list_store_set(GTK_LIST_STORE(ls), &iter, 1,
                       preview_theme(name, title_layout, active_window_font,
                                     inactive_window_font, menu_title_font,
                                     menu_item_font, osd_font),
                       -1);

    return TRUE;
}
