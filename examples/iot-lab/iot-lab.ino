#include <WiFi.h>
#include <ESP32SocketIoClient.h>
#include <ArduinoJson.h>

#define HOST "data.healthcarevn.vn"
#define HTTP_PORT 80
#define SOCKET_PORT 2000
#define GET_SESSION_PATH "device/session"
#define LOGIN_PATH "device/session/login"
#define IDENTIFY_CODE "AAAAA0000000001"
#define PASSWORD "12344321"
#define RESPONSE_MAX_LENGTH 500
#define TIME_EMIT 10000000

const char* ssid     = "Tang-4";
const char* password = "!@#$%^&*o9";

String response;
char responseChar[RESPONSE_MAX_LENGTH];

String httpToken, authenticateToken;
ESP32SocketIOClient socket;

hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// volatile uint32_t isrCounter = 0;
// volatile uint32_t lastIsrAt = 0;

void setupNetwork() {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void emitData() {
    socket.emit("device_state", "{ \"device_id\": \"1\", \"data\": \"39\" }");
}

void IRAM_ATTR onTimer() {
    portENTER_CRITICAL_ISR(&timerMux);
    // isrCounter++;
    emitData();
    // lastIsrAt = millis();
    portEXIT_CRITICAL_ISR(&timerMux);
    xSemaphoreGiveFromISR(timerSemaphore, NULL);
}

void setupTimer() {
    timerSemaphore = xSemaphoreCreateBinary();
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, TIME_EMIT, true);
    timerAlarmEnable(timer);
}

void getToken() {
    response = socket.getREST(HOST, HTTP_PORT, GET_SESSION_PATH);
    response.toCharArray(responseChar, RESPONSE_MAX_LENGTH);
    StaticJsonBuffer<RESPONSE_MAX_LENGTH> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(responseChar);
    String token = root["token"];
    httpToken = token;
    Serial.print("HTTP Token: ");
    Serial.println(httpToken);
}

void login() {
    String status;
    do {
        String data = "{\"identify_code\":\"" + String(IDENTIFY_CODE) + "\",\"password\":\"" + String(PASSWORD) + "\"}";
        response = socket.postREST(HOST, HTTP_PORT, LOGIN_PATH, httpToken, data);

        response.toCharArray(responseChar, RESPONSE_MAX_LENGTH);
        StaticJsonBuffer<RESPONSE_MAX_LENGTH> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(responseChar);
        String st = root["status"];
        status = st;
        if (status != "success") {
            delay(3000);
            Serial.println("Try login after 3 second...");
        } else {
            String authToken = root["data"]["auth_token"];
            authenticateToken = authToken;
        }
    } while (status != "success");

    Serial.print("Authenticate Token: ");
    Serial.println(authenticateToken);
}

void authenticated (String payload){
  Serial.println("device da ket noi voi server: " + payload);
}

void setup() {
    Serial.begin(115200);
    delay(10);
    Serial.println();
    Serial.println();
    setupNetwork();
    getToken();
    login();

    socket.setAuthToken(authenticateToken);
    socket.on("authenticated", authenticated);
    socket.connect(HOST, SOCKET_PORT);
    setupTimer();
}

void loop()
{
    socket.monitor();
}
