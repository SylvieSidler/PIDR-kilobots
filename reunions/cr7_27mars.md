# Réunion n°7 du 27 mars

## Backlog 
- réorganisation des fichiers dans le git (directories src et C)
- ajout de la résolution avec `/set_resolution/<mode>`
- fix [move.c](../src/C/move.c) et autres fichiers pour fonctionner avec simulateur **et** kilobots
- tester avec le simulateur kilombo les fichiers non testés avec les robots 
- naming game en cours de test sur simulateur 

***
## Discussion et avancement

### Détection
pouvoir tester en boucle avec la détection en vidéo sur [circles.py](./../misc/circles.py)

### Code naming game & sent_receive.c 
utiliser la parité de l'id des kilobots pour savoir qui est 'speaker' et qui est 'listener' 
-> avoir un nombre pair de kilobots
-> utiliser un switch pour vérifier la valeur de l'id 

***

# Prochaine réunion 

## Jeudi 27 mars - 15h30 
## TODO 
- ajouter le parametre de taille d'image 
- fix [sent_receive.c](../src/C/send_receive.c)
- tester avec le simulateur kilombo les fichiers non testés avec les robots 
- modifier les prétraitements de la détection 


