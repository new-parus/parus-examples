
printf("before free data_out=%p node=%d\n",data_out,px_node);
fflush(stdout);

free(data_out);

printf("before free data_in=%p node=%d\n",data_in,px_node);
fflush(stdout);

free(data_in);

printf("before free data_in_double=%p node=%d\n",data_in_double,px_node);
fflush(stdout);

free(data_in_double);

printf("after all free node=%d\n",px_node);
fflush(stdout);
