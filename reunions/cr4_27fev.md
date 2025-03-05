# Réunion n°4 du 27 février

## Backlog 
- réception du mail avec les ressources
- serveur HTTP python avec flask et 2 routes : `/capture_image` pour la capture d'image et `/video_feed` pour la video en direct de ce qui est filmé
- reconnaissance d'un cercle dans une image 

*** 

# Retour sur le fonctionnement web
Il faudrait faire un client HTTP au lieu d'un serveur.

Utiliser un appel de procédure à distance : RPC (Remote Procedure Call) qui sollicite un service (appel de méthodes ou fonctions distantes) auprès d'un autre programme.
Exemple du fonctionnement : un programme appelle une fonction d'un autre processus, la méthode s'exécute dans le processus et renvoie la réponse au programme initial.

Dans notre cas, la requête vers un processus extérieur est faite du serveur web 

On ne fait que répondre à des requêtes GET : transmettre des paramètres mais pas de données.

Quand le processus démarre, il faut qu'il soit à l'arrêt pour donner une sémantique.
- start : récupère l'accès à la caméra
- stop : couper la caméra
- capture_image : capture de la vue de caméra 

Quand il y a un problème il faudrait renvoyer un code d'erreur (côté serveur ≈ 500) au lieu de couper l'accès, ça permettra d'avoir une réponse HTTP.

flask : comment transmettre données images dans une réponse HTTP en brut à sérialiser 
L'objet frame représente l'image dans [[../app.py]].

# Reconnaissance de forme 
L'algo de Hugh est en pipeline : seuillage, détection de contour et essayer d'insérer des cercles.
Paramètres disponbles : 
- préciser le nombre de cercles à détecter 
- donner des min et max pour le.s diamètre.s des cercles à détecter dans l'image 
- paramétrage de détection de contours 

Faire un test avec la caméra pour calibrer les tailles des cercles 

Pour plus tard : 
déporter le traitement d'image sur une route du serveur

# Liens envoyés
Des articles de références pour le naming game à étudier et le git https://github.com/acornejo/kilobot-labs/ de tutoriel testable en simulation puis en réel 

### Emergence of Consensus in a Multi-Robot Network 

Le simulateur ARGoS est générique pour différents robots dont les kilobots.
Ils ont fait le code pour kilobots qui réalise ce qui est décrit dans l'article ce devrait être facilement transférable sur KiloGUI.

But : refaire l'expérience qui a été réalisée dans l'article avec l'infrastructure disponible 
Avoir une interface qui permet d'afficher les positions des robots et leur transférer des informations.

On peut essayer de communiquer avec les kilobots de manière unidirectionnelle pour les perturber pendant l'expérience d'algorithme de consensus. Par exemple les faire se déplacer, ne plus écouter, prendre une certaine couleur, ...


# La suite 
### Réaliser une forme 
Est-ce possible de faire communiquer les robots entre eux pour se positionner en forme (carré, rond ou triangle) ?

--> À étudier pour plus tard

Le programme doit connaître la position initiale de chaque robot et leur position finale dans la forme pour communiquer au robot coment il doit se déplacer. 

Problème à réssoudre : l'identification du robot avec lequel communiquer la postion.
Il faut réussir à suivre un robot entre 2 images malgré le temps de traitement. 
On peut utiliser un flux vidéo en interne (pas sur le client) ça serait possible : on détecte les positions en permanence et les communique au programme.

Chaque robot peut avoir un identifiant en interne mais quand faut-il partager son id ?

Solution de QRCode : trouver une structure en papier ou carton à poser sur le robot. 

### Partie graphe 
À partir de la position des robots, les connecter en graphe avec des connexions si ils sont suffisament près pour communiquer. 

# Prises de vue 
Des photos ont été prises pour avoir des images pour l'analyse et la détection de cercles. Elles sont dispo sur le git. 

***

# TODO 
- faire fonctionner le serveur avec les réponses brutes en HTTP
- tester la détection d'image avec le code de Hugh partagé par Mr. Boumaza
- commencer à manipuler le code pour le mouvement de kilobots 

# Prochaine réunion : jeudi 15h15 