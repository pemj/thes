#include "forge.cpp"


int main(int argc, char *argv[]){
  Protocol *proto;
  proto = new Protocol(argv[1], argv[2]);
  proto->mute(argv[3]);
  for(int i=0;i<100000;i++){
    proto->send();
  }

}
