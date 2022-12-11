#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <ESP_Mail_Client.h>

#define WIFI_SSID "Nokia"
#define WIFI_PASSWORD "foobar123"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* The sign in credentials */
#define AUTHOR_EMAIL "botesp0032@gmail.com"
#define AUTHOR_PASSWORD "tzsernorvhblqrao"

/* Recipient's email*/
#define RECIPIENT_EMAIL "snowballrohini@gmail.com"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

#define trigPin 23
#define echoPin 22
#define BUZZER_PIN 17


float duration, distance;
void setup() {
  // put your setup code here, to run once:
 Serial.begin (115200);
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 pinMode(BUZZER_PIN, OUTPUT);
}


void trigger() {
 Serial.begin(115200);
 Serial.println();
 Serial.print("Connecting to AP");
 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 while (WiFi.status() != WL_CONNECTED){
  Serial.print(".");
  delay(200);
 }
 Serial.println("");
 Serial.println("WiFi connected.");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
 Serial.println();

  /** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
  */
 smtp.debug(1);

  /* Set the callback function to get the sending results */
 smtp.callback(smtpCallback);

  /* Declare the session config data */
 ESP_Mail_Session session;

  /* Set the session config */
 session.server.host_name = SMTP_HOST;
 session.server.port = SMTP_PORT;
 session.login.email = AUTHOR_EMAIL;
 session.login.password = AUTHOR_PASSWORD;
 session.login.user_domain = "";

  /* Declare the message class */
 SMTP_Message message;

  /* Set the message headers */
 message.sender.name = "ESP";
 message.sender.email = AUTHOR_EMAIL;
 message.subject = "ESP Alert Email";
 message.addRecipient("Sara", RECIPIENT_EMAIL);

  /*Send HTML message*/
 String htmlMsg = "<div style=\"color:#2f4468;\"><h1>INTRUDER ALERT!</h1><p>- Sent from ESP board</p></div>";
 message.html.content = htmlMsg.c_str();
 message.html.content = htmlMsg.c_str();
 message.text.charSet = "us-ascii";
 message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  /*
  //Send raw text message
  String textMsg = "INTRUDER ALERT! - Sent from ESP board";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;*/

  /* Set the custom message header */
  //message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

  /* Connect to server with the session config */
 if (!smtp.connect(&session))
  return;

  /* Start sending Email and close the session */
 if (!MailClient.sendMail(&smtp, &message))
  Serial.println("Error sending Email, " + smtp.errorReason());
}

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
 Serial.println(status.info());

  /* Print the sending result */
 if (status.success()){
  Serial.println("----------------");
  ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
  ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
  Serial.println("----------------\n");
  struct tm dt;

  for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
   SMTP_Result result = smtp.sendingResult.getItem(i);
   time_t ts = (time_t)result.timestamp;
   localtime_r(&ts, &dt);

   ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
   ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
   ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
   ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
   ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
  }
  Serial.println("----------------\n");
 }
 
}
void loop() {
  // put your main code here, to run repeatedly:
 digitalWrite(trigPin, LOW);
 delayMicroseconds(2);
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPin, LOW);
 
  // Measure the response from the HC-SR04 Echo Pin
 duration = pulseIn(echoPin, HIGH);
 
  // Determine distance from duration
  // Use 343 metres per second as speed of sound
 distance = (duration / 2) * 0.0343;
 
  // Send results to Serial Monitor
 Serial.println("Reading ...... ");
 if (distance >= 400 || distance <= 2) {
  Serial.println("Out of range");
 }
 else {
   if(distance<=10) {
    Serial.print(distance);
    Serial.println(" cm");  
    delay(5000);
    digitalWrite(BUZZER_PIN,HIGH);
    delay(3000);
    digitalWrite(BUZZER_PIN,LOW);
    delay(1000);
    trigger();
    delay(1000);
   }
 }
 delay(5000);
}