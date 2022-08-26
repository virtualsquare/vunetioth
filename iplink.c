#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libgen.h>
#include <strcase.h>
#include <nlinline.h>

char *progname;
void usage(void) {
	fprintf(stderr, 
			"Usage: %s add [ name ] NAME\n"
			"                    [ index IDX ]\n"
			"                    [ data DATA ]\n"
			"                    type TYPE\n\n"
			"       %s del { DEVICE | dev DEVICE }\n"
			"                    [index IDX ]\n"
			"                    [type TYPE]\n\n"
			"TYPE := any type supported by the stack\n",
			progname,progname);
	exit(1);
}

#define IPLINK_NONE 0
#define IPLINK_ADD 1
#define IPLINK_DEL 2
int main(int argc, char *argv[]) {
	int iplink_op = IPLINK_NONE;
	int rv;
	char *name = NULL;
	char *type = NULL;
	char *sindex = NULL;
	char *data = NULL;
	int i;
	unsigned int index = 0;

	progname = basename(argv[0]);
	if (argc < 3)
		usage();
	switch(strcase(argv[1])) {
		case STRCASE(a,d,d):  iplink_op = IPLINK_ADD; 
													break;
		case STRCASE(d,e,l,e,t,e):
		case STRCASE(d,e,l): iplink_op = IPLINK_DEL;
												 break;
		default:
												 usage();
	}
	for (i = 2; i < argc; i++) {
		switch(strcase(argv[i])) {
			case STRCASE(n,a,m,e):
			case STRCASE(d,e,v):
				if (++i >= argc) usage();
				name = argv[i];
				break;
			case STRCASE(t,y,p,e):
				if (++i >= argc) usage();
				type = argv[i];
				break;
			case STRCASE(i,n,d,e,x):
				if (++i >= argc) usage();
				sindex = argv[i];
				break;
			case STRCASE(d,a,t,a):
				if (++i >= argc) usage();
				data = argv[i];
				break;
			default:
				if (name == NULL)
					name = argv[i];
				else
					usage();
		}
	}
	if (sindex) index = strtoul(sindex, NULL, 0);
	switch(iplink_op) {
		case IPLINK_ADD:
			if (name == NULL) usage();
			if (type == NULL) usage();
			rv = nlinline_iplink_add(name, index, type, data);
			break;
		case IPLINK_DEL:
			if (name == NULL && index == 0) usage();
			rv = nlinline_iplink_del(name, index);
			break;
		default:
			usage();
	}
	if (rv == -1) {
    perror(progname);
		return 1;
	} else
		return 0;
}
