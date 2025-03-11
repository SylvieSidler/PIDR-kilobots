# Réunion n°5 du 6 mars

## Backlog 
- serveur HTTP python Flask : 
dans un autre terminal on fait une requete pour avoir l'image 
-> réponse HTTP + sauvegarde de l'image sur ordi 
serialiser les données : on récupère la matrice et pas l'image en jpeg 
- détection d'images : reconnaissance des **kilobots** dans l'arène *uniquement* et renvoi des positions du centre des cercles
- manipulation du code [blink](../blink.c) & [move](../move.c) (version moins lourde [ici](../move_simple.c)) en se basant sur cette [ressource](https://github.com/acornejo/kilobot-labs/?tab=readme-ov-file#lab0-blinky)
- fix en cours en local pour utiliser KiloGUI sur Mac

*** 

## Discussion et avancement

`frame` et `imp` sont des tableaux numpy à sérialiser 
Après avoir désérialisé, on retrouve le même type de données.
Utilisation de json

***
# Prochaine réunion 
## Jeudi 13 mars - 13h30
## TODO 
- afficher les positions à l'image 
:warning: **lisibilité** : le cercle est petit & risque de problème d'affichage si il y a beaucoup de kilobots au même endroit :rightarrow: faire une légende 
- afficher l'image depuis openCV qui réalise des requêtes au serveur
- mise à jour des [requirements](../requirements.txt)
- tester sur KiloGUI le code blink et move
