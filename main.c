#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "Generation.h"
#define ull unsigned long long

char Progress_bar[102];
int int_percent;

void process(double percent)
{
	if ((int)(percent*100)>int_percent)
	{
		int_percent=(int)(percent*100);
		Progress_bar[int_percent]='='; Progress_bar[int_percent+1]='>';
		for (int i=1; i<=117; i++) printf("\b");
		printf("Processing %s %2d%%",Progress_bar,int_percent);
		fflush(stdout);
	}
}

int main()
{
	clock_t t=clock();	
	for (int i=1; i<=100; i++) Progress_bar[i]='-'; Progress_bar[0]='['; Progress_bar[101]=']';
	
	ull seed=12412414124;
	ull sample_size=128;
	ull sample_num=0x100;
	ull mutation=10000;
	ull experiment_time=10000;
	ull points[sample_size+0x10];
	
	for (int i=0; i<=sample_size-1; i++) points[i]=1; 
	
	init(seed,sample_num,sample_size,mutation,points);
	
	sample **Generation=Generation_init();
	
	for (int i=1; i<=experiment_time; i++) 
	{
		next_Generation(Generation,3);
		process(i*1.0/experiment_time);
	}
	
	printf("\nTime used :%fs\n",(double)(clock()-t)/CLOCKS_PER_SEC);
	
	printf("Press Enter to continue...");
	getchar();
	
	for (int i=0; i<=sample_num-1; i++)
	{
		sample *test_sample=*(Generation+i);
		printf("%s %llu\n",gene_decompress(test_sample),grade(test_sample));
	}
}

