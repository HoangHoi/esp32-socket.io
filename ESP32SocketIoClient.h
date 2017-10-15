#ifndef ESP32SocketIoClient_H_
#define ESP32SocketIoClient_H_

#include <Arduino.h>
#include <WiFiClient.h>

#define DEBUG true

// Length of static data buffers
#define DATA_BUFFER_LEN 1024
#define SID_LEN 24

// prototype for 'on' handlers
// only dealing with string data at this point
typedef void (*functionPointer)(String data);

#define ORIGIN F("Origin: ArduinoSocketIOClient\r\n")
#define MAX_ON_HANDLERS 8
#define MAX_HOSTNAME_LEN 128
#define PING_INTERVAL 5000

#ifdef SOCKETIOCLIENT_USE_SSL
    #define DEFAULT_PORT 443
#else
    #define DEFAULT_PORT 80
#endif

#define DEFAULT_URL "/socket.io/?transport=websocket"
#define DEFAULT_FINGERPRINT ""

#ifdef DEBUG
    #define ECHO(m) Serial.println(m)
#else
    #define ECHO(m)
#endif

class ESP32SocketIOClient {
public:
    ESP32SocketIOClient();
    // void loop();
    bool connect(String thehostname, int port = 80);
    // bool connectHTTP(String thehostname, int port = 80);
    bool connected();
    void disconnect();
    bool reconnect(String thehostname, int port = 80);
    bool monitor();
    void setAuthToken(String newAuthToken);
    // void begin(const char* host, const int port = DEFAULT_PORT, const char* url = DEFAULT_URL);
    void on(String id, functionPointer f);
    void emit(String id, String data);
    void heartbeat(int select);
    String getREST(String host,int port, String path);
    String postREST(String host, int port, String path, String token, String data);
    // void putREST(String host, String path, String type, String data);
    // void deleteREST(String host, String path);

private:
    bool httpConnect(String host, int port);
    bool checkResponseStatus(int status);
    bool stopConnect();
    bool beginConnect();
    bool readSid();
    void eventHandler(int index);
    bool handshake();
    void sendHandshake();
    bool readHandshake();
    bool connectViaSocket();
    bool authenticate();
    void sendRequestAuthenticate();
    void sendConnectToSocket();
    void readLine();
    bool waitForInput();
    void eatHeader();

    WiFiClient internets;

    char *dataptr;
    char databuffer[DATA_BUFFER_LEN];
    char sid[SID_LEN];
    char key[28];
    char hostname[128];
    int port;

    functionPointer onFunction[MAX_ON_HANDLERS];
    String onId[MAX_ON_HANDLERS];
    String authToken;
    String requestToken;
    String cookie;
    uint8_t onIndex = 0;
};
#endif
