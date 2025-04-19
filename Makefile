all: oss worker

oss: oss.c shared.h
	$(CC) -o oss oss.c -Wall -g

worker: worker.c shared.h
	$(CC) -o worker worker.c -Wall -g

clean:
	rm -f oss worker *.o log.txt
