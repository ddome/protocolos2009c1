.SILENT:

CC = gcc -c
LD = gcc -lm
CC_OP = -Wall -g -c -o
LD_OP = -lm -o
LD_OP_SERVER = -lldap -lssl

SERVER_SRC = ./
COMMON_SRC = ../Common
DES_SRC = ../Common/des/source

SERVER_OBJ = server.o main.o hashADT.o client_ldap.o cypher.o fileHandler.o genlib.o TCPLib.o UDPLib.o des.o bit.o database_hanlder.o counter.o movieDB.o paymentServerLib.o config_parser.o scannerPS.o listADT.o md5.o

SERVER_NAME = server

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
	@-echo "Compilando el MovieStoreServer."
	$(CC) $(CC_OP) server.o $(SERVER_SRC)/server.c
	$(CC) $(CC_OP) hashADT.o $(SERVER_SRC)/hashADT.c
	$(CC) $(CC_OP) main.o $(SERVER_SRC)/main.c
	$(CC) $(CC_OP) client_ldap.o $(SERVER_SRC)/client_ldap.c
	$(CC) $(CC_OP) database_hanlder.o $(SERVER_SRC)/database_handler.c
	$(CC) $(CC_OP) counter.o $(SERVER_SRC)/counter.c
	$(CC) $(CC_OP) movieDB.o $(SERVER_SRC)/movieDB.c
	$(CC) $(CC_OP) md5.o $(SERVER_SRC)/md5.c
	$(CC) $(CC_OP) listADT.o $(SERVER_SRC)/listADT.c
	$(LD) $(LD_OP_SERVER) $(LD_OP) $(SERVER_NAME) $(SERVER_OBJ)
	-make clean
	
	
	
	
	
	
	
	
	
clean:
	@-echo ""
	@-echo "Borrando los archivos objeto."
	-rm *.o
