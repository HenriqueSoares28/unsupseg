
#include "gft.h"



int main(int argc, char **argv){
  gft::sImageGraph *ig;
  gft::sCTree *ct;
  gft::sImage32 *grid;
  gft::sGraph *G,*Gnew;
  gft::sAdjRel *A;
  gft::sPQueue32 *Q;
  gft::sQueue *FIFO;
  gft::sBMap *visited;
  int *ext, *inv_mapping, *pred;
  int p,q,i,j,k,n,energy,ncols,nrows,extmax,w,wmax,qmax;
  FILE *fp,*fp_new;
  if(argc < 4){
    fprintf(stdout,"usage:\n");
    fprintf(stdout,"mergehistory_ctree <ncols> <nrows> <mergehistory>\n");
    exit(0);
  }

  fp_new = fopen((char *)"merge_new.txt", "w");
  if(fp_new == NULL){
    printf("Error opening file.\n");
    return 0;
  }
  ncols = atoi(argv[1]);
  nrows = atoi(argv[2]);

  /*
  ig = gft::ImageGraph::SaliencyMap(argv[3],
				    ncols,
				    nrows);
  grid = gft::ImageGraph::KhalimskyGrid(ig);
  gft::Image32::Write(grid, (char *)"khalimskygrid.pgm");
  ct = gft::CTree::EdgeBasedMinTree(ig, 0);
  */

  G = gft::Graph::Create(ncols*nrows, 4, NULL);

  fp = fopen(argv[3], "r");
  if(fp == NULL){
    printf("Warning: file not found\n");
    return 0;
  }

  fscanf(fp, "%d", &n);
  for(i = 0; i < n; i++){
    fscanf(fp, "%d %d %d", &p, &q, &energy);
    gft::Graph::AddEdge(G, p, q, energy);
  }
  fclose(fp);

  ct = gft::CTree::EdgeBasedMinTree(G, 0);

  gft::CTree::ComputeHeight(ct);
  gft::CTree::ComputeArea(ct, NULL);
  gft::CTree::ComputeVolume(ct);
  //gft::CTree::Print(ct);

  //gft::height  gft::area
  ext = gft::CTree::ComputeExtinctionValue(ct, gft::volume);
  //printf("ext (volume): ");
  extmax = 0;
  for(i = 0; i < ct->nleaves; i++){
    //printf("%d  ", ext[i]);
    if(ext[i] > extmax)
      extmax = ext[i];
  }
  printf("\n");

  inv_mapping = (int *)malloc(sizeof(int)*ct->nleaves);
  for(p = 0; p < G->nnodes; p++){
    i = ct->mapping[p];
    if(i < ct->nleaves)
      inv_mapping[i] = p;
  }

  Gnew = gft::Graph::Clone(G);

  Q = gft::PQueue32::Create(extmax+1, ct->nleaves, ext);
  for(i = 0; i < ct->nleaves; i++)
    gft::PQueue32::InsertElem(&Q, i);

  visited = gft::BMap::Create(G->nnodes);
  
  FIFO = gft::Queue::Create(G->nnodes);
  pred = (int *)malloc(sizeof(int)*G->nnodes);
  
  i = gft::PQueue32::FastRemoveMaxFIFO(Q);
  gft::BMap::Set1(visited, inv_mapping[i]);

  fprintf(fp_new, "%d\n", n);
  while(!gft::PQueue32::IsEmpty(Q)){
    i = gft::PQueue32::FastRemoveMaxFIFO(Q);
    p = inv_mapping[i];

    gft::Queue::Push(FIFO, p);
    pred[p] = NIL;
    while(!gft::Queue::IsEmpty(FIFO)){
      p = gft::Queue::Pop(FIFO);
      if(gft::BMap::Get(visited, p))
	break;
      for(k = 0; k < G->nodes[p].outdegree; k++){
	q = G->nodes[p].adjList[k];
	if(pred[p] != q){
	  pred[q] = p;
	  gft::Queue::Push(FIFO, q);
	}
      }
    }

    gft::Queue::Reset(FIFO);
    
    wmax = 0;
    q = p;
    while(pred[q] != NIL){
      w = gft::Graph::GetArcWeight(G, pred[q], q);
      if(w > wmax){
	wmax = w;
	qmax = q;
      }
      q = pred[q];
    }

    //printf("ext: %d, p_qmax: %d, qmax: %d\n", ext[i], pred[qmax], qmax);
    fprintf(fp_new, "%d %d %d\n", pred[qmax], qmax, ext[i]);
    
    gft::Graph::RemoveEdge(G, pred[qmax], qmax);
    gft::Graph::UpdateEdge(Gnew, pred[qmax], qmax, ext[i]);

    p = inv_mapping[i];
    gft::BMap::Set1(visited, p);
  }

  for(p = 0; p < Gnew->nnodes; p++){
    for(k = 0; k < Gnew->nodes[p].outdegree; k++){
      q = Gnew->nodes[p].adjList[k];
      w = Gnew->nodes[p].Warcs[k];
      if(q > p && w == 0)
	fprintf(fp_new, "%d %d %d\n", p, q, w);
    }
  }
  fclose(fp_new);

  
  ig = gft::ImageGraph::SaliencyMap((char*)"merge_new.txt",
				    ncols,
				    nrows);
  grid = gft::ImageGraph::KhalimskyGrid(ig);
  gft::Image32::Write(grid, (char *)"saliencymap_new.pgm");

  
  gft::Image32::Destroy(&grid);
  gft::ImageGraph::Destroy(&ig);
  gft::PQueue32::Destroy(&Q);
  gft::Queue::Destroy(&FIFO);
  gft::Graph::Destroy(&Gnew);
  gft::Graph::Destroy(&G);
  gft::CTree::Destroy(&ct);
  gft::BMap::Destroy(&visited);
  free(inv_mapping);
  free(ext);
  
  return 0;
}

