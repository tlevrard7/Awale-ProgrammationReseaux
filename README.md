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

- Erreur compilation : Il y avait un appel à handle_stdin dans server.C or la fonction n'était pas définie, j'ai regardé dans le fichier du cours et à cet endroit il n'y avait rien donc handle_stdin est une fonction vide donc j'ai mis en commentaire pour le moment (on en aura peut(être besoin après))

- problème init_score : awale.score[0] au lieu de awale.score[i] 

- J'ai la partie qui vérifie si on met des graines chez l'adversaire, cela est obligatoire ssi il n'en a plus et que c'est possible (j'ai fait le cas où il y a un adversaire, puisqu'on doit implémenter variante ABAPA avec 2 joueurs)

- erreur dans seeds_on_side car awale->turn et non le paramètre player qui est utilisé

- rajouter la verif que la cell contient des graines


## Manquant :

- logique de distribution des graines érronées car pas de prise en compte du sens + Peut prendre ses propres graines alors que non

- Il ne manque pas un attribut qui définit le sens du jeu, à mettre en paramètre de play ? 

- Il faudra dire au joueur s'il est player 0 ou 1 pour savoir dans quelles celulles il joue

## Améliorations : 

- mettre un retour de play qui correspond à une enum qui contient les différents problèmes ?




