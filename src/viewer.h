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
#ifndef VIEWER_H_
#define VIEWER_H_

#ifdef __cplusplus
extern "C" {
#endif

int init();
void cleanup();

void ev_display();
void ev_reshape(int x, int y);
void ev_keyboard(int key, int press, int x, int y);
void ev_mouse_button(int bn, int press, int x, int y);
void ev_mouse_motion(int x, int y);

// functions provided by the frontend
void swap_buffers();
void redisplay();
void quit();
void get_window_size(int *xsz, int *ysz);
unsigned int get_modifiers();

#ifdef __cplusplus
}
#endif

#endif	// VIEWER_H_
