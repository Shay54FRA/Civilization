# Documentation Technique - Early Civilization

Ce document détaille l'architecture, les structures de données et les algorithmes principaux du projet **Early Civilization**, un jeu de stratégie au tour par tour inspiré de la célèbre licence.

---

## 1. Architecture Globale

Le projet suit une architecture modulaire en C, séparant strictement la logique métier (le "moteur") des interfaces utilisateur (CLI et SDL).

### Organisation des dossiers (`src/`)
- `game/` : Moteur de jeu central, gestion des tours et des ressources.
- `map/` : Représentation et génération de la carte du monde.
- `tile/` : Définition des tuiles et de leurs propriétés.
- `city/` : Gestion des villes, de leur population et des projets.
- `unit/` : Gestion des unités (Colons, Guerriers, Éclaireurs), mouvement et combat.
- `technology/` : Arbre technologique et système de recherche.
- `barbarian/` : Intelligence artificielle et gestion des camps barbares.
- `cli/` : Interface en ligne de commande (utilisant `ncurses`).
- `sdl/` : Interface graphique (utilisant `libSDL2`).
- `configuration/` : Gestion des paramètres de lancement.

---

## 2. Structures de Données Principales

### Le Coeur : `Game`
La structure `Game` (`src/game/game.h`) centralise l'état complet d'une partie.
- `gold`, `science` : Ressources globales.
- `active_turn` : Compteur de tours.
- `map` : Pointeur vers la structure `Map`.
- `unitList`, `cityList`, `barbarianList` : Listes chaînées des entités en jeu.
- `tech_tree` : Arbre technologique du joueur.

```c
typedef struct _Game {
    int gold; //Ressource globale partagé entre toutes les villes
    int science; //idem, pour l'arbre de technologie
    int active_turn; //Tour en cours
    bool poverty; //Etat de greve ou non
    int turns_10_cities; //Nombre de tours consécutifs avec +10 villes
    int turns_no_productions; //Nombre de tours consécutifs productions nulles
    int barbs_number; //Nombre de barbares sur la carte
    TupleRessources* new_ressources;
    Position starting_point; //Utile pour placer les camps
    Map* map; //Carte de la partie
    UnitList* unitList; //Liste des unités du joueur
    CityList* cityList; // Liste des villes du joueur
    Configuration* configuration; // Configuration de la partie (nbr de camps barabares, taille map,...)
    BarbarianList* barbarianList;
    TechTree* tech_tree;
    int active_research_id;   // Le post-it pour retenir le projet en cours
} Game;
```

```c
typedef struct _Configuration {
    int w;          //largeur
    int h;          // hauteur
    unsigned int s; // graine (entier forcément positif)
    int t;          // nombre max de tours
    int b;          // nombre de camps de barbares (nombre de barbares limité à 3 * b)
    int is_sdl;     // 1 si mode SDL, 0 si mode CLI
} Configuration;
```

### La Carte : `Map` et `Tile`
La carte est une grille de tuiles (`src/map/map.h`).
- `Map` : Contient un tableau 2D de pointeurs vers des `Tile`, ainsi que ses dimensions.
- `Tile` : Représente une case avec :
    - `biome` : Type de terrain (Plaine 'P', Forêt 'F', Montagne 'M', Eau 'E', Désert 'D', Toundra 'T').
    - `city_on`, `camp_on` : Indicateurs de présence de ville ou de camp barbare.
    - `unit`, `barb_on` : Pointeurs vers l'unité ou le barbare présent.
    - `fog_level` : Niveau de visibilité (0: inconnu, 1: exploré, 2: visible).

### Les Entités : `Unit`, `City` et `Barbarian`
- `Unit` (`src/unit/unit.h`) : Possède des points de vie (PV), d'attaque (ATK), de défense (DEF) et de mouvement (PM).
    - Colon     : 10pv / 0atk / 1def / 2pm / 2vision
    - Guerrier  : 15pv / 3atk / 2def / 3pm / 2vision
    - Eclaireur : 10pv / 0atk / 2def / 4pm / 4vision

```c
typedef struct _Unit {
    char type;

    int atk;
    int def;
    int cost_per_turn;
    int fog_range;

    int pv;
    int max_pv;

    int pm;
    int max_pm;

    Position pos;
} Unit;
```
- `City` (`src/city/city.h`) : Gère sa population, ses réserves de nourriture, sa production et son projet actuel (unité ou bâtiment), avec un pointeur vers une liste chainée de batiments `buildings`. Au lieu de stocker les pv, il stock les dégàts subits ce qui simplifie les calcules lorsqu'il y a diminution/augmentation des pv max : l'accès au pv se fait par la variable globale `CITY_PV`. 7 types de batiments présents : 

| Type | Nom | BONUS | Entretien (or) | Coût (prod) |Techno requise |
| :--- | :--- | :--- | :--- | :--- | :--- |
| G | Grenier | +3 food/tour et seuil croissance /1.5 | 1 | 30 | - |
| A | Atelier | +3 prod/tour | 1 | 40 | - |
| B | Bibliothèque | +4 science/tour | 1 | 50 | Ecriture |
| M | Marché | +3 gold/tour | 1 | 40 | Commerce |
| C | Caserne | Produire des unités | 2 | 60 | - |
| R | Rempart/Muraille | Double PV ville | 2 | 80 | Maçonnerie |
| P | Phare (Extension) | Ville +2 rayon de vision | 0 | 100 | Mirador |

```c
typedef struct _City {
    int population;
    int damage;
    int food;
    int production;
    int strength;
    int basements_number; //Nombre de greniers
    int walls_number; //Nombre de murailles (Connaître le multiplicateur des pv)
    Position pos;
    TupleRessources* new_ressources;
    bool can_produce_unit; //Présence d'une caserne
    bool has_phare; //Présence d'un phare
    bool has_taken_damage; //Possibilité de régénération à la fin du tour
    Project* project;
    BuildList* buildings;
} City;
```

```c
typedef struct _Project {
    char type; //Minuscule pour unité, Majuscule pour batiment
    Position pos; //Position jamais utilisé
    int production_cost; //Coût en production restant du projet
} Project;
```
Le champ pos est devenu obsolète -> Au départ on était parti sur une version plus "Clash of Clan" c'est-à-dire un batiment par tuile, mais au vu des difficultés techniques on est retourné au modèle simple sur une tuile où même l'unité apparaît par dessus

- `Barbarian` (`src/barbarian/barbarian.h`) : Même statistiques qu'un guerrier, sans le champ de vision 

### Le système : les listes chaînées
Pour une grande partie des structures (`Tile` , `City` , `Building`, `Barbarian`, `Unit`) il existe une contrepartie sous forme de liste chaîné (`TileList`, `CityList`, `BuildList`, `BarbarianList`, `UnitList`). Il est difficile de prédire l'issue d'une partie et surtout le nombre de chacune des entitées. Ainsi plutôt qu'un tableau dynamique ou prévu beaucoup trop large nous avons opté pour des listes chaînés qui s'adaptent mieux au changement de taille récurrent. Elles prennent la forme suivante : 
```c
struct _BuildList {
    Building* data;
    struct _BuildList* next;
}
```

---

## 3. Algorithmes et Mécaniques

### Séquence d'un tour
Le passage au tour suivant (`end_turn` dans `src/game/game.c`) suit cet ordre :
1. **Calcul des ressources** : Perception de l'or et de la science, application des bonus technologiques.
2. **Mise à jour des villes** : Croissance de la population, calcul de la famine, avancement des projets de production.
3. **Phase des Barbares** : Déplacement et attaque des barbares existants via un algorithme de recherche de cible puis apparition de nouveaux barbares dans les camps, l'ordre a été choisi de manière à éviter un mouvement trop rapide.
4. **Réinitialisation** : Les unités et barbares récupèrent leurs points de mouvement, potentiel soin des villes.
5. **Fog of War** : Mise à jour de la visibilité autour des unités et des villes.

### Calcul des ressources
`City` et `Game` possède `TupleRessources` pour stocker simplement 2 integer. Il permet d'accumuler les ressources bonus brutes avant applications des bonus technologiques. Il est ensuite attribué aux cagnottes correspondante et est remis à 0 

- Le calcul des bonus de batiments se fait en bouclant sur la liste chaînée de ses batîments, cela pour toutes les villes.
- Pour le bonus de biome, afin de s'assurer que la plus vieille ville récupère la tuile, on récupère par ville par rayon : 
    - (Ville_1, r=0), ..., (Ville_m, r=0), (Ville_1, r=1), ... (Ville_m, r=n) où n est limité au rayon d'exploitation de la ville -> `EXPLOITATION_RANGE = ((city->population > 25) + (city->population > 9) + 1)`
- Les bonus de l'arbres technologiques sont appliqués à la fin de tous les calculs, pour éviter d'avoir que des +10% de 2 ou 3 toujours arrondi à la même valeur

### Génération de Carte
La carte est générée procéduralement à partir d'une graine (`seed`). Un algorithme de **lissage** (`smooth_map` dans `src/map/map.c`) est appliqué après une génération aléatoire initiale pour créer des zones de biomes cohérentes et des continents. Cette méthode provient d'un algorithme d'automates cellulaires, détaillé dans l'état de l'art.

### Système de Combat
Le combat est résolu instantanément lorsqu'une unité tente de se déplacer sur une case occupée par un ennemi. Les dégâts sont calculés selon la formule :
- `Dégâts = max(1, ATK_attaquant - DEF_cible)`
- Une riposte est également calculée simultanément, cette fois-ci pouvant valoir 0 si la défense de l'attaquant est assez élevée

### Intelligence Artificielle des Barbares
Les barbares utilisent une recherche de la cible la plus proche (unité ou ville du joueur) et se déplacent vers elle en utilisant l'algorithme de **Dijkstra** pour optimiser leur trajet à travers les différents coûts de terrain.

---

## 4. Interfaces

### Mode CLI (`ncurses`)
- Utilise des caractères ASCII colorés pour représenter la carte : sysème de coloration par pairs background/foreground (couleur de fond et couleur de police)
- Navigation au clavier (flèches directionnelles).
- Menus interactifs pour la gestion des villes et des technologies.

### Mode SDL (`SDL2`)
- Rendu graphique basé sur des sprites (fichiers `.bmp` dans `src/sprites/`).
- Navigation à la souris (traquage du pointeur par rapport aux bords de la fenêtre)
- Affichage d'une carte à tuiles avec gestion du défilement.
- Panneaux latéraux pour les informations de jeu et les actions.

---

## 5. Compilation et Tests

Le projet utilise un **Makefile** pour automatiser la compilation et les tests.
- `make` / `make all` : Compile l'exécutable `civ` et tout le projet.
- `make cli` / `make sdl` : Compile et lance le jeu dans le mode souhaité.
- `make test` : Compile et exécute les suites de tests unitaires pour chaque module (ex: `src/map/map_test.c`).
- `make req` : Installe les dépendances nécessaires (SDL2, Ncurses).
- `make clean` : Supprimer les fichiers exécutables (en gardant les données de sauvegarde)
- `make reset` : Supposément make clean mais en supprimant les sauvegardes aussi, dans ce projet il n'y a pas de données de sauvegarde donc les 2 sont identiques

---

## 6. Extensions Implémentées

### **Brouillard de Guerre** (EXT-6) : Système de visibilité dynamique sur 3 niveaux.

On y retrouve le système de brouillard, il est entièrement géré par `update_fog`. 2 nouvelles technologies sont implémentées avec celles-ci une nouvelle unité : l'éclaireur, et un nouveau batiment : le phare.
- Le brouillard : une case visible n'a aucun brouillard, une case non visible mais qui l'a déjà été possède son biome visible mais le contenu est inconnue (présence potentielle d'un barbare ou camp), une case jamais visité ne donne absolument aucune information 
- `update_fog` : appelé à chaque raffraichissement de l'écran, d'abord toutes les cases visibles (`tuile->fog_level = 2`) en cases explorées, puis recalcule les cases visibles en bouclant sur la liste des unités et la liste des villes
- 2 nouvelles technologies :
    - Mirador : disponible directement contre 40 sciences, elle débloque le batiment Phare
    - Expédition : disponible après Mirador contre 80 sciences, elle débloque l'éclaireur ainsi qu'un bonus de champ de vision d'unité de 1
- Phare : 100 productions, 0 gold d'entretien, il augmente le champ de vision de la ville de 2.
- Eclaireur : 40 productions, 1 gold d'entretien, 10pv / 0atk / 2def / 4pm /4 rayon de vision

## 7. Les choix

Les données du jeu sont reprises dans l'ensemble depuis le sujet proposé mis à part ces 2 points : 

- Rééquilibrage : avec les données du sujet, la partie était surtout un jeu de chance sur le point de départ -> si l'eau ou le désert n'est pas dans un rayon de 3 cases, la pauvreté est assuré et toute unité meurt après un tour -> C'est pourquoi le système de pauvreté a été retiré tant qu'il n'y a pas de production d'or

- Condition de défaite : l'ambiguité autour de la règle "Aucune production pendant 5 tours" nous a amené après discussions à penser que c'était la production des ressources type food ou or. Sauf que chaque ville commence avec un grenier donc une production est toujours assurés. Ainsi cette condition de défaite n'a pas été implémentée


## 8. Les limites rencontrées

- La principale erreur rencontrée est un défaut de compréhension du sujet : comme expliqué plus haut au départ on prévoyait une case par batîment, tout un calcul complexe de rayon d'exploitation avait été fait dessus mais au moment d'implémenter la muraille et la terminaison de projet les choix commencaient à être trop complexes ainsi on a changé drastiquement d'approche

- Le CLI limite grandement l'usage de figures complexes : la création d'hexagones semblait trop difficile

- Le taux de raffraichissement SDL : on désirait en faire un jeu à la souris mais le rendu finalement se retrouve assez saccadé ce qui rend l'expérience de jeu moins satisfaisante

- Les tests de parties entières : Une partie prend un temps modérément long ainsi la quantité de tests permettant d'aller jusqu'au bout est assez limité et bien souvent avec des versions boostés du jeu
