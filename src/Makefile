CC = gcc -Oz -s -static -D_GNU_SOURCE 
CFLAGS = -flto -ffunction-sections -Wl,--gc-sections 
PACKER = gzexe # upx --lzma --all-methods --overlay=strip 

run: fgraph
	./$<

compress: fgraph
	$(PACKER) $< 
	rm -f "$<~"

fgraph: fgraph.c base.c vec.c flist.c fgraph.c file.c
	$(CC) -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f "fgraph" 
