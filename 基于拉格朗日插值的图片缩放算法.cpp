#include <time.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h> 
#define mac(a, s) a=(U8*)malloc(sizeof(U8)*s) 
using namespace std;
typedef  unsigned char  U8;
typedef  unsigned short U16;
typedef  unsigned int   U32;

#pragma  pack(1)
typedef struct
{
    //bmp header
    U16 bfType;         // B  M
    U32 bfSize;         //文件大小
    U16 bfReserved1;
    U16 bfReserved2;
    U32 bfOffBits;      //文件头偏移量

    //DIB header
    U32 biSize;         //DIB头大小
    U32 biWidth;        //文件宽度
    U32 biHeight;       //文件高度
    U16 biPlanes;
    U16 biBitCount;     //每个相素点的位数
    U32 biCompression;
    U32 biSizeImage;    //图文件大小
    U32 biXPelsPerMeter;
    U32 biYPelsPerMeter;
    U32 biClrUsed;
    U32 biClrImportant;
} BMP_HEADER;
#pragma  pack()

U8 Lagrange(int, int, double[], double );
void out(BMP_HEADER); // 打印bmp文件头\信息头信息 
void read_Imag(char*, char*, BMP_HEADER&, U8*&);// 读取bmp文件信息 
void scan_H_W_n(int&, int&, int&); // 输入目标图片的宽度、高度和插值样本点数 
void change_Imag(BMP_HEADER, U8*&, U8*&, U8*, int, int, int );//修改图片信息 
void write_Imag(char*, BMP_HEADER, U8*&, U8*, int, int );//将信息保存至bmp文件中 

int main()
{	
	U8 clrPal[256*4];
    int width,height,n;
    U8* data, *temp;
    char fName0[256],fName[256];
    BMP_HEADER  header;
    int ok = 1, cnt=0;

	while(1) {
		printf("是否需要进行图片缩放？（0表示否，其它表示是）:"); scanf("%d", &ok);
		if(!ok) break;
		read_Imag(fName0, fName, header, data);
		scan_H_W_n(width, height, n);
		change_Imag(header, data, temp, clrPal, width, height, n);
    	write_Imag(fName, header, temp, clrPal, width, height);
	}
    
}

void scan_H_W_n(int& width, int& height, int& n){
	printf("输入缩放后的宽度、高度："); scanf("%d %d", &width, &height);
    printf("输入样本点个数：");         scanf("%d", &n);
}
U8 Lagrange(int pos, int num, double x[], double g) {
    double y = 0, temp;
    for(int i=0;i<num;i++) {
        temp = 1.0;
        for(int j=pos;j<pos+num;j++)
            if(j != i + pos) 
                temp = temp*(g-j)*1.0/(i+pos-j);
        y += x[i]*temp;
    }
    return (U8)y;
}
void out(BMP_HEADER header){
	printf(" bfType          : %c%c\n", header.bfType%256,header.bfType/256 );
    printf(" bfSize          : %d  \n", header.bfSize          );
    printf(" bfReserved1     : %d  \n", header.bfReserved1     );
    printf(" bfReserved2     : %d  \n", header.bfReserved2     );
    printf(" bfOffBits       : %d\n\n", header.bfOffBits       );
    printf(" biSize          : %d  \n", header.biSize          );
    printf(" biWidth         : %d  \n", header.biWidth         );
    printf(" biHeight        : %d  \n", header.biHeight        );
    printf(" biPlanes        : %d  \n", header.biPlanes        );
    printf(" biBitCount      : %d  \n", header.biBitCount      );
    printf(" biCompression   : %d  \n", header.biCompression   );
    printf(" biSizeImage     : %d  \n", header.biSizeImage     );
    printf(" biXPelsPerMeter : %d  \n", header.biXPelsPerMeter );
    printf(" biYPelsPerMeter : %d  \n", header.biYPelsPerMeter );
    printf(" biClrUsed       : %d  \n", header.biClrUsed       );
    printf(" biClrImportant  : %d  \n", header.biClrImportant  );
}
void read_Imag(char *fName0, char* fName, BMP_HEADER &header, U8*&data){
	printf("请输入需要进行缩放图片文件名:"); scanf("%s", fName0);
	printf("请输入保存缩放图片的文件名:"); scanf("%s", fName);
    FILE* fp;
    fp = fopen(fName0,"rb");
    if(NULL == fp)
        perror("open bmp file fail");
    fread(&header, sizeof(BMP_HEADER), 1, fp);
	out(header);
    fseek(fp,header.bfOffBits,SEEK_SET);
    mac(data, header.biSizeImage);
    fread(data,header.biSizeImage,1,fp);
    fclose(fp);
    
} 
void change_Imag(BMP_HEADER header, U8*& data, U8*&temp, U8* clrPal,int width, int height, int n){
	int lwidth, lheight, stride; 
	stride = header.biSizeImage / header.biHeight;
	lwidth = header.biWidth;
	lheight = header.biHeight;
    int h = header.biBitCount/8;
	mac(temp, (width*h+3)/4*4*height);
    if(h==1)
    {
        for(int i=0; i<256; i++)
        {
            clrPal[4*i] = clrPal[4*i+1] = clrPal[4*i+2] = i;
            clrPal[4*i+3] = 0;
        }
    }
    double x[n+1];
    if(h != 1 && h != 3);
    else {
    	int ww = (width*h+3)/4*4;
        for(int i=0, l;i<height;i++) {
        	int ii = (int)(1.0*i*(lheight-1)/(height-1)+0.5);
            for(int j=0;j<width;j++){
				double  g = 1.0*j*(lwidth-1)/(width-1);
				if(n&1) l = (int)(g+0.5) - n/2    ;
				else 	l = (int)(g+0.0) - n/2 + 1;
				for(int k=0;k<h;k++){	
                	for(int r=l;r<l+n;r++)
                		if(r<0) 			x[r-l] = data[ii*stride - 	           r*h+k];
                		else if(r < lwidth) x[r-l] = data[ii*stride +    		   r*h+k];
                		else				x[r-l] = data[ii*stride + (2*lwidth-2-r)*h+k];
                	temp[i*ww + j*h+k] = (U8)Lagrange(l, n, x, g);
                }
         	}
            for(int j=width*h; j<ww;j++) temp[i*ww + j] = 0;
        }
    }
	free(data);	
}


void write_Imag(char *fName, BMP_HEADER header, U8* &temp, U8* clrPal, int width, int height){
	int bpp = header.biBitCount;
	FILE *fp = fopen(fName, "wb+");
	if(fp == NULL )
    	perror("open x.bmp fail");
	header.biSizeImage  = (width*bpp/8+3)/4*4*height;
	header.bfSize 		= header.bfOffBits + header.biSizeImage;
    header.biWidth 		= width;
    header.biHeight 	= height; 
    puts("******缩放后bmp文件头、信息头******");
    out(header);
	fwrite(&header, sizeof(BMP_HEADER), 1, fp);  
	if(bpp == 8) fwrite(clrPal, 1024, 1, fp);
	fwrite(temp, header.biSizeImage, 1, fp);
	fclose(fp);
	free(temp);
	puts("图片缩放成功！\n");
}
