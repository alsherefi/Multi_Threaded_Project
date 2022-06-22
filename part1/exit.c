#include "project.h"

void __exit(void)
{
	free(cmd.A);
	free(ln.A);
	exit(0);
}
