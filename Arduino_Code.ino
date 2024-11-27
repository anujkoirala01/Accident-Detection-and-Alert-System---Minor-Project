#include<SoftwareSerial.h>
#include<TinyGPS++.h>
SoftwareSerial Serial1(2,3); // RX arduino line  pin 2, TX arduino line is pin 3
int GPSBaud = 9600;

// Create a TinyGPS++ object
TinyGPSPlus gps;
SoftwareSerial gpsSerial(11,10); //Tx 10 Rx 11

#include<LiquidCrystal.h>
int Contrast=40;        //for Vee in LCD
LiquidCrystal lcd(4,5,6,7,8,9);

#define x A1

#define y A2

#define z A3


int xsample=0;

int ysample=0;

int zsample=0;


#define samples 10

#define minVal -50

#define MaxVal 50

double latitude ;
double longitude ;


void initModule(String cmd, char *res, int t)

{while(1)

  {

    Serial.println(cmd);

    Serial1.println(cmd);

    delay(100);

    while(Serial1.available()>0)

    {

       if(Serial1.find(res))

       {

        Serial.println(res);

        delay(t);

        return;

       }


       else

       {

        Serial.println("Error");

       }

    }

    delay(t);
 }

}


void setup() 

{

  Serial.begin(9600);

  Serial1.begin(9600);

  analogWrite(12,Contrast);

  

  lcd.begin(16,2);  

  lcd.print("Accident Alert  ");

  lcd.setCursor(0,1);

  lcd.print("     System     ");

  delay(2000);

  lcd.clear();

  lcd.print("Initializing");

  lcd.setCursor(0,1);

  lcd.print("Please Wait...");

  delay(1000);

  

  Serial.println("Initializing....");

  initModule("AT","OK",1000);

  initModule("ATE1","OK",1000);

  initModule("AT+CPIN?","READY",1000);  

  initModule("AT+CMGF=1","OK",1000);     

  initModule("AT+CNMI=2,2,0,0,0","OK",1000);  

  Serial.println("Initialized Successfully");

  lcd.clear();

  lcd.print("Initialized");

  lcd.setCursor(0,1);

  lcd.print("Successfully");

  delay(2000);

  lcd.clear();

  lcd.print("Callibrating ");

  lcd.setCursor(0,1);

  lcd.print("Acceleromiter");

  for(int i=0;i<samples;i++)

  {

    xsample+=analogRead(x);

    ysample+=analogRead(y);

    zsample+=analogRead(z);

  }
   xsample/=samples;

  ysample/=samples;

  zsample/=samples;


  Serial.println(xsample);

  Serial.println(ysample);

  Serial.println(zsample);

  delay(1000);


  lcd.clear();

  lcd.print("Waiting For GPS");

  lcd.setCursor(0,1);

  lcd.print("     Signal    ");

  delay(2000);

  gpsSerial.begin(GPSBaud);

  get_gps();

  delay(2000);

  lcd.clear();

  lcd.print("GPS is Ready");
  delay(1000);

  lcd.clear();

  lcd.print("System Ready");

  Serial.println("System Ready..");

}


void loop() 

{

    while (gpsSerial.available() > 0)
      {gps.encode(gpsSerial.read());}

    int value1=analogRead(x);

    int value2=analogRead(y);

    int value3=analogRead(z);


    int xValue=xsample-value1;

    int yValue=ysample-value2;

    int zValue=zsample-value3;

    

    Serial.print("x=");

    Serial.println(xValue);
    Serial.print("y=");

    Serial.println(yValue);

    Serial.print("z=");

    Serial.println(zValue);


    if(xValue < minVal || xValue > MaxVal  || yValue < minVal || yValue > MaxVal  || zValue < minVal || zValue > MaxVal)

    {

      get_gps();
      
      displayInfo();
      Send();
      
      lcd.clear();

      lcd.print("Sending SMS ");

      Serial.println("Sending SMS");

      Send();
      

      Serial.println("SMS Sent");

      delay(2000);

      lcd.clear();

      lcd.print("System Ready");

    }       

}


void get_gps()

{

  lcd.clear();

  lcd.print("Getting GPS Data");

  lcd.setCursor(0,1);

  lcd.print("Please Wait.....");

  while (gpsSerial.available() > 0)
      if (gps.encode(gpsSerial.read()))
        displayInfo();

    if (millis() > 50000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while(true);
  }
  

}



void displayInfo()
{
  if (gps.location.isValid())
  {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    double latitude = gps.location.lat();
   double longitude = gps.location.lng();
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
    Serial.println("http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=");
    Serial.print(latitude,6);

   Serial.print("+");              

   Serial.print(longitude,6);
  }
  else
  {
    Serial.println("Location: Not Available");
  }
}


void Send()

{ 
  double latitude = gps.location.lat();
   double longitude = gps.location.lng();

   Serial1.println("AT");

   delay(500);

   serialPrint();

   Serial1.println("AT+CMGF=1");

   delay(500);

   serialPrint();

   Serial1.print("AT+CMGS=");

   Serial1.print('"');

   Serial1.print("9762420890");    //mobile no. for SMS alert

   Serial1.println('"');

   delay(500);

   serialPrint();

   Serial1.print("Latitude:");

   Serial1.println(latitude);

   delay(500);

   serialPrint();

   Serial1.print("Longitude:");

   Serial1.println(longitude);

   delay(500);

   serialPrint();

   Serial1.print("http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=");

   Serial1.print(latitude,6);

   Serial1.print("+");              

   Serial1.print(longitude,6);

   Serial1.write(26);

   delay(2000);

   serialPrint();

}


void serialPrint()

{

  while(Serial1.available()>0)

  {

    Serial.print(Serial1.read());

  }

}
