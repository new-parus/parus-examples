int i,j;
int data_id;
int num_steps=0;
double coefficient=1/(double)MAX_VALUE;
float tmp;
int flag;


if(!short_data_flag)
{
	
	
	/*
	 * Netcdf file open in Header File
	 */
	
	nc_inq_varid(netcdf_fd,"amplitude",&data_id);
	nc_get_vara_short(netcdf_fd, data_id , &position , &window_size , data_in);

	for(i=0;i<window_size;i++)
	{
		data_in_double[i]=(double)data_in[i]*coefficient;
	}

	nc_get_att_float(netcdf_fd, NC_GLOBAL, "dt", &tmp);
	
	dt=(double)tmp;
	
	lspWSFilter wsf((int)(1.0/dt), (double)Cutoff/44100.0, F_LEN, FType);

	memset(data_out,0,sizeof(double)*F_LEN);


	num_steps=window_size/F_LEN;

	for(i=0;i<num_steps;i++)
	{
		flag=wsf.process2(&data_in_double[i*F_LEN], F_LEN, bdata_out, F_LEN*2);
		if(flag)
		{
			printf("Problem while processing data by Window Sinced Filter\n");
			return -1;
		}
    
		for(j=0;j<F_LEN;j++)
		{
			data_out[i*F_LEN+j]+=bdata_out[j];
		}
	
		for(;j<F_LEN*2;j++)
		{
			data_out[i*F_LEN+j]=bdata_out[j];
		}
	
	}

	/*
	 * Process tail
	 */
	
	if(window_size%F_LEN)
	{
		int offs = num_steps*F_LEN;
	
		flag=wsf.process2
		(
	 		&data_in_double[offs],
			window_size%F_LEN ,
			bdata_out,
			window_size%F_LEN+F_LEN
		);

    	if(flag)
		{
			printf("Problem while processing data by Window Sinced Filter\n");
			return -1;
		}
		
		printf("OK 0 %d\n",px_node);
		fflush(stdout);

		for(j=0;j<F_LEN;j++)
		{
			data_out[offs+j]+=bdata_out[j];
		}

		printf("OK 1 %d\n",px_node);
		fflush(stdout);
	
		for(j=0;j<window_size%F_LEN;j++)
		{
			data_out[offs+F_LEN+j]=bdata_out[j+F_LEN];
		}
	
		printf("OK 2 %d\n",px_node);
		fflush(stdout);
	}

	nc_close(netcdf_fd);

} /* End if(!short_data_flag) */

