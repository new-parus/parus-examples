printf("Node %d body starting\n",px_node);
fflush(stdout);

int source;

source=open("weights.dat",O_RDONLY);
if (source==-1) { printf("Data file missing\n"); return -1; }

lseek(source,get_wt_start*sizeof(double),SEEK_SET);

printf("Node %d alive (1)\n",px_node);

for (i=0; i<get_width; i++)
  for (j=0; j<num_ent; j++)
	read(source,&weights[i][j],sizeof(double));

printf("Input:\n");
for (j=0; j<num_ent; j++)
	printf(" %lf",input[j]);
printf("\n");
printf("Weight factors:\n");
for (i=0; i<get_width; i++) {
  for (j=0; j<num_ent; j++)
	printf(" %lf",weights[i][j]);
  printf("\n"); }

printf("Node %d alive (2) width=%d wt_start=%d\n",px_node, get_width, get_wt_start);

printf("Output:\n");
for (i=0; i<get_width; i++) {
  output[i]=0;
  for (j=0; j<num_ent; j++)
	output[i]+=weights[i][j]*input[j];
	printf(" %lf",output[i]);
  if (output[i]>1) output[i]=1; else output[i]=0;
}

printf("\n");

printf("Node %d alive (3)\n",px_node);

close(source);
printf("Node %d alive (3.5)\n",px_node);
free(input);

printf("Node %d alive (4)\n",px_node);

for (i=0; i<get_width; i++) free(weights[i]);
printf("Node %d body done\n",px_node);
fflush(stdout);
