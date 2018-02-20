CC = gcc -g
PROGS = master producer consumer
OBJ1 = master.o 
OBJ2 = producer.o 
OBJ3 = consumer.o

all: $(PROGS)

master: $(OBJ1)
	$(CC) -o $@ $^

producer: $(OBJ2)
	$(CC) -o $@ $^

consumer: $(OBJ3)
	$(CC) -o $@ $^

clean:
	rm -rf *.o *.log $(PROGS)
