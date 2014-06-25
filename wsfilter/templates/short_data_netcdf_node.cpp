int flag;

if(short_data_flag)
{
	
	
	/*
	 * Netcdf file open in Header File
	 */
	
	nc_inq_varid(netcdf_fd,"amplitude",&data_id);
	nc_get_vara_short(netcdf_fd, data_id , &position , &length , data_in);

	for(i=0;i<length;i++)
	{
		data_in_double[i]=(double)data_in[i]*coefficient;
	}

	nc_get_att_float(netcdf_fd, NC_GLOBAL, "dt", &tmp);
	
	dt=(double)tmp;
	
	lspWSFilter wsf((int)(1.0/dt), (double)Cutoff/44100.0, F_LEN, FType);

	memset(data_out,0,sizeof(double)*F_LEN);


	num_steps=length/F_LEN;

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
	
	if(length%F_LEN)
	{
		int offs = num_steps*F_LEN;
	
		flag=wsf.process2
		(
	 		&data_in_double[offs],
			length%F_LEN,
			bdata_out,
			length%F_LEN+F_LEN
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
	
		for(j=0;j<length%F_LEN;j++)
		{
			data_out[offs+F_LEN+j]=bdata_out[j+F_LEN];
		}
	
		printf("OK 2 %d\n",px_node);
		fflush(stdout);
	}

	nc_close(netcdf_fd);

	status=nc_open(output_file_name,NC_WRITE,&netcdf_out_fd);
	if(status!=NC_NOERR)
	{
		printf("Can't open file %s \n",output_file_name);
		printf("NetCDF error: %s\n",nc_strerror(status));
		return -1;
	}

	nc_inq_varid(netcdf_out_fd,"amplitude",&amplitude_var_id);

	printf("hello first %d\n",px_node);
	fflush(stdout);
	
	length+=F_LEN;
	
	for(i=0;i<length;i++)
	{
		result_data[i]=(short)(data_out[i]*(NORMALIZE_COEFFICIENT));
	}

	printf("Ehlo %d\n",px_node);
	fflush(stdout);
	
	nc_put_vara_short(netcdf_out_fd,amplitude_var_id,&position,&length,result_data);

	nc_close(netcdf_out_fd);
	

} /* End if(short_data_flag) */

