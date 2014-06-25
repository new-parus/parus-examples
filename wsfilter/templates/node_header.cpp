int staus;
int window_number;
size_t window_size;
size_t position;

short *data_in=NULL;
double *data_out=NULL;
double *data_in_double=NULL;
double bdata_out[F_LEN*2];


window_size=length/NUM_PROCS;
window_number=px_node;
position=window_number*window_size;

data_in=(short *)malloc(window_size*sizeof(short));
if(data_in==NULL)
{
	printf("Can't allocate memory for data_in in node %d\n",px_node);
	return -1;
}

data_in_double=(double *)malloc(window_size*sizeof(double));
if(data_in_double==NULL)
{
	printf("Can't allocate memory for data_in_double in node %d\n",px_node);
	return -1;
}

data_out=(double *)malloc((window_size+F_LEN)*sizeof(double));
if(data_out==NULL)
{
	printf("Can't allocate memory for data_out in node %d\n",px_node);
	return -1;
}
