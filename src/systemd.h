
/*
 *  Copyright (C) 2015 Adrien Vergé
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _SYSTEMD_H
#define _SYSTEMD_H
#include <systemd/sd-bus.h>

#define BUS_SERVICE   "org.freedesktop.resolve1"
#define BUS_OBJECT    "/org/freedesktop/resolve1"
#define BUS_INTERFACE "org.freedesktop.resolve1.Manager"

struct systemd_config {
  sd_bus *bus;
  int use_systemd; // FIXME
};
struct tunnel;

int systemd_add_nameservers(struct tunnel *tunnel);
int systemd_del_nameservers(struct tunnel *tunnel);

#endif // _SYSTEMD_H
