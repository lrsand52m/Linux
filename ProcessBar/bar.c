#include<stdio.h>
#include<unistd.h>
#include<math.h>
#define Black 40
#define White 46
#define Blue 44
#define Green 47
void printbp(int color,size_t n)
{
	char p[100] = {0};
	sprintf(p,"\e[0;30;%dm \e[0m",color);
	while(n--)
		printf(p);
	return;
}
int main()
{
	char*label = "-\\|/";
	size_t i = 1,j = 0,k = 0;
	for (; i <= 100; ++i)
	{
		int G,B,W;
		G = i*i/100;
		B = i-G;
		W = 10*sqrt(i)-B-G;
		printbp(Green,G);
		printbp(Blue,B);
		printbp(White,W);
		printbp(Black,100-G-B-W);
		printf("[%3d%%][%c]\r",i,label[i%4]);
		fflush(stdout);
		usleep(100000);
     }
	printf("\n");
	return 0;
}
