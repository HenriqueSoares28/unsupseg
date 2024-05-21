
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



int main(int argc, char **argv){
  gft::sCImage *cimg=NULL, *ctmp;
  gft::sImage32 *img=NULL, *R, *label;
  char file_img[512];
  char file_his[512];  
  int type, n, i, nMH, p, q, energy, k, rp, rq, rmax, l;
  clock_t end, start;
  double totaltime;
  gft::Graph::sMergeHistory *MH = NULL;
  FILE *fp = NULL;
  int *hist, *mapping;
  
  if(argc < 5){
    fprintf(stdout,"usage:\n");
    fprintf(stdout,"unsupseg <type> <image> <mergehistory> <k>\n");
    fprintf(stdout,"\t type.... 0: grayscale\n");
    fprintf(stdout,"\t          1: color\n");
    fprintf(stdout,"\t k....... The desired number of regions\n");
    exit(0);
  }

  type = atoi(argv[1]);
  strcpy(file_img, argv[2]);
  strcpy(file_his, argv[3]);
  k = atoi(argv[4]);
  
  if(type == IMG_GRAYSCALE){
    img  = ReadAnyImage(file_img);
    cimg = gft::CImage::Clone(img);
  }
  else
    cimg = ReadAnyCImage(file_img);

  n = cimg->C[0]->n;

  start = clock();
  fp = fopen(file_his, "r");
  if(fp == NULL){
    printf("Cannot open file %s\n",file_his);
    return 0;
  }
  fscanf(fp, "%d", &nMH);
  MH = (gft::Graph::sMergeHistory *)malloc(sizeof(gft::Graph::sMergeHistory)*nMH);
  for(i = 0; i < nMH; i++){
    fscanf(fp, "%d %d %d", &p, &q, &energy);
    MH[i].p = p;
    MH[i].q = q;
    MH[i].energy = energy;
  }
  fclose(fp);

  R = gft::Image32::Create(cimg->C[0]->ncols,
			   cimg->C[0]->nrows);
  for(p = 0; p < R->n; p++)
    R->data[p] = p;
  for(i = nMH-1; i >= k-1; i--){
    p = MH[i].p;
    q = MH[i].q;
    rp = Find(R, p);
    rq = Find(R, q);
    R->data[rp] = rq;
  }
  for(p = 0; p < R->n; p++)
    rp = Find(R, p);

  //Fixing the values to match the number of partitions:
  rmax = gft::Image32::GetMaxVal(R);
  hist = gft::AllocIntArray(rmax+1);
  for(p = 0; p < R->n; p++){
    hist[R->data[p]] += 1;
  }
  mapping = gft::AllocIntArray(rmax+1);
  i = 0;
  for(l = 0; l < rmax+1; l++){
    if(hist[l] > 0){
      mapping[l] = i;
      i++;
    }
  }
  label = gft::Image32::Create(R);
  for(p = 0; p < R->n; p++)
    label->data[p] = mapping[R->data[p]];
  gft::FreeIntArray(&mapping);
  gft::FreeIntArray(&hist);

  gft::Image32::Write(label, (char *)"label.pgm");
  ctmp = gft::CImage::RandomColorize(label);
  gft::CImage::Write(ctmp, (char *)"label.ppm");
  gft::CImage::Destroy(&ctmp);
  
  end = clock();

  totaltime = ((double)(end-start))/CLOCKS_PER_SEC;
  printf("Time: %f sec\n", totaltime);

  free(MH);
  if(cimg != NULL) gft::CImage::Destroy(&cimg);
  if(img != NULL)  gft::Image32::Destroy(&img);
  gft::Image32::Destroy(&label);
  gft::Image32::Destroy(&R);
  return 0;
}

