#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char **argv) {    

  struct hostent *hp;
  
  // check argument or STDIN
  if (argc == 2)  {
    hp = gethostbyname(argv[1]);

  }else{
    char hostName[256];
    printf("Enter host name : \n");
    scanf("%s",&hostName);
    hp = gethostbyname(hostName);
  }

  //check null hostent
  if (hp == NULL) {
    printf("Null Pointer\n");
  } else { // print address
    printf("%s = ", hp->h_name);
    int i=0;
    while ( hp -> h_addr_list[i] != NULL) {
      printf("%s\n", inet_ntoa( (struct in_addr) *((struct in_addr *) hp->h_addr_list[i])));
      i++;
    }
    printf("\n");
  }
}