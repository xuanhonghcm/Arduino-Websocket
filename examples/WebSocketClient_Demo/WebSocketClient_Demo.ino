#include <SPI.h>
#include <Ethernet.h>
#include <sha1.h>
  
#include <WebSocketClient.h>

static void hang()
{
  while(true) {}
}

EthernetClient client;
class MyWebSocket : public websocket::WebSocket
{
public:
    MyWebSocket(Client& client)
        : websocket::WebSocket(client)
    { }
    
protected:
  
    virtual void onClose()
    {
      Serial.println("WebSocket closed");
      hang();
    }  
    
    virtual void onError(websocket::Result error)
    {
      Serial.print("WebSocket error:");Serial.println(error);
    }
    
    virtual void onTextFrame(char const* msg, uint16_t size, bool isLast)
    {  
      Serial.print("Got text frame: '");Serial.print(msg);Serial.println("'");
    }
};
MyWebSocket webSocketClient(client);


void setup()
{
  
  Serial.begin(115200);

  uint8_t mac[] = { 0xde, 0xad, 0xbe, 0xef, 0xf0, 0x0d };  
  Ethernet.begin(mac);
  
  // This is how you get the local IP as an IPAddress object
  Serial.println(Ethernet.localIP());
  
  // Connect to the websocket server
  if (client.connect("192.168.41.55", 80)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
    hang();// Hang on failure
  }

  // Handshake with the server
  if (websocket::clientHandshake(client, "192.168.41.55", "/ws") == websocket::Success_Ok) {  
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
    webSocketClient.dispatchEvents();
    delay(1);
  }
  else
  {
    Serial.println("Connection closed.");
    hang();
  }
}
