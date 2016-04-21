CC=gcc
CFLAGS=-I../.. -I../../plat_dep/ -I../../plat_dep/ -I../../plat_dep/linux -g -O0
ODIR=obj
#LDIR=../lib
#LIBS=−lm

_DEPS=str_hash_plat.h  str_hash.h
DEPS=$(patsubst %,$(IDIR )/%,$(_DEPS))

_OBJ = str_hash_plat.o  str_hash.o 
OBJ = $(patsubst %,$(ODIR)/% , $(_OBJ))

ht_test : ht_test.c $(OBJ)
	$(CC) $^ -o ht_test $(CFLAGS)

$(ODIR)/str_hash.o : ../../str_hash.c
	$(CC) -c -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/str_hash_plat.o : ../../plat_dep/linux/str_hash_plat.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean :
	rm -f $(ODIR)/*.o ht_test
