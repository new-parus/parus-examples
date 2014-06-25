#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <netcdf.h>
#include <unistd.h>
#include "lspwsfilter.h"
#include "module_header.h"

/* Function to handle NetCDF errors */

void H(int status)
{
    if (status!=NC_NOERR)
    {
        printf("WSFilter: NetCDF error: %s",nc_strerror(status));
        exit(0);
    }
}

int main(int argc,char **argv)
{
    size_t points;
    int totalpoints;
    int i, j;

    short  *data;
    double *fdata;
    void   *d;


    // WSFilter related
    int           FLen   = 2048;  // Filter Length and Convolution Length
    int           Cutoff = 15000; // Filter Cutoff Frequency
    WSFILTER_TYPE FType  = WSFILTER_LOWPASS; // Filter Type
    float         dt; // 1/dt = SampleRate

    // NetCDF fileheader
    char file[256];
    Module_parameters parms = {0, NULL, 0, NULL};

    // NetCDF
    int ncid, ncid_in, ncid_out;
    int file_id;
    size_t namelen;
    char *name_in, *name_out;

    size_t titlelen;
    char *title;

    int ndims;
    size_t dimlen;
    char dimname[NC_MAX_NAME];
    int dimid, timeid;
    int dimids[NC_MAX_VAR_DIMS];

    int nvars;
    char varname[NC_MAX_NAME];
    nc_type vartype;
    int varnatts, varndims;
    int varid;

    int natts;
    char attname[NC_MAX_NAME];

    //  float max;
    //  double flr, tmp;

    //  printf("Content-type: text/plain\n\n");

    /* header file */
    if(argc < 2)
        scanf("%255s", file);
    else
        strncpy(file, argv[1], 255);

    fprintf(stderr,"Input string \"%s\"\n", file);

    if(read_module_header(file, &parms))
    {
        fprintf(stderr, "Can't read header\n");
        return -1;
    }

    // Read Parameters!!!

    if(get_module_attribute_value(&parms, "Cutoff" , &Cutoff))
    {
        fprintf(stderr,"can't read attribute omission_length");
        return -1;
    }

    if(get_module_attribute_value(&parms, "FilterType" , &FType))
    {
        fprintf(stderr,"can't read attribute omission_length");
        return -1;
    }

    /* Open in read-only mode */
    H(nc_open(file, NC_NOWRITE, &ncid));

    /* file1:name attribute */
    H(nc_inq_varid(ncid,"file1",&file_id));
    H(nc_inq_attlen(ncid,file_id,"name",&namelen));
    name_in = (char *)calloc(1,namelen+1); /* +1 for zero-termination */
    H(nc_get_att_text(ncid,file_id,"name",name_in));
    /* file2:name attribute */
    H(nc_inq_varid(ncid,"file2",&file_id));
    H(nc_inq_attlen(ncid,file_id,"name",&namelen));
    name_out = (char *)calloc(1,namelen+1); /* +1 for zero-termination */
    H(nc_get_att_text(ncid,file_id,"name",name_out));

    H(nc_close(ncid));

    /* IN & OUT data files */

    /* Open IN in read-only mode */
    H(nc_open(name_in,NC_NOWRITE,&ncid_in));
    free(name_in);

    /* Create OUT */
    H(nc_create(name_out,NC_CLOBBER,&ncid_out));
    free(name_out);

    /* Copying dimesions */
    H(nc_inq_ndims(ncid_in,&ndims));
    H(nc_inq_dimid(ncid_in,"time",&timeid));
    for(i = 0; i < ndims; i++)
    {
        H(nc_inq_dim(ncid_in, i, dimname, &dimlen));
        H(nc_def_dim(ncid_out, dimname, dimlen+FLen, &dimid));
        // Note, that ids of these two dimensions will be equal,
        // i.e. (i == dimid)
        // Note, that the length of the new dimension is greater than the original
        // one. This is due-to the convolution maths.
    }

    /* Copying variables & attributes */
    H(nc_inq_nvars(ncid_in,&nvars));
    for(i=0;i<nvars;i++)
    {
        H(nc_inq_varname(ncid_in,i,varname));
        H(nc_inq_vartype(ncid_in,i,&vartype));
        H(nc_inq_varndims(ncid_in,i,&varndims));
        H(nc_inq_vardimid(ncid_in,i,dimids));
        //    if (varndims == 1 && vartype != NC_CHAR)
        //      vartype = NC_DOUBLE; // This method will convert type to DOUBLE
        H(nc_def_var(ncid_out,varname,vartype,varndims,dimids,&varid));

        H(nc_inq_varnatts(ncid_in,i,&varnatts));
        for(j=0;j<varnatts;j++)
        {
            H(nc_inq_attname(ncid_in,i,j,attname));
            H(nc_copy_att(ncid_in,i,attname,ncid_out,i));
        }
    }
    H(nc_inq_natts(ncid_in,&natts));
    for(j=0;j<natts;j++)
    {
        H(nc_inq_attname(ncid_in,NC_GLOBAL,j,attname));
        if (strncmp(attname,"title",5))
            H(nc_copy_att(ncid_in,NC_GLOBAL,attname,ncid_out,NC_GLOBAL));
        else // special for 'title' attribute
        {
            H(nc_inq_attlen(ncid_in,NC_GLOBAL,"title",&titlelen));
            title = (char *)calloc(1,titlelen+1+12); /* +1 for zero-termination */
            H(nc_get_att_text(ncid_in,NC_GLOBAL,"title",title));
            strcat(title,", filtered");
            H(nc_put_att_text(ncid_out,NC_GLOBAL,"title",strlen(title),title));
            free(title);
        }
    }

    H(nc_enddef(ncid_out));
 
 	lspWSFilter wsf((int)(1.0/dt), (double)Cutoff/44100.0, FLen, FType);


    /* Copying data, applying the filter */
    for(i=0;i<nvars;i++)
    {
        H(nc_inq_vartype(ncid_in,i,&vartype));
        H(nc_inq_varndims(ncid_in,i,&varndims));
        H(nc_inq_vardimid(ncid_in,i,dimids));

        H(nc_get_att_float(ncid_in, NC_GLOBAL, "dt", &dt));
        unsigned int k;

        short  *data      = new short[FLen];
        double *fdata_in  = new double[FLen];
        double *fdata_out = new double[FLen*2];
        double *ftail     = new double[FLen];
        memset(data,      0, sizeof(short)*FLen);
        memset(fdata_in,  0, sizeof(double)*FLen);
        memset(fdata_out, 0, sizeof(double)*FLen*2);
        memset(ftail,     0, sizeof(double)*FLen);

        double tmp = 32767.0 + 32767.0/32768.0;
        unsigned int tail = 0;

        H(nc_inq_dimlen(ncid_in,dimids[0],&points));
        for(k = 0; k < points/FLen; k++)
        {
            size_t start[] = {k*FLen};
            size_t count[] = {FLen};
            H(nc_get_vara_short(ncid_in,  i, start, count, data));
            for(j = 0; j < FLen; j++)
                fdata_in[j] = ((double)data[j]) / 32768.0;

            //      wsf.processWindow(fdata_in, FLen, fdata_out);
            wsf.process2(fdata_in, FLen, fdata_out, FLen*2);
            for(j = 0; j < FLen; j++)
                fdata_out[j] += ftail[j];
            memcpy(ftail, fdata_out+FLen, sizeof(double)*FLen);

            for(j = 0; j < FLen; j++)
                data[j] = (int)(fdata_out[j] * tmp);
            H(nc_put_vara_short(ncid_out, i, start, count, data));
        }
        size_t start[] = {k*FLen};
        size_t count[] = {FLen};
        for(j = 0; j < FLen; j++)
            data[j] = (int)(fdata_out[j+FLen] * tmp);
        H(nc_put_vara_short(ncid_out, i, start, count, data));

        tail = points % FLen;
        if(tail)
        {
            memset(fdata_in,  0, sizeof(double)*FLen);

            size_t start[]  = {k*FLen};
            size_t start2[] = {(k+1)*FLen};
            size_t count[]  = {tail};
            size_t count2[] = {FLen};
            H(nc_get_vara_short(ncid_in,  i, start, count, data));
            for(j = 0; j < tail; j++)
                fdata_in[j] = ((double)data[j]) / 32768.0;

            //      wsf.processWindow(fdata_in, FLen, fdata_out);
            wsf.process2(fdata_in, tail, fdata_out, FLen+tail);
            for(j = 0; j < FLen; j++)
                fdata_out[j] += ftail[j];

            for(j = 0; j < FLen; j++)
                data[j] = (int)(fdata_out[j] * tmp);
            H(nc_put_vara_short(ncid_out, i, start, count2, data));
            for(j = 0; j < tail; j++)
                data[j] = (int)(fdata_out[j+FLen] * tmp);
            H(nc_put_vara_short(ncid_out, i, start2, count, data));
        }

        delete data;
        delete fdata_in;
        delete fdata_out;
        delete ftail;
    }

    /* close */
    H(nc_close(ncid_in));
    H(nc_close(ncid_out));

    free_parameters_struct(&parms);
    return 0;
}
