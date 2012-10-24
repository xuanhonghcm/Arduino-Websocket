// 21086 bytes with bundled SHA1
// 21016 bytes - optimized bundled SHA1
// 20942 bytes - optimized   

#include <SPI.h>
#include <Ethernet.h>
#include <Sha1.h>

// Here we define a maximum framelength to 64 bytes. Default is 256.
#define MAX_FRAME_LENGTH 64

// Define how many callback functions you have. Default is 1.
#define CALLBACK_FUNCTIONS 1

#include <WebSocketClient.h>

EthernetClient client;
websocket::WebSocketClient webSocketClient;


static void hang()
{
  while(true) {}
}

void setup() {
  
  Serial.begin(115200);

  uint8_t mac[] = { 0xde, 0xad, 0xbe, 0xef, 0xf0, 0x0d };  
  Ethernet.begin(mac);
  
  // This is how you get the local IP as an IPAddress object
  Serial.println(Ethernet.localIP());
  
  // This delay is needed to let the WiFly respond properly
  delay(100);

  // Connect to the websocket server
  if (client.connect("192.168.41.112", 80)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
    hang();// Hang on failure
  }

  // Handshake with the server
  webSocketClient.path = "/ws";
  webSocketClient.host = "auth-service";
  
  if (webSocketClient.handshake(client)) {  
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");  
    hang();// Hang on failure
  }
  webSocketClient.sendData("{\"door_id\":2}");
  client.flush();
}

void loop()
{
  if (client.connected())
  {
    Serial.print(".");
    uint8_t response[64] = {0};
    websocket::WebSocketClient::Result const res = webSocketClient.readFrame(response, sizeof(response));
    switch (res)
    {
    default:
      Serial.print("Failed to get response: "); Serial.println(res);
      break;
    case websocket::WebSocketClient::Success_Ok:
      Serial.print("Received data: ");
      Serial.println(reinterpret_cast<char*>(response));
      break;
    }
  }
  else
  {  
    
    Serial.println("Client disconnected.");
    hang();// Hang on disconnect
  }
  
  // wait to fully let the client disconnect
  delay(3000);
}
