# Awale-ProgrammationReseaux

## Utilisation de l'appli

make 

./bin/awale_server 

./bin/awale_client [address] [pseudo]


## Explication des différents fichiers

### Partie jeu : 

- awale/
   - {awale.h/awale.c} --> Implémente la logique du jeu (règles, déroulement d'un coup, grille, scores...)

- awale_local.c --> Test du jeu uniquement en local (code réutilisable pour jeu en réseau, notamment représentation de la grille)

### Partie Réseau :

- awale_client.c
   - main() --> On vérifie s'il y a un nouveau message de reçu et dans ce cas on appelle on_receive qui va faire ce qu'il faut de la même manière que le on_receive de server
- awale_server.c
   - on_receive(Server *server, int recvFrom, char *buffer, size_t n) --> Vérifie quel paquet a été reçu (switch case) pour effectuer ce qu'il faut faire dans le cas de ce paquet (on peut creer une fonction par paquet), on va globalement souvent deserialiser le buffer pour recréer le paquet, faire des affichages, et créer puis renovyer un autre paquet après
   - main --> Crée le serveur et après simule le serveur donc while(true) : 
      - On vérifie s'il y a une nouvelle connexion
      - Ou on vérifie s'il y a un nouveau message de reçu et dans ce cas on appelle on_receive ... etc .

- packets/
   - {packets.c/packets.h} --> Définit tous les paquets qui pourront être transmis.
     Pour chacun, on définit son packet_id, son contenu et 2 fonctions : 1 de sérialisation et 1 de désérialisation.
     Sérialisation : Permet d'écrire le contenu du packet dans un buffer de char (retourne le nombre d'octets écrit dans le buffer)
     Désérialisation : Permet de créer un packet à partir d'un buffer de char (retourne le packet)

- network/
   - {network.c/network.h} 
      - open_socket() -> Retourne un 'SOCKET' càd un int qui représente le descripteur du socket créé
      - close_socket()
      - recv_from(socket, buffer) -> Lit les données du socket pour les écrire dans le buffer
      - send_to(socket, buffer, n) -> Envoie des données depuis le buffer spécifié vers le socket désigné avec n nombre max d'octets à envoyer
      - check_read(int ndfs, fd_set *rdfs) -> Renvoie le nombre de sockets de rdfs prêts à être lus (time out de 100 ms, càd que check_read attend au max 100 ms pour savoir si des sockets peuvent être lus) 

   - {client.c/client.h}
      - SOCKET create_client(address, port) -> Crée le socket du client et le connecte   au serveur (déterminé par les params address et port)
      - void close_client(SOCKET client) -> ferme le socket du client
      - size_t receive_from(SOCKET client, buffer) -> encapsule recv_from

   - {server.c/server.h} (les i sont le numéro du joueur dans le tableau contenant tous les joueurs)
      - Définit une struct Server contenant le socket du server (sock), la liste des sockets clients qui sont connectés au serveur (clients), maxFD des sockets clients et le nombre de clients connectés

      - Server create_server(int port); -> Crée le socket server, l'associe au port en param et définit le socket comme un socket server, càd qu'il va pouvoir accepter des connexions de sockets clients
      - remove_client(Server *server, int i) -> Ferme le socket du client i
      - void close_server(Server *server); -> Ferme les sockets de tous les clients + le socket du server
        
      - int accept_connection(Server* server, SOCKET* client); --> accepte la connexion du socket client (param) sur le socket du server, MAJ clientCount & maxFD, rajoute le socket du client au tableau des sockets clients du server
      - void disconnect_client(Server *server, int i); -> close le socket du client et MAJ le tableau des sockets clients de la struct server

      - void send_all(Server *server, const char *buffer, size_t); -> Envoie à tous les clients connectés au server le message contenu dans buffer
      - ssize_t receive_any(Server* server, SOCKET* client, char *buffer); -> Cherche le premier client qui veut parler (premier dans le sens ou son num dans la liste des clients du serveur est min) et met son message/paquet dans le buffer en param

## Autres Explications :

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout); -> Permet de surveiller tous les sockets (+ genralement descripteur de fichier donc stdin marche aussi) contenus dans readfds ou/et writefds ou/et xceptfds et de savoir lesquels sont prêts pour la lecture (readfds), l'écriture (writefds) ou pour signaler une erreur (exceptfds)

Retour :
< 0 : Une erreur s'est produite 
0 : Aucun descripteur n'est prêt, le délai d'attente a expiré.

> 0 : Renvoie le nombre de descripteurs prêts.
Il faut donc vérifier chaque descripteur dans readfds pour savoir lequel est prêt
->Si FD_ISSET(i, &readfds) retourne vrai, cela signifie que ce descripteur est prêt à être utilisé.










## Ce que j'ai fait :


## Manquant :

- Gestion de plusieurs sens de jeu


