int status;
int netcdf_out_fd;
int time_dim_id;
int time_var_id;
int amplitude_var_id;
char fictive_var;

status=nc_create(output_file_name,NC_CLOBBER,&netcdf_out_fd);
if(status!=NC_NOERR)
{
	printf("Can't create file %s \n",output_file_name);
	printf("NetCDF error: %s\n",nc_strerror(status));
	return -1;
}

nc_def_dim(netcdf_out_fd,"time" ,length+F_LEN, &time_dim_id);

nc_def_var(netcdf_out_fd,"time",NC_SHORT,0,NULL,&time_var_id);

nc_def_var(netcdf_out_fd,"amplitude",NC_SHORT,1,&time_dim_id,&amplitude_var_id);


nc_put_att_text(netcdf_out_fd,time_var_id,"units",strlen(units),units);

nc_put_att_text(netcdf_out_fd,NC_GLOBAL,"type",strlen("uniform timescale process"),"uniform timescale process");
if(status!=NC_NOERR)
{
	printf("%s\n",nc_strerror(status));
	return -1;
}
   
nc_put_att_text(netcdf_out_fd,NC_GLOBAL,"title",strlen("Filtered sound file"),"Filtered sound file");
if(status!=NC_NOERR)
{
	printf("%s\n",nc_strerror(status));
	return -1;
}

nc_put_att_float(netcdf_out_fd,NC_GLOBAL,"t0",NC_FLOAT,1,&t0);
nc_put_att_float(netcdf_out_fd,NC_GLOBAL,"dt",NC_FLOAT,1,&dt);

nc_enddef(netcdf_out_fd);
nc_close(netcdf_out_fd);

printf("units=%s\n",units);
fflush(stdout);

