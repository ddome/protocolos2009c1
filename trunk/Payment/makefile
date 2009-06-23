.SILENT:

CC = gcc -c
LD = gcc -lm
CC_OP = -Wall -g -c -o
LD_OP = -lm -o

COMMON_SRC = ../Common
PAYMENT_SRC = ./
DES_SRC = ../Common/des/source

PAYMENT_OBJ = main.o hashADT.o payment.o payment_db.o paymentServerLib.o scannerPS.o TCPLib.o config_parser.o

PAYMENT_NAME = payment
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
	@-echo "Compilando el PaymentServer."
	$(CC) $(CC_OP) main.o $(PAYMENT_SRC)/main.c
	$(CC) $(CC_OP) hashADT.o $(PAYMENT_SRC)/hashADT.c
	$(CC) $(CC_OP) payment.o $(PAYMENT_SRC)/payment.c
	$(CC) $(CC_OP) payment_db.o $(PAYMENT_SRC)/payment_db.c
	$(LD) $(LD_OP) $(PAYMENT_NAME) $(PAYMENT_OBJ)
	-make clean
	
	
	
	
	
	
	
	
	
clean:
	@-echo ""
	@-echo "Borrando los archivos objeto."
	-rm *.o
