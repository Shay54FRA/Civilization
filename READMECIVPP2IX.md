# Projet Civilization - Jeu de stratégie en C

## Présentation du projet

Ce projet est une adaptation simplifiée du jeu **Civilization**, développée en langage C dans le cadre du projet de programmation.

Le jeu propose deux modes d’utilisation :

- un **mode CLI** jouable directement dans le terminal ;
- un **mode SDL** avec interface graphique.

L’objectif est de développer une civilisation en gérant des unités, des villes, des bâtiments, des technologies et des ressources, tout en explorant la carte et en interagissant avec des barbares.

Le projet est organisé de manière modulaire afin de séparer clairement les différentes parties du jeu : carte, unités, villes, technologies, interface CLI, interface SDL, etc.

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

## Compilation du projet

Le projet est fourni avec un `Makefile` qui gère automatiquement la compilation des différents fichiers source.

Il n’est donc pas nécessaire de compiler les fichiers un par un à la main : le `Makefile` s’occupe de compiler les modules, de générer les fichiers objets `.o` et de créer l’exécutable principal.

Depuis la racine du projet, il suffit d’utiliser :

```bash
make
```

Cette commande compile l’ensemble du projet et génère l’exécutable principal :

```bash
./civ
```

---

## Lancement du jeu

Le jeu se lance avec l’exécutable :

```bash
./civ
```

On peut ensuite choisir le mode de jeu et certains paramètres directement grâce aux options de lancement.

---

## Lancer le jeu en mode SDL

Le mode SDL correspond à la version graphique du jeu.

Exemple de lancement :

```bash
./civ --mode sdl --width 40 --height 25 --seed 12345
```

Cette commande lance le jeu :

- en mode graphique SDL ;
- avec une carte de largeur `40` ;
- avec une carte de hauteur `25` ;
- avec la graine de génération `12345`.

La graine, ou `seed`, permet de générer une carte reproductible. Cela signifie que si l’on relance le jeu avec la même seed, on obtient la même génération de carte.

Si les options courtes sont utilisées, la commande équivalente peut être écrite sous la forme :

```bash
./civ -m sdl -w 40 -h 25 -s 12345
```

Le mode SDL affiche la carte, les unités, les villes, les barbares et les différents panneaux d’information grâce à la bibliothèque SDL.

---

## Lancer le jeu en mode CLI

Le mode CLI correspond à la version textuelle du jeu, jouable directement dans le terminal.

Exemple de lancement :

```bash
./civ -m cli -t 150 -b 5
```

Cette commande lance le jeu :

- en mode terminal ;
- avec une limite de `150` tours ;
- avec `5` camps barbares, si l’option `-b` correspond bien au nombre de camps barbares dans l’implémentation.

Le mode CLI est utile pour tester rapidement les mécaniques principales du jeu, comme les déplacements, les productions, les technologies et les actions de tour.

On peut aussi utiliser une forme longue si elle est prévue par le programme :

```bash
./civ --mode cli --turns 150 --barbarians 5
```

---

## Options de lancement

Le programme accepte plusieurs options permettant de configurer la partie au lancement.

| Option longue | Option courte | Description | Exemple |
|---|---|---|---|
| `--mode` | `-m` | Choisit le mode de jeu : `cli` ou `sdl` | `--mode sdl` |
| `--width` | `-w` | Définit la largeur de la carte | `--width 40` |
| `--height` | `-h` | Définit la hauteur de la carte | `--height 25` |
| `--seed` | `-s` | Définit la graine de génération de la carte | `--seed 12345` |
| `--turns` | `-t` | Définit le nombre maximal de tours | `-t 150` |
| `--barbarians` | `-b` | Définit le nombre de camps barbares ou d’éléments barbares selon l’implémentation | `-b 5` |

---

## Nettoyer les fichiers compilés

Le `Makefile` permet aussi de supprimer les fichiers objets et exécutables générés pendant la compilation.

Pour nettoyer le projet :

```bash
make clean
```

Cette commande supprime les fichiers temporaires comme les fichiers `.o` et permet de repartir d’une compilation propre.

---

## Résumé des commandes utiles

| Commande | Description |
|---|---|
| `make` | Compile le projet |
| `./civ --mode sdl --width 40 --height 25 --seed 12345` | Lance le jeu en mode SDL avec une carte 40x25 et une seed précise |
| `./civ -m sdl -w 40 -h 25 -s 12345` | Lance le jeu en mode SDL avec les options courtes |
| `./civ -m cli -t 150 -b 5` | Lance le jeu en mode CLI avec 150 tours et 5 barbares |
| `./civ --mode cli --turns 150 --barbarians 5` | Lance le jeu en mode CLI avec les options longues |
| `make clean` | Supprime les fichiers générés par la compilation |

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
- `civ` : exécutable principal du jeu.
- `main` : ancien exécutable ou exécutable généré selon la configuration du projet.
- `README.md` : fichier général de présentation du projet.
- `README_CIV.md` : documentation spécifique au jeu Civilization.
- `README_CARCASSONNE.md` : documentation spécifique à l'autre jeu proposé.
- `GP` : tout le contenu de la gestion de projet.

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
- `barbarian_test.c`
- `building_test.c`
- `configuration_test.c`
- `sdl_test.c`
- `sdl_map_test.c`
- `sdl_panneaux_test.c`

Ces fichiers permettent de tester séparément les différents modules du jeu.

Selon les règles définies dans le `Makefile`, les tests peuvent être compilés et exécutés séparément afin de vérifier le bon fonctionnement des fonctions principales.

---

## Compilation manuelle

La compilation manuelle n’est normalement pas nécessaire.

Le `Makefile` est prévu pour gérer automatiquement :

- les fichiers sources ;
- les dépendances entre modules ;
- la génération des fichiers objets `.o` ;
- la création de l’exécutable ;
- le nettoyage du projet.

Il est donc conseillé d’utiliser uniquement :

```bash
make
```

puis de lancer le jeu avec les options voulues, par exemple :

```bash
./civ --mode sdl --width 40 --height 25 --seed 12345
```

ou :

```bash
./civ -m cli -t 150 -b 5
```

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
```
CHAUMONT Camille
DURIN Grégoire
LEFEBVRE Nathan
MURIS Eliott
```
---
Projet réalisé dans le cadre du projet de semestre 6.