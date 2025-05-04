# Réunion n°10 du 2 mai

## Backlog 
- fix du comportement naming game avec simulateur

***
## Discussion et avancement

### Naming game 

+ déplacement kilobots 

Si un kilbots en état de `LISTENER` n'a pas recu de message au bout de 4 secondes, il se déplace dans une direction aléatoire.

+ faire une course entre les kilobots 

[move_straight.c](../src/C/move_straight.c) -> réality gap (différence simulateur / réalité) : les robots ne vont pas tous tout droit contrairement à ce qui a été programmé

### Détection 
détection de la couleur et affichage 
choix si affichage des liens de communication ou des cercles de communication 

### Rapport 
voir le [plan](./plan_rapport.md)

### Soutenance 
reprendre le rapport mais en plus sythétique 
étudier la prise de décision décentralisée = auto gestion
illustration par le naming game : convergence vers une association objet-mot 

1. 1ère slide : problématique 
2. setup / état de l'art : qui a fait quoi dans la thématique, présenter le principe de l'algo, montrer rapidement avec un visuel (photo des kilobots)
3. expliquer la dépendance temps de convergence avec les différents 
4. vidéo 

faire comprendre les grandes lignes du rapport mais pas tous les détails
on parle à des scientifiques, pas besoin de trop vulgariser, il faut laisser de la marge pour les qestions après 

La soutenance est notée sur :
- le respect du temps
- la qualité des slides
- la qualité de la prise de parole
- les réponses aux questions

***

# Prochaine réunion 

## Date : à définir
## TODO 
- analyser si les couleurs sont les mêmes : les robots sont arrivés à un consensus 
- avancer sur le rapport 
