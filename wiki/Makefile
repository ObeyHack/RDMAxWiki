all: client server

server: bw_template.c
	gcc bw_template.c bw_template.h client.c client.h database.c database.h main.c -o server -libverbs

client: bw_template.c
	ln -s server client -f
