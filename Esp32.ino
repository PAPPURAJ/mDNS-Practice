#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* WIFI_SSID = "IOT";
const char* WIFI_PASS = "11223344";

// mDNS hostname will become: esp32-gate.local
static const char* MDNS_HOST = "esp32-gate";

// Service type: _myapp._tcp.local
static const char* SERVICE_TYPE = "_myapp";
static const char* SERVICE_PROTO = "_tcp";

static const uint16_t HTTP_PORT = 9078;

WebServer server(HTTP_PORT);

String deviceId()
{
  // Quick unique-ish ID from MAC
  uint64_t mac = ESP.getEfuseMac();
  char buf[32];
  snprintf(buf, sizeof(buf), "ESP32-%04X%08X",
           (uint16_t)(mac >> 32), (uint32_t)mac);
  return String(buf);
}

void handleRoot() {
  server.send(200, "text/plain", "ESP32 is online. Try /info");
}

void handleInfo() {
  String json = "{";
  json += "\"name\":\"ESP32 Gate\",";
  json += "\"deviceId\":\"" + deviceId() + "\",";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  json += "\"port\":" + String(HTTP_PORT) + ",";
  json += "\"mdnsHost\":\"" + String(MDNS_HOST) + ".local\",";
  json += "\"service\":\"" + String(SERVICE_TYPE) + "." + String(SERVICE_PROTO) + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());

  // Start mDNS responder for hostname (esp32-gate.local)
  if (!MDNS.begin(MDNS_HOST)) {
    Serial.println("ERROR: mDNS start failed!");
  } else {
    Serial.print("mDNS hostname: ");
    Serial.print(MDNS_HOST);
    Serial.println(".local");
  }

  // Advertise a discoverable service (DNS-SD)
  // This is what Python will "search"
  // Instance name can be anything friendly:
  const char* instanceName = "ESP32 Gate";

  // Add service: ESP32 Gate._myapp._tcp.local:9078
  bool ok = MDNS.addService(SERVICE_TYPE, SERVICE_PROTO, HTTP_PORT);
  if (!ok) Serial.println("ERROR: addService failed!");

  // Optional TXT records (metadata)
  MDNS.addServiceTxt(SERVICE_TYPE, SERVICE_PROTO, "deviceId", deviceId());
  MDNS.addServiceTxt(SERVICE_TYPE, SERVICE_PROTO, "ver", "1");
  MDNS.addServiceTxt(SERVICE_TYPE, SERVICE_PROTO, "cap", "gate,control,info");

  server.on("/", handleRoot);
  server.on("/info", handleInfo);

  server.begin();
  Serial.print("HTTP server started on port ");
  Serial.println(HTTP_PORT);
}

void loop() {
  server.handleClient();
}
