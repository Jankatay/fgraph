run: fgraph
	./$<

fgraph: fgraph.c base.c vec.c flist.c fgraph.c file.c
	gcc -o $@ $< -D_XOPEN_SOURCE=700 
