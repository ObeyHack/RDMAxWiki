all: client server

server: bw_template.c
	gcc bw_template.c -o server -libverbs

client: bw_template.c
	ln -s server client -f
