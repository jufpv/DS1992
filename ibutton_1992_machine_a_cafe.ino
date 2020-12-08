/*
HACK iButton DS1992 (Machine à café)
ju-dev.fr 2017
*/


/*======================================================
      INCLUDES
======================================================*/
#include <OneWire.h>


/*======================================================
      DUMP
======================================================*/
int dump_page0[] = {
    0x1D, 0x6C, 0x46, 0x88, 0x92, /**/0x13/**/, 0x03, 0xEA, 0xE9, 0xED, 0x32,
    0xA9, /**/0x5F, 0x94/**/, 0x5A, 0xD5, 0x84, 0xF6, 0x90, /**/0x69, 0x69/**/, 0x0B,
    0x32, 0x2E, 0x46, /**/0xB4/**/, 0x58, 0x4F, 0xAC, 0x22, /**/0xC7, 0x30/**/
};
int dump_page1[] = {
    0x05, 0x9D, 0xEC, 0xBE, 0x3C, 0xA5, 0x46, 0x68, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xEF, 0xEF, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55
};
int dump_page2[] = {
    0x05, 0x42, 0xE5, 0x25, 0xCF, 0xBD, 0x1C, 0xCB, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55
};
int dump_page3[] = {
    0x05, 0x40, 0xBE, 0x8F, 0x3C, 0x10, 0x95, 0x04, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55
};


/*======================================================
      SETTINGS
======================================================*/
// Pin 12 de l'Arduino pour les données
OneWire ds(12);

// Device
const uint8_t   DS_TYPE           = 0x08;   // DS1992 = 0x08 | DS1993 = ?
const uint8_t   MEMORY_NB_OCTETS  = 128;    // DS1992 & DS1993 = 32 octets
const uint8_t   NB_PAGES          = 4;      // DS1992 = 4p. | DS1993 = 16p.

// Adresses mémoire des fonctions
const uint8_t   WRITE_SCRATCHPAD  = 0x0F;   // DS1992 & DS1993 = IDEM
const uint8_t   READ_SCRATCHPAD   = 0xAA;   // DS1992 & DS1993 = IDEM
const uint8_t   COPY_SCRATCHPAD   = 0x55;   // DS1992 & DS1993 = IDEM
const uint8_t   READ_MEMORY       = 0xF0;   // DS1992 & DS1993 = IDEM

// Les adresses des pages
const word      PAGE0     = 0x0000;
const word      PAGE1     = 0x0020;
const word      PAGE2     = 0x0040;
const word      PAGE3     = 0x0060;


/*======================================================
      VARIABLES
======================================================*/
byte addr[8];                   // addr: adresse trouvé sur le bus OneWire
byte data[MEMORY_NB_OCTETS];    // memory data
char authorization_code[3];     // read scratchpad to get autorization code






/*======================================================
      INIT
======================================================*/
void setup(void) {
  // Initialisation du port série
  Serial.begin(9600);
  // UI
  Serial.print("ju-dev 2017 DS1992 Hacking\n");
  Serial.print("En attente de lecture du iButton DS1992 ...\n");
  Serial.print("\n  |-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-|\n\n\n");
}





/*======================================================
      CHECK DEVICE
======================================================*/
bool checkDevice(){
  
  bool is_device_ok = true;
  
  // Si on a trouvé un device sur le bus OneWire
  if ( ds.search(addr) ) {
    
    // Affichage de la ROM du DSxx détecté :
    Serial.print(">>>> ROM detectée : ");
    for(int i = 0; i < 8; i++){
      Serial.print(addr[i], HEX);
      Serial.print(' ');
    }

    // Vérification du type de DSxx
    if(addr[0] != DS_TYPE) {
      Serial.print("addr[0] = ");
      Serial.print(addr[0]);
      Serial.print("(ibutton incompatible)");
      is_device_ok = false;
    }
    
    // Vérification de la validité de la ROM
    else if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.print("(Erreur de CRC sur addr)");
      is_device_ok = false;
    }

    // Device is DS1992
    else {
      Serial.print("(DS1992)");
    }

    Serial.println();
    Serial.println();

  }

  // SI AUCUN device present sur bus
  else {
    is_device_ok = false;
  }

  // Return if device is present or not
  return is_device_ok;
  
}





/*======================================================
      READ MEMOIRY
======================================================*/
void readMemory() {

  // LECTURE
  Serial.println("\nLECTURE MEMOIRE");
  
  ds.write(READ_MEMORY, 1);   /* Appel de la fonction READ_MEMORY du iButton  en ecrivant à l'adresse necessaire */
  ds.write(0x00);             /* Lsb pour fonction lecture memoire */
  ds.write(0x00);             /* Msb pour fonction lecture memoire */
  
  // lire la mémoire
  for (int i=0; i<MEMORY_NB_OCTETS; i++)
    data[i] = ds.read();
    
  // Afficher la mémoire
  for (int i = 0; i < MEMORY_NB_OCTETS; i++) {
    Serial.print(data[i], HEX); Serial.print(" ");
    if (i==31 || i==63 || i==95 || i==127) Serial.println();
  }
  
}





/*======================================================
      WRITE SCRATCHPAD
======================================================*/
void writeOnScratchpad(int page) {

  // ECRITURE DU SCRATCHPAD
  Serial.println("\nECRITURE DU SCRATCHPAD");
  
  ds.reset();
  ds.skip();
  ds.write(0xF, 1);   // write scratchpad
  // ----------------------------------------------------
  switch (page) {
    
    case 0: // PAGE 0
      ds.write(PAGE0);                // ADDR TA1 index | 0 = 0x20, 1 = 0x21, 
      ds.write(0x00);                 // Si 0x00 alors TA1 = TA2 = index ;)
      for (int i = 0; i < 32; i++){   // write page dump
        ds.write( dump_page0[i] );
      }
      break;
      
    case 1: // PAGE 1
      ds.write(PAGE1);                // ADDR TA1 index | 0 = 0x20, 1 = 0x21, 
      ds.write(0x00);                // Si 0x00 alors TA1 = TA2 = index ;)
      for (int i = 0; i < 32; i++){   // write page dump
        ds.write( dump_page1[i] );
      }
      break;
      
    case 2: // PAGE 2
      ds.write(PAGE2);                // ADDR TA1 index | 0 = 0x20, 1 = 0x21, 
      ds.write(0x00);                 // Si 0x00 alors TA1 = TA2 = index ;)
      for (int i = 0; i < 32; i++){   // write page dump
        ds.write( dump_page2[i] );
      }
      break;
      
    case 3: // PAGE 3
      ds.write(PAGE3);                // ADDR TA1 index | 0 = 0x20, 1 = 0x21, 
      ds.write(0x00);                 // Si 0x00 alors TA1 = TA2 = index ;)
      for (int i = 0; i < 32; i++){   // write page dump
        ds.write( dump_page3[i] );
      }
      break;
      
    default:
      // nothing
      break;
      
  }
  // ----------------------------------------------------
  ds.reset();         // reset
  ds.skip();          // skip

}





/*======================================================
      READ SCRATCHPAD
======================================================*/
void readScratchpad() {

  // LECTURE DU SCRATCHPAD
  Serial.println("\nLECTURE DU SCRATCHPAD");
  
  uint8_t scratchpad[32];
  
  ds.write(READ_SCRATCHPAD, 1);

  // Autorization Code
  authorization_code[0] = ds.read();  // RX TA1
  authorization_code[1] = ds.read();  // RX TA2
  authorization_code[2] = ds.read();  // RX end offset
  Serial.print("Authorization Code : [0] = "); Serial.print(authorization_code[0]);
  Serial.print(" | Authorization Code : [1] = "); Serial.print(authorization_code[1]);
  Serial.print(" | Authorization Code : [2] = "); Serial.print(authorization_code[2]);
  Serial.println();

  // Read scratchpad data and verify
  for (int i = 0; i < 32; i++){
    scratchpad[i] = ds.read();
  }

  // display scratchpad
  for (int i = 0; i < 32; i++) {
    Serial.print(scratchpad[i], HEX); Serial.print(" ");
    if (i==31 || i==63 || i==95 || i==127) Serial.println();
  }

  ds.reset();         // reset pulse
  ds.skip();          // skip ROM command
  
}





/*======================================================
      SCRATCHPAD TO MEMORY
======================================================*/
void scratchpadToMemory() {

  // COPIE DU SCRATCHPAD EN MEMOIRE
  Serial.println("\nCOPIE DU SCRATCHPAD DANS LA MÉMOIRE");
  
  ds.write(COPY_SCRATCHPAD, 1);     // Issue copy scratchpad command
  ds.write(authorization_code[0]);  // TA1
  ds.write(authorization_code[1]);  // TA2
  ds.write(authorization_code[2]);  // ending offset
  ds.reset();                       // reset
  ds.skip();                        // skip
  ds.write(READ_MEMORY, 1);         // Issue read memory command
  ds.write(0x00);                   // alway 00h TA1
  ds.write(0x00);                   // alway 00h TA2
  readMemory();                     // read entire memory
  ds.reset();                       // reset
}





/*======================================================
      MAIN LOOP
======================================================*/
void loop(void){

  if ( checkDevice() == true ) {  // Si on a trouvé un device sur le bus OneWire
    Serial.print("\n===========================\n     PAGE 0     \n===========================\n");
    readMemory();
    writeOnScratchpad(0);             // Écrire le backup de la page X dans le scratchpad
    readScratchpad();                 // Lire le scratchpas (vérification)
    scratchpadToMemory();             // Envoyer les données du scratchpad vers la mémoire
    Serial.print("\n===========================\n\n\n");
  }

  ds.reset_search();  // On reset la recherche après chaque écriture de page

  if ( checkDevice() == true ) {  // Si on a trouvé un device sur le bus OneWire
    Serial.print("\n===========================\n     PAGE 1     \n===========================\n");
    readMemory();
    writeOnScratchpad(1);             // Écrire le backup de la page X dans le scratchpad
    readScratchpad();                 // Lire le scratchpas (vérification)
    scratchpadToMemory();             // Envoyer les données du scratchpad vers la mémoire
    Serial.print("\n===========================\n\n\n");
  }

  ds.reset_search();  // On reset la recherche après chaque écriture de page
  
  if ( checkDevice() == true ) {  // Si on a trouvé un device sur le bus OneWire
    Serial.print("\n===========================\n     PAGE 2     \n===========================\n");
    readMemory();
    writeOnScratchpad(2);             // Écrire le backup de la page X dans le scratchpad
    readScratchpad();                 // Lire le scratchpas (vérification)
    scratchpadToMemory();             // Envoyer les données du scratchpad vers la mémoire
    Serial.print("\n===========================\n\n\n");
  }

  ds.reset_search();  // On reset la recherche après chaque écriture de page

  if ( checkDevice() == true ) {  // Si on a trouvé un device sur le bus OneWire
    Serial.print("\n===========================\n     PAGE 3     \n===========================\n");
    readMemory();
    writeOnScratchpad(3);             // Écrire le backup de la page X dans le scratchpad
    readScratchpad();                 // Lire le scratchpas (vérification)
    scratchpadToMemory();             // Envoyer les données du scratchpad vers la mémoire
    Serial.print("\n===========================\n\n\n");
  }

  ds.reset_search();  // On reset la recherche après chaque écriture de page
  ds.depower();

  delay(1000);
  
} 



