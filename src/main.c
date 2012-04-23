/* -*- indent-tabs-mode: nil; tab-width: 4; c-basic-offset: 4; -*-

   main.c for ObConf, the configuration tool for Openbox
   Copyright (c) 2003        Ben Jansens
   Copyright (c) 2003        Tim Riley

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   See the COPYING file for a copy of the GNU General Public License.
*/

#include "main.h"
#include "handlers.h"

#include <gdk/gdkx.h>
#define SN_API_NOT_YET_FROZEN
#include <libsn/sn.h>
#undef SN_API_NOT_YET_FROZEN

static GtkWidget *mainwin;

GladeXML *glade;
xmlDocPtr doc;
xmlNodePtr root;

void obconf_error(gchar *msg)
{
    GtkWidget *d;

    d = gtk_message_dialog_new(mainwin ? GTK_WINDOW(mainwin) : NULL,
                               GTK_DIALOG_DESTROY_WITH_PARENT,
                               GTK_MESSAGE_ERROR,
                               GTK_BUTTONS_CLOSE,
                               "%s", msg);
    g_signal_connect_swapped(GTK_OBJECT(d), "response",
                             G_CALLBACK(gtk_widget_destroy),
                             GTK_OBJECT(d));
    gtk_widget_show(d);
}

int main(int argc, char **argv)
{
    SnDisplay *sn_d;
    SnLauncheeContext *sn_cx;
    gchar *p;

    parse_paths_startup();

    gtk_init(&argc, &argv);

    p = g_build_filename(GLADEDIR, "obconf.glade", NULL);
    glade = glade_xml_new(p, NULL, NULL);
    g_free(p);

    if (!glade) {
        obconf_error("Failed to load the obconf.glade interface file. You "
                     "have probably failed to install ObConf properly.");
        return 1;
    }

    xmlIndentTreeOutput = 1;
    if (!parse_load_rc(&doc, &root)) {
        obconf_error("Failed to load an rc.xml. You have probably failed to "
                     "install Openbox properly.");
        return 1;
    }

    glade_xml_signal_autoconnect(glade);

    {
        gchar *s = g_strdup_printf
            ("<span weight=\"bold\" size=\"xx-large\">ObConf %s</span>",
             PACKAGE_VERSION);
        gtk_label_set_markup(GTK_LABEL
                             (glade_xml_get_widget(glade, "title_label")), s);
        g_free(s);
    }

    setup_behavior_tab();
    setup_dock_tab();
    setup_focus_mouse(glade_xml_get_widget(glade, "focus_mouse"));
    setup_focus_raise(glade_xml_get_widget(glade, "focus_raise"));
    setup_focus_delay(glade_xml_get_widget(glade, "focus_delay"));
    setup_focus_new(glade_xml_get_widget(glade, "focus_new"));
    setup_place_mouse(glade_xml_get_widget(glade, "place_mouse"));
    setup_resist_window(glade_xml_get_widget(glade, "resist_window"));
    setup_resist_edge(glade_xml_get_widget(glade, "resist_edge"));
    setup_resize_contents(glade_xml_get_widget(glade, "resize_contents"));
    setup_dock_position(glade_xml_get_widget(glade, "dock_position"));
    setup_dock_float_x(glade_xml_get_widget(glade, "dock_float_x"));
    setup_dock_float_y(glade_xml_get_widget(glade, "dock_float_y"));
    setup_dock_stacking(glade_xml_get_widget(glade, "dock_stack_top"),
                        glade_xml_get_widget(glade, "dock_stack_normal"),
                        glade_xml_get_widget(glade, "dock_stack_bottom"));
    setup_dock_direction(glade_xml_get_widget(glade, "dock_direction"));
    setup_dock_hide(glade_xml_get_widget(glade, "dock_hide"));
    setup_dock_hide_delay(glade_xml_get_widget(glade, "dock_hide_delay"));
    setup_theme_names(glade_xml_get_widget(glade, "theme_names"));
    setup_theme_layout(glade_xml_get_widget(glade, "theme_layout"));
    setup_desktop_num(glade_xml_get_widget(glade, "desktop_num"));
    setup_desktop_names(glade_xml_get_widget(glade, "desktop_names"));

    mainwin = glade_xml_get_widget(glade, "main_window");

    sn_d = sn_display_new(GDK_DISPLAY_XDISPLAY(gdk_display_get_default()),
                          NULL, NULL);

    sn_cx = sn_launchee_context_new_from_environment
        (sn_d, gdk_screen_get_number(gdk_display_get_default_screen
                                     (gdk_display_get_default())));

    if (sn_cx)
        sn_launchee_context_setup_window
            (sn_cx, GDK_WINDOW_XWINDOW(GDK_WINDOW(mainwin->window)));

    gtk_widget_show_all(mainwin);

    if (sn_cx)
        sn_launchee_context_complete(sn_cx);

    if (sn_cx)
        sn_launchee_context_unref(sn_cx);
    sn_display_unref(sn_d);

    gtk_main();

    parse_paths_shutdown();

    xmlFreeDoc(doc);
    return 0;
}
