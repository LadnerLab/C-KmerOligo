CFLAGS= -O0 -Wall -Wextra -std=c99 -mtune=native 

kmer_oligo: kmer_oligo.o protein_oligo_library.o dynamic_string.o hash_table.o array_list.o set.o thpool.o
	gcc kmer_oligo.o protein_oligo_library.o dynamic_string.o hash_table.o array_list.o set.o thpool.o -lm -lpthread -o kmer_oligo 
kmer_oligo.o: kmer_oligo.c protein_oligo_library.h hash_table.h array_list.h set.h

thpool.o: thpool.c thpool.h

protein_oligo_library.o: protein_oligo_library.c protein_oligo_library.h hash_table.h array_list.h set.h

dynamic_string.o: dynamic_string.c dynamic_string.h

array_list.o: array_list.c array_list.h 

hash_table.o: hash_table.c hash_table.h

array_list: array_list_main.o array_list.o
array_list_main.o: array_list_main.c array_list.h
array_list.o: array_list.c array_list.h

set.o: set.c set.h 


.PHONY: debug clean optimized
debug: CFLAGS+= -g -O0
debug: clean
debug: kmer_oligo

optimized: CFLAGS += -O3  -ffast-math
optimized: clean
optimized: kmer_oligo
clean:
	rm -rf *.o *.gch kmer_oligo
