#include <WiFi.h>
#include "ESP32SocketIoClient.h"
#define LED_BUILTIN 2

const char* ssid = "NCT";
const char* password = "dccndccn";


// Server Ip
String host = "192.168.1.129";
// Server port
int port = 3000;

// Khởi tạo socket
ESP32SocketIOClient socket;

// Kết nối wifi
void setupNetwork() {
    WiFi.begin(ssid, password);
    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
    if (i == 21) {
        while (1) delay(500);
    }

    // Hàm này là hàm in log ra serial
    Serial.println("Wifi connected!");
}

// Thay đổi trạng thái đèn theo dữ liệu nhận được
void changeLedState(String data) {
    if (data == "[\"led-change\",\"off\"]") {
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("Led off!");
    } else {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("Led on!");
    }
}

void setup() {

    // Cài đặt chân LED_BUILTIN là chân đầu ra tín hiệu
    pinMode(LED_BUILTIN, OUTPUT);

    // Cài đặt giá trị mặc định là đèn tắt
    digitalWrite(LED_BUILTIN, HIGH);

    // Bắt đầu kết nối serial với tốc độ baud là 115200.
    // Khi bạn bật serial monitor lên để xem log thì phải set đúng tốc độ baud này.
    Serial.begin(115200);
    setupNetwork();

    // Lắng nghe sự kiện led-change thì sẽ thực hiện hàm changeLedState
    socket.on("led-change", changeLedState);

    // Kết nối đến server
    socket.connect(host, port);
}

void loop() {
     // Luôn luôn giữ kết nối với server.
    socket.monitor();
}
