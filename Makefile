cat > Makefile << 'EOF'
all: oss worker

oss: oss.c shared.h
	gcc -o oss oss.c -Wall -g

worker: worker.c shared.h
	gcc -o worker worker.c -Wall -g

clean:
	rm -f oss worker *.o log.txt
EOF
