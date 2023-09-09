#include "triangle.h"
#include "display.h"
#include <math.h>

void fill_flat_bottom_triangle(int x0,int y0, int x1, int y1, int x2, int y2, uint32_t color){
  

    float x_start = x0;
    float x_end = x0;
  
    float inv_slope_1 = (float)(x1-x0) / (float)(y1-y0);
    float inv_slope_2 = (float)(x2-x0) / (float)(y2-y0);

    

    for(int y = y0; y <= y2; y++){
        draw_line(round(x_start),round(y),round(x_end),round(y),color);
        x_start += inv_slope_1;
        x_end += inv_slope_2;
       

    }
}
void fill_flat_top_triangle(int x0,int y0, int x1, int y1, int x2, int y2, uint32_t color){
   
  
    float inv_slope_1 = (float)(x2-x0) / (float)(y2-y0);
    float inv_slope_2 = (float)(x2-x1) / (float)(y2-y1);

    float x_start = x2;
    float x_end = x2;

    
    for(int y = y2; y > y0; y--){
        draw_line(round(x_start),round(y),round(x_end),round(y),color);
        x_start -= inv_slope_1;
        x_end   -= inv_slope_2;
       

    }

}
void int_swap(int* a, int* b){
    int temp = *a;
    *a = *b;
    *b = temp;
}
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2,int y2, uint32_t color){
    if(y0 > y1){
        int_swap(&y0,&y1);
        int_swap(&x0,&x1);
    }
    if(y1 > y2){
        int_swap(&y1,&y2);
        int_swap(&x1,&x2);
    }
    if ( y0 > y1){
        int_swap(&y0,&y1);
        int_swap(&x0,&x1);
    }


    int My = y1;
    int Mx = ( ( (float)(x2-x0)*(y1-y0) ) /(float) (y2-y0) ) + x0;

    fill_flat_bottom_triangle(x0,y0,x1,y1,Mx,My,0xFFFFFF);
    fill_flat_top_triangle(x1,y1,Mx,My,x2,y2,0xFFFFFF);


}