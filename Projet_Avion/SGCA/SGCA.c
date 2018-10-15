/*
 * File:   main.c
 * Author: acapbern
 *
 * Created on 31 mars 2017, 14:02
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "SGCA.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../common.h"
#include "shm-avions.h"
#define PORT_CONSOLE 5013

/*
 *
 */


int ecouteNouvelAvion(int sock_ecoute) {
    int sock;
    struct sockaddr_in adresse;
    char buffer[TAILLE_BUFFER];
    int requete;
    int port;
    struct sockaddr_in sin;
    int addrlen = sizeof (sin);
    if (getsockname(sock_ecoute, (struct sockaddr *) &sin, (socklen_t *) & addrlen) == 0 && sin.sin_family == AF_INET && addrlen == sizeof (sin)) {
        port = ntohs(sin.sin_port);
    } else return ERREUR;
    sock = creerSocketMultiCast(&adresse);
    if (sock == -1) {
        printf("Erreur création socket multicast");
        return ERREUR;
    }
    while (1) {
        recv(sock, buffer, sizeof (int), 0);
        memcpy(&requete, buffer, sizeof (int));
        switch (requete) {
            case DEMANDE_SOCKET: sendto(sock, &port, sizeof (int), 0, (struct sockaddr *) &adresse, sizeof (adresse));
                break;
            default: break;
        }
    }
}

void ecouterAvion(char * numVol, shm_avions * shm) {
    int nb_octets;
    int type;
    char buffer[TAILLE_BUFFER];
    sem_t * S = sem_open(SNAME_S, 0);
    Avion * a;
    if (S == SEM_FAILED) {
        fprintf(stderr, "Erreur ouverture de semaphore lors d'une copie d'avion\n");
        return;
    }
    do {
        /*sem_wait(S); // Entrée en SC
        if (shm->nb_lecteurs == MAX_LECTEURS) {
            shm->nb_lecteurs = 0;
            sem_post(S); //Sortie SC*/
            a = getAvion(shm, numVol);
            if (a == NULL) {
                printf("Avion %s non trouvé\n", numVol);
                return;
            }
            nb_octets = read(a->socket, &type, sizeof (int));
            switch (type) {
                case ENVOI_CARACTERISTIQUES:
                    nb_octets = read(a->socket, a->numero_vol, sizeof (char)*6);
                    nb_octets = read(a->socket, &(a->dep), sizeof (struct deplacement));
                    nb_octets = read(a->socket, &(a->coord), sizeof (struct coordonnees));
                    if (nb_octets == -1) {
                        perror("Erreur ecoute avion");
                    }
                    a->donnees_recues = 1;
                    printf("Avion %s données reçues : %d, %d, %d\n", a->numero_vol, a->coord.x, a->coord.y, a->coord.altitude);
                    break;
                case CRASH: printf("%s s'est crashé ! Quelle tristesse...\n", a->numero_vol);
                    a->crash = 1;
                    break;
                default: break;
            }
            if (a->change.envoyer == 1) {
                printf("Avion %s marqué envoi de nouvelles caracteriques...\n", a->numero_vol);
                memcpy(buffer, &(a->change.type), sizeof (int));
                memcpy(buffer + sizeof (int), &(a->change.value), sizeof (int));
                if (write(a->socket, buffer, sizeof (int)*2) == -1) {
                    perror("Erreur envoi nouvelles caracteristiques");
                }
                a->change.envoyer = 0;
            }
           /* sem_wait(S);
            shm->nb_lecteurs = MAX_LECTEURS;
            sem_post(S);*/
        /*} else {
            sem_post(S); //Sortie SC
        }*/
    } while (nb_octets != -1 && a->crash != 1 && nb_octets != 0);
    printf("L'ecoute de %s s'est arretée :  %d\n", a->numero_vol, nb_octets);
    if(nb_octets == 0){
        a->crash = LOST_CONNEXION;
    }

}

int connexionTCP(int socket_ecoute) {
    static struct sockaddr_in addr_client;
    int lg_addr = sizeof (struct sockaddr_in);
    shm_avions * shm = getSHM_AVIONS(PATHNAME, PROJ_ID);
    if (shm == (void *) - 1) {
        fprintf(stderr, "%s:%d: Unable to get the identifier of the System V shared memory segment.\n", __FILE__, __LINE__);
        return ERREUR;
    }
    if (listen(socket_ecoute, 5) == -1) {
        perror("erreur listen");
        return ERREUR;
    }
    printf("Ecoute...\n");
    while (1) {
        int socket_service = accept(socket_ecoute, (struct sockaddr *) &addr_client, (socklen_t *) & lg_addr);
        printf("Connecté : ip %s\n", inet_ntoa(addr_client.sin_addr));
        if (socket_service == -1) {
            perror("erreur accept");
            return ERREUR;
        } else {
            int pid = fork();
            if (pid < 0) {
                perror("ERROR on fork");
                exit(EXIT_FAILURE);
            }
            if (pid == 0) {
                Avion a;
                Avion * nouvelAvion;
                strcpy(a.numero_vol, "-1-1-");
                a.socket = socket_service;
                a.change.envoyer = 0;
                a.donnees_recues = 0;
                a.crash = 0;
                nouvelAvion = insererAvion(a, shm);
                if (nouvelAvion != NULL) {
                    ecouterAvion(nouvelAvion->numero_vol, shm);
                    if (nouvelAvion->crash != 1 && nouvelAvion->crash != LOST_CONNEXION) {
                        printf("Supression de %s\n", nouvelAvion->numero_vol);
                        supprimerAvion(nouvelAvion, shm);
                    }
                    exit(0);
                } else {
                    exit(EXIT_FAILURE);
                }
            } else {
                close(socket_service);
            }

        }
    }
    return NO_PROBLEM;
}

void envoyerListeAvion(int sock, struct sockaddr_in adresse) {
    char buffer[TAILLE_BUFFER];
    int i;
    int decallage;
    int lg_addr = sizeof (struct sockaddr_in);
    shm_avions * shm = getSHM_AVIONS(PATHNAME, PROJ_ID);
    int fait = 0;
    int nb_avions_non_envoyes = 0;
    int nb_avions;
    sem_t * S = sem_open(SNAME_S, 0);
    if (S == SEM_FAILED) {
        fprintf(stderr, "Erreur ouverture de semaphore lors de la lecture d'un avion\n");
        return;
    }
    while (fait == 0) {
        sem_wait(S);
        if (shm->nb_lecteurs > 0) {
            shm->nb_lecteurs--;
            sem_post(S);
            if (shm != (void *) - 1) {
                for (i = 0; i < shm->nb_avions; i++) {
                    if ((shm->tab[i]).donnees_recues != 0) {
                        printf("Envoyé a la console : %s %d %d\n", (shm->tab[i]).numero_vol, (shm->tab[i]).coord.x, (shm->tab[i]).coord.y);
                        decallage = (i - nb_avions_non_envoyes) * 30;
                        memcpy(buffer + 4 + decallage, (shm->tab[i]).numero_vol, sizeof (char)*6);
                        memcpy(buffer + 10 + decallage, &((shm->tab[i]).coord.x), sizeof (int));
                        memcpy(buffer + 14 + decallage, &((shm->tab[i]).coord.y), sizeof (int));
                        memcpy(buffer + 18 + decallage, &((shm->tab[i]).coord.altitude), sizeof (int));
                        memcpy(buffer + 22 + decallage, &((shm->tab[i]).dep.cap), sizeof (int));
                        memcpy(buffer + 26 + decallage, &((shm->tab[i]).dep.vitesse), sizeof (int));
                        memcpy(buffer + 30 + decallage, &((shm->tab[i]).crash), sizeof (int));
                    } else {
                        nb_avions_non_envoyes++;
                    }
                }
                nb_avions = (shm->nb_avions - nb_avions_non_envoyes);
                memcpy(buffer, &nb_avions, sizeof (int));
                sendto(sock, buffer, ((sizeof (int)) + (sizeof (char) * 6) + (sizeof (int) * 6)) * nb_avions, 0, (struct sockaddr *) &adresse, lg_addr);
            }
            sem_wait(S);
            shm->nb_lecteurs++;
            sem_post(S);
            fait = 1;
        } else {
            sem_post(S);
        }

    }
};

void ChangerCaracteristiques(char * numAvion, int caracteristique, int value) {
    shm_avions * shm = getSHM_AVIONS(PATHNAME, PROJ_ID);
    int fait = 0;
    sem_t * S = sem_open(SNAME_S, 0);
    if (S == SEM_FAILED) {
        fprintf(stderr, "Erreur ouverture de semaphore lors de la lecture d'un avion\n");
        return;
    }
    if (shm != (void*) - 1) {
        while (fait == 0) {
            sem_wait(S);
            if (shm->nb_lecteurs == MAX_LECTEURS) {
                shm->nb_lecteurs = 0;
                sem_post(S);
                Avion * a = getAvion(shm, numAvion);
                if (a != NULL && a->crash != 1) {
                    printf("Avion trouvé marquage pour envoi de nouvelle caracteristiques...\n");
                    a->change.type = caracteristique;
                    a->change.value = value;
                    a->change.envoyer = 1;
                } else {
                    printf("Avion non trouvé !\n");
                }
                sem_wait(S);
                shm->nb_lecteurs = MAX_LECTEURS;
                sem_post(S);
                fait = 1;
            } else {
                sem_post(S);
            }
        }
        shmdt(shm);
    }

}

void Menu() {
    int choix = 0;
    printf("Bienvenue sur le SGCA\n");
    do {
        scanf("%d%*s", &choix);
    } while (choix != 1);
    exit(EXIT_USER);
}

int ecouteConsole() {
    int sock;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in adresse, adresse_console;
    int lg_addr = sizeof (adresse_console);
    if (sock < 0) {
        perror("Erreur création socket");
        return ERREUR;
    }
    bzero((char *) &adresse, sizeof (struct sockaddr_in));
    adresse.sin_family = AF_INET;
    adresse.sin_addr.s_addr = htonl(INADDR_ANY);
    adresse.sin_port = htons(PORT_CONSOLE);
    if (bind(sock, (struct sockaddr *) &adresse, sizeof (struct sockaddr_in)) < 0) {
        perror("Erreur liaison socket");
        return ERREUR;
    }
    while (1) {
        char buffer[TAILLE_BUFFER];
        int type;
        char numeroVol[6];
        int value;
        recvfrom(sock, buffer, sizeof (char)*TAILLE_BUFFER, 0, (struct sockaddr *) &adresse_console, (socklen_t *) & lg_addr);
        memcpy(&type, buffer, sizeof (int));
        switch (type) {
            case ENVOYER_LISTE: envoyerListeAvion(sock, adresse_console);
                break;
            default: memcpy(numeroVol, buffer + sizeof (int), sizeof (char)*6);
                memcpy(&value, buffer + sizeof (int)+ (sizeof (char) *6), sizeof (int));
                printf("Console ordre : %s, %d, %d\n", numeroVol, type, value);
                ChangerCaracteristiques(numeroVol, type, value);
                break;
        }
    }
    return NO_PROBLEM;


}

int main(int argc, char** argv) {
    int socket_ecoute;
    shm_avions * shm = creerSHM_AVIONS(PATHNAME, PROJ_ID);
    int pid2;
    int pid;
    int pid3;
    int pid4;
    sem_t * S = sem_open(SNAME_S, O_CREAT, 0666, 1);
    if (S == SEM_FAILED) {
        perror("Erreur creation semaphores ");
        exit(EXIT_FAILURE);
    }
    sem_post(S);
    if (shm == (void *) - 1) {
        fprintf(stderr, "Erreur lors de la récupération du segment mémoire partagée fermeture...\n");
        exit(EXIT_FAILURE);
    }
    socket_ecoute = creerSocketTCP(0);
    if (socket_ecoute == -1) {
        perror("Erreur création socket ecoute");
    }

    pid = fork();
    if (pid < 0) {
        perror("ERROR on fork : ");
        exit(EXIT_FAILURE);
    }
    switch (pid) {
        case 0: Menu();
            break;
        default: pid2 = fork();
            if (pid2 < 0) {
                perror("ERROR on fork2 : ");
                exit(EXIT_FAILURE);
            }
            if (pid2 == 0) {
                if (connexionTCP(socket_ecoute) == ERREUR) {
                    fprintf(stderr, "Erreur lors de la connexion TCP fermeture...\n");
                    exit(EXIT_FAILURE);
                } else exit(EXIT_SUCCESS);
            }
            pid3 = fork();
            if (pid3 < 0) {
                perror("ERROR on fork3 : ");
                exit(EXIT_FAILURE);
            }
            if (pid3 == 0) {
                if (ecouteConsole() == ERREUR) {
                    fprintf(stderr, "Erreur lors de l'ecoute de la console...\n");
                    exit(EXIT_FAILURE);
                } else exit(EXIT_SUCCESS);
            }
            pid4 = fork();
            if (pid4 < 0) {
                perror("ERROR on fork4 : ");
                exit(EXIT_FAILURE);
            }
            if (pid4 == 0) {
                ecouteNouvelAvion(socket_ecoute);
                exit(EXIT_SUCCESS);
            }
            while (1) {
                sleep(3);
                if (kill(pid, 0) == -1 || kill(pid2, 0) == -1 || kill(pid3, 0) == -1 || kill(pid4, 0) == -1) {
                    printf("Au moins un processus s'est arreté\n");
                    kill(pid4, SIGTERM);
                    kill(pid3, SIGTERM);
                    kill(pid2, SIGTERM);
                    kill(pid, SIGTERM);
                    sem_destroy(S);
                    exit(EXIT_FAILURE);
                }
            }
            break;

    }
    return (EXIT_SUCCESS);
}

