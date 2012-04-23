/* -*- indent-tabs-mode: nil; tab-width: 4; c-basic-offset: 4; -*-

   main.h for ObConf, the configuration tool for Openbox
   Copyright (c) 2003        Ben Jansens

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

#ifndef obconf__main_h
#define obconf__main_h

#include "openbox/parse.h"

#include <gtk/gtk.h>
#include <glade/glade-xml.h>

extern GladeXML *glade;
extern xmlDocPtr doc;
extern xmlNodePtr root;

void obconf_error(gchar *msg);

#endif
