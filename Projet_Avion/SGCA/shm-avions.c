#include<stdio.h>
#include<string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "shm-avions.h"
#include "../common.h"

shm_avions * creerSHM_AVIONS(char * pathname, int id) {
    key_t clef;
    int code;
    shm_avions * shm;
    clef = ftok(pathname, id);
    if (clef == -1) {
        perror("clef: ");
        fprintf(stderr, "%s:%d: Unable to create the System V IPC key from the \"%s\" pathname and the \"%d\" project identifier.\n", __FILE__, __LINE__, pathname, id);
        return (void *) - 1;
    }
    code = shmget(clef, sizeof (shm_avions), IPC_CREAT | 0666);
    if (code == -1) {
        perror("code : ");
        fprintf(stderr, "%s:%d: Unable to get the identifier of the System V shared memory segment from the \"0x%x\" key.\n", __FILE__, __LINE__, clef);
        return (void *) - 1;
    }
    shm = (shm_avions *) shmat(code, NULL, 0);
    shm->nb_avions = 0;
    shm->nb_lecteurs = MAX_LECTEURS;
    return shm;
}

shm_avions * getSHM_AVIONS(char * pathname, int id) {
    key_t clef;
    int code;
    shm_avions * shm;
    clef = ftok(pathname, id);
    if (clef == -1) {
        perror("clef: ");
        fprintf(stderr, "%s:%d: Unable to get the System V IPC key from the \"%s\" pathname and the \"%d\" project identifier.\n", __FILE__, __LINE__, pathname, id);
        return (void *) - 1;
    }
    code = shmget(clef, sizeof (shm_avions), 0666);
    if (code == -1) {
        perror("code : ");
        fprintf(stderr, "%s:%d: Unable to get the identifier of the System V shared memory segment from the \"0x%x\" key.\n", __FILE__, __LINE__, clef);
        return (void *) - 1;
    }
    shm = (shm_avions *) shmat(code, NULL, 0);
    return shm;
}

Avion * insererAvion(Avion a, shm_avions * shm) {
    int fait = 0;
    Avion * result = NULL;
    sem_t * S = sem_open(SNAME_S, 0);
    if (S == SEM_FAILED) {
        fprintf(stderr, "Erreur ouverture de semaphore lors d'une copie d'avion\n");
        return result;
    }
    while (fait == 0) {
        sem_wait(S); //Entrée SC
        printf("Entré en SC\n");
        if (shm->nb_lecteurs == MAX_LECTEURS) {
            shm->nb_lecteurs = 0;
            sem_post(S); //Sortie SC
            if (shm->nb_avions < NB_AVIONS) {
                copierAvion(&(shm->tab[shm->nb_avions]), a);
                result = &(shm->tab[shm->nb_avions]);
                shm->nb_avions++;
                printf("Avion inséré : nb avion : %d\n", shm->nb_avions);
            } else {
                fprintf(stderr, "%s:%d: Shm full\n", __FILE__, __LINE__);
            }
            sem_wait(S); //Entrée SC
            shm->nb_lecteurs = MAX_LECTEURS;
            sem_post(S); //Sortie SC
            return result;
        } else {
            printf("Lectures en cours sortie de SC\n");
            sem_post(S);
        }
    }
    return NULL;
}

int copierAvion(Avion * dest, Avion src) {
    if (dest != NULL) {
        /*memcpy(dest,&src,sizeof(Avion));*/
        dest->socket = src.socket;
        strcpy(dest->numero_vol, src.numero_vol);
        (dest->dep).cap = src.dep.cap;
        (dest->dep).vitesse = src.dep.vitesse;
        (dest->coord).x = src.coord.x;
        (dest->coord).y = src.coord.y;
        (dest->coord).altitude = src.coord.altitude;
        (dest->change).envoyer = src.change.envoyer;
        (dest->change).type = src.change.type;
        (dest->change).value = src.change.value;
        (dest->crash) = src.crash;
        return 0;
    } else return -1;
}

void supprimerAvion(Avion * a, shm_avions * shm) {
    int i = 0;
    int fait = 0;
    sem_t * S = sem_open(SNAME_S, 0);
    if (S == SEM_FAILED) {
        fprintf(stderr, "Erreur ouverture de semaphore lors d'une copie d'avion\n");
        return;
    }
    while (fait == 0) {
        sem_wait(S); // Entrée en SC
        if (shm->nb_lecteurs == MAX_LECTEURS) {
            shm->nb_lecteurs = 0;
            sem_post(S); //Sortie SC
            while(i < shm->nb_avions && strcmp(a->numero_vol, (shm->tab[i]).numero_vol) != 0){
                i++;
            }
            for (i=i; i < (shm->nb_avions - 1); i++) {
                printf("Copie de  %s sur %s\n",(shm->tab[i + 1]).numero_vol, (shm->tab[i]).numero_vol);
                copierAvion(&(shm->tab[i]), shm->tab[i + 1]);
            }
            shm->nb_avions--;
            sem_wait(S); //Entrée SC
            shm->nb_lecteurs = MAX_LECTEURS;
            sem_post(S); //Sortie SC
            fait = 1;

        } else {
            sem_post(S); //Sortie SC
        }
    }
}

Avion * getAvion(shm_avions * shm, char * numVol) {
    int i;
    for (i = 0; i < shm->nb_avions; i++) {
        if (strcmp(numVol, (shm->tab[i]).numero_vol) == 0) {
            return &(shm->tab[i]);
        }
    }
    return NULL;
}
