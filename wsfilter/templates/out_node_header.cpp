/*
 *
 * Out node header!!!
 *
 */

int amplitude_var_id;
char fictive_var;
int status;
int netcdf_out_fd;

size_t window_size;
int    window_number;
size_t position;


double *data_out=NULL;
short *result_data=NULL;
double tail[F_LEN];


window_number=px_node-NUM_PROCS;
window_size=length/NUM_PROCS;
position=window_number*window_size;

data_out=(double *)malloc((window_size)*sizeof(double));
if(data_out==NULL)
{
	printf("Can't allocate memory for data_out in node %d\n",px_node);
	return -1;
}

result_data=(short *)malloc((window_size)*sizeof(short));
if(result_data==NULL)
{
	printf("Can't allocate memory for result_data in node %d\n",px_node);
	return -1;
}

memset(tail,0,F_LEN*sizeof(double));
