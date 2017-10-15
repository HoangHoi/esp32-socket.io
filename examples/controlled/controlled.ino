#include <SocketIOClient.h>
#include <ArduinoJson.h>

#define POWER D3
#define LED_PIN D4

const char* ssid = "Tang-4";
const char* password = "!@#$%^&*o9";
String host = "iot-farm.vn";
int httpPort = 80;
String g_token, authenticate_token;
unsigned long prevTime;
long interval = 5000;
SocketIOClient socket;

void authenticated (String payload){
  Serial.println("device da ket noi voi server: " + payload);
}
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

void setup() {
    Serial.begin(115200);
    //get REST to get token
    String path = "device/session";
    setupNetwork();
    char responseChar[500];
    char post_responseChar[500];
    String response = socket.getREST(host, httpPort, path);
    Serial.println("Day la chuoi REST "+response);

    response.toCharArray(responseChar, 500);
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(responseChar);

    String token = root["token"];
    Serial.println("Chuoi token: "+token);

    //POSTREST to get auth_token
    char*  loginPath = "device/session/login";
    char*  data = "{\"identify_code\":\"AAAAA0000000001\",\"password\":\"12344321\"}";
    String post_response = socket.postREST(host, httpPort, loginPath, g_token, data);
    Serial.println("Day la chuoi postREST "+post_response); //get String post_rest contain auth_token

    post_response.toCharArray(post_responseChar, 500);
    char s[500];
    int cnt=0;
    int mark;
    //get auth_token
    for(int i=0 ; i<=sizeof(post_responseChar); i++){
       if(post_responseChar[i]=='e'&&post_responseChar[i+1]=='n'&&post_responseChar[i+2]=='"'){
              mark = i;
       }
    }
    mark = mark+5;
    for(int i=mark ; i<sizeof(post_responseChar); i++){

      if(post_responseChar[i]=='"'&&post_responseChar[i+1]=='}'){
        cnt= i;
      }
    }

      for(int i=mark; i<=cnt;i ++){
        s[i-mark+1] = post_responseChar[i];
      }
      String auth_token="";
      for(int i=1; i<=cnt-mark;i ++){
         auth_token = auth_token+s[i];
      }
      Serial.println();


    String auth = "authenticate";
    String auth_token_data = "{\"token\": \"";
    auth_token_data += auth_token;
    auth_token_data += "\"}";
    authenticate_token = auth_token_data;
    Serial.println("Day la chuoi auth_token ne: "+authenticate_token );
    Serial.println("auth_token: "+auth_token );
    socket.setAuthToken(auth_token);
    socket.on("authenticated", authenticated);
    socket.connect(host, 3000);

}

void loop() {
   if(prevTime + interval < millis() || prevTime == 0){
    prevTime = millis();
    // socket.emit("device_state", "{ \"device_id\": \"1\", \"data\": \"39*C\" }");
  }
  socket.monitor();
}
