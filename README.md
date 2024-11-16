# Awale-ProgrammationReseaux

## Utilisation de l'appli

make 


./bin/server 

./bin/client [address] [pseudo]


## Explication des différents fichiers

- network.h --> Permet simplement de regrouper des trucs qui étaient répétés dans plusieurs fichiers (client.h) et (server.h)

- (awale.h / awale.c) --> Permet d'implémenter la logique du jeu (Représentation interne de la grille, règles du jeu, déroulement d'une partie)

- Server/ 
        --> (server.h / server.c) : Permet d'implementer le serveur
        --> (client.h) : Permet d'implementer la représentation d'une client au niveau du serveur 

- Client/ 
        --> (server.h / server.c) : Permet d'implementer un client

Pour l'instant, le seul truc fait c'est que quand un client envoie un message, il est répété à tous les autres clients

## Ce que j'ai fait :


## Manquant :

- Gestion de plusieurs sens de jeu



