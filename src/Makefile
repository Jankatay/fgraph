CC = gcc -Oz
PACKER = gzexe

run: fgraph
	./$<

compress: fgraph
	strip $<
	$(PACKER) $< 
	rm -f "$<~"

fgraph: fgraph.c base.c vec.c flist.c fgraph.c file.c
	$(CC) -o $@ $< -D_GNU_SOURCE

.PHONY: clean
clean:
	rm -f "fgraph" 
