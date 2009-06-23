.SILENT:

CC = gcc -c
LD = gcc -lm
CC_OP = -Wall -g -c -o
LD_OP = -lm -o

CLIENT_SRC = ./
COMMON_SRC = ../Common
DES_SRC = ../Common/des/source

CLIENT_OBJ = client.o main.o Promt.o scannerADT.o tree.o cypher.o fileHandler.o genlib.o TCPLib.o UDPLib.o des.o bit.o config_parser.o



CLIENT_NAME = client
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
	@-echo "Compilando el MovieStoreClient."
	$(CC) $(CC_OP) client.o $(CLIENT_SRC)/client.c
	$(CC) $(CC_OP) main.o $(CLIENT_SRC)/main.c
	$(CC) $(CC_OP) Promt.o $(CLIENT_SRC)/Prompt.c
	$(CC) $(CC_OP) scannerADT.o $(CLIENT_SRC)/scannerADT.c
	$(CC) $(CC_OP) tree.o $(CLIENT_SRC)/tree.c
	$(LD) $(LD_OP) $(CLIENT_NAME) $(CLIENT_OBJ)
	-make clean
	
	
	
	
	
	
	
	
	
clean:
	@-echo ""
	@-echo "Borrando los archivos objeto."
	-rm *.o
