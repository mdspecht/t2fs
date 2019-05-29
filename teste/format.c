#include <stdio.h>
#include "t2fs.h"
#include "debug.h"

int main(void)
{
	int ret= format2(2);
//	int ret=0;
//	mount();
	debug_printf("ret_val: %d\n", ret);
	return 0;
}
