#include <stdio.h>
#include "t2fs.h"
#include "t2fs_aux.h"
#include "debug.h"

#define debug_printf printf

int main(void)
{
//	int ret= format2(4);
	int ret=0;
	t2fs_mount();
	debug_printf("ret_val: %d\n", ret);
	return 0;
}
