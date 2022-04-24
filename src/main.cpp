#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

#define sensor D2
#define blow D6

IPAddress apIP(192, 168, 1, 1);
String _ApSSID = "cat_catcher";
String _ApPASSWD = "12345677";
File fsUploadFile;

String str;
DynamicJsonDocument doc(100);
char buffer_[100];

boolean active = false;
boolean used = false;

long timer;
long counter_time;
long start_programm;

void WifiInit();
void startAPmode();
void http_init();
void handleNotFound();
void handleRoot();
void configApp();
void Fs_init();

void handlecss();
void handlejavascript();
void handlePost();
void handleCommand();
void initAction(String action);
void sleep();

void postRequest(String stri);

void EEPROM_write(String buffer, int N);
String EEPROM_read(int min, int max);

String boom_time = EEPROM_read(0, 10).c_str();

void buttonOnChange();

ESP8266WebServer http(80);

void setup()
{
  pinMode(blow, OUTPUT);
  digitalWrite(blow, LOW);
  pinMode(sensor, INPUT);

  Serial.begin(115200);
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  };
  WiFi.hostname("CATCHER");
  WifiInit();
  http_init();
  timer = millis();
  counter_time = millis();
  start_programm = millis();
}

void loop()
{
  if (millis() - counter_time >= 3000)
  {
    counter_time = millis();
    start_programm += 3;
    Serial.print("Время start_programm");
    Serial.println(start_programm);
  }
  http.handleClient();
  if (digitalRead(sensor) && active && !used)
  {
    timer = millis();
    while (digitalRead(sensor) && millis() - timer < 1000)
    {
      // Serial.println("Подготовка....");
    }
    if (digitalRead(sensor) && active)
    {
      Serial.println("Н-н-н-а нахуй!!!");
      digitalWrite(blow, HIGH);
      boom_time = String(start_programm);
      used = true;
      EEPROM_write(boom_time, 0);
    }
  }
  if (used && millis() - timer > 2000)
  {
    digitalWrite(blow, LOW);
    used = false;
    active = false;
  }
};

void WifiInit()
{
  WiFi.mode(WIFI_STA);
  byte tryes = 1;
  while (--tryes && WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
    delay(1000);
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Запускаем точку доступа");
    startAPmode();
  }
  else
  {
    Serial.println("Подключен к Вайвай....");
  }
}

void startAPmode()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(_ApSSID.c_str(), _ApPASSWD.c_str());
  return;
}

void http_init()
{
  http.onNotFound(handleNotFound);
  http.on("/", HTTP_GET, handleRoot);
  http.on("/", HTTP_PUT, configApp);
  http.on("/style.css", HTTP_GET, handlecss);
  http.on("/script.js", HTTP_GET, handlejavascript);
  http.on("/", HTTP_POST, handleCommand);
  http.begin();
}

void handleNotFound()
{
  http.send(404, "text/plain", "Error page...");
}
void handleRoot()
{
  File file = SPIFFS.open("/index.html", "r");
  size_t sent = http.streamFile(file, "text/html");
  file.close();
  return;
}

void handlecss()
{
  File file = SPIFFS.open("/style.css", "r");
  size_t sent = http.streamFile(file, "text/css");
  file.close();
  return;
}
void handlejavascript()
{
  File file = SPIFFS.open("/script.js", "r");
  size_t sent = http.streamFile(file, "text/javascript");
  file.close();
  return;
}

void handleCommand()
{
  String response = http.arg(0);
  String res = http.arg(0);
  Serial.println(res);
  memset(buffer_, 0, sizeof(buffer_));
  response.toCharArray(buffer_, 100);
  deserializeJson(doc, buffer_);
  const char *line = doc["action"];
  const char *time = doc["date"];
  String str_line = String(line);
  Serial.print("Время из запроса: ");
  Serial.println(String(time));
  EEPROM_write(String(time), 0);
  http.send(200, "application/json", "{\"status\":\"OK\"}");
  delay(1000);
  initAction(str_line);
}

void initAction(String action)
{
  if (action == "on")
  {
    // start_programm = long(EEPROM_read(0, 10).c_str());
    String t_programm = EEPROM_read(0, 10);
    char charBuf[11];
    t_programm.toCharArray(charBuf, 11);
    start_programm = strtoul(charBuf, NULL, 10);
    for (int i = 0; i < sizeof(charBuf); i++){
      Serial.print(charBuf[i]);
    }
    Serial.print("Читаем время из памяти: ");
    Serial.println(start_programm);
    delay(1000);
    active = true;
  }
  if (action == "off")
  {
    memset(buffer_, 0, sizeof(buffer_));
    EEPROM_write(buffer_, 0);
    start_programm = 0;
    active = false;
  }
}

void EEPROM_write(String buffer, int N) //Запись в eeprom
{
  EEPROM.begin(512);
  delay(10);
  for (int L = 0; L < 32; ++L)
  {
    EEPROM.write(N + L, buffer[L]);
  }
  EEPROM.commit();
}

String EEPROM_read(int min, int max) //Чтение eeprom
{
  EEPROM.begin(512);
  delay(10);
  String buffer;
  for (int L = min; L < max; ++L)
    if (EEPROM.read(L))
      buffer += char(EEPROM.read(L));
  return buffer;
}

void configApp()
{
  String json = "{";
  json += "\"start_programm\":\"";
  json += boom_time;
  json += "\"}";
  http.send(200, "text/json", json);
}