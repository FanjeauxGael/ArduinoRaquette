/* pour Nano 33 IoT; crée une page server sur le réseau
    IP: 192.168.0.25
    affiche 50 mesures récentes de la gravité dans l'axe vertical
*/

#include <SPI.h>
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
#include <Arduino_LSM6DS3.h>
#define constG 9.8

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "Gael";        // your network SSID (name)
char pass[] = "H3t6s356";    // your network password (use for WPA, or use as key for WEP)
const char adresse[] = "172.20.10.6";
int port = 5071;

WiFiClient wifi;
HttpClient client = HttpClient( wifi, adresse, port);
int status = WL_IDLE_STATUS;

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
  // you're connected now, so print out the status:
  printWifiStatus();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (store_Gyro_Acc()) {
    TAMPON();
    send_To_Client();
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

int store_Gyro_Acc() {  // à changer en renvoie 0 , ou 1 si FULL, prêt à afficher
  static byte aa;
  if (flag_encours) {
    count = 0;
    flag_encours = false;
  }
  float Gx, Gy, Gz;
  float Accx, Accy, Accz;
  float Angx, Angy, Angz;
  unsigned long m;
  if (read_Gyro_Acc(Gx, Gy, Gz, Accx, Accy, Accz, Angx, Angy, Angz)) {
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

int read_Gyro_Acc(float& Gx, float& Gy, float& Gz, float& Accx, float& Accy, float& Accz, float& Angx, float& Angy, float& Angz  ) {

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

void send_To_Client() {
  if (client) {
    Serial.println("client: NOUVEAU");
    String contentType = "application/json";
    String postData = "{\"AccX\": "+ String(_AccX[count]) +", \"AngX\":"+ String(_AngX[count]) +"}";

    client.post("/post", contentType, postData);

    int statusCode = client.responseStatusCode();
    Serial.print("Status code : ");
    Serial.println(statusCode);
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
