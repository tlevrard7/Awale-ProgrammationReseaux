#Options de compilation
CFLAGS = -Wall -Wextra -g

#Dossiers sources 
dossierClient = ./Client
dossierServeur = ./Server

# Règle par défaut
all: server client


# Compilation de l'exécutable server
server: $(dossierServeur)/server.c $(dossierServeur)/server.h $(dossierServeur)/client.h network.h
	mkdir -p bin
	gcc $(CFLAGS) -o bin/server $(dossierServeur)/server.c

# Compilation de l'exécutable client
client: $(dossierClient)/client.c $(dossierClient)/client.h network.h
	mkdir -p bin
	gcc $(CFLAGS) -o bin/client $(dossierClient)/client.c

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm bin/*