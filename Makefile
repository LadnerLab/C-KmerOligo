CFLAGS= -O0 -Wall -Wextra -std=c99

kmer_oligo: kmer_oligo.o protein_oligo_library.o dynamic_string.o

kmer_oligo.o: kmer_oligo.c protein_oligo_library.h

protein_oligo_library.o: protein_oligo_library.c protein_oligo_library.h 

dynamic_string.o: dynamic_string.c dynamic_string.h

ht_main: ht_main.o hash_table.o
ht_main.o: ht_main.c hash_table.h
hash_table.o: hash_table.c hash_table.h


.PHONY: debug clean
debug: CFLAGS+= -g
debug: clean
debug: kmer_oligo

clean:
	rm -rf *.o *.gch kmer_oligo
