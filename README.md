# Serveur de jeu Awale
#### Contributeurs : 

- JEANNE Corentin
- LEVRARD Thomas

## Comment compiler ?
1. Se positionner dans le répertoire racine du projet puis executez la commande suivante :
```bash
    $ make all
```
## Fonctionnalités implémentées
1. Connexion au serveur avec un pseudo et un id (facultatif)
2. Affichage de la liste des pseudos des joueurs en ligne
3. Un client A peut défier un autre client B. B peut accepter ou refuser. S'il accepte, cela lance une partie d'Awale.
4. Si un joueur se déconnecte pendant une partie, il peut se reconnecter et reprendre la partie. Pour cela, il doit indiquer son id qui lui avait été attribué (ou qu'il avait choisi) lors de sa connexion initiale.
5. Possibilité d'avoir plusieurs parties d'Awale en même temps

## Comment utiliser l'application ?
0. Se positionner dans le répertoire racine du projet
#### Partie serveur : 
1. Démarrer le serveur : 
    ```bash
    $ ./bin/awale_server <port>
    ```
#### Partie client : 
1. Connecter un ou plusieurs clients à ce serveur :
    ```bash
    $ ./bin/awale_client <adresse_serveur> <port_serveur> <pseudo> [id]
    ```
    Remarque : l'id est facultatif donc si aucun id n'est précisé, le serveur en attribuera automatiquement un id au joueur
    
2. Choisir une commande parmis : 
- **help** (ou h) : Affiche la liste des commandes disponibles
- **players** (ou p) : Affiche la liste des pseudos (et id) des joueurs en ligne
- **challenge** (ou c) : Permet de dire qu'on veut défier un joueur (voir *Détails sur le challenge*)
    - Il sera alors demandé de saisir l'id du joueur à défier 

##### Détails sur le challenge :
- **Si vous avez demandé un challenge :** 
    - vous serez notifié quand le joueur aura accepté ou refusé le défi (ou encore si le joueur n'existe pas)
- **Si vous avez reçu un challenge**
    - Pour l'**accepter** : tapez 'y', cela va alors créer une nouvelle partie d'Awale (voir *Détails sur une partie d'Awale*) 
    - Pour le **refuser** : tapez 'n'

##### Détails sur une partie d'awale :
Une fois qu'un défi a été accepté, les 2 joueurs concernés rejoignent une nouvelle partie d'Awale (se joue dans le sens trigonométrique). Le joueur qui commence est alors choisi au hasard. Le plateau, les scores, le joueur qui doit jouer le coup ainsi que les indices des différentes cases du plateau sont affichés.
Les joueurs vont donc pouvoir jouer, à tour de rôle, leur coup. Pour ce  faire, il leur suffit de saisir dans le terminal l'indice de la case du plateau choisie pour leur coup.

**Remarque** : Si un joueur se déconnecte pendant une partie, il peut se reconnecter et reprendre la partie en cours. Pour cela, il doit indiquer son id qui lui avait été attribué (ou qu'il avait choisi) lors de sa connexion initiale 

    

## Fonctionnalités à implémenter
- Chat
- Observateurs de partie
- Un joueur peut écrire une bio. On peut demander au serveur de nous afficher la bio d’un pseudo particulier.
- Afficher clairement notre id lors de la deconnexion pour savoir lequel mettre pour se reco
- Mettre un timeout lorsqu'un client se déconnecte pendant une partie càd que s'il ne se reconnecte pas après n secondes, la partie est arrêtée
- Mettre un timeout à la demande de challenge ?
- Mode privé: un joueur peut limiter la liste des observateurs à une liste d’amis. 
- Sauvegarde de partie jouée pour pouvoir la regarder par la suite.


