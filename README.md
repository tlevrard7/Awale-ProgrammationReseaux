# Serveur de jeu Awale
### Contributeurs : 

- JEANNE Corentin
- LEVRARD Thomas

## Comment compiler ?
1. Se positionner dans le répertoire racine du projet (**ProgrammationReseaux-Awale/**) puis executer la commande suivante :
   
    ```bash
    $ make all
    ```
    **Remarque :** Il faut être sur une machine linux
## Evolutivité 
Nous avons mis l'accent sur l'évolutivité de notre code en adoptant une architecture modulaire. En effet, le projet est organisé en modules distincts, chacun ayant une responsabilité spécifique.
De plus, les données échangées sont encapsulées sous forme de paquets de communication (cf. packet.h/packet.c). Chaque type de paquet dispose de ses propres fonctions de sérialisation et de désérialisation, permettant une interaction simplifiée avec le buffer utilisé pour la transmission. D'ailleurs, ces fonctions sont largement simplifiées grâce aux fonctionnalités offertes par buffer.h/buffer.c.
## Fonctionnalités implémentées 
### 🔗 Connexion et affichage des joueurs  
- **Connexion au serveur**
   - Un joueur peut se connecter en fournissant un **pseudo** et un **id** (facultatif)  
- **Liste des joueurs en ligne**  
   - Un joueur peut afficher la liste des pseudos des joueurs en ligne 
### 🕹️ Défis et parties  
- **Défis entre joueurs**  
   - Un joueur **A** peut défier un autre joueur **B**
   - **B** peut **accepter** ou **refuser**  
   - En cas d’acceptation, une **partie d’Awale** démarre immédiatement
### ♻️ Gestion multi-parties  
- **Parties simultanées**  
   - Le serveur peut prendre en charge plusieurs **parties d’Awale** en parallèle.
### 📜 Affichage des parties en cours  
- **Affichage des parties en cours**  
   - Un joueur peut consulter la **liste des parties d’Awale** qui sont entrain de se jouer
### 📩 Communication entre joueurs  
- **Messagerie instantanée (Chat)**  
   - Un joueur peut envoyer un **message privé** à un autre joueur connecté
### 🔌 Gestion des déconnexions  
- **Reconnexion et reprise de partie**  
   - Si un joueur se déconnecte en pleine partie, il peut se **reconnecter** et **reprendre la partie**  
   - Nécessite l’utilisation de l’**id attribué ou choisi** lors de la connexion initiale

## Comment utiliser l'application ?
0. Se positionner dans le répertoire racine du projet (**ProgrammationReseaux-Awale/**)
### Partie serveur : 
1. Démarrer le serveur : 
    ```bash
    $ ./bin/awale_server <port>
    ```
### Partie client : 
1. Connecter un ou plusieurs clients à ce serveur :
    ```bash
    $ ./bin/awale_client <adresse_serveur> <port_serveur> <pseudo> [id]
    ```
    **Remarque :** l'id est facultatif donc si aucun id n'est précisé, le serveur attribuera automatiquement un id au joueur
    
2. Choisir une commande parmi : 
- **help** (ou h) : Affiche la liste des commandes disponibles
- **players** (ou p) : Affiche la liste des pseudos (et id) des joueurs en ligne
- **games** (ou g) : Affiche la liste des parties qui se jouent actuellement
- **message** (ou m) : Permet de spécifier qu'on veut envoyer un message à un autre joueur
    - Il sera alors demandé de saisir l'id du joueur destinatire. Puis, de saisir le message à envoyer.
- **challenge** (ou c) : Permet de spécifier qu'on veut défier un joueur (voir *Détails sur le challenge*)
    - Il sera alors demandé de saisir l'id du joueur à défier.

#### Détails sur le challenge :
- **Si vous avez challengé un joueur :** 
    - Vous serez notifié quand le joueur aura accepté ou refusé le défi (ou encore si le joueur n'existe pas)
- **Si vous avez reçu un challenge**
    - Pour l'**accepter** : tapez 'y', cela va alors créer une nouvelle partie d'Awale (voir *Détails sur une partie d'Awale*) 
    - Pour le **refuser** : tapez 'n'

#### Détails sur une partie d'awale :
Une fois qu'un défi a été accepté, les 2 joueurs concernés rejoignent une nouvelle partie d'Awale (se joue dans le sens trigonométrique). Le joueur qui commence est alors choisi au hasard. Le plateau, les scores, le joueur qui doit jouer le coup ainsi que les indices des différentes cases du plateau sont affichés.
Les joueurs vont donc pouvoir jouer, à tour de rôle, leur coup. Pour ce  faire, il leur suffit de saisir dans le terminal l'indice de la case du plateau choisie pour leur coup.

**Remarque** : Si un joueur se déconnecte pendant une partie, il peut se reconnecter et reprendre la partie en cours. Pour cela, lors de sa reconnexion (voir 1.) il doit indiquer son id qui lui avait été attribué (ou qu'il avait choisi) lors de sa connexion initiale. 


