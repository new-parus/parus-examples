int i;

if(!short_data_flag)
{

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

	for(i=0;i<F_LEN;i++)
	{
		data_out[i]+=tail[i];
	}

	printf("hello %d\n",px_node);
	fflush(stdout);

	for(i=0;i<window_size;i++)
	{
		result_data[i]=(short)(data_out[i]*(NORMALIZE_COEFFICIENT));
	}

	printf("Ehlo %d\n",px_node);
	fflush(stdout);

	nc_put_vara_short(netcdf_out_fd,amplitude_var_id,&position,&window_size,result_data);

	nc_close(netcdf_out_fd);

}

