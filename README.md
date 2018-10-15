# Culture_Entreprise

Il y a 4 containers :
* SGCA
* Avion
* Console_Vue_Avions
* Console_Controle_Avions

Attention Console_Vue_Avions et Console_Controle_Avions sont de containers JavaFX. Il faut donc utiliser la commande xhost + pour pouvoir les afficher avant de lancer docker-compose.

## Comment ça marche
![Diagramme1](/Diagramme1.png)

Le SGCA est un programme C. Lors de son lancement il s'abonne à un service multicast UDP dans lequel il transmet son adresse IP. 

Avion est un programme C. Il se lance et s'abonne au même multicast pour récuperer l'adresse IP du SGCA.

Il établit ensuite une connexion TCP avec le SGCA. Le SGCA et l'avion communiquent ensuite ensemble via cette connexion TCP. Des logs de ces echanges sont visibles dans le terminal.

Console_Vue_Avions est une interface en JavaFx permettant de consulter la liste des avions connectés au SGCA. Il faut renseigner manuellement l'adresse IP du SGCA pour qu'elle fonctionne.

Console_Controle_Avions est une interface en JavaFx permettant de donner des ordres aux avions connectés au SGCA. Il faut renseigner manuellement l'adresse IP du SGCA pour qu'elle fonctionne ainsi que le nom de l'avion.

Petite astuce : lorsque l'avion se connecte il affiche l'ip du SGCA dans la console.
