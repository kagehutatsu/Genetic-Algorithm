#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Generation.h"
#define ull unsigned long long

// -----------------------------------------------------------------------------

struct byte;

typedef struct byte byte;

const ull byte_size=8*sizeof(unsigned long);
ull sample_num,sample_size,mutation;
ull *points;

// -----------------------------------------------------------------------------

struct byte 
{
	ull data;
	byte *next;
};

struct sample
{
	ull size;
	ull score;
	byte *head;
};

// -----------------------------------------------------------------------------


void init(ull seed,ull user_sample_num,ull user_sample_size,ull user_mutation,ull *user_points)
{
	srand(seed);
	sample_size=user_sample_size;
	sample_num=user_sample_num;
	mutation=user_mutation;
	points=user_points;
}

static byte *create_byte()
{
	byte *ptr=malloc(sizeof(byte));
	ptr->data=0; ptr->next=NULL;
	return ptr;
}

static sample *create_sample()
{
	sample *test_sample=malloc(sizeof(sample));
	test_sample->size=sample_size/byte_size; test_sample->score=0;
	if (sample_size%byte_size!=0) test_sample->size++;
	byte* ptr=NULL; byte* previous=NULL;
	for (int i=1; i<=test_sample->size; i++)
	{
		ptr=create_byte();
		ptr->data=0;
		ptr->next=previous; previous=ptr;
	}
	test_sample->head=ptr;
	return test_sample;
}

void kill_sample(sample *test_sample)
{
	for (byte *ptr=test_sample->head; ptr!=NULL;)
	{
		byte *temp=ptr->next;
		free(ptr);
		ptr=temp;
	}
	free(test_sample); test_sample=NULL;
}

ull grade(sample *test_sample)
{
	char *gene=gene_decompress(test_sample);
	ull score=0;
	for (int i=0; i<=sample_size-1; i++)
	{
		if (gene[i]=='1') score+=points[i];
	}
	free(gene); gene=NULL;
	return score;
}

static void sort(sample **Generation)
{
	int cmp(const void *a,const void *b)
	{
		return (*((sample**)a))->score>(*((sample**)b))->score;
	}
	qsort(Generation,sample_num,sizeof(sample*),cmp);
}

sample **Generation_init()
{
	sample **Generation=malloc(sample_num*sizeof(sample*));
	for (int i=0; i<=sample_num-1; i++) 
	{
		char *str=malloc(sample_size+0x10);
		for (int j=0; j<=sample_size-1; j++) str[j]=(char)(rand()%2+0x30);
		sample *test_sample=gene_compress(str);				
		test_sample->score=grade(test_sample);
		*(Generation+i)=test_sample;
	}
	sort(Generation);
	return Generation;
}

sample *gene_compress(char *str)
{
	sample *test_sample=create_sample();
	ull split=1; byte *ptr=test_sample->head;
	for (int i=0; i<=sample_size-1; i++)
	{
		ptr->data+=str[i]*split;
		split=split<<1;
		if (i==byte_size-1) split=1,ptr=ptr->next;
	}
	free(str);
	return test_sample;
}

char *gene_decompress(sample *test_sample)
{
	char *str=malloc(sample_size+0x10);
	memset(str,0,sample_size+0x10);
	byte *ptr=test_sample->head; ull split=1;
	for (int i=0; i<=sample_size-1; i++)
	{
		*(str+i)=(char)((ptr->data&split)/split+0x30);
		split=split<<1;
		if (i==byte_size-1) split=1,ptr=ptr->next;
	}
	return str;
}

static int gene_mutation(int chromosome)
{
	if (rand()%mutation==0) return !chromosome;
	else return chromosome;
} 

static sample *marriage(sample *father,sample *mother)
{
	sample *child=create_sample();
	int chromosome_father,chromosome_mother,chromosome_child;
	char *gene_father=gene_decompress(father);
	char *gene_mother=gene_decompress(mother);
	char *gene_child=gene_decompress(child);
	for (int i=0; i<=sample_size-1; i++)
	{
		chromosome_father=gene_father[i]-0x30;
		chromosome_mother=gene_mother[i]-0x30;
		if (rand()%2) chromosome_child=chromosome_father;
		else chromosome_child=chromosome_mother;
		chromosome_child=gene_mutation(chromosome_child);
		gene_child[i]=(char)(chromosome_child+0x30);
	}
	child=gene_compress(gene_child); free(gene_father); free(gene_mother);
	return child;
}

void next_Generation(sample **Generation,ull mode)
{
	if (mode==1)
	{
		for (int i=0; i<=sample_num-1; i+=2)
		{
			sample *child1=marriage(*(Generation+i),*(Generation+i+1));
			sample *child2=marriage(*(Generation+i),*(Generation+i+1));
			child1->score=grade(child1); child2->score=grade(child2);
			kill_sample(*(Generation+i)); kill_sample(*(Generation+i+1));
			*(Generation+i)=child1; *(Generation+i+1)=child2;
		}
	}
	if (mode==2)
	{
		for (int i=0; i<=sample_num-1; i+=2)
		{
			ull father_idx=1,mother_idx=0;
			for ( ; father_idx==mother_idx; )
			{
				father_idx=rand()%sample_num;
				mother_idx=rand()%sample_num;
			}
			sample *child1=marriage(*(Generation+father_idx),*(Generation+mother_idx));
			sample *child2=marriage(*(Generation+father_idx),*(Generation+mother_idx));
			child1->score=grade(child1); child2->score=grade(child2);
			kill_sample(*(Generation+father_idx)); kill_sample(*(Generation+mother_idx));
			*(Generation+father_idx)=child1; *(Generation+mother_idx)=child2;
		}
	}
	if (mode==3)
	{
		for (int i=sample_num/2,cnt=0; i<=sample_num-1; i+=2,cnt+=2)
		{
			ull father_idx=rand()%(sample_num/2)+(sample_num/2);
			ull mother_idx=rand()%(sample_num/2)+(sample_num/2);
			sample *child1=marriage(*(Generation+father_idx),*(Generation+mother_idx));
			sample *child2=marriage(*(Generation+father_idx),*(Generation+mother_idx));
			child1->score=grade(child1); child2->score=grade(child2);
			kill_sample(*(Generation+cnt)); kill_sample(*(Generation+cnt+1));
			*(Generation+cnt)=child1; *(Generation+cnt+1)=child2;
		}
	}
	sort(Generation);
}
