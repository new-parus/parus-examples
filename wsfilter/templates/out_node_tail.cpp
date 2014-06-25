
printf("before free data_out=%p node=%d\n",data_out,px_node);
fflush(stdout);

free(data_out);

printf("before free result_data=%p node=%d\n",result_data,px_node);
fflush(stdout);

free(result_data);

printf("after free result_data node=%d\n",px_node);
fflush(stdout);