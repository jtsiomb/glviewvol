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
