#include "FS.h" // pour le SPIFFS
#include <ESP8266WiFi.h> 
#include <IRremote.h>

int RECV_PIN = 4;
IRrecv irrecv(RECV_PIN);
decode_results results;

int sensitivity = 200; // ms
boolean carDetected = false;

const char * nomDeFichier = "/lapcounterweb.html";        
const char* ssid     = "NoLanBox";
const char* password = "mumu-COCO-2019";

const byte pinBouton = D3;

const uint16_t HTTPPort = 80;
WiFiServer serveurWeb(HTTPPort); 

const byte maxHTTPLine = 100;
char httpLine[maxHTTPLine + 1]; // +1 pour avoir la place du '\0'

const byte maxURL = 50;
char urlRequest[maxURL + 1]; // +1 pour avoir la place du '\0'



/* lap counting vars */
long int t0;
long int t1;
bool lapStarted = false;
String laptime = "";
boolean updated = false;

void printHTTPServerInfo()
{
  Serial.print(F("Site web http://")); Serial.print(WiFi.localIP());
  if (HTTPPort != 80) 
  {
    Serial.print(F(":"));
    Serial.print(HTTPPort);
  }
  Serial.println();
}

void resUpdateChrono(WiFiClient &cl)
{
  if (updated == true)
  {
    cl.print(laptime);
    updated = false;
  }
  else
  {
    cl.print("");
  }
}

boolean gestionRequetesHTTP()
{
  boolean requeteHTTPRecue = false;
  byte indexMessage = 0;
  char * ptrGET, *ptrEspace;

  WiFiClient client = serveurWeb.available();
  
  if (!client) return requeteHTTPRecue; // pas de client connecté
  boolean currentLineIsBlank = true;
  while (client.connected()) 
  {
    if (client.available()) 
    {
      char c = client.read();
  
      if (c == '\n' && currentLineIsBlank)
      { 
     
        // ON GENERE LA PAGE WEB
        if (strcmp(urlRequest, "/favicon.ico")) 
        { 
          // si ce n'est pas pour le favicon
          requeteHTTPRecue = true;

          // On envoie un en tête de réponse HTTP standard
          client.println("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n");

          // gestion des requetes get
          if (strstr(urlRequest, "/update")) 
          { 
            resUpdateChrono(client);
          } 
          else if (strstr(urlRequest, "/reset")) 
          { 
            Serial.println("resetting race");
            lapStarted=false;
            laptime="";
            updated=false;
          } 
          else 
          { 
            // on envoie la page web par défaut
            if (SPIFFS.exists(nomDeFichier)) 
            {
              File pageWeb = SPIFFS.open(nomDeFichier, "r");
              client.write(pageWeb);
              pageWeb.close();
            } 
            else 
            {
              Serial.println(F("Erreur de fichier"));
            }
          }
        }
        break;           
      }

      if (c == '\n') 
      {
        currentLineIsBlank = true;
        httpLine[indexMessage] = '\0'; // on termine la ligne correctement (c-string)
        indexMessage = 0; // on se reprépare pour la prochaine ligne
        
        if (ptrGET = strstr(httpLine, "GET")) 
        {
          // c'est la requête GET, la ligne continent "GET /URL HTTP/1.1", on extrait l'URL
          ptrEspace = strstr(ptrGET + 4, " ");
          *ptrEspace = '\0';
          strncpy(urlRequest, ptrGET + 4, maxURL);
          urlRequest[maxURL] = '\0'; // par précaution si URL trop longue
        }
      } 
      else if (c != '\r') 
      {
        currentLineIsBlank = false;
        if (indexMessage <= maxHTTPLine - 1) 
        {
          httpLine[indexMessage++] =  c; // sinon on ignore le reste de la ligne
        }
      }
    } 
  } 
  delay(1);
  client.stop(); 
  return requeteHTTPRecue;
}

void getLap()
{
    if (!lapStarted)
    {
      t0 = millis();
      lapStarted = true;
      Serial.println("Starting lap");
    }
    else
    {
      Serial.println("new lap !");
      t1=millis();
      long int delta = t1 - t0;

      String result = "";
      int mm = (int) delta / 60000;
      if (mm == 0) 
        result = "00:";
      else if (mm < 10)
        result = "0" + mm;
      else
        result = mm + ":";

      delta = delta - mm * 60000;

      int ss = (int) delta/1000;
      if (ss == 0) 
        result = result + "00.";
      else if (ss < 10)
        result = result + "0" + ss + ":";
      else
        result = result + ss + ":";

      delta = delta - ss * 1000;
      
      if (delta == 0) 
        result = result + "000";
      else if (delta < 10)
        result = result + "00" + delta;
      else if (delta < 100)
        result = result + "0" + delta;
      else
        result = result + delta;

      laptime = result;
     
      t0=t1;
      updated = true;
    }
}

/* ***************************** */
/* SETUP                         */
/* ***************************** */
void setup() {

  pinMode(pinBouton, INPUT_PULLUP);

  Serial.begin(74880); // parce que mon Wemos et par défaut à peu près à cette vitesse, évite les caractères bizarre au boot
  Serial.println("\n\nTest SPIFFS\n");

  // on démarre le SPIFSS
  if (!SPIFFS.begin()) 
  {
    Serial.println("erreur SPIFFS");
    while (true); 
  }

  WiFi.begin(ssid, password);

  Serial.println();
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.write('.');
  }
  Serial.println();

  // on démarre le serveur
  serveurWeb.begin();
  printHTTPServerInfo();

  // ir
  irrecv.enableIRIn();
}











/* ***************************** */
/* LOOP                          */
/* ***************************** */
void loop() 
{
  // gestion requete http
  gestionRequetesHTTP();

  // detection voiture
  if (irrecv.decode(&results)) 
  {
    unsigned long val = results.value;
    Serial.println(val);

    if (val != 1082195712)
    {
      Serial.println("car detected !");
      getLap();
      
    }
    //Serial.println(val);
    irrecv.resume(); // Receive the next value
  }

  delay(200);

}
