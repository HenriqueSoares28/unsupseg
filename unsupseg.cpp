
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



int main(int argc, char **argv){
  gft::sCImage *cimg=NULL, *ctmp;
  gft::sImage32 *img=NULL, *label, *spixels;
  gft::sAdjRel *A;
  gft::sGraph *G;
  int *label_G;
  char filename[512];
  char command[512];
  int k, k_sp, type, n, p, sp, method, SPsize = 100;
  float alpha, polarity = 0.5;
  clock_t end, start;
  double totaltime;
  
  if(argc < 5){
    fprintf(stdout,"usage:\n");
    fprintf(stdout,"unsupseg <type> <image> <method> <k> [pol] [SPsize]\n");
    fprintf(stdout,"\t type.... 0: grayscale\n");
    fprintf(stdout,"\t          1: color\n");
    fprintf(stdout,"\t method...0: Divisive clustering by OIFT\n");
    fprintf(stdout,"\t          1: Divisive clustering by MST\n");
    fprintf(stdout,"\t          2: Divisive clustering by OIFT with DCCsize\n");
    fprintf(stdout,"\t k....... The desired number of regions\n");
    fprintf(stdout,"\t pol..... Boundary polarity in [-1.0, 1.0]\n");
    fprintf(stdout,"\t SPsize...Superpixel size in pixels\n");
    exit(0);
  }

  type = atoi(argv[1]);
  strcpy(filename, argv[2]);
  method = atoi(argv[3]);
  k = atoi(argv[4]);
  if(argc >= 6)
    polarity = atof(argv[5]);
  printf("k: %d, pol: %f\n", k, polarity);
  if(argc >= 7)
    SPsize = atoi(argv[6]);
  printf("SPsize: %d\n", SPsize);
  
  if(type == IMG_GRAYSCALE){
    img  = ReadAnyImage(filename);
    cimg = gft::CImage::Clone(img);
  }
  else
    cimg = ReadAnyCImage(filename);

  n = cimg->C[0]->n;
  label = gft::Image32::Create(cimg->C[0]);

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
    case 0: label_G = gft::Graph::DivisiveClusteringByOIFT(G, k); break;
    case 1: label_G = gft::Graph::DivisiveClusteringByMST(G, k);  break;
    case 2: label_G = gft::Graph::DivisiveClusteringByOIFT_2(G, k); break;
    }
    end = clock();
    totaltime = ((double)(end-start))/CLOCKS_PER_SEC;
    printf("Clustering Time: %f sec\n", totaltime);
    
    for(p = 0; p < n; p++){
      sp = spixels->data[p];
      label->data[p] = label_G[sp];
    }
    
    gft::FreeIntArray(&label_G);
    gft::Graph::Destroy(&G);
    gft::Image32::Destroy(&spixels);
    //-------------------------------------------------  
  }
  else
    return 0;
  
  gft::Image32::Write(label, (char *)"label.pgm");
  ctmp = gft::CImage::RandomColorize(label);
  gft::CImage::Write(ctmp, (char *)"label.ppm");
  gft::CImage::Destroy(&ctmp);
  ctmp = gft::CImage::ColorizeByAverageColor(cimg, label);
  gft::CImage::Write(ctmp, (char *)"labelcolor.ppm");
  gft::CImage::Destroy(&ctmp);
  
  printf("(Min,Max): (%d %d)\n",
	 gft::Image32::GetMinVal(label),
	 gft::Image32::GetMaxVal(label));

  gft::Image32::Destroy(&label);
  if(cimg != NULL) gft::CImage::Destroy(&cimg);
  if(img != NULL)  gft::Image32::Destroy(&img);
  return 0;
}

