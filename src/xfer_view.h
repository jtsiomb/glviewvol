/*
glviewvol is an OpenGL 3D volume data viewer
Copyright (C) 2014  John Tsiombikas <nuclear@member.fsf.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef XFER_VIEW_H_
#define XFER_VIEW_H_

#include "volume.h"
#include "xfermap.h"

bool xfview_init(TransferFunc *xfer);
void xfview_destroy();

void xfview_set_volume(Volume *vol);

void xfview_draw();

void xfview_button(int bn, int press, int x, int y);
void xfview_motion(int x, int y);

#endif	// XFER_VIEW_H_
