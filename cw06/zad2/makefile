main:
	gcc -Wall -o serwer serwer.c -lrt
	gcc -Wall -o client client.c -lrt

clean:
	rm -f serwer client
# CC		= gcc
# CFLAGS		= -Wall -lrt
#
# OBJS1 = client.o
# OBJS2 = serwer.o
# TARGETS1 = clientOut
# TARGETS2 = serwerOut
#
# compile : $(TARGETS1) $(TARGETS2)
#
# $(TARGETS1): $(OBJS1)
# 	$(CC) $(CFLAGS) $^ -o $@
#
# $(TARGETS2): $(OBJS2)
# 	$(CC) $(CFLAGS) $^ -o $@
#
# %.o: %.c
# 	$(CC) $(CFLAGS) -c $^ -o $@
#
# .PHONY: clean
# clean:
# 	@rm -f *.o $(TARGETS1) $(TARGETS2)
