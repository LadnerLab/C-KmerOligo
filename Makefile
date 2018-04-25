CFLAGS= -O0 -Wall -Wextra 

kmer_oligo: kmer_oligo.o protein_oligo_library.o

kmer_oligo.o: kmer_oligo.c protein_oligo_library.h

protein_oligo_library.o: protein_oligo_library.c protein_oligo_library.h

.PHONY: debug clean
debug: CFLAGS+= -g
debug: clean
debug: kmer_oligo

clean:
	rm -rf *.o *.gch kmer_oligo
