#include "forge.cpp"
#define dash_x "x"
#define dash_s "s"
#define dash "-"
using namespace std;
int getLine(char **line, FILE *file);
int main(int argc, char *argv[]){
  int ch = 0;
  bool sanity = false;//run sanity check
  bool xTern = false;//specify extra filename
  bool verbose = false;// verbose mode, gives more details.
  char *fileName= "lib.conf";
  char *fileOut = "out.h";
  char *line;
  FILE *conf;
  const char *usage = "Usage: ./pacSmith -(sx) [lib.conf]";
  const char *welcome = "Running libPacSmith, the libPacForge extender.";
  while( (ch = getopt(argc, argv, "svx:")) != -1) {
    switch (ch) {
    case 'x':
      xTern = true;
      if(optarg==0){
	cout<<usage<<endl;
	exit(EXIT_FAILURE);
      }
      fileName = optarg;
      break;
    case 's':
      sanity = true;
      fileOut = "out.h";
      break;
    case 'v':
      verbose = true;
      break;
    default:
      cout<<usage<<endl;
    }
  }

  if((conf = fopen(fileName, "r")) == NULL){
    if(errno==ENOENT){
      cout<<"file: "<<fileName<<" not found"<<endl;
    }else{
      perror("File error: ");
    }
    exit(EXIT_FAILURE);
  }
  if(verbose){
    cout<<welcome<<endl;
    cout<<"You have enabled verbose mode,";
    cout<<"full details will be provided during runtime"<<endl;
    if(xTern)
      cout<<"You have elected to use a custom config file: "<<fileName<<endl;
    else{
      cout<<"You have elected to use the default configuration file: ";
      cout<<"lib.conf"<<endl;
    }
    if(sanity){
      cout<<"You have furthermore selected test-run mode, ";
      cout<<"which writes output to out.h"<<endl;
      //we write to a different file
    }
  }
  int ret = 0;
  int lineNo=0;
  while(!(ret = getLine(&line, conf))){
    if(verbose){
      cout<<"Line #"<<lineNo<<": " <<line<<endl;
    }
    lineNo++;
    if((!strncmp(line,"//", 2))&&(strlen(line)<2)){
      free(&line);
      continue;//on blank line or comment
    }
    //else tokenize
    //if it's an asterisk we switch state (realloc?)
    //now that we have a line, we process it, keeping track of any variables. 
    //we'll use a map, mapping from string to a pair<value, type>

  }
  //now that the preamble is out of the way
  //we want to read in a line at a time, and scan each line.
  //we can match variables later

  fclose(conf);

}

//reads until a newline is hit, and allocates
int getLine(char **lineOut, FILE *file){
  char *line;
  fpos_t init_pos;
  int lineLen = 0;
  if(fgetpos(file, &init_pos)){
    perror("line read error: ");
    exit(EXIT_FAILURE);
  }
  char c = '\0';
  while((c=fgetc(file))!='\n'){
    lineLen++;  
    if(lineLen>1000){
      return -1;//we catch this and indicate that an invalid line was struck
    }
  }
  if(fsetpos(file, &init_pos)){
    perror("line read error: ");
    exit(EXIT_FAILURE);
  }
  line = (char*) malloc(sizeof(char)*(lineLen+1));
  int i = 0;
  while((c=fgetc(file))!='\n'){
    line[i] = c;  
    i++;
  }
  line[++i] = '\0';
  *lineOut = line;
  return 0;
}
