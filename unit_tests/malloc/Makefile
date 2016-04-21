CC=gcc
CFLAGS=-I../.. -I../../plat_dep/ -I../../plat_dep/ -I../../plat_dep/linux -g -O0
ODIR=obj
#LDIR=../lib
#LIBS=−lm

_DEPS=str_hash_plat.h  str_hash.h
DEPS=$(patsubst %,$(IDIR )/%,$(_DEPS))

_OBJ = str_hash_plat.o  str_hash.o stack_heap.o time_plat.o 
OBJ = $(patsubst %,$(ODIR)/% , $(_OBJ))

malloc_test : malloc_test.c $(OBJ)
	$(CC) $^ -o malloc_test $(CFLAGS)

$(ODIR)/str_hash.o : ../../str_hash.c
	$(CC) -c -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/stack_heap.o: ../../stack_heap.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/str_hash_plat.o : ../../plat_dep/linux/str_hash_plat.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/time_plat.o: ../../plat_dep/linux/time_plat.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)



.PHONY: clean
clean :
	rm -f $(ODIR)/*.o malloc_test
