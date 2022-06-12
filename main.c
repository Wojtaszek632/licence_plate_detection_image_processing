/* arm-linux-gnueabihf-gcc main.c -static -mfpu=neon -ftree-vectorize -o main */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>




struct image
{
  char p;
  int format;
  int width;
  int height;
  int intensity; 
  unsigned char *pixels;
};

void grayScale(uint8_t * dest, uint8_t * src, int num);
void histEqualization(uint8_t * dest, uint8_t * src, int num, int intensity);
void otsuThreshold(uint8_t * dest, uint8_t * src, int num, int greyLevels);
void conv(unsigned char * gray_in, unsigned char * gray_out, float kernel[3][3],int Width,int Height);
void dilation(unsigned char * gray_in, unsigned char * gray_out,int Width,int Height);
void saveAsPGM(const char * filename,struct image *image,unsigned char * data);
void blobdetection(unsigned char * gray_in,unsigned char * org_img,int Width,int Height,int distThreshold);
typedef struct Blob
{
  float min_x;
  float min_y;
  float max_x;
  float max_y;
}Blob;


float  sobelx[3][3] ={
      {-1,0,1},
      {-2,0,2},
      {-1,0,1}
  };

float  sobely[3][3] ={
      {-1,-2,-1},
      {0,0,0},
      {1,2,1}
  };

int main (int argc, char* argv[])
{
  struct image m;
  
    
 FILE *fp = fopen (argv[1], "rb");	//fopen(...);
 if (fp == 0)
{    printf("Could not open 1 file");
     return -1;
  }
 
 fscanf  (fp, "%c%d\n", &m.p, &m.format);   
 fscanf  (fp, "%d %d\n", &m.width, &m.height);
 fscanf  (fp, "%d\n", &m.intensity);

printf ("Version: %c%d\n", m.p, m.format);
printf ("Dims: %d %d\n", m.width, m.height);
printf ("Max val: %d\n", m.intensity);


  m.pixels = (unsigned char *) malloc (m.width * m.height * 3 *sizeof (unsigned char));

  //fread(m.pixels, sizeof *(m.pixels), size, fp);
  fread (m.pixels, sizeof (*m.pixels), m.width * m.height * 3, fp);

     
  unsigned char *outpixels_grayscale,*outpixels_hist_equalized,*outpixels_otsu,*outpixels_sobelx,*outpixels_sobely,*outpixels_sobel_merge;
  unsigned char  *outpixels_dilete,*outpixels_crop;
  outpixels_grayscale = (unsigned char *) malloc (m.width * m.height * sizeof (unsigned char));
  
 
  
  
 
 
 
  grayScale (outpixels_grayscale, m.pixels, m.width * m.height);
  saveAsPGM("grayscaled.pgm",&m, outpixels_grayscale);
  printf("Done grayScale\n");
  free(m.pixels);

  outpixels_hist_equalized = (unsigned char *) malloc (m.width * m.height * sizeof (unsigned char));
  histEqualization(outpixels_hist_equalized,outpixels_grayscale, m.width * m.height, m.intensity);
  printf("Done histEqualization\n");
  //free(outpixels_grayscale);

  outpixels_otsu = (unsigned char *) malloc (m.width * m.height * sizeof (unsigned char));
  //otsuThreshold (outpixels_otsu, outpixels_hist_equalized, m.width * m.height,m.intensity);
  for(int i=0;i<m.width * m.height;i++){
  if(outpixels_hist_equalized[i]>=215)
  {
    outpixels_otsu[i]=255;
  }
  
  else{
    outpixels_otsu[i]=0;
  }

  }
  saveAsPGM("tresholded.pgm",&m, outpixels_otsu); 
  printf("Done otsuThreshold\n");
  free(outpixels_hist_equalized);

  outpixels_sobelx = (unsigned char *) malloc (m.width * m.height * sizeof (unsigned char));
  conv(outpixels_otsu, outpixels_sobelx, sobelx, m.width, m.height);
  printf("Done convx\n"); 
  outpixels_sobely = (unsigned char *) malloc (m.width * m.height * sizeof (unsigned char));
  conv(outpixels_sobelx, outpixels_sobely, sobely, m.width, m.height);
  printf("Done convy\n");
  free(outpixels_otsu);

  outpixels_sobel_merge = (unsigned char *) malloc (m.width * m.height * sizeof (unsigned char));
  for (int i=0; i < m.width * m.height; i++) 
    {
     // if (sqrt ( outpixels_sobelx[i]*outpixels_sobelx[i] +  outpixels_sobely[i]) > 170)
     if (outpixels_sobely[i]> 215)
      {
        outpixels_sobel_merge[i] = 255;
      }
      else 
      {
          outpixels_sobel_merge[i] = 0;
      }
     //outpixels_sobel_merge[i]= sqrt ( outpixels_sobelx[i]*outpixels_sobelx[i] +  outpixels_sobely[i]*outpixels_sobely[i]);
    }
   saveAsPGM("edge.pgm",&m, outpixels_sobel_merge); 
   printf("Done merge\n");
   free(outpixels_sobelx);
   free(outpixels_sobely);

  outpixels_dilete = (unsigned char *) malloc (m.width * m.height * sizeof (unsigned char));
  dilation(outpixels_sobel_merge, outpixels_dilete, m.width , m.height);
  printf("Done dilation\n");
  for (int i=0;i<12;i++)
  {
     dilation(outpixels_dilete, outpixels_dilete, m.width , m.height);
      printf("Done dilation %d\n",i+2);
  }
  saveAsPGM("dialeted.pgm",&m, outpixels_dilete);
  free(outpixels_sobel_merge);
  

 blobdetection( outpixels_dilete,outpixels_grayscale,m.width,m.height,15);
 // fprintf (fpout1, "P5\n");
 // fprintf (fpout1, "%d %d\n", m.width, m.height);
 // fprintf (fpout1, "%d\n\n", m.intensity);
 // fwrite (outpixels_dilete, sizeof (*outpixels_dilete), m.width * m.height, fpout1);
 // fclose (fp);
 // fclose (fpout1);
     printf("Done\n");
  return 0;
}

void grayScale(uint8_t * dest, uint8_t * src, int num)
{
  int blacked_counter = 0;
  for (int i=0; i < num; i++) 
    {
     dest[i] = src[3*i]/3 + src[3*i+1]/3 + src[3*i+2]/3;
     if ( dest[i] < 20) blacked_counter++;
    }
    if ( blacked_counter > (num * 0.8))  printf("BLACKED IMAGE DETECTED\n");
    else printf("Non zero image grayscaled\n");
    
}

void histEqualization(uint8_t * dest, uint8_t * src,int num, int intensity)
{
 
 int *H = (int *) calloc ( intensity + 1 , sizeof (int) );
 
 unsigned char gmin = intensity;

 int *Hc = (int *) calloc ( intensity + 1 , sizeof (int) );

float *T = (float *) calloc ( intensity + 1 , sizeof (float) );
  
  
  for (int p=0; p < num; p++) 
    {
     H[src[p]] =  H[src[p]] + 1;
           
    }

     for (int p=0; p < 256; p++) 
    {
     //printf ("Liczba:%d dla wartosci: %d\n", H[p],p);        
    }

  for (int i=intensity; i >= 0; i--)
  {
    if (H[i] > 0)
    {
      gmin = i;
    }
  }

 // printf ("Min val %d\n", gmin); 

  Hc[0] = H[0];

  for (int g=1; g <= intensity; g++)
  {
    Hc[g] = Hc[g - 1] + H[g];

  //printf ("cum = %d dla %d\n", Hc[g],g); 
 
  }

  unsigned char Hmin =  Hc[gmin];

  for (int g=0; g <= intensity; g++)
  {
   T[g] = Hc[g] - Hmin;
   T[g] = T[g] / (num - Hmin);
  // printf ("%d - %d / %d - %d = %f dla %d\n",Hc[g],Hmin,num,Hmin, T[g],g); 

   T[g] = round( T[g] * intensity);
  }

  for (int i=0; i < num; i++) 
    {
     dest[i]= T[src[i]];
     //printf ("Zapisano = %d dla %d\n", dest[i],i); 
    }
} 

void otsuThreshold(uint8_t * dest, uint8_t * src,int num, int greyLevels)
{
 
 float *H = (float *) calloc ( greyLevels , sizeof (float) );
 float *Hnormalized = (float *) calloc ( greyLevels , sizeof (float) );
 float *tresholdsVar = (float *) calloc ( greyLevels-1 , sizeof (float) );

  
 // compute gray-level histogram  
 for (int p=0; p < num; p++) 
 {
    H[src[p]] =  H[src[p]] + 1;           
 }
 // normalize the histogram
 for (int i=0; i < greyLevels; i++) 
 {
   // printf (" Hist = %.6f dla %d\n",  H[i],i); 
    Hnormalized[i] =  H[i] / num;         
   // printf ("Norm Hist = %.6f dla %d\n",  Hnormalized[i],i);
 }
   
 // for each possible threshold
 for (int t=0; t < greyLevels-1; t++)
 {
   // printf ("----------------For treshold= %d ------------\n",  t ); 
    
    //compute variance for background B
    float backgroundAvg = 0;
    int bcksum = 0;
    float backgroundVariance = 0;
 
    for (int i=0; i<= t; i++){backgroundAvg += i * H[i];bcksum += H[i];}
    if (bcksum == 0){tresholdsVar[t] = NAN;  printf("No elements in the background!\n");continue;} 
    backgroundAvg = backgroundAvg / bcksum;
    //printf ("backgroundAvg= %.6f \n",  backgroundAvg );   
    
    for (int i=0; i<= t; i++) {backgroundVariance +=  pow((i - backgroundAvg),2) * H[i]; }
    backgroundVariance = backgroundVariance / bcksum;
    //printf ("backgroundVariance = %.6f \n\n",  backgroundVariance);   

     //compute variance for foreground F 
    float foregroundAvg = 0;
    int foresum = 0;
    float foregroundVariance = 0;

   for (int i=(t+1); i< greyLevels; i++) {foregroundAvg += i * H[i];foresum += H[i];}
   if (foresum == 0){tresholdsVar[t] = NAN;  printf("No elements in the foreground!\n");continue;}
   foregroundAvg = foregroundAvg / foresum ;  
   //printf ("foregroundAvg= %.6f \n",  foregroundAvg );   

   for (int i=(t+1); i< greyLevels; i++) {foregroundVariance += pow((i - foregroundAvg),2) * H[i]; }
   foregroundVariance = foregroundVariance / foresum;  
   //printf ("foregroundVariance = %.6f \n",  foregroundVariance);   

   //compute probabilities
   float wb=0;
   float wf=0;

   for (int j=0; j<= t; j++) { wb += Hnormalized[j]; }
   for (int j=(t+1); j< greyLevels; j++) { wf += Hnormalized[j]; }
   //printf ("wb = %.6f \n",  wb);  
   //printf ("wf = %.6f \n",  wf);  
  
   //compute end variance for treshold "t"
   tresholdsVar[t] = wb * backgroundVariance + wf * foregroundVariance;
   //printf ("tresholdsVar = %.6f \n",  tresholdsVar[t]);  
  
  }
  //find min treshold
  float treshold = INFINITY;
  int tresholdNum = 0;

  for (int t=0; t < greyLevels-1; t++)
  {  
    if (tresholdsVar[t] < treshold )
    {
      treshold =  tresholdsVar[t];
      tresholdNum = t;
    }
  }
  
 // printf ("----------treshold--------- \n%d \nvar= %.6f\n", tresholdNum,treshold);  

  //create new image according to set treshold
 for (int i=0; i < num; i++) 
 {
   if(src[i] <= tresholdNum) dest[i]= 0;
   else dest[i] = greyLevels - 1;    
 }  

 
} 

void conv(unsigned char * gray_in, unsigned char * gray_out,float kernel[3][3],int Width,int Height){

  const int width = Width;
  const int height = Height;
  

  unsigned char input [width][height];
 
  unsigned char output [width][height];

  int curflipedKernelX = 0;
  int curflipedKernelY = 0;

  int curInputX = 0;
  int curInputY = 0;
  
  for(int i=0;i<height*width;i++) {
  input[i%width][i/width]=gray_in[i];   
 // printf("Zapisano %d jako [%d][%d]\n",i,i%width,i/width);
  }
 
  for (int x=0; x < width; x++){
    
    for (int y=0; y < height; y++){
      
      for (int curKernelX=0; curKernelX < 3; curKernelX++){
        curflipedKernelX = 2 - curKernelX;
        for (int curKernelY=0; curKernelY < 3; curKernelY++){
          curflipedKernelY = 2 - curKernelY;

          curInputX = x + (1 - curflipedKernelX);
          curInputY = y + (1 - curflipedKernelY);

          if( curInputX >= 0 &&  curInputX < width && curInputY >= 0 && curInputY < height ){
            output[x][y] += input[ curInputX][curInputY] * kernel[curflipedKernelX][curflipedKernelY];  
          } 
        }
      }
    }
  }
  
  for (int y=0; y < height; y++){
    for (int x=0; x < width; x++){
      gray_out[x+(y*width)] = output[x][y];
     // printf("Zapisano [%d][%d] jako %d\n",x,y,x+(y*width));
      }
    }
}

void dilation(unsigned char * gray_in, unsigned char * gray_out,int Width,int Height){
 
  const int width = Width;
  const int height = Height;
  
  unsigned char input [width][height];
  unsigned char output [width][height];

    for(int i=0;i<height*width;i++) {
    input[i%width][i/width]=gray_in[i];    
    }

  for (int x=0; x < width; x++){
    for (int y=0; y < height; y++){
          bool hit = false;

          for(int horizontal = -1; horizontal < 1; horizontal = horizontal+2)
          { 
            if(hit == true) continue;
            int relative_horizontal = x + horizontal;
            if (relative_horizontal >= 0 && relative_horizontal <= width )
            {
              if(input[relative_horizontal][y]> 100){hit = true; 
              //printf("wykryto nalozenie dla %d %d\n",relative_horizontal,y); 
              }              
            } 
            //printf("Jestem w %d %d a jego wartosc to %d\n",relative_horizontal,y,input[relative_horizontal][y]);           
          }

          for(int vertical = -1; vertical < 1; vertical = vertical+2)
          {
            if(hit == true) continue;
            int relative_vertical = y + vertical;
            if (relative_vertical >= 0 && relative_vertical <= height )
            {
              if(input[x][relative_vertical]> 100)  { hit = true; // printf("wykryto nalozenie dla %d %d\n",x,relative_vertical); 
              }           
            }   
             //printf("Jestem w %d %d a jego wartosc to %d\n",x,relative_vertical,input[x][relative_vertical]);                  
          }

          if (hit == true) {output[x][y] = 50;}
     
  }
  }
   for (int x=0; x < width; x++){
    for (int y=0; y < height; y++){ if(output[x][y] == 50)output[x][y] = 255; } }

  for (int y=0; y < height; y++){
    for (int x=0; x < width; x++){
      gray_out[x+(y*width)] = output[x][y];     
      }
    }
}

void saveAsPGM(const char * filename,struct image *image,unsigned char * data){

FILE *fpout = fopen (filename, "w");
  fprintf (fpout, "P5\n");
  fprintf (fpout, "%d %d\n",image->width, image->height);
  fprintf (fpout, "%d\n\n", image->intensity);
  fwrite (data, sizeof (*data), image->width * image->height, fpout);
  fclose (fpout);
}


void blobdetection(unsigned char * gray_in,unsigned char * org_img,int Width,int Height,int distThreshold){
const int width = Width;
const int height = Height;
Blob blobarr[90000];
int blobCounter=0;
  //printf("1\n");
  unsigned char input [width][height];
 
    for(int i=0;i<height*width;i++) {
    input[i%width][i/width]=gray_in[i];    
    }
//////////////////

//printf("2\n");
printf("Blob detection starts!\n");
for (int x=0; x < width; x++){
    for (int y=0; y < height; y++){      
      if (input[x][y] > 250)
      {

       bool found = false;
       //printf("%d %d %d\n",x,y,blobCounter);
       //printf("Sprawdzam %d %d\n",x,y);  
       for(int i=0; i<=blobCounter;i++)
        {
         float cx = (abs(x - (int)blobarr[i].min_x) > abs(x-(int)blobarr[i].max_x)) ? abs(x-(int)blobarr[i].max_x): abs(x - (int)blobarr[i].min_x);
         float cy = (abs(y - (int)blobarr[i].min_y) > abs(y-(int)blobarr[i].max_y)) ? abs(y-(int)blobarr[i].max_y): abs(y - (int)blobarr[i].min_y);
         
 
         float d = (cx)*(cx) + (cy)*(cy);
          //printf("odleglosc od bloba nr %d wynosi %f\n",i,d);
          if (d < distThreshold){
            //printf("FOUND NEW BLOB AT %d %d\n",x,y);
          blobarr[i].min_x = (blobarr[i].min_x > x) ? x : blobarr[i].min_x;
          blobarr[i].min_y = (blobarr[i].min_y > y) ? y : blobarr[i].min_y;

          blobarr[i].max_x = (blobarr[i].max_x > x) ?  blobarr[i].max_x : x;
          blobarr[i].max_y = (blobarr[i].max_y > y) ?  blobarr[i].max_y : y;
        
          found = true;
          break;
          }
        }
       
        if (!found) {
          blobCounter++;
          blobarr[blobCounter].min_x = x;
          blobarr[blobCounter].min_y = y;
          blobarr[blobCounter].max_x = x;
          blobarr[blobCounter].max_y = y;
          
        }
      }
    }
  }
//printf("3\n");
int bestIndex = blobCounter;
float bestaspect = 50.0;
float perfectaspcet = 4.5;
float blobwidth=0.0;
float blobheight=0.0;
float aspect=0.0;
float difrence = 100.0;


for(int i=0; i<=blobCounter;i++){

   blobwidth = blobarr[i].max_x - blobarr[i].min_x;
   blobheight = blobarr[i].max_y - blobarr[i].min_y;
   aspect = blobwidth/blobheight;  
   difrence = fabs(aspect - perfectaspcet);

  //printf("aspect %f at X%f Y%f %f %f\n",aspect, blobarr[i].min_x,blobarr[i].min_y,blobarr[i].max_x,blobarr[i].max_y);
 // printf(" %d ",difrence <= bestaspect);
 // printf("diference: %f best:%f\n\n",difrence,bestaspect);

  if ( difrence <= fabs(bestaspect - perfectaspcet) && aspect > 0.0 && blobwidth >= (width /10) && blobheight > (height/12) && blobarr[i].min_x >= (width /7) && blobarr[i].min_y >= (height /3) ) {
    //printf("new best: %f\n\n",difrence);5)
    bestIndex = i;
    bestaspect=aspect;
    }

  
  
  }
//printf("4\n");
printf("Closest aspect %f at X%f Y%f         X%f Y%f\n",bestaspect,blobarr[bestIndex].min_x,blobarr[bestIndex].min_y, blobarr[bestIndex].max_x,blobarr[bestIndex].max_y);
printf("Blobs:%d\n",blobCounter);
//--------------------------------------------------------

unsigned char crop [width][height];
 for(int i=0;i<height*width;i++) {
    crop[i%width][i/width]=org_img[i];    
    }

 int blobwidthi = blobarr[bestIndex].max_x - blobarr[bestIndex].min_x;
 int blobheighti = blobarr[bestIndex].max_y - blobarr[bestIndex].min_y;

 unsigned char  *outpixels_crop;
  outpixels_crop = (unsigned char *) malloc (blobwidthi * blobheighti * sizeof (unsigned char));
  


for (int y=0; y < blobheighti; y++){
    for (int x=0; x < blobwidthi; x++){
       outpixels_crop[x+(y*blobwidthi)] = crop[x+(int)blobarr[bestIndex].min_x][y+(int)blobarr[bestIndex].min_y];
       //printf("Zapisano [%d][%d] [%d][%d] jako %d\n",x,y,x+(int)blobarr[bestIndex].min_x,y+(int)blobarr[bestIndex].min_y,x+(y*blobwidthi));
      }
    }



FILE *fpout = fopen ("crop.pgm", "w");

  fprintf (fpout, "P5\n");
  fprintf (fpout, "%d %d\n",blobwidthi, blobheighti);
  fprintf (fpout, "%d\n\n", 255);
  fwrite (outpixels_crop, sizeof (*outpixels_crop), blobwidthi * blobheighti, fpout);
  fclose (fpout);
  printf("Croped & Saved!\n");

}
