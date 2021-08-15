/* pour Nano 33 IoT; crée une page server sur le réseau
    IP: 192.168.0.25
    affiche 50 mesures récentes de la gravité dans l'axe vertical
*/

#include <SPI.h>
#include <WiFiNINA.h>
#include <Arduino_LSM6DS3.h>
#define constG 9.8

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "SSID";        // your network SSID (name)
char pass[] = "password";    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;
WiFiServer server(80);

#define SAMPLE_SIZE 50
float _AngX[SAMPLE_SIZE];
float temp_AngX[SAMPLE_SIZE];
float _AngY[SAMPLE_SIZE];
float temp_AngY[SAMPLE_SIZE];
float _AngZ[SAMPLE_SIZE];
float temp_AngZ[SAMPLE_SIZE];

float _AccX[SAMPLE_SIZE];
float temp_AccX[SAMPLE_SIZE];
float _AccY[SAMPLE_SIZE];
float temp_AccY[SAMPLE_SIZE];
float _AccZ[SAMPLE_SIZE];
float temp_AccZ[SAMPLE_SIZE];

bool flag_ready = false;
bool flag_encours = false;
unsigned int compteur = 0;
byte count = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(5000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
}

void loop() {
  // put your main code here, to run repeatedly:
  Answer_client();
  if (Store_Gyro()) {
    TAMPON();
  }
}

void TAMPON() {
  for (byte w = 0; w < SAMPLE_SIZE; w++) {
    _AngX[w] = temp_AngX[w];
    _AngY[w] = temp_AngY[w];
    _AngZ[w] = temp_AngZ[w];
    _AccX[w] = temp_AccX[w];
    _AccY[w] = temp_AccY[w];
    _AccZ[w] = temp_AccZ[w];
    
  }
  // Serial.println("Transfert terminé");
}

int Store_Gyro() {  // à changer en renvoie 0 , ou 1 si FULL, prêt à afficher
  static byte aa;
  if (flag_encours) {
    count = 0;
    flag_encours = false;
  }
  float Gx, Gy, Gz;
  float Accx, Accy, Accz;
  float Angx, Angy, Angz;
  unsigned long m;
  if (Read_Gyro(Gx, Gy, Gz, Accx, Accy, Accz, Angx, Angy, Angz)) {
    // Serial.print(m); Serial.print(" ms // ");
    // Serial.print(z); Serial.println(" g ");
    
    temp_AngX[count] = Angx;
    temp_AngY[count] = Angy;
    temp_AngZ[count] = Angz;
    temp_AccX[count] = Accx;
    temp_AccY[count] = Accy;
    temp_AccZ[count] = Accz;
    
    count++;
    //  Serial.println(count);
    if (count > SAMPLE_SIZE + 1) {
      //    Serial.println("Tableau plein");
      flag_encours = true;
      compteur++;
      if (!(flag_ready)) {
        flag_ready = true;
      }
      return 1;
    }
  }
  else {
    return 0;
  }
}

int Read_Gyro(float& Gx, float& Gy, float& Gz, float& Accx, float& Accy, float& Accz, float& Angx, float& Angy, float& Angz  ) {

    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    IMU.readAcceleration(Gx, Gy, Gz);
    IMU.readGyroscope(Angx, Angy, Angz);
    Accx = Gx * constG;
    Accy = Gy * constG;
    Accz = Gx * constG;
    return 1;
  }
  else {
    return 0;
  }
}

void Answer_client() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("client: NOUVEAU");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          Serial.println("Données client: FIN");
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");


          // output the value of recorded z-axis gyroscopic data
          if (flag_ready) {
            client.println("<table><tr>");
            client.println("<td> Nr </td><td> Ang X </td><td> Ang Y </td><td> Ang Z </td><td> Acc X </td><td> Acc Y </td><td> Acc Z </td></tr>");
            //    client.println(" Nr # axe Y (dps) # time (ms)");
            for (byte u = 0; u < SAMPLE_SIZE; u++) {
              // affiche les mesures et le millis()
              client.println("<tr><td>");
              client.print(u + 1);
              client.print("</td><td>");
              client.print(_AngX[u]);
              client.print("</td><td>");
              client.print(_AngY[u]);
              client.print("</td><td>");
              client.print(_AngZ[u]);
              client.print("</td><td>");
              client.print(_AccX[u]);
              client.print("</td><td>");
              client.print(_AccY[u]);
              client.print("</td><td>");
              client.print(_AccZ[u]);
              client.print("</td>");
            }
            client.println("</tr>");
          }
          else {
            client.println(" Pas de donnees disponibles");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    delay(1); // give the web browser time to receive the data
    client.stop();   // close the connection:
    Serial.print("duree connexion (ms): ");
    Serial.println("\r");
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}
