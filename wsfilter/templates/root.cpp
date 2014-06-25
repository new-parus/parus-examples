int status;
int time_dim_id;
int time_var_id;
size_t len;

if(px_argc<2)
{
	fprintf(stderr, "You must show module Header file\n");
	return -1;
}

if(read_module_header(px_argv[1], &parms))
{
	fprintf(stderr, "Can't read header\n");
	return -1;
}


if(get_module_attribute_value(&parms, "Cutoff" , &Cutoff))
{
	fprintf(stderr,"can't read attribute Cutoff\n");
	return -1;
}

if(get_module_attribute_value(&parms, "FilterType" , &FType))
{
	fprintf(stderr,"can't read attribute FilterType\n");
	return -1;
}

input_file_name=  parms.files[0].name;
output_file_name= parms.files[1].name;


status=nc_open(input_file_name,NC_NOWRITE,&netcdf_fd);
if(status!=NC_NOERR)
{
	printf("Can't open file %s \n",input_file_name);
	printf("NetCDF error: %s\n",nc_strerror(status));
	return -1;
}

nc_inq_dimid(netcdf_fd,"time",&time_dim_id);
nc_inq_dimlen(netcdf_fd,time_dim_id,&length);

nc_inq_varid(netcdf_fd,"time",&time_var_id);
nc_inq_attlen(netcdf_fd,time_var_id,"units",&len);

units=(char *)malloc((len+1)*sizeof(char));

status=nc_get_att_text(netcdf_fd,time_var_id,"units",units);
if(status!=NC_NOERR)
{
	fprintf(stderr,"Can't get 'units'\n");
	fprintf(stderr,"%s\n\n",nc_strerror(status));
	return -1;
}

units[len]='\0';



nc_inq_attlen(netcdf_fd,NC_GLOBAL,"type",&len);
data_type=(char *)malloc((len+1)*sizeof(char));
status=nc_get_att_text(netcdf_fd,NC_GLOBAL,"type",data_type);
if(status!=NC_NOERR)
{
	fprintf(stderr,"Can't get 'type'\n");
	fprintf(stderr,"%s\n\n",nc_strerror(status));
	return -1;
}
data_type[len]='\0';


nc_inq_attlen(netcdf_fd,NC_GLOBAL,"title",&len);
data_title=(char *)malloc((len+1)*sizeof(char));
status=nc_get_att_text(netcdf_fd,NC_GLOBAL,"title",data_title);
if(status!=NC_NOERR)
{
	fprintf(stderr,"Can't get 'title'\n");
	fprintf(stderr,"%s\n\n",nc_strerror(status));
	return -1;
}
data_title[len]='\0';


status=nc_get_att_float(netcdf_fd,NC_GLOBAL,"t0",&t0);
if(status!=NC_NOERR)
{
	fprintf(stderr,"Can't get 't0'\n");
	fprintf(stderr,"%s\n\n",nc_strerror(status));
	return -1;
}

status=nc_get_att_float(netcdf_fd,NC_GLOBAL,"dt",&dt);
if(status!=NC_NOERR)
{
	fprintf(stderr,"Can't get 'dt'\n");
	fprintf(stderr,"%s\n\n",nc_strerror(status));
	return -1;
}

if(length/NUM_PROCS<2*F_LEN)
{
	short_data_flag=1;
	fprintf(stderr,"Can't split NetCDF file.\n Slices < 2*F_LEN \n");
	//return -1;
}



printf("length=%lld\n",(long long int)length);
printf("units='%s'\n",units);
printf("data_type='%s'\n",data_type);
printf("data_title='%s'\n",data_title);
printf("t0=%f\n",t0);
printf("dt=%f\n",dt);



