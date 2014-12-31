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
#include <stdio.h>
#include <string.h>
#include "opt.h"

static struct {
	const char *name;
	RendererType rtype;
} rend[] = {
	{ "fast", REND_FAST },
	{ 0, REND_NONE }
};

Options opt;

static void init_opt()
{
	opt.fname = 0;
	opt.rend_type = REND_FAST;
	opt.xsz = 800;
	opt.ysz = 800;
}

static RendererType renderer_type(const char *name)
{
	for(int i=0; rend[i].name; i++) {
		if(strcmp(rend[i].name, name) == 0) {
			return rend[i].rtype;
		}
	}
	return REND_NONE;
}

int parse_args(int argc, char **argv)
{
	init_opt();

	for(int i=1; i<argc; i++) {
		if(argv[i][0] == '-') {
			if(argv[i][2] == 0) {
				switch(argv[i][1]) {
				case 's':
					if(sscanf(argv[++i], "%dx%d", &opt.xsz, &opt.ysz) != 2) {
						fprintf(stderr, "-s must be followed by the window size (WxH)\n");
						return -1;
					}
					break;

				case 'r':
					if((opt.rend_type = renderer_type(argv[++i])) == REND_NONE) {
						if(strcmp(argv[i], "help") == 0) {
							printf("available renderers: ");
							for(int j=0; rend[j].name; j++) {
								printf("%s ", rend[j].name);
							}
							putchar('\n');
							return -1;
						}
						fprintf(stderr, "unknown renderer type: %s (use -r help to show list of renderers)\n", argv[i]);
						return -1;
					}
					break;

				default:
					fprintf(stderr, "invalid option: %s\n", argv[i]);
					return -1;
				}
			} else {
				fprintf(stderr, "invalid option: %s\n", argv[i]);
				return -1;
			}
		} else {
			if(opt.fname) {
				fprintf(stderr, "unexpected argument: %s\n", argv[i]);
				return -1;
			}
			opt.fname = argv[i];
		}
	}

	return 0;
}
