#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include "avion.h"
#include "../common.h"
// caract�ristiques du d�placement de l'avion
struct deplacement dep;

// coordonn�es spatiales de l'avion
struct coordonnees coord;

// num�ro de vol de l'avion : code sur 5 caract�res
char numero_vol[6];

int socketTCP;

/********************************
 ***  3 fonctions � impl�menter
 ********************************/

int ouvrir_communication() {
    char buffer[TAILLE_BUFFER];
    struct sockaddr_in adresse_SGCA;
    struct timeval tv;
    int requete = DEMANDE_SOCKET;
    int port = DEMANDE_SOCKET;
    struct sockaddr_in adresse;
    socklen_t fromlen = sizeof (adresse_SGCA);
    int sock_TCP;
    char * ip;

    int sock = creerSocketMultiCast(&adresse);
    sendto(sock, &requete, sizeof (int), 0, (struct sockaddr *) &adresse, sizeof (adresse));
    /*recv(sock,buffer,sizeof(int),0);*/
    while (port == DEMANDE_SOCKET) {
        bzero((char *) &adresse_SGCA, sizeof (struct sockaddr_in));

	fd_set socks;
	struct timeval t;
	FD_ZERO(&socks);
	FD_SET(sock, &socks);
	t.tv_sec = 2;
	if (select(sock + 1, &socks, NULL, NULL, &t))
	{


        recvfrom(sock, buffer, sizeof (int), 0, (struct sockaddr *) &adresse_SGCA, &fromlen);

	}
	else {
	printf("SGCA non trouvé ! Fermeture...\n");
	exit(EXIT_FAILURE);
	}
	
        memcpy(&port, buffer, sizeof (int));
    }
    sock_TCP = creerSocketTCP(0);
    adresse_SGCA.sin_port = htons(port);
    if (connect(sock_TCP, (struct sockaddr *) &adresse_SGCA, sizeof (struct sockaddr_in)) == -1) {
        fprintf(stderr, "Erreur liaison TCP\n");
        return -1;
    }
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(sock_TCP, SOL_SOCKET, SO_RCVTIMEO, (char*) &tv, sizeof (struct timeval));
    socketTCP = sock_TCP;
    ip = inet_ntoa(adresse_SGCA.sin_addr);
    printf("Connexion établie avec le SGCA ! Son ip est : %s \n", ip);
    return 1;
}


void fermer_communication() {
    int type = CRASH;
    write(socketTCP, &type, sizeof (int));
    close(socketTCP);
}

void envoyer_caracteristiques() {
    int nb_octets;
    char message[TAILLE_BUFFER];
    int type = ENVOI_CARACTERISTIQUES;
    memcpy(message, &type, sizeof (int));
    memcpy(message + sizeof (int), numero_vol, sizeof (char)*6);
    memcpy(message + sizeof (int) + sizeof (char)*6, &dep, sizeof (dep));
    memcpy(message + sizeof (int) + sizeof (char)*6 + sizeof (dep), &coord, sizeof (coord));
    nb_octets = write(socketTCP, message, sizeof (int) + sizeof (char)*6 + sizeof (dep) + sizeof (coord));
    if (nb_octets == -1) {
        perror("Erreur envoi caracteristiques : ");
    }
    // fonction � impl�menter qui envoie l'ensemble des caract�ristiques
    // courantes de l'avion au gestionnaire de vols
}

/********************************
 ***  Fonctions g�rant le d�placement de l'avion : ne pas modifier
 ********************************/

// initialise al�atoirement les param�tres initiaux de l'avion

void initialiser_avion() {
    // initialisation al�atoire du compteur al�atoire
    time_t seed;
    time((time_t *) & seed);
    srandom(seed);

    // intialisation des param�tres de l'avion
    coord.x = 1000 + random() % 1000;
    coord.y = 1000 + random() % 1000;
    coord.altitude = 900 + random() % 100;

    dep.cap = random() % 360;
    dep.vitesse = 600 + random() % 200;

    // initialisation du numero de l'avion : chaine de 5 caract�res
    // form�e de 2 lettres puis 3 chiffres
    numero_vol[0] = (random() % 26) + 'A';
    numero_vol[1] = (random() % 26) + 'A';
    sprintf(&numero_vol[2], "%03d", (int) (random() % 999) + 1);
    numero_vol[5] = 0;
}

// modifie la valeur de l'avion avec la valeur pass�e en param�tre

void changer_vitesse(int vitesse) {
    if (vitesse < 0)
        dep.vitesse = 0;
    else if (vitesse > VITMAX)
        dep.vitesse = VITMAX;
    else dep.vitesse = vitesse;
}

// modifie le cap de l'avion avec la valeur pass�e en param�tre

void changer_cap(int cap) {
    if ((cap >= 0) && (cap < 360))
        dep.cap = cap;
}

// modifie l'altitude de l'avion avec la valeur pass�e en param�tre

void changer_altitude(int alt) {
    if (alt < 0)
        coord.altitude = 0;
    else if (alt > ALTMAX)
        coord.altitude = ALTMAX;
    else coord.altitude = alt;
}

// affiche les caract�ristiques courantes de l'avion

void afficher_donnees() {
    printf("Avion %s -> localisation : (%d,%d), altitude : %d, vitesse : %d, cap : %d\n",
            numero_vol, coord.x, coord.y, coord.altitude, dep.vitesse, dep.cap);
}

// recalcule la localisation de l'avion en fonction de sa vitesse et de son cap

void calcul_deplacement() {
    float dep_x, dep_y;

    if (dep.vitesse < VITMIN) {
        printf("Vitesse trop faible : crash de l'avion\n");
        fermer_communication();
        exit(2);
    }
    if (coord.altitude == 0) {
        printf("L'avion s'est ecrase au sol\n");
        fermer_communication();
        exit(3);
    }


    dep_x = cos(dep.cap * 2 * M_PI / 360) * dep.vitesse * 10 / VITMIN;
    dep_y = sin(dep.cap * 2 * M_PI / 360) * dep.vitesse * 10 / VITMIN;

    // on se d�place d'au moins une case quels que soient le cap et la vitesse
    // sauf si cap est un des angles droit
    if ((dep_x > 0) && (dep_x < 1)) dep_x = 1;
    if ((dep_x < 0) && (dep_x > -1)) dep_x = -1;

    if ((dep_y > 0) && (dep_y < 1)) dep_y = 1;
    if ((dep_y < 0) && (dep_y > -1)) dep_y = -1;

    //printf(" x : %f y : %f\n", dep_x, dep_y);

    coord.x = coord.x + (int) dep_x;
    coord.y = coord.y + (int) dep_y;

    afficher_donnees();
}

void ChangerCaracteriques() {
    int requete;
    int value;
    char buffer[sizeof (int)*2];
    if (read(socketTCP, buffer, sizeof (int)*2) != -1) {
        memcpy(&requete, buffer, sizeof (int));
        memcpy(&value, buffer + sizeof (int), sizeof (int));
        printf("Demande de changement de caracteristiques : %d, %d\n",requete,value);
        switch (requete) {
            case REQUETE_VITESSE: changer_vitesse(value);
                break;
            case REQUETE_CAP: changer_cap(value);
                break;
            case REQUETE_ALTITUDE: changer_altitude(value);
                break;
            default: break;
        }
    }
}

// fonction principale : g�re l'ex�cution de l'avion au fil du temps

void se_deplacer() {
    while (1) {
        sleep(PAUSE);
        calcul_deplacement();
        envoyer_caracteristiques();
        ChangerCaracteriques();
    }
}



int main() {
    // on initialise l'avion
    initialiser_avion();

    afficher_donnees();
    // on quitte si on arrive � pas contacter le gestionnaire de vols
    if (!ouvrir_communication()) {
        printf("Impossible de contacter le gestionnaire de vols\n");
        exit(1);
    }

    // on se d�place une fois toutes les initialisations faites
    se_deplacer();
	return EXIT_SUCCESS;
}
