.SILENT:

CC = gcc -c
LD = gcc -lm
CC_OP = -Wall -g -c -o
LD_OP = -lm -o


PL_SERVER = ./
COMMON_SRC = ../Common
DES_SRC = ../Common/des/source

PL_OBJ = main.o lookup.o list.o UDPLib.o hashADT.o config_parser.o

PL_NAME = lookup
all:
	@-echo ""
	@-echo "Compilando archivos comunes."
	$(CC) $(CC_OP) cypher.o $(COMMON_SRC)/cypher.c
	$(CC) $(CC_OP) fileHandler.o $(COMMON_SRC)/fileHandler.c
	$(CC) $(CC_OP) genlib.o $(COMMON_SRC)/genlib.c
	$(CC) $(CC_OP) config_parser.o $(COMMON_SRC)/config_parser.c
	$(CC) $(CC_OP) TCPLib.o $(COMMON_SRC)/TCPLib.c
	$(CC) $(CC_OP) UDPLib.o $(COMMON_SRC)/UDPLib.c
	$(CC) $(CC_OP) paymentServerLib.o $(COMMON_SRC)/paymentServerLib.c
	$(CC) $(CC_OP) scannerPS.o $(COMMON_SRC)/scannerPS.c
	$(CC) $(CC_OP) des.o $(DES_SRC)/des.c
	$(CC) $(CC_OP) bit.o $(DES_SRC)/bit.c

	@-echo ""
	@-echo "Compilando el PaymentLookupServer."
	$(CC) $(CC_OP) main.o $(PL_SERVER)/main.c
	$(CC) $(CC_OP) lookup.o $(PL_SERVER)/lookup.c
	$(CC) $(CC_OP) list.o $(PL_SERVER)/list.c
	$(CC) $(CC_OP) hashADT.o $(PL_SERVER)/hashADT.c
	$(LD) $(LD_OP) $(PL_NAME) $(PL_OBJ)
	-make clean
	
	
	
	
	
	
	
	
	
clean:
	@-echo ""
	@-echo "Borrando los archivos objeto."
	-rm *.o
