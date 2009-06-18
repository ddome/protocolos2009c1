.SILENT:

CC = gcc -c
LD = gcc -lm
CC_OP = -Wall -g -c -o
LD_OP = -lm -o
LD_OP_SERVER = -lldap -lssl

CLIENT_SRC = Client
SERVER_SRC = Server
PL_SERVER = Lookup
COMMON_SRC = Common
DES_SRC = Common/des/source

CLIENT_OBJ = client.o main.o Promt.o scannerADT.o tree.o cypher.o fileHandler.o genlib.o TCPLib.o UDPLib.o des.o bit.o
SERVER_OBJ = server.o main.o hashADT.o client_ldap.o cypher.o fileHandler.o genlib.o TCPLib.o UDPLib.o des.o bit.o database_hanlder.o counter.o movieDB.o listADT.o md5.o
PL_OBJ = main.o lookup.o list.o UDPLib.o


CLIENT_NAME = client
SERVER_NAME = server
PL_NAME = lookup

all:
	@-echo ""
	@-echo "Compilando archivos comunes."
	$(CC) $(CC_OP) cypher.o $(COMMON_SRC)/cypher.c
	$(CC) $(CC_OP) fileHandler.o $(COMMON_SRC)/fileHandler.c
	$(CC) $(CC_OP) genlib.o $(COMMON_SRC)/genlib.c
	$(CC) $(CC_OP) TCPLib.o $(COMMON_SRC)/TCPLib.c
	$(CC) $(CC_OP) UDPLib.o $(COMMON_SRC)/UDPLib.c
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

	@-echo ""
	@-echo "Compilando el PaymentLookupServer."
	$(CC) $(CC_OP) main.o $(PL_SERVER)/main.c
	$(CC) $(CC_OP) lookup.o $(PL_SERVER)/lookup.c
	$(CC) $(CC_OP) list.o $(PL_SERVER)/list.c
	$(LD) $(LD_OP) $(PL_NAME) $(PL_OBJ)
	-make clean
	
	
	
	
	
	
	
	
	
clean:
	@-echo ""
	@-echo "Borrando los archivos objeto."
	-rm *.o
