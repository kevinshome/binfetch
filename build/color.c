#include <stdio.h>
#include "pair.h"
#include "color.h"

void set_color(const tcolor c)
{
	printf("\033[38;2;%d;%d;%dm", c.r, c.g, c.b);
}

void set_blank(void)
{
	printf("\033[0m");
}

