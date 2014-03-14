#define MAX_STRING 60

char stringBuffer[MAX_STRING];

struct  OSDLsensor {
  char id[10];
  char name[50];
  char data_url[50]; 
  char request_path[10];
};

struct OSDLaction {
  int id;
  char name[50];
  char url[50];
  char path[10];
};

char* OSDLgetString(const char* str) {
  strcpy_P(stringBuffer, (char*)str);
  return stringBuffer;
}

char* OSDLsafeDeviceName(char* str) {
  strcpy(stringBuffer, (char*)str);
  for (int i=0; i < MAX_STRING; i++) {
   if (stringBuffer[i] == '\0') {
     break;
   }
   if (stringBuffer[i] == '.') {
     stringBuffer[i] = '_';
   } 
   if (stringBuffer[i] == ' ') {
     stringBuffer[i] = '-';
   }
  }
  return stringBuffer;
}


