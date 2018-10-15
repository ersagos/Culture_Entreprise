#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>

int creerSocketMultiCast(struct sockaddr_in * adresse){
  if(adresse == NULL){
    fprintf(stderr,"Erreur la structure sockaddr_in ne peut être nulle\n");
    exit(1);
  }
  int sock;
  struct in_addr ip;
  struct ip_mreq gr_multicast;
  int reuse = 1;
  /*Création de la socket UDP*/
  sock = socket(AF_INET,SOCK_DGRAM,0);
  if(sock <0){
    perror("Erreur création socket");
    exit(1);
  }
  /*Autoriser plusieurs socket sur le même port*/
  if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse)) <0){
    perror("Erreur reuse");
    exit(1);
  };


  /*Adresse de destination*/
  inet_aton(ADRESSE_MULTICAST,&ip);
  bzero((char *) adresse,sizeof(struct sockaddr_in));
  adresse->sin_family=AF_INET;
  adresse->sin_addr.s_addr=ip.s_addr;
  adresse->sin_port=htons(PORT_MUTLICAST);

  /*Liaison de la socket*/
  if(bind(sock,(struct sockaddr *) adresse,sizeof(struct sockaddr_in)) <0){
    perror("Erreur liaison socket");
    exit(1);
  }

  /*Liaison avec le groupe multicast*/
  gr_multicast.imr_multiaddr.s_addr = ip.s_addr;
  gr_multicast.imr_interface.s_addr = htons(INADDR_ANY);
  if(setsockopt(sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,&gr_multicast,sizeof(struct ip_mreq)) <0){
    perror("Erreur setsockopt avec le multicast");
    exit(1);
  }
  return sock;
}

int creerSocketTCP(int port) {
     static struct sockaddr_in addr_serveur;
     int la_socket;
     la_socket = socket(AF_INET, SOCK_STREAM, 0);
     if (la_socket == -1) {
         perror("creation socket");
         return (-1);
     }

     addr_serveur.sin_family = AF_INET;
     addr_serveur.sin_port = htons(port);
     addr_serveur.sin_addr.s_addr = htonl(INADDR_ANY);
     if (bind(la_socket,
             (struct sockaddr*) &addr_serveur,
             sizeof (addr_serveur)) == -1) {
         perror("erreur bind socket écoute");
         return (-1);
     }

     return la_socket;
 }
