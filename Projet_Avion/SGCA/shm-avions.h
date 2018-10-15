#ifndef SHM_AVIONS_H
#define SHM_AVIONS_H
#define NB_AVIONS 20
#include "../Avion/avion.h"
typedef struct{
    int envoyer;
    int type;
    int value;
} ChangeCaracteristiques;

typedef struct{
  int socket;
  char numero_vol[6];
  struct deplacement dep;
  struct coordonnees coord;
  ChangeCaracteristiques change;
  int donnees_recues;
  int crash;
} Avion;

typedef struct{
  Avion tab[NB_AVIONS];
  int nb_avions;
  int nb_lecteurs;
} shm_avions;
shm_avions * creerSHM_AVIONS(char * pathname, int id);
shm_avions * getSHM_AVIONS(char * pathname, int id);
Avion * insererAvion(Avion a, shm_avions * shm);
int copierAvion(Avion * dest, Avion src);
void supprimerAvion(Avion * a, shm_avions * shm);
Avion * getAvion(shm_avions * shm, char * numVol);
#endif
