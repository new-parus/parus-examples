int  amplitude_var_id;
char fictive_var;
int status;
int netcdf_out_fd;

size_t position;


short *data_in=NULL;
double *data_in_double=NULL;
double *data_out=NULL;
short *result_data=NULL;
double bdata_out[F_LEN*2];

position=0;

int i,j;
int data_id;
int num_steps=0;
double coefficient=1/(double)MAX_VALUE;
float tmp;

if(short_data_flag)
{

	data_in=(short *)malloc(length*sizeof(short));
	if(data_in==NULL)
	{
		printf("Can't allocate memory for data_in in node %d\n",px_node);
		return -1;
	}

	data_in_double=(double *)malloc(length*sizeof(double));
	if(data_in_double==NULL)
	{
		printf("Can't allocate memory for data_in_double in node %d\n",px_node);
		return -1;
	}


	data_out=(double *)malloc((length+F_LEN)*sizeof(double));
	if(data_out==NULL)
	{
		printf("Can't allocate memory for data_out in node %d\n",px_node);
		return -1;
	}

	result_data=(short *)malloc((length+F_LEN)*sizeof(short));
	if(result_data==NULL)
	{
		printf("Can't allocate memory for result_data in node %d\n",px_node);
		return -1;
	}

}
