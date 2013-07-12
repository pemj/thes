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
#define UINT8_MAX 0xff

class Field {
public:
  Field& operator=(uint8_t);
  Field& operator=(char*);
  Field& operator=(char);
  Field(char*, uint8_t, uint8_t);
  //these guys
  uint8_t get();
  Field& operator+(uint8_t rVal);
  Field& operator+=(const uint8_t rVal);
  Field& operator++();
  Field operator++(int);

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
Field& Field::operator=(uint8_t value){	
  *position = (*position) & (~mask);
  //and the original with the inverted mask, to clear the appropriate 
  //segment of the field
  value = value << 8 - bitWidth;
  value = value >> bitOffset;  
  value = value & mask;
  //clear out any extras
  *position = (*position) | value;
  return *this;
}
//just autocasts the char.
Field& Field::operator=(char value){
  operator=((uint8_t) value);
  return *this;
}
//translates from string (i.e. "1001 1100")
Field& Field::operator=(char* value){
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
  operator=(numeric_val);
  return *this;
}



//returns integer representation of the field
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

Field& Field::operator+=(const uint8_t rVal){
  uint8_t temp = get();
  temp = temp + rVal;
  if (temp > (1 << (bitWidth) - 1) ){
    perror("Overflow in the course of Field addition");    
    //we overflowed, warn them!
  }
  operator=(temp);  
  return *this;
}


inline Field operator+(Field lVal, uint8_t rVal){
  lVal += rVal;
  return lVal;
}

Field& Field::operator++(){
  uint8_t temp = get();
  if (temp >= UINT8_MAX){
    perror("++: too large of integer");
  }
  operator=((uint8_t) (temp + 1));
  return *this;
}
Field Field::operator++(int){
  Field tmp(*this);
  operator++();
  return tmp;
}


/*
  Field& Field::operator-(uint8_t rVal);
  Field& Field::operator*(uint8_t rVal);
  Field& Field::operator/(uint8_t rVal);
  Field& Field::operator--();
  Field& Field::operator-=();
  Field& Field::operator==();
  Field& Field::operator!=();
  Field& Field::operator<();
  Field& Field::operator>();
  Field& Field::operator<=();
  Field& Field::operator>=();
*/

