# Algorithmes de génération de cartes procédurales

#### Introduction 

Dans le cadre du développement d'un jeu de stratégie comme Civilization, la création de la carte consistue un enjeu majeur pour l'expérience de jeu. L'objectif est d'obtenir une carte géographiquement cohérente, présentant une répartition logique des biomes, des vastes étendues océaniques et des chaînes de reliefs.

Ce document présente le cheminement technique ainsi que l'analyse des différents algorithmes étudiés pour accomplir cette génération procédurale du terrain.

## 1. Génération naïve et aléatoire

L'approche initiale a consisté en une méthode naïve basée sur un hasard total. 
Pour chaque coordonnée de la grille bidimensionnelle, un type de terrain (eau, plaine, forêt, montagne, désert, toundra) est attribué de manière indépendante en fonction de probabilités fixes définies à l'avance et l'utilisation d'un générateur de nombres aléatoires.

AVANTAGES : L'implémentation est extrêmement simple et le temps d'exécution est instantané. Cela nous a permis de faire rapidement des premiers tests d'affichage et de structures de données.

INCONVENIENTS : Il n'y avait aucune cohérence spatiale. On pouvait par exemple avoir une ville entourée d'eau, rendant la partie techniquement injouable. Et on observait des tuiles de biome totalement isolées.

## 2. Analyse du Bruit de Perlin (NON IMPLEMENTE)

Pour pallier le chaos de la génération naïve, on a énormément entendu parler de l'algorithme de bruit de Perlin.
C'est certainement l'algorithme le plus connu en terme de génération procédurale de terrain. 
Il permet de générer des transitions fluides et naturelles entre les biomes et tuiles.

Cependant, cette approche n'a pas été retenue dans le cadre de ce projet pour les raisons suivantes :
- Complexité mathématique : Son implémentation requiert la gestion d'une grille de vecteurs de gradients pseudo-aléatoires, le calcul de produits scalaires et l'utilisation de fonctions d'interpolation. Un algorithme bien trop compliqué à comprendre alors qu'on peut faire plus simple.
- Son avantage : Le bruit de Perlin excelle principalement dans le création de mondes infinis ou de terrains en haute résolution. Sa force est de pouvoir évaluer le biome d'une coordonnée (x, y) de manière déterministe sans jamais avoir besoin de calculer ou de stocker les tuiles voisines en mémoire.
Pour des jeux avec une map infinie comme Minecraft, il est indispensable de pouvoir générer seulement un chunk (une partie de la map) sans connaître les autres.
Toutefois ici, pour une carte de dimensions 50x30 par défaut, il n'est pas dérangeant de générer la map entièrement.

## 3. Algorithme d'automate cellulaire

Nous avons donc décidé d'implémenter un algorithme d'automate cellulaire qui allie génération cohérente de biome et simplicité. Cette méthode repose sur le principe de propagation de proche en proche.
Cette technique permet de regrouper efficacement les blocs isolés pour former de véritables masses continentales et des océans bien définis, répondant parfaitement aux exigences structurelles du jeu.

Le processus se déroule en plusieurs étapes clés :
1. Génération d'une carte de départ aléatoire (basée sur l'approche naïve).
2. Boucles d'itération sur chaque cellule : On regarde les 8 cases voisines.
Si une cellule est entourée d'une majorité de cellules de terre par exemple, elle se transforme en terre.
3. Répétition du processus sur un nombre fixe d'itérations (généralement entre 3 et 5) afin
de lisser les contours et consolider les structures.


## SOURCES :
https://fr.wikipedia.org/wiki/Bruit_de_Perlin
https://www.youtube.com/watch?v=eh5N1zQiiVQ
https://fr.khanacademy.org/computing/computer-programming/programming-natural-simulations/programming-noise/a/perlin-noise
https://musiquealgorithmique.fr/automates-cellulaires/
