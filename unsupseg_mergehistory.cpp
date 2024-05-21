
#include "gft.h"

#include <string>

#define IMG_COLOR     1
#define IMG_GRAYSCALE 0

gft::sCImage *ReadAnyCImage(char *file){
  gft::sCImage *cimg;
  char command[512];
  int s;

  s = strlen(file);
  if(strcasecmp(&file[s-3], "ppm") == 0){
    cimg = gft::CImage::Read(file);
  }
  else{
    sprintf(command, "convert %s cimage_tmp.ppm", file);
    system(command);
    cimg = gft::CImage::Read("cimage_tmp.ppm");
    system("rm cimage_tmp.ppm");
  }
  return cimg;
}


gft::sImage32 *ReadAnyImage(char *file){
  gft::sImage32 *img;
  char command[512];
  int s;

  s = strlen(file);
  if(strcasecmp(&file[s-3], "pgm") == 0){
    img = gft::Image32::Read(file);
  }
  else{
    sprintf(command, "convert %s image_tmp.pgm", file);
    system(command);
    img = gft::Image32::Read("image_tmp.pgm");
    system("rm image_tmp.pgm");
  }
  return img;
}


int Find(gft::sImage32 *R, int p){
  if(R->data[p] == p)
    return p;
  else{
    R->data[p] = Find(R, R->data[p]);
    return R->data[p];
  }
}


void IncludeSPinHistory(gft::Graph::sMergeHistory **MH,
			gft::sGraph *G,
			gft::sImage32 *spixels){
  gft::Graph::sMergeHistory *NMH;
  gft::sImage32 *R;
  gft::sAdjRel *A;
  int p,q,px,py,qx,qy,lp,lq,i,j,k,rp,rq;
  int **Mp = NULL;
  int **Mq = NULL;
  bool find;
  A = gft::AdjRel::Neighborhood_4();

  R = gft::Image32::Create(spixels);
  for(p = 0; p < R->n; p++){
    R->data[p] = p;
  }
  NMH = (gft::Graph::sMergeHistory* )malloc(sizeof(gft::Graph::sMergeHistory)*(spixels->n-1));
  Mp = (int **)malloc(sizeof(int *)*G->nnodes);
  Mq = (int **)malloc(sizeof(int *)*G->nnodes); 
  for(i = 0; i < G->nnodes; i++){
    Mp[i] = (int *)malloc(sizeof(int)*G->nodes[i].outdegree);
    Mq[i] = (int *)malloc(sizeof(int)*G->nodes[i].outdegree);    
  }

  j = spixels->n-2;
  for(p = 0; p < spixels->n; p++){
    lp = spixels->data[p];
    px = p%spixels->ncols;
    py = p/spixels->ncols;
    for(i = 1; i < A->n; i++){
      qx = px + A->dx[i];
      qy = py + A->dy[i];
      if(gft::Image32::IsValidPixel(spixels, qx, qy)){
	q = qx + qy*spixels->ncols;
	lq = spixels->data[q];
	if(lp != lq){
	  //-----------------
	  find = false;
	  for(k = 0; k < G->nodes[lp].outdegree; k++){
	    if(lq == G->nodes[lp].adjList[k]){
	      find = true;
	      Mp[lp][k] = p;
	      Mq[lp][k] = q;
	      break;
	    }
	  }
	  if(!find)
	    printf("Error\n");
	  //-----------------
	}
	else{ //lp == lq
	  rp = Find(R, p);
	  rq = Find(R, q);
	  if(rp != rq){
	    R->data[rp] = rq;
	    NMH[j].energy = 0;
	    NMH[j].p = p;
	    NMH[j].q = q;
	    j--;
	  }
	}
      }
    }
  }

  if(j != G->nnodes-2)
    printf("Error\n");
  
  for(i = 0; i < G->nnodes-1; i++){
    NMH[i].energy = (*MH)[i].energy + 1;
    lp = (*MH)[i].p;
    lq = (*MH)[i].q;
    find = false;
    for(k = 0; k < G->nodes[lp].outdegree; k++){
      if(lq == G->nodes[lp].adjList[k]){
	find = true;
	NMH[i].p = Mp[lp][k];
	NMH[i].q = Mq[lp][k];
      }
    }
    if(!find){
      printf("Error: lp: %d, lq: %d\n",lp, lq);
    }
  }
  
  for(i = 0; i < G->nnodes; i++){
    free(Mp[i]);
    free(Mq[i]);
  }
  free(Mp);
  free(Mq);
  gft::Image32::Destroy(&R);
  gft::AdjRel::Destroy(&A);
  free(*MH);
  *MH = NMH;
}


int main(int argc, char **argv){
  gft::sCImage *cimg=NULL, *ctmp;
  gft::sImage32 *img=NULL, *spixels, *grid;
  gft::sAdjRel *A;
  gft::sGraph *G;
  char filename[512];
  char command[512];
  int k_sp, type, n, p, sp, method, SPsize = 100;
  float alpha, polarity = 0.5;
  clock_t end, start;
  double totaltime;
  gft::Graph::sMergeHistory *MH = NULL;
  gft::sImageGraph *ig;
  
  if(argc < 4){
    fprintf(stdout,"usage:\n");
    fprintf(stdout,"unsupseg_mergehistory <type> <image> <method> [pol] [SPsize]\n");
    fprintf(stdout,"\t type.... 0: grayscale\n");
    fprintf(stdout,"\t          1: color\n");
    fprintf(stdout,"\t method...0: Divisive clustering by OIFT\n");
    fprintf(stdout,"\t          1: Divisive clustering by MST\n");
    fprintf(stdout,"\t          2: Divisive clustering by OIFT with DCCsize\n");
    fprintf(stdout,"\t pol..... Boundary polarity in [-1.0, 1.0]\n");
    fprintf(stdout,"\t SPsize...Superpixel size in pixels\n");
    exit(0);
  }

  type = atoi(argv[1]);
  strcpy(filename, argv[2]);
  method = atoi(argv[3]);
  if(argc >= 5)
    polarity = atof(argv[4]);
  printf("pol: %f\n", polarity);
  if(argc >= 6)
    SPsize = atoi(argv[5]);
  printf("SPsize: %d\n", SPsize);
  
  if(type == IMG_GRAYSCALE){
    img  = ReadAnyImage(filename);
    cimg = gft::CImage::Clone(img);
  }
  else
    cimg = ReadAnyCImage(filename);

  n = cimg->C[0]->n;

  if(method >= 0 && method <= 2){
    //-------------------------------------------------
    // IFT-SLIC:
    k_sp = ROUND(n/((float)SPsize));
    alpha = 0.06;
    start = clock();
    if(type == IMG_GRAYSCALE)
      spixels = gft::Superpixels::IFT_SLIC(img, k_sp, alpha, 
					   0.5, 0.5, 10); 
    else
      spixels = gft::Superpixels::IFT_SLIC(cimg, k_sp, alpha, 
					   0.5, 0.5, 10);
    end = clock();
    totaltime = ((double)(end-start))/CLOCKS_PER_SEC;
    printf("IFT-SLIC Time: %f sec\n", totaltime);

    printf("(Min,Max): (%d %d)\n",
	   gft::Image32::GetMinVal(spixels),
	   gft::Image32::GetMaxVal(spixels));
    
    gft::Image32::Write(spixels, (char *)"spixels.pgm");
    ctmp = gft::CImage::RandomColorize(spixels);
    gft::CImage::Write(ctmp, (char *)"spixels.ppm");
    gft::CImage::Destroy(&ctmp);
    
    /*Region Adjacency Graph (RAG)*/
    if(type == IMG_GRAYSCALE){
      if(method == 1)
	G = gft::Graph::RAG(img, spixels);
      else
	G = gft::Graph::RAG(img, spixels, polarity);
    }
    else
      G = gft::Graph::RAG(cimg, spixels);

    start = clock();
    switch(method){
    case 0:
      MH = gft::Graph::DivisiveClusteringByOIFT(G);
      break;
    case 1:
      //MH = gft::Graph::DivisiveClusteringByMST(G);
      printf("To be implemented\n"); return 0;
      break;
    case 2:
      MH = gft::Graph::DivisiveClusteringByOIFT_2(G);
      break;      
    }
    end = clock();
    totaltime = ((double)(end-start))/CLOCKS_PER_SEC;
    printf("Clustering Time: %f sec\n", totaltime);

    IncludeSPinHistory(&MH, G, spixels);
    gft::Graph::WriteMergeHistory(MH, spixels->n-1, (char*)"merge.txt");

    ig = gft::ImageGraph::SaliencyMap((char*)"merge.txt",
				      spixels->ncols,
				      spixels->nrows);
    grid = gft::ImageGraph::KhalimskyGrid(ig);
    gft::Image32::Write(grid, (char *)"saliencymap.pgm");

    free(MH);
    gft::Graph::Destroy(&G);
    gft::Image32::Destroy(&spixels);
    gft::Image32::Destroy(&grid);
    gft::ImageGraph::Destroy(&ig);
    //-------------------------------------------------  
  }
  else
    return 0;

  if(cimg != NULL) gft::CImage::Destroy(&cimg);
  if(img != NULL)  gft::Image32::Destroy(&img);
  return 0;
}

