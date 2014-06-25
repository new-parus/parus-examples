printf("Node %d body starting\n",px_node);
fflush(stdout);
/*

  File specification:
  
  input.dat stores the input (text)
  wght####.dat stores weights for the (####-1)th layer (text)
  
  output.dat will store the output (text)
  
*/

FILE *source;
//printf("alive(1)\n"); fflush(stdout);
double *net_input=(double *)malloc(num_ent*sizeof(double));
//printf("alive(2)\n"); fflush(stdout);
int i;

source=fopen("input.dat","rt");
if (source==NULL) {
  printf("Input file not found\n");
  return -1;
}
//printf("alive(3)\n"); fflush(stdout);
for (i=0; i<num_ent; i++) 
//  printf("reading input item #%d/%d\n",i,num_ent), fflush(stdout),
  fscanf(source,"%lf",&net_input[i]); 

//printf("alive(4)\n"); fflush(stdout);
printf("Node %d body done\n",px_node); 
fflush(stdout);