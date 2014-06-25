#line 1 "mtrstupidmul.c"



void main ()
{
double Result[10*10], Left[10*10], Right[10*10]; 
int i, j, k;
for (i = 0; i < 10; i++)
for (j = 0; j < 10; j++)
for (k = 0; k < 10; k++)
Result[i*10 + j] += Left [i*10 + k] + Right [k*10 + j];
}

