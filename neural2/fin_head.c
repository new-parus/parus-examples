double *result=(double *)malloc(num_res*sizeof(double));
if (result==NULL) printf("%d: failed to malloc result\n",px_node), fflush(stdout),  exit(1);
printf("Node %d head working\n",px_node);
fflush(stdout);
