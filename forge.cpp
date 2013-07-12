//NOTE: bitwise operations do not roll over to the next element in the char array.
//This is a Good Thing.
//multifields are soon up.  A multifield will be of, say, variable size, and specify the bit patterns for that big damn field.  Make it able to offset the whole damn thing, by god.
//work on simple TCP/UDP sending at the moment.  Raw sockets can happen later.
//work on IPv4 at the moment.  IPv6 can happen later.
//sending won't be that hard.  Right?  Guys?
//even those disgusting DNS names with letters and numbers involved have 
#include "Field.cpp"

class Protocol{
public:
  char *name;
  std::list<Field> fieldList;
  void send();
  void mute(char*);
  Protocol(char*, char*);
  
private:
  char* packet;
  int sockfd;
  int error;
  struct addrinfo *p;
  
  /*
    for (std::list<int>::const_iterator iterator = intList.begin(), end = intList.end(); iterator != end; ++iterator) {
    std::cout << *iterator;
    }
  */
  
  //variables stored as std::list<Field>::const_iterator var_name
};
//lots of work has to go in here
//convert to raw sockets, support for manual UDP crafting.
Protocol::Protocol(char* portNum, char *nodeName){
  struct addrinfo hints, *servInfo;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  //first argument is hostname/IP, second is port (53 for ref)
  if ((error = getaddrinfo(nodeName, portNum, &hints, &servInfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
    return;
  }
  for(p = servInfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
			 p->ai_protocol)) == -1) {
      perror("talker: socket");
      continue;
    }
    break;
  }
  if (p == NULL) {
    fprintf(stderr, "talker: failed to bind socket\n");
    return;
  }
}

void Protocol::send(){
  int numbytes = 0;
  //We're going to need to add an IP, we can take any version of it.
  //take an entire address struct?  Nah, no need for them to do that.
  //ask them to define type in constructor, methinks.  But they can change it as well.  Have mutators.
  //
  if ((numbytes = sendto(sockfd, packet, strlen(packet), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
    perror("talker: sendto");
    exit(1);
  }
}
void Protocol::mute(char *message){
  int len = strlen(message);
  packet = (char*) malloc(len);
  strncpy(packet, message, len);
}

