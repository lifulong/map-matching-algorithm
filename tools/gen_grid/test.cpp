#include "gen_grid.h"

void test()
{
	GenMapGrid *gen_grid = new GenMapGrid("segs.txt");
	gen_grid->genGrid("segs.txt");
	gen_grid->dumpGrid("grid.txt");
	//gen_grid->dumpGrid("");
}


int main()
{
	test();
	return 0;
}


