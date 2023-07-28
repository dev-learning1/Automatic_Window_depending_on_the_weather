//[출처]http://turtleshell.kr/57?category=763466    
//[출처]https://kocoafab.cc/make/view/359    필요한 정보만 추출
//[출처]developer.mozilla.org/ko/docs/Web/HTTP/Message   GET함수 사용
#include "SPI.h"
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>

char ssid[] = "";       //와이파이 SSID
char pass[] = "";   //와이파이 password 
String location = "Daejeon"; //날씨정보를 보고싶은 지역

//인스턴스 변수 초기화
ESP8266WebServer server(80);
//WiFiServer server(80);
WiFiClient client ;
//WiFiClient client = server.available();
const unsigned long requestInterval = 60000;  // 요구 시간 딜레이(1 min)

IPAddress hostIp;
uint8_t ret;

unsigned long lastAttemptTime = 0;            // 마지막으로 서버에서 데이터를 전송받은 시간
bool tagInside = false;  // 태그 안쪽인지 바깥쪽인지 구별하는 변수
bool flagStartTag = false; // 스타트 태그인지 구별하는 변수
String currentTag = ""; // 현재 태그를 저장하기 위한 변수, 공백으로 초기화함
String currentData = ""; // 태그 사이의 컨텐츠를 저장하는 변수
String startTag = ""; // 현재 elements의 start tag 저장
String endTag = "";   // 현재 elements의 end tag 저장
String temp="";



void setup() {
  Serial.begin(115200);    

  delay(10);
  //WiFi연결 시도
  Serial.println("Connecting to WiFi....");  
  WiFi.begin(ssid, pass);  //WiFi가 패스워드를 사용한다면 매개변수에 password도 작성

  while (WiFi.status() != WL_CONNECTED) { 
      delay(500); 
      Serial.print("."); 
    } 
    Serial.println(""); 
    Serial.println("WiFi connected"); 
  //Serial.println("Connect success!");
  server.begin();
  Serial.println("Server started"); 
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Waiting for DHCP address");
  //DHCP주소를 기다린다
  while(WiFi.localIP() == INADDR_NONE) {
    Serial.print(".");
    delay(300);
  }
    
  Serial.println("\n");
  printWifiData();
  connectToServer();  
  //server.on("/", [](){                          
  //server.send(200, "text/plain", "this works as well");  
  //});
  server.on("/",weather);
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
}

void printWifiData() {
  // Wifi쉴드의 IP주소를 출력
  Serial.println();
  Serial.println("IP Address Information:");  
  //IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  //서브넷 마스크 출력
  //IPAddress subnet = WiFi.subnetMask();
  Serial.print("NetMask: ");
  Serial.println(WiFi.subnetMask());

  //게이트웨이 주소 출력
  //IPAddress gateway = WiFi.gatewayIP();
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  ret = WiFi.hostByName("api.openweathermap.org", hostIp);

  Serial.print("ret: ");
  Serial.println(ret);

  Serial.print("Host IP: ");
  Serial.println(hostIp);
  Serial.println("");
  //eserver.handleClient();
}

//서버와 연결
void connectToServer() {
  Serial.println("connecting to server...");
  String content = "";
  if (client.connect(hostIp, 80)) {
    Serial.println("Connected! Making HTTP request to api.openweathermap.org for "+location+"...");
    client.println("GET /data/2.5/weather?q=Daejeon&appid=fd2feb6166d777ad1b0948c661a036c9&units=metric&mode=xml HTTP/1.1");
    //위에 지정된 주소와 연결한다.
    client.print("HOST: api.openweathermap.org");
    client.println("Connection: close");

    client.println();
    Serial.println("Weather information for "+location);
  }
  //마지막으로 연결에 성공한 시간을 기록
  lastAttemptTime = millis();
  pashing();
}

void pashing(){
  //WiFiClient client = server.available(); 
  if (client.connected()) {
   while (client.available()) {
    char c = client.read();
    //Serial.print(c);
    if (c == '<') {
      tagInside = true;
    }
    if (tagInside) {
      currentTag += c;
    } else if (flagStartTag) {
      currentData += c;
    } 
    if (c == '>') {
      tagInside = false;
      if (currentTag.startsWith("</")) {
        flagStartTag = false;
        endTag = currentTag;  // store end Tag
        if (startTag.indexOf("country") != -1) {
          if (endTag.indexOf("country") != -1) {
            Serial.print("Country : ");
            Serial.println(currentData);
          }
        }
        currentData = ""; 
      } else {
        flagStartTag = true;                
        startTag = currentTag; // store start Tag
        startTagProcessing();
      } 
      currentTag = "";
    }
  }
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
    // do nothing forevermore:
    while (true);
   }   
  }
//  client.close();
client.stop();
Serial.print("diconnect");
}

void startTagProcessing() {
  if (startTag.startsWith("<city")) {
    int attribName = startTag.indexOf("name=");
    if (attribName != -1) {
      String cityName = startTag.substring(attribName + 6);
      int quote = cityName.indexOf("\"");
      cityName = cityName.substring(0, quote);
      Serial.println("City : " + cityName);
    }
  }
  else if (startTag.startsWith("<temperature")) {
    int attribValue = startTag.indexOf("value=");
    if (attribValue != -1) {
      String tempValue = startTag.substring(attribValue + 7);
      int quote = tempValue.indexOf("\"");
      tempValue = tempValue.substring(0, quote);
      Serial.println("Temperature : " + tempValue);
      temp+="Temperature : ";
      temp+=tempValue;
    }
  }
  else if (startTag.startsWith("<humidity")) {
    int attribValue = startTag.indexOf("value=");
    if (attribValue != -1) {
      String humidValue = startTag.substring(attribValue + 7);
      int quote = humidValue.indexOf("\"");
      humidValue = humidValue.substring(0, quote);
      Serial.println("Humidity : " + humidValue + "%");
      temp+="\nHumidity : ";
      temp+=humidValue;
      temp+="%";
      Serial.println("temp");
      Serial.print(temp);      
    }
  }
}
void weather(){
  server.send(200,"text/plain",temp);
}

