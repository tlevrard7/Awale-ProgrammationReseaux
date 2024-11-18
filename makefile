#Options de compilation
CFLAGS = -Wall -Wextra -g
DEFINES = -D NETLOG
#Dossiers sources 
awaDir = awale
pktDir = packets
binDir = bin
netDir = network

# Règle par défaut
all: server client

# Compilation de l'exécutable local
local: awale_local.c $(awaDir)/awale.h $(awaDir)/awale.c
	mkdir -p $(binDir)
	gcc $(CFLAGS) $(DEFINES) -o $(binDir)/awale_local awale_local.c $(awaDir)/awale.c

# Compilation de l'exécutable server
server: awale_server.c $(netDir)/network.h $(netDir)/network.c $(netDir)/server.h $(netDir)/server.c $(netDir)/buffer.h $(netDir)/buffer.c $(pktDir)/packets.h $(pktDir)/packets.c player.h
	mkdir -p $(binDir)
	gcc $(CFLAGS) $(DEFINES) -o $(binDir)/awale_server awale_server.c $(netDir)/server.c $(netDir)/network.c $(netDir)/buffer.c $(pktDir)/packets.c

# Compilation de l'exécutable client
client: awale_client.c $(netDir)/network.h $(netDir)/network.c $(netDir)/client.h $(netDir)/client.c $(netDir)/buffer.h $(netDir)/buffer.c $(pktDir)/packets.h $(pktDir)/packets.c player.h
	mkdir -p $(binDir)
	gcc $(CFLAGS) $(DEFINES) -o $(binDir)/awale_client awale_client.c $(netDir)/client.c $(netDir)/network.c $(netDir)/buffer.c $(pktDir)/packets.c

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm bin/*