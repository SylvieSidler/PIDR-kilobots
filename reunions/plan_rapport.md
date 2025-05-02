# Rapport sous forme d'article scientifique - notes

penser à mettre les numéros de pages

# 1ère page
- un titre : Analyse et implémentation de stratégies de diffusion d'information dans un essaim de Kilobots 
- la liste des auteurs et leurs affiliations : les noms des membres du groupe et de l’encadrant du projet
nom prénom, affiliation, email & 
- un résumé
- une liste de mots-clés : robotique en essaim, analyse d'image, diffusion de l'information

# Introduction 
petite photo des kilobots
- le contexte général du projet : domaine, application, processus,...
"dans des systemes distribués, on s'est interessés à l'arrivée à un consensus appliqué sur des kilobots avec le naming game"
- la problématique : identifier précisément le problème ou la question spécifique du projet.
cb de temps faut-il à N robots pour se mettre d'accord ?
- les contributions : décrire les grandes lignes de l’approche proposée pour répondre à la problématique.
- la présentation de la démarche de l'article : plan.

# État de l'art 
Une revue de littérature pour un projet de type « découverte de la recherche » (état de l’art). 

Dans ce projet, on s'intéresse à k'implémentation du naming game pour des kilobots
Algo du naming game général (canonique)
ce que les autres ont fait : 
introdution quand par qui ? 
qui les a fait pour les kilobots et qu'est-ce qu'ils ont fait 
-> récupérer les informations essentielles 

# Méthodologie
Expliquer en détails les principales étapes de la méthodologie :
- analyse du problème
- solutions(s) proposée(s), critères de choix
- matériel :
les **kilobots**, la caméra, l'arène, la douche ...
- conception, implémentation


sections pour les kilobots, l'arene, ce qu'on a fait comme experimenta°

types de topologies de tests mesurer le temps de convergence 
préciser la démarche sci 

Problèmatique : dépend de la topologie, le nb de mots, faire test de diff configurations 
fixer selon le nb de mots
fixer la topologie : graphes spécifiques fixés : graphe connecté (chemin etre chaque paire de noeud), graphe complet (chaque kilobot peut discuter avec tous les autres noeuds), en ligne, en rond, avec des influenceurs (2 clusters rejoints par une ligne), en étoile, 

-> récupérer des chiffres -> faire varier :
- le nb de robots /!\ pas toujours applicable (densité doit rester la meme)
- le rayon de communication

pour telle configuration, graphe, on a tels 

### kilobots : 
moins de tests que sur simulateur 
robots disposés, on a la caméra qui détecte les couleurs,
on lance, on a les robots qui bougent ou restent statiques et mesurer le temps (normal que corresponde pas à la simulation : reality gap)

-> détection 
basée sur l'algo de Hugh avec des images, la calibration, 

## Analyse d'image 
reconnaissance de kilobot
image statique puis temps réel -> explication
graphe de communication visible en réalité augmentée

## Robots & communication 
-> différentes stratégies de communication 
- diffusion seule
- comunication entre 2 kilobots 
- 1 qui communique et des kilobots qui écoutent 
- plusieurs kilobots qui diffusent et plusieurs qui reçoivent 
- idem avec changement de rôles aléatoire
- déplacement des kilobots listener si pas de message reçu pendant 4 secondes

### Naming game 
associer 1 mot à 1 objet

Univers : 
- 1 objet 5 mots
- 1 objet pleins de mots
- 2 objet 5 mots
- 2 objets plein de mots

# Résultats 
Présentation et interprétation : tous les tableaux et figures sont numérotés et doivent être cités dans le texte.

mettre les graphes de résultats des temps réalisés par le naming game dans chaque cas 

## Analyse d'image 

## Robots, communication & naming game 
temps moyen pris pour arriver à un consensus 
photos de différentes configurations : tas, ligne, rond, ....

# Conclusion
une conclusion qui fait le point sur les résultats obtenus, leurs limites et éventuellement des développements ultérieurs envisagés.
Une liste de références bibliographiques qui contient les articles, rapports techniques, thèses et autres publications cités dans l’article.

on recap : "le sujet c'était de faire ça , on l'a fait avec le naming game et on s'intéresse au temps de convergence"
qu'est-ce qu'il reste à faire : future work 

# Remerciements
L'encadrant de projet M. Boumaza, Loria et TN

# Référence - sources
Une référence doit être présente sous la forme : 
nom des auteurs, titre de la référence, nom du journal/éditeur, références éditoriales (numéro de page pour une revue,…), année de publication. 
À éviter : les références de pages Web (webographie), les travaux non publiés.

# Annexes 
une série d’annexes : détails techniques, tableaux, données, etc, qui ne sont pas essentielles à la compréhension de l’article.

