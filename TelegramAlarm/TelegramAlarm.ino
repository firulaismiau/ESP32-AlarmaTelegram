#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>


// Replace with your Wifi network data
#define WIFI_SSID "****"
#define WIFI_PASSWORD "****"


//Telegram BOT Token are obtained from Botfather in telegram
#define BOT_TOKEN "8888888:*****************************"
const unsigned long timea = 6000; //average time between messages scan
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long lastTime; // last time messages scan was realized

//Define the alarm variables
#define TRIGGER 14  //Trigger Pin
#define ECHO 12     //Echo Pin
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
#define BUZZER 32   //Buzzer Pin

bool sound = LOW; //state of alarm 
long duration;
float distanceCm;
float distanceInch;
float distanceSelect=70; //Maximum distance that will trigger the alarm
bool alarmState = LOW;
int distanceMax = 300; //Maximum distance allowed


int first = 1;
String chat_id;
#define ID_Chat "********"//ID_Chat obtained from Telegram

//This fuction read the messages writed in Telegram and realize the changes of the system
void newMessages(int numMessages)
{
  for (int i = 0; i < numMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    if (text == "ON")
    {
      
      sound = HIGH;
      bot.sendMessage(chat_id, "Alarm: ON", "");
    }

    if (text == "OFF")
    {
      sound = LOW;
      
      bot.sendMessage(chat_id, "Alarm: OFF", "");
    }
    

////////State of alarm   

    if (text == "State")
    {
      ////Estado luz 1////
      if (sound)
      {
        bot.sendMessage(chat_id, "Alarm is ON", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Alarm is OFF", "");
      }
     
    }

    //Distance 
    String myString2;
    if(text=="Distance"){
     myString2 = String(distanceSelect);
      bot.sendMessage(chat_id,myString2);
    }
    //Modify distance
    if(text =="MOD"){
      alarmState = HIGH;    //State HIGH : next number will be the new distance   State LOW : no changes allowed
      bot.sendMessage(chat_id,"Write a number between 1-300");
    }
  String myString;
    for(int j=0;j<distanceMax;j++){
      myString = String(j);
      if(text==myString && alarmState == HIGH){
        distanceSelect = j;
        alarmState = LOW;
        
        bot.sendMessage(chat_id,"Distance changed to"+text);
      }
    }
    if (text == "Help")
    {
      String help = "Welcome to ESP32 System, " ".\n";
      help += "here are your options.\n\n";
      help += "ON: turn on the alarm \n";
      help += "OFF:  turn off the alarm \n";
      help += "MOD:  change the Maximum distance";
      help += "Distance : get the Maximum distance ";
      help += "State : get the alarm state\n";
      help += "Help: print this menu \n";
      help += "Remember this system distinction between caps and minuscules \n";
     
      bot.sendMessage(chat_id,help, "");
      
    }
    
    
  }
}



void setup()
{
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  pinMode(TRIGGER,OUTPUT);
  pinMode(ECHO,INPUT);

  // Trying to connect to WIFI Red
  Serial.print("Connectin to red ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); //Add root certificate to api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nConnected to Wifi red. IP Direction: ");
  Serial.println(WiFi.localIP());
   if(first == 1){
    Serial.println("System ready");
    bot.sendMessage(ID_Chat, "System ready!!!, Write Help to see the options", "");//We send a message to Telegram to notify the system is ready
    first = 0; 
    }
}



void loop()
{
  //Verify is there are messages in telegram for every seconds
  if (millis() - lastTime > timea)
  {
    int numMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numMessages)
    {
      Serial.println("Command received");
      newMessages(numMessages);
      numMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    lastTime = millis();
  }
  if(sound){
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(1);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIGGER, HIGH);
  
  delayMicroseconds(5);
  digitalWrite(TRIGGER, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;

  delay(100);
  if(distanceCm<distanceSelect ){
      
        digitalWrite(BUZZER,HIGH);
        //Buzzer is activated
        bot.sendMessage(ID_Chat, "ENTER ", "");
    
     
  }
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);

  delay(500);

  }else{
    digitalWrite(BUZZER, LOW);
  }

  
}
