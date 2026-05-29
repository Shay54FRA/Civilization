# Projet Civilization - Jeu de stratégie en C

## Présentation du projet

Ce projet est une adaptation simplifiée du jeu **Civilization**, développée en langage C dans le cadre du projet de programmation.

Le jeu propose deux modes d’utilisation :

- un **mode CLI** jouable dans le terminal ;
- un **mode SDL** avec interface graphique.

L’objectif est de développer une civilisation en gérant des unités, des villes, des bâtiments, des technologies et des ressources, tout en explorant la carte et en interagissant avec des barbares.

Le projet est organisé de manière modulaire afin de séparer clairement les différentes parties du jeu : carte, unités, villes, technologies, interface CLI, interface SDL, etc.

---

## Lancement du jeu

Le projet est fourni avec un `Makefile` qui gère automatiquement la compilation des différents fichiers source.

Il n’est donc pas nécessaire de compiler les fichiers un par un à la main.

### Compiler le projet

Depuis la racine du projet, utiliser simplement :

```bash
make
```

Cette commande compile l’ensemble du projet et génère les exécutables nécessaires.

---

## Lancer le jeu en mode CLI

Le mode CLI permet de jouer directement dans le terminal.

Après compilation avec :

```bash
make
```

on peut lancer le jeu en mode terminal avec :

```bash
./main
```

Ce mode permet d’interagir avec le jeu à l’aide de commandes textuelles. Il est utile pour tester rapidement les mécaniques principales du jeu sans interface graphique.

---

## Lancer le jeu en mode SDL

Le mode SDL permet de lancer le jeu avec une interface graphique.

Après compilation avec :

```bash
make
```

on peut lancer la version graphique avec :

```bash
./civ
```

Ce mode affiche la carte, les unités, les villes, les barbares et les différents panneaux d’information grâce à la bibliothèque SDL.

---

## Nettoyer les fichiers compilés

Le `Makefile` permet aussi de supprimer les fichiers objets et exécutables générés pendant la compilation.

Pour nettoyer le projet :

```bash
make clean
```

Cette commande supprime les fichiers temporaires comme les fichiers `.o` et permet de repartir d’une compilation propre.

---

## Organisation générale du projet

L’arborescence principale du projet est la suivante :

```txt
├── civ
├── GP
│   └── comptes-rendus
│       └── CR1.pdf
├── main
├── Makefile
├── README_CARCASSONNE.md
├── README_CIV.md
├── README.md
└── src
```

### Fichiers principaux à la racine

- `Makefile` : gère la compilation du projet.
- `main` : exécutable du mode CLI.
- `civ` : exécutable du mode SDL.
- `README.md` : fichier général de présentation du projet.
- `README_CIV.md` : documentation spécifique au jeu Civilization.
- `README_CARCASSONNE.md` : documentation spécifique à l’autre partie du projet.
- `GP/comptes-rendus/CR1.pdf` : compte-rendu de gestion de projet.

---

## Organisation du dossier `src`

Le dossier `src` contient tout le code source du jeu. Chaque sous-dossier correspond à un module précis.

```txt
src
├── barbarian
├── building
├── city
├── cli
├── configuration
├── game
├── map
├── sdl
├── sdl_map
├── sdl_panneaux
├── sprites
├── technology
├── tile
└── unit
```

---

## Description des modules

### `src/game`

Ce module contient la logique principale du jeu.

Il gère notamment :

- l’état global de la partie ;
- les tours de jeu ;
- les joueurs ;
- les actions principales ;
- la coordination entre les unités, la carte, les villes et les technologies.

Fichiers principaux :

- `game.c`
- `game.h`
- `game_test.c`

---

### `src/map`

Ce module gère la carte du jeu.

Il permet notamment :

- de créer la carte ;
- d’accéder aux cases ;
- de gérer les dimensions de la carte ;
- de placer les différents éléments du jeu.

Fichiers principaux :

- `map.c`
- `map.h`
- `map_test.c`

---

### `src/tile`

Ce module représente les cases individuelles de la carte.

Chaque case peut contenir différentes informations :

- un type de terrain ;
- une unité ;
- une ville ;
- un bâtiment ;
- une ressource ;
- un camp barbare ou un autre élément particulier.

Fichiers principaux :

- `tile.c`
- `tile.h`
- `tile_test.c`

---

### `src/unit`

Ce module gère les unités du jeu.

Les unités peuvent par exemple :

- se déplacer ;
- interagir avec la carte ;
- fonder une ville ;
- attaquer ;
- être limitées par des points de mouvement.

Le dossier contient aussi des fonctions spécifiques au mode CLI.

Fichiers principaux :

- `unit.c`
- `unit.h`
- `unit_cli.c`
- `unit_cli.h`
- `unit_test.c`

---

### `src/city`

Ce module gère les villes.

Les villes permettent au joueur de développer sa civilisation. Elles peuvent produire des ressources, construire des bâtiments ou participer à la progression globale du joueur.

Fichiers principaux :

- `city.c`
- `city.h`
- `city_test.c`

---

### `src/building`

Ce module gère les bâtiments.

Les bâtiments permettent d’améliorer les villes et d’apporter différents bonus au joueur.

Fichiers principaux :

- `building.c`
- `building.h`
- `building_test.c`

---

### `src/technology`

Ce module gère l’arbre des technologies.

Les technologies permettent de débloquer progressivement de nouvelles possibilités de jeu, comme :

- de nouvelles unités ;
- de nouveaux bâtiments ;
- de nouvelles actions ;
- des améliorations pour la civilisation.

Le dossier contient aussi une partie dédiée à l’affichage ou à l’utilisation des technologies en mode CLI.

Fichiers principaux :

- `technology.c`
- `technology.h`
- `technology_cli.c`
- `technology_cli.h`
- `technology_test.c`

---

### `src/barbarian`

Ce module gère les barbares.

Les barbares sont des entités adverses présentes sur la carte. Ils peuvent gêner le développement du joueur et ajouter un aspect stratégique au jeu.

Fichiers principaux :

- `barbarian.c`
- `barbarian.h`
- `barbarian_test.c`

---

### `src/configuration`

Ce module contient les paramètres de configuration du jeu.

Il permet de centraliser certaines valeurs utiles, par exemple :

- la taille de la carte ;
- les constantes du jeu ;
- les paramètres de départ ;
- les réglages généraux.

Fichiers principaux :

- `configuration.c`
- `configuration.h`
- `configuration_test.c`

---

### `src/cli`

Ce module contient l’interface en ligne de commande.

Il permet de jouer au jeu directement dans le terminal. Il gère l’affichage textuel, les menus, les panneaux d’information et les interactions utilisateur en mode CLI.

Fichiers principaux :

- `cli.c`
- `cli.h`
- `cli_panneaux.c`
- `cli_panneaux.h`

---

### `src/sdl`

Ce module contient la logique principale de l’interface graphique SDL.

Il gère notamment :

- l’ouverture de la fenêtre ;
- la boucle d’événements ;
- les clics utilisateur ;
- l’affichage général ;
- la coordination entre la carte et les panneaux graphiques.

Fichiers principaux :

- `sdl.c`
- `sdl.h`
- `sdl_test.c`

---

### `src/sdl_map`

Ce module gère l’affichage graphique de la carte en SDL.

Il s’occupe notamment :

- du rendu des cases ;
- de l’affichage des unités ;
- de l’affichage des villes ;
- de l’affichage des barbares ;
- de la gestion visuelle de la carte.

Fichiers principaux :

- `sdl_map.c`
- `sdl_map.h`
- `sdl_map_test.c`

---

### `src/sdl_panneaux`

Ce module gère les panneaux de l’interface SDL.

Il permet d’afficher :

- les informations du joueur ;
- les ressources ;
- les technologies ;
- les messages d’action ;
- les détails liés aux unités ou aux villes.

Fichiers principaux :

- `sdl_panneaux.c`
- `sdl_panneaux.h`
- `sdl_panneaux_test.c`

---

### `src/sprites`

Ce dossier contient les images utilisées par l’interface SDL.

```txt
barbares.bmp
batiment_construction.bmp
camp_barbares.bmp
colon.bmp
guerrier.bmp
ville_muraille.bmp
ville.bmp
```

Ces fichiers servent à représenter graphiquement les unités, les villes, les bâtiments et les barbares dans le mode SDL.

---

## Fonctionnement général du jeu

Le jeu se déroule au tour par tour.

À chaque tour, le joueur peut effectuer différentes actions selon l’état de sa civilisation.

Les actions principales sont :

- déplacer des unités ;
- explorer la carte ;
- fonder ou développer des villes ;
- produire des ressources ;
- construire des bâtiments ;
- rechercher des technologies ;
- combattre ou éviter les barbares ;
- faire progresser sa civilisation au fil des tours.

---

## Les unités

Les unités sont les éléments mobiles du jeu.

Elles peuvent se déplacer sur la carte en fonction de leurs points de mouvement.

Certaines unités peuvent avoir des rôles particuliers, par exemple :

- le colon peut fonder une ville ;
- le guerrier peut combattre ;
- certaines unités ou actions peuvent être débloquées grâce aux technologies.

Le déplacement d’une unité dépend de plusieurs conditions :

- une unité doit être sélectionnée ;
- elle doit disposer de points de mouvement ;
- la case d’arrivée doit être valide ;
- certaines cases peuvent être occupées ou bloquées.

---

## Les villes

Les villes représentent le cœur du développement de la civilisation.

Elles permettent de produire des ressources et d’améliorer la puissance du joueur.

Une ville peut notamment servir à :

- produire de la nourriture ;
- produire de l’or ;
- produire de la science ;
- construire des bâtiments ;
- renforcer la civilisation.

Les villes sont donc essentielles pour progresser dans la partie.

---

## Les bâtiments

Les bâtiments sont construits dans les villes.

Ils apportent des bonus et permettent d’améliorer les capacités de la civilisation. Selon le bâtiment construit, le joueur peut obtenir des avantages sur la production, la défense ou le développement scientifique.

---

## Les technologies

Le jeu contient un système de technologies.

Le joueur accumule de la science au fil des tours. Cette science permet de progresser dans l’arbre technologique.

Les technologies débloquent de nouvelles possibilités de jeu, par exemple :

- de nouveaux bâtiments ;
- de nouvelles unités ;
- de nouvelles mécaniques ;
- des améliorations stratégiques.

Dans l’interface SDL, la progression technologique peut être affichée à l’aide d’une barre de progression.

---

## Les barbares

Les barbares sont des ennemis présents sur la carte.

Ils peuvent être liés à des camps barbares et représentent une menace pour le joueur. Ils ajoutent un aspect stratégique : le joueur doit décider s’il veut les attaquer, les éviter ou renforcer sa civilisation avant de les affronter.

---

## Mode CLI

Le mode CLI est une version textuelle du jeu.

Il permet de tester et de jouer sans interface graphique.

Il est particulièrement utile pour :

- vérifier rapidement les mécaniques du jeu ;
- tester les déplacements ;
- tester les productions ;
- tester les technologies ;
- déboguer plus facilement certaines fonctions.

L’affichage se fait directement dans le terminal avec des commandes et des messages textuels.

---

## Mode SDL

Le mode SDL est la version graphique du jeu.

Il permet une utilisation plus intuitive grâce à :

- une carte affichée graphiquement ;
- des sprites pour les unités, les villes et les barbares ;
- des panneaux d’informations ;
- une gestion des clics ;
- une meilleure lisibilité de l’état de la partie.

Ce mode utilise les fichiers du dossier `src/sprites`.

---

## Tests

Plusieurs modules possèdent des fichiers de test.

Par exemple :

- `game_test.c`
- `map_test.c`
- `unit_test.c`
- `city_test.c`
- `technology_test.c`

Ces fichiers permettent de tester séparément les différents modules du jeu.

Selon les règles définies dans le `Makefile`, les tests peuvent être compilés et exécutés séparément afin de vérifier le bon fonctionnement des fonctions principales.

---

## Dépendances

Le projet est écrit en langage C.

Pour le mode SDL, il faut disposer des bibliothèques SDL utilisées par le projet, notamment SDL2 et SDL2_gfx selon l’environnement de compilation.

Sur macOS avec Homebrew, elles peuvent généralement être installées avec :

```bash
brew install sdl2 sdl2_gfx
```

Sur Linux, l’installation dépend de la distribution utilisée.

Par exemple, sur une distribution basée sur Debian ou Ubuntu :

```bash
sudo apt install libsdl2-dev libsdl2-gfx-dev
```

---

## Compilation manuelle

La compilation manuelle n’est normalement pas nécessaire.

Le `Makefile` est prévu pour gérer automatiquement :

- les fichiers sources ;
- les dépendances entre modules ;
- la génération des fichiers objets `.o` ;
- la création des exécutables ;
- le nettoyage du projet.

Il est donc conseillé d’utiliser uniquement :

```bash
make
```

puis :

```bash
./main
```

ou :

```bash
./civ
```

selon le mode voulu.

---

## Résumé des commandes utiles

| Commande | Description |
|---|---|
| `make` | Compile le projet |
| `./main` | Lance le jeu en mode CLI |
| `./civ` | Lance le jeu en mode SDL |
| `make clean` | Supprime les fichiers générés par la compilation |

---

## Remarque sur les fichiers générés

Certains fichiers visibles dans l’arborescence peuvent être générés automatiquement par la compilation, par exemple :

```txt
*.o
main
civ
*_test
```

Ces fichiers ne sont pas forcément à écrire ou modifier directement. Ils sont produits par le `Makefile` à partir des fichiers sources `.c` et `.h`.

Dans un dépôt Git propre, on peut choisir de ne pas versionner ces fichiers générés et de les ignorer avec un fichier `.gitignore`.

Exemple :

```gitignore
*.o
main
civ
src/*/*_test
.DS_Store
```

---

## Auteurs

Projet réalisé dans le cadre du projet de programmation.