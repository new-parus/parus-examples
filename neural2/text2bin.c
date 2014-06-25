#include <stdio.h>

int main(void)
{
	double buf;
	while (!feof(stdin))
	{
		scanf("%ld",&buf);
		fwrite(&buf,1,sizeof(double),stdout);
	}
	
	return 0;
}
