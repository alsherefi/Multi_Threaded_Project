#include "project.h"

void __exit(void)
{
	if (cmd != NULL)
		free(cmd);

	exit(0);
}
