#ifndef GENETIC_H
#define GENETIC_H

// -----------------------------------------------------------------------------

struct sample;

typedef struct sample sample;

// -----------------------------------------------------------------------------

void init(unsigned long long,unsigned long long,unsigned long long,unsigned long long,unsigned long long*);
sample **Generation_init();
sample *gene_compress(char *);
char *gene_decompress(sample *);
unsigned long long grade(sample *);
void next_Generation(sample **,unsigned long long);

// -----------------------------------------------------------------------------

#endif

