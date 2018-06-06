CFLAGS= -O0 -Wall -Wextra -std=c99 -mtune=native 

kmer_oligo: kmer_oligo.o protein_oligo_library.o dynamic_string.o hash_table.o array_list.o set.o
	gcc kmer_oligo.o protein_oligo_library.o dynamic_string.o hash_table.o array_list.o set.o -lm -o kmer_oligo
kmer_oligo.o: kmer_oligo.c protein_oligo_library.h hash_table.h array_list.h set.h

protein_oligo_library.o: protein_oligo_library.c protein_oligo_library.h hash_table.h array_list.h set.h

dynamic_string.o: dynamic_string.c dynamic_string.h

array_list.o: array_list.c array_list.h 

ht_main: ht_main.o hash_table.o
ht_main.o: ht_main.c hash_table.h
hash_table.o: hash_table.c hash_table.h

array_list: array_list_main.o array_list.o
array_list_main.o: array_list_main.c array_list.h
array_list.o: array_list.c array_list.h

set: set_main.o set.o hash_table.o
	gcc set.o set_main.o hash_table.o -lm -o set
set_main.o: set_main.c hash_table.h set.h 
set.o: set.c set.h 

plug_leaks: plug_leaks.o protein_oligo_library.o dynamic_string.o hash_table.o array_list.o set.o
	gcc plug_leaks.o protein_oligo_library.o dynamic_string.o hash_table.o array_list.o set.o -lm -o plug_leaks
plug_leaks.o: plug_leaks.c protein_oligo_library.h hash_table.h array_list.h set.h


.PHONY: debug clean optimized
debug: CFLAGS+= -g -O0
debug: clean
debug: kmer_oligo

optimized: CFLAGS += -O2 -funswitch-loops -fpredictive-commoning -ftree-loop-vectorize -floop-interchange -floop-unroll-and-jam -fvect-cost-model -ftree-partial-pre -fpeel-loops -fipa-cp-clone -fomit-frame-pointer -ffast-math -fsplit-paths -ftree-slp-vectorize -ftree-loop-distribute-patterns -fgcse-after-reload
optimized: clean
optimized: kmer_oligo
clean:
	rm -rf *.o *.gch kmer_oligo
