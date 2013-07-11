//NOTE: bitwise operations do not roll over to the next element in the char array.
//This is a Good Thing.
#include<math.h>
#include<stdint.h>

class Field {
public:
  Field(char*, int, int);
  void set(int);
  void set(char*);
  void set(char);
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
  uint8_t mask_assist = '\x80';//1000 0000
  //move our 1 to the beginning of the field
  mask_assist >> bitOffset;
  //now, having acquired the specifications for the field,
  //we use those specs to create a bitmask for altering it.
  mask = 0;// 0000 0000
  //we add one 1 at a time to the mask
  for(int i=0;i<bitWidth;i++){//for each bit of width
    mask_assist = mask_assist >> 1;
    mask = mask | mask_assist;    
  }
  mask = mask ^ mask;
  //we invert, to make zeroing out the field easier later
}

//for straight-up integers
Field::set(uint8_t value){	
  *position = (*position) ^ mask;
  //There, see how easy that was?
  value = value << 8 - bitWidth;
  value = value >> bitOffset;  
  value = value & (mask ^ mask);
  *position = (*position) | value;
}

Field::set(char* value){

}

/*
  First, we AND the original with the mask.
  Then, we bitshift the value into the right place.
  Then we mask it to only have values in the right places.
  Then we OR together the original and the new value
*/
