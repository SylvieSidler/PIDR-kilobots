# pidr-24-25-kilobots

## Description
Les Kilobots sont des robots de petite taille (3cm de diamètre) utilisés dans l’études de la robotique en essaim. 
Il s’agit de dispositifs robotiques avec des dizaines d’éléments. Ces robots sont capables de communiquer localement et de se mouvoir par vibration. Cette plateforme est un banc d’essai, qui permet d’étudier à petite échelle en laboratoire des algorithmes qui pourront être déployés sur d’autres robots de plus grande envergure. 

Dans le domaine de la robotique en essaim, l’émergence de comportements collectifs cohérents à partir d’interactions locales représente un défi majeur. Ce projet se concentre sur l’implémentation et l’analyse du _Naming Game_, un modèle d’émergence de consensus linguistique, au sein d’un groupe de robots Kilobots. 
Notre objectif est d’évaluer les dynamiques temporelles nécessaires à un groupe de N robots pour atteindre un accord global. Nous avons adapté le Naming Game aux contraintes matérielles des Kilobots et développé une approche méthodologique combinant trois axes : la programmation des robots, l’utilisation du simulateur Kilombo, et l’analyse d’images pour le suivi des interactions. 
Les résultats montrent que la structure du réseau d’interaction impacte significativement le temps de convergence, les configurations se rapprochant d’un graphe complet atteignant un consensus plus rapidement.

**Mots clés** : robotique en essaim, analyse d’image, diffusion de l’information, Naming Game
**Environnement de travail** (matériel, logiciel) : Une arène robotique en construction sera mise à disposition des étudiants ainsi qu’un ensemble de robots. Les kilobots sont programmables en C, quelques connaissances dans ce langage seront nécessaires. Un simulateur de robots est aussi disponible pour test avant le déploiement. Les étudiants auront accès au CreativeLab du Loria.

## Installation
Le [compte rendu n°2 de réunion](./reunions/cr2_0502.md) décrit toutes les procédures à suivre pour installer les bibliothèques nécessaires.

## Rapport de projet 
Le rapport sous format d'article de recherche est disponible [ici](./PIDR_Rapport_Biche_Sidler_Vernicos.pdf) 
