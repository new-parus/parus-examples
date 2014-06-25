int j, i;
double *input=(double *)malloc(num_ent*sizeof(double));

if (input==NULL) printf("%d: failed to malloc input\n",px_node), fflush(stdout),  exit(1);

double **weights=(double **)malloc(get_width*sizeof(double *));

if (weights==NULL) printf("%d: failed to malloc weights\n",px_node), fflush(stdout),  exit(1);

for (i=0; i<get_width; i++) {
  weights[i]=(double *)malloc(num_ent*sizeof(double));
  if (weights[i]==NULL) printf("%d: failed to malloc weights[%d]\n",px_node,i), fflush(stdout),  exit(1);}
  
double *output=(double *)malloc(get_width*sizeof(double));

if (output==NULL) printf("%d: failed to malloc output\n",px_node), fflush(stdout), exit(1);

printf("Node %d head working\n",px_node); 

fflush(stdout);
