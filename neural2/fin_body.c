printf("The executing of the node %d body started.\n",px_node);
fflush(stdout);
 
FILE *resfile=fopen("output.dat","w+");
for (int i=0; i<num_res; i++)
  fprintf(resfile,"%lf ",result[i]),
  printf("%lf ",result[i]);  
fclose(resfile);

free(result);
printf("The executing of the node %d body is done\n",px_node);
fflush(stdout);
