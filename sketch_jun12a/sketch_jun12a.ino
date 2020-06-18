#define LED_PIN 13
 
#define CMD_SEND_BEGIN  "AT+CIPSEND=0"
#define CMD_SEND_END    "AT+CIPCLOSE=0"
 
#define E360_PROTOCOL_HTTP     80
#define E360_PROTOCOL_HTTPS    443
#define E360_PROTOCOL_FTP      21
#define E360_PROTOCOL_CURRENT  E360_PROTOCOL_HTTP
 
#define E360_CHAR_CR     0x0D
#define E360_CHAR_LF     0x0A
 
#define E360_STRING_EMPTY  ""
 
#define E360_DELAY_SEED  1000
#define E360_DELAY_1X    (1*E360_DELAY_SEED)
#define E360_DELAY_2X    (2*E360_DELAY_SEED)
#define E360_DELAY_3X    (3*E360_DELAY_SEED)
#define E360_DELAY_4X    (4*E360_DELAY_SEED)
#define E360_DELAY_0X    (5*E360_DELAY_SEED)

bool hasRequest = false;
String htmlResponse = "";

void setup()
{
  delay(E360_DELAY_0X);
  Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT); 
  digitalWrite(LED_PIN, LOW);
  
  initESP8266();
}
 
void loop()
{
  while(Serial.available())
  {   
    bufferingRequest(Serial.read());
  }

  if(hasRequest) 
  {    
    String beginSendCmd = String(CMD_SEND_BEGIN) + "," + htmlResponse.length();
    deliverMessage(beginSendCmd, E360_DELAY_1X);
    deliverMessage(htmlResponse, E360_DELAY_1X);
    deliverMessage(CMD_SEND_END, E360_DELAY_1X);
    hasRequest = false;
  }
}
 
void initESP8266()
{
  deliverMessage("AT+RST", E360_DELAY_2X);
  deliverMessage("AT+CWMODE=2", E360_DELAY_3X);
  deliverMessage("AT+CWSAP=\"esp8266\",\"11111111\",1,4", E360_DELAY_3X);
  deliverMessage("AT+CIFSR", E360_DELAY_1X);
  deliverMessage("AT+CIPMUX=1", E360_DELAY_1X); // để cho phép các kết nối TCP
  deliverMessage(String("AT+CIPSERVER=1,") + E360_PROTOCOL_CURRENT, E360_DELAY_1X);  //để tạo 1 TCP server
}
 
void bufferingRequest(char c)
{
  static String bufferData = E360_STRING_EMPTY;
 
  switch (c)
  {
    case E360_CHAR_CR:
      break;
    case E360_CHAR_LF:
    {
      E360Procedure(bufferData);
      bufferData = E360_STRING_EMPTY;
    }
      break;
    default:
      bufferData += c;
  }
} 

bool isPostMethod(const String& command) {
   return command.indexOf("POST") != -1;
}

bool isRequesttLedOff(String command) {
  return command.indexOf("led=off") != -1;
}

boolean isRequestLedOn(String command) {
  return command.indexOf("led=on") != -1;
}

void sendResponse(String response) {
  htmlResponse = response;
  hasRequest = true;
}

void E360Procedure(const String& command)
{ 
  Serial.print("\n command: " + command);
  if (! isPostMethod(command)) {
    return;
  }
  
  if(isRequesttLedOff(command)){ 
    digitalWrite(LED_PIN, LOW);
    sendResponse("{\"code\": 200}");
  } else if(isRequestLedOn(command)){ 
    digitalWrite(LED_PIN, HIGH);
    sendResponse("{\"code\": 200}");
  } else {
    sendResponse("{\"code\": 400}");
  }
}
 
void deliverMessage(const String& msg, int dt)
{
  Serial.println(msg);
  delay(dt);

}
