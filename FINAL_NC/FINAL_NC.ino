#include <HardwareSerial.h>   // Thư viện giao tiếp phần cứng UART
#include <WebSocketsClient.h>
#include <AccelStepper.h>
#include <WiFi.h>

// Định nghĩa các chân điều khiển động cơ
#define motorPin1  15     // IN1 trên driver ULN2003
#define motorPin2  2      // IN2 trên driver ULN2003
#define motorPin3  22      // IN3 trên driver ULN2003
#define motorPin4  18      // IN4 trên driver ULN2003

// Định nghĩa loại giao diện cho động cơ 4 dây ở chế độ half-step
#define MotorInterfaceType 8

const char* ssid = "IoT LAB";
const char* password = "kvt1ptit";

const char* websocket_server = "192.168.76.103";
const int websocket_port = 8080;

WebSocketsClient webSocket;
HardwareSerial mySerial(2);   // Tạo đối tượng UART2 của ESP32
AccelStepper stepper = AccelStepper(MotorInterfaceType, motorPin1, motorPin3, motorPin2, motorPin4);


int dist;                     // Khoảng cách đo được từ LiDAR
int strength;                 // Cường độ tín hiệu của LiDAR
int check;                    // Biến lưu giá trị kiểm tra
int i;
int uart[9];                  // Mảng lưu dữ liệu đo từ LiDAR
const int HEADER = 0x59;      // Đầu khung dữ liệu (0x59)
const int stepsPerRevolution = 2048; // Số bước cho một vòng quay hoàn chỉnh
unsigned long previousMillis = 0;     // Biến lưu thời gian trước đó
const long interval = 50;             // Khoảng thời gian để in giá trị (ms)


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("Disconnected from WebSocket server");
      break;
    case WStype_CONNECTED:
      Serial.println("Connected to WebSocket server");
      webSocket.sendTXT("ID: Client1"); // Gửi ID client để xác định
      break;
  }
}


void xulyLidar();

void setup() {
  Serial.begin(115200);       // Khởi động UART0 để giao tiếp với máy tính
  mySerial.begin(115200, SERIAL_8N1, 16, 17);  // Cấu hình UART2
  stepper.setMaxSpeed(1300);  // Thiết lập tốc độ tối đa của động cơ

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  webSocket.begin(websocket_server, websocket_port, "/");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  
  stepper.setCurrentPosition(0);  // Đặt lại vị trí hiện tại

  // Quay theo chiều kim đồng hồ
  while (stepper.currentPosition() <= 4096) {  // Quay một vòng (4096 bước)
    stepper.setSpeed(1200);                   // Tốc độ quay theo chiều kim đồng hồ
    stepper.runSpeed();
    xulyLidar();
    unsigned long currentMillis = millis(); // Lấy thời gian hiện tại

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis; // Cập nhật thời gian trước đó
      int x = (int)(180.0 * stepper.currentPosition() / stepsPerRevolution);
      Serial.print(x);       // In ra góc quay
      Serial.print(",");
      Serial.print(dist);   // In ra khoảng các
      Serial.print(".");

     String message =  String(x) + ", " + String(dist);
    webSocket.sendTXT(message);  // Gửi giá trị qua WebSocket
    webSocket.loop();  // Duy trì kết nối WebSocket trong quá trình gửi
    
    
  }
  }

  // Đặt vị trí hiện tại về 4096 trước khi quay ngược
  stepper.setCurrentPosition(4096);
 
  // Quay ngược chiều kim đồng hồ
  while (stepper.currentPosition() > 0) {    // Quay lại về 0
    stepper.setSpeed(-1200);                 // Tốc độ quay ngược chiều kim đồng hồ
    stepper.runSpeed();
    xulyLidar();
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      int x = (int)(180.0 * stepper.currentPosition() / stepsPerRevolution);
      Serial.print(x);       // In ra góc quay
      Serial.print(",");
      Serial.print(dist);   // In ra khoảng cách
      Serial.print(".");
      
      String message =  String(x) + ", " + String(dist) ;
      webSocket.sendTXT(message);
      webSocket.loop();
      
    }
  }

}
  
 
void xulyLidar() {
  if (mySerial.available() >= 9) {               // Kiểm tra xem có dữ liệu đầu vào từ LiDAR
    if (mySerial.read() == HEADER) {             // Kiểm tra xem byte đầu tiên có phải là 0x59
      uart[0] = HEADER;
      if (mySerial.read() == HEADER) {           // Kiểm tra byte thứ hai có phải là 0x59
        uart[1] = HEADER;
        for (i = 2; i < 9; i++) {                // Lưu các dữ liệu còn lại vào mảng
          uart[i] = mySerial.read();
        }
        check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
        if (uart[8] == (check & 0xff)) {         // Xác minh tính hợp lệ của dữ liệu
          dist = uart[2] + uart[3] * 256;        // Tính toán khoảng cách đo
          strength = uart[4] + uart[5] * 256;    // Tính toán cường độ tín hiệu 
        }
      }
    }
  }
}