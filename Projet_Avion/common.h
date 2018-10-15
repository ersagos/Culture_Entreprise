#ifndef COMMON_H
#define COMMON_H
#define PORT_MUTLICAST 5000
#define ADRESSE_MULTICAST "225.0.0.1"
#define DEMANDE_SOCKET -12
#define ENVOI_CARACTERISTIQUES -11
#define CRASH -10
#define TAILLE_BUFFER 250
#define REQUETE_CAP -5
#define REQUETE_ALTITUDE -4
#define REQUETE_VITESSE -3
#define SNAME_S "SEMAPHORE_S"
#define MAX_LECTEURS 10
#define ERREUR -1
#define NO_PROBLEM 0
#define LOST_CONNEXION 2
#define ENVOYER_LISTE -6
#define SEM_INS "SEMAPHORE_INS"
#define EXIT_USER -89
#include <netinet/in.h>
int creerSocketMultiCast(struct sockaddr_in * adresse);
int creerSocketTCP(int port);
#endif
