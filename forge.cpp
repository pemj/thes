//NOTE: bitwise operations do not roll over to the next element in the char array.
//This is a Good Thing.
//multifields are soon up.  A multifield will be of, say, variable size, and specify the bit patterns for that big damn field.  Make it able to offset the whole damn thing, by god.
//work on simple TCP/UDP sending at the moment.  Raw sockets can happen later.
//work on IPv4 at the moment.  IPv6 can happen later.
//sending won't be that hard.  Right?  Guys?
//even those disgusting DNS names with letters and numbers involved have 
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <list>
#include <iostream>

#define bits_in_byte 8

class Field {
public:
  void mute(uint8_t);
  void mute(char*);
  void mute(char);
  Field(char*, uint8_t, uint8_t);
private:
  char *position;
  uint8_t mask;
  uint8_t bitOffset;
  uint8_t bitWidth;
};

Field::Field(char* pos, uint8_t offset, uint8_t width){
  position = pos;
  bitOffset = offset;
  bitWidth = width;
  if((bitWidth + bitOffset) > 8){
    perror("Field construction error: invalid offset or width.");
    exit(0);
  }

  uint8_t mask_assist = '\x80';//1000 0000
  //move our 1 to the beginning of the field
  mask_assist = mask_assist >> bitOffset;
  //now, having acquired the specifications for the field,
  //we use those specs to create a bitmask for altering it.
  mask = 0;// 0000 0000
  //we add one 1 at a time to the mask
  for(int i=0;i<bitWidth;i++){//for each bit of width
    mask_assist = mask_assist >> 1;
    mask = mask | mask_assist;    
  }
}

/*
  First, we AND the original with the mask.
  Then, we bitshift the value into the right place.
  Then we mask it to only have values in the right places.
  Then we OR together the original and the new value
*/
//for straight-up integers
void Field::mute(uint8_t value){	
  *position = (*position) & (~mask);
  //and the original with the inverted mask, to clear the appropriate 
  //segment of the field
  value = value << 8 - bitWidth;
  value = value >> bitOffset;  
  value = value & mask;
  *position = (*position) | value;
}
//just autocasts the char.
void Field::operator=(char value){
 mute((uint8_t) value);
}
//translates from string (i.e. "1001 1100")
void Field::operator=(char* value){
  uint8_t numeric_val = 0;
	for(int i=0;i<8;i++){		
		if(value[i]=='0'){
			continue;
		}else if(value[i]=='1'){
		numeric_val = numeric_val + (1 << (7-i));
		}else{
		  std::cout<<"Neither a 1 nor a zero"<<std::endl;
		}
	}
	mute(numeric_val);
}

//these guys
uint8_t Field::get();
//forgot about this one.  We will need to actually find out what the hell the thing is
uint8_t Field::operator+(uint8_t rVal);
uint8_t Field::operator-(uint8_t rVal);
uint8_t Field::operator*(uint8_t rVal);
uint8_t Field::operator/(uint8_t rVal);
uint8_t Field::operator++();
uint8_t Field::operator+=();
uint8_t Field::operator--();
uint8_t Field::operator-=();
uint8_t Field::operator==();


uint8_t Field::get(){
  //extract value
  uint8_t temp = (*position) & mask;
  //shift to the left until it's in the most significant column
  temp = temp << bitOffset;
  //shift to the right until it ends in the least significant column
  temp = temp >> (8 - bitWidth);
  //return that value
  return temp;
}

uint8_t Field::operator+(uint8_t rVal){
  uint8_t temp = get();
  temp = temp + rVal;
  if (temp > (1 << (bitWidth) - 1) ){
    perror("Overflow in the course of Field addition");    
    //we overflowed, warn them!
  } 
  return temp;
}

uint8_t Field::operator++(){
  uint8_t temp = get();
  temp = temp + 1;
  if (temp > (1 << (bitWidth) - 1) ){
    perror("Overflow in the course of Field addition");    
    //we overflowed, warn them!
  }
  mute(temp);  
  return temp;
}

uint8_t Field::operator+=(uint8_t rVal){
  uint8_t temp = get();
  temp = temp + rVal;
  if (temp > (1 << (bitWidth) - 1) ){
    perror("Overflow in the course of Field addition");    
    //we overflowed, warn them!
  }
  mute(temp);  
  return temp;
}







/*
class MultiField: public Field{
public:
  MultiField(char*, int, int);
  void mute(char*, int);	
  uint32_t bitWidth;
};
//We'll extend this later to arbitrary array assignment.  That seems useful, no?
MultiField::MultiField(char* pos, int offset, int width){
  position = pos;
  bitOffset = offset;
  bitWidth = width;
}
Multifield::mute(void* source, uint32_t bit_count){
  //this copies from the source into the char array.
  uint32_t byte_count = bit_count / bits_in_byte;//convert to bytes
  uint32_t extra_bits = bit_count % bits_in_byte;//modolu eight for how many bytes
  //we already know where it starts.
  //store the start and end values.
  //memcpy the whole thing over.
  //if shifting necessary, go through the whole damn thing and shift it.
}
*/
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

