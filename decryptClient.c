//----------------------------------------------------------------------------

#include "crsUtils.h"

#define PASSWORD_LENGTH 5
#define FIRST_CHAR 33
#define LAST_CHAR 126
#define CHAR_RANGE (1+LAST_CHAR-FIRST_CHAR)

int
main(int argc,
     char ** argv)
{
//---- check command line arguments ----
if(argc!=3)
  { fprintf(stderr,"usage: %s destination port\n",argv[0]); exit(1); }

//---- extract destination IP address ----
//
// Obtenir l'adresse IP de ``argv[1]''
//
 struct hostent* dest;
 dest = gethostbyname(argv[1]);
 in_addr_t ipAddress = *((in_addr_t *)(dest->h_addr));

//---- extract destination port number ----
int portNumber;
if(sscanf(argv[2],"%d",&portNumber)!=1)
  { fprintf(stderr,"invalid port %s\n",argv[2]); exit(1); }

//---- create client socket ----
//
// Créer une connexion TCP vers la destination et le port indiqués sur la
// ligne de commande.
//
 int clientSocket=socket(PF_INET, SOCK_STREAM, 0);
 
 struct sockaddr_in toAddr;
 toAddr.sin_family = AF_INET;
 toAddr.sin_port = htons(portNumber);
 toAddr.sin_addr.s_addr=ipAddress;
 
 // connect server
 connect(clientSocket, (struct sockaddr *)&toAddr, sizeof(toAddr));


for(;;)
  {
  char buffer[0x100];

  //---- receive a slice to be tested ----
  char encrypted[14]="";
  long long start=0,end=0;
  //
  // Obtenir une ligne de texte
  //   "mot_de_passe_chiffré indice_de_début indice_de_fin\n"
  // depuis le serveur.
  // En cas d'erreur de lecture, on met fin au programme.
  //
  int nb = recv(clientSocket, buffer,0x100,0);
  if(nb>0){
    buffer[nb]='\0';
  }else{
    printf("Server closed\n");
    break;
  }
  sscanf(buffer,"%s %lld %lld\n",encrypted, &start, &end);
  printf("#####################\n#    NEW SLICE      #\n#####################\nStart: %lld\nEnd: %lld\n", start, end);
  
  //---- test the combinations of this slice ----
  sprintf(buffer,"FAILURE\n"); // should be overwritten in case of success
  for(long long attempt=start;attempt<end;++attempt)
    {
    //---- generate a password from ``attempt'' ----
    long long value=attempt;
    char password[PASSWORD_LENGTH+1];
    for(int i=0;i<PASSWORD_LENGTH;++i)
      {
      password[i]=(char)(FIRST_CHAR+value%CHAR_RANGE);
      value/=CHAR_RANGE;
      }
    password[PASSWORD_LENGTH]='\0';
    //---- test it against the encrypted version ----
    if(!strcmp(encrypted,crypt(password,encrypted)))
      {
      sprintf(buffer,"SUCCESS %s\n",password);
      break;
      }
    }

  //---- send reply to server ----
  //
  // Envoyer le message ``buffer'' au serveur.
  // En cas d'erreur d'envoi, on met fin au programme.
  //
  send(clientSocket,buffer, strlen(buffer),0);
  }

//---- close client socket ----
//
// Fermer la connexion.
//
 close(clientSocket);

return 0;
}

//----------------------------------------------------------------------------
