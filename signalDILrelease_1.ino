/*                                                signalDIL release1
 *Version 1 mod 3 nov 2021
            mod 4 dil swt added 17/11/21
            mod 5 data setup added 23 Nov 2021
            mod 6 dil keys modified 30 Nov 2021
            mod 7 resetting printout 8  DEC 2021
            mod 8 flashled added befor void loop 14 DEC 2021
   Board ref :-6A S4b.sch
  This is the first version of the fully operational
  track circuit for  instation on a signal post
                   Analouge inputs are used as follows:
  A0 - track detectors:
  P2 - push button
  P3 - Link in monitor input
                  didital i/o is used for output as follows:
  op4 - relay A TPR
  op5 - relay B Linkin switching relay
  op6 - LED
  iP7 bluetooth tansmit (promini rec)
  iP8 bluetooth recieve (promini receive)
  note: bluetooth receive input is s3.3v

                               Program summary:
  TRAFFIC DETECTION
  The track detector input AO is monitored every 10 millisecs
  if it is below the TRACKTARGETVALUE
      then DELAYTIME is set the DELAYTARGET + current time in
      milliseconds and op4 is set HIGH
  If the track detector input is above the TRACKTARGETVALUE
      and the current time is greater than the
      DELAYTIME then op4 is set low.
      (Note relay input is inverted ie low input turns relay on)

  LINKIN
  If the Linkin monitor input P3 is HIGH then op5 is set LOW
  If Linkin monitor input is LOW then op5 is set HIGH
*/
#include <SoftwareSerial.h>
SoftwareSerial BTserial(7, 8); // RX | TX
int sensorPin = A0;
int sensorValue = 123;
int long tracktargetvalue = 150; // holds the analogue detect setting
unsigned long delaytime;
int long delayvalue;
int trackcircuitstate = 0;
int linkinstate = 0;
int pushbuttonstate = 0;
int relayA = 4;               // track circuit relay A
int relayB = 5;               // Link in
int led1 = 6;
int trackanalog = A0;         // track circuit input
int linkin = 3 ;              // Linkin input
int pushbutton = 2;
unsigned long flashtime = 0;
int ledflashflag = 0;
int flashstate = 0;
int long btt = 0;
int long bttold = 0;
int cycletime = 0;
int long  stime = 0;
int long ftime = 0;
unsigned long flashperiod = 500;
float volts = 0;
float tripv = 0;
int trackbusy = 0 ;
int oldtrackbusy = 0;
int exittrack = 0 ;
int oldexittrack = 0;
float transittimestart = 0;
float transittime = 0.0 ;
float sspeed = 0.0;
long tracklength = 170.0;
//                                  dil switch
int dil0 = 13;
int dil1 = 12;
int dil2 = 11;
int dil3 = 10;
int dil4 = 9;
int dilacc1 = 0;
int dilacc2 = 0;
int dils0 = 0;
int dils1 = 0;
int dils2 = 0;
int dils3 = 0;
int dils4 = 0;

void setup()
{


  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  // constants won't change. They're used here to
  // set pin numbers
  // variables will change:
   Serial.begin(9600);
  BTserial.begin(9600);
  long delaytime = 0;
  delayvalue = 5000; /*delays the clearance of the RED for a
                                specified time*/
  // initialize the output pins:
  pinMode(relayA , OUTPUT);
  pinMode(relayB , OUTPUT);
  pinMode(led1 , OUTPUT);
  // initialize the input pins:
  pinMode (pushbutton, INPUT_PULLUP);
  pinMode (linkin, INPUT);
  delay (5000);

  // Test for track clear.

  tracktargetvalue = (analogRead(trackanalog) / 2);
  if (tracktargetvalue < 75)
  {
    tracktargetvalue = 1200;
  }
  // Note setttng the TTV higher than the maximum possible ensures
  // that the signal is set to RED IF the system was powered up
  // with the track section  short circuited.

  trackcircuitstate = analogRead(trackanalog);

  if (trackcircuitstate < 150) {  // track busy detected
    digitalWrite (relayA , HIGH); //turn relay A off  sets
    // signal
  }
}
  void loop()
  {
    stime = micros();
    int temp1 = digitalRead(pushbutton);
    if (temp1 < 1)
    {
      BTserial.println("resetting");
      tracktargetvalue = (analogRead(trackanalog) / 2);
      ledflashflag = 1;
      flashled();
    }
    else
    {
      ledflashflag = 0;
      flashled();
    }
    //process relayA track
    oldtrackbusy = trackbusy;
    trackcircuitstate = analogRead(trackanalog);
    if (trackcircuitstate < tracktargetvalue) // train detected
    {
      digitalWrite (relayA , HIGH); //turn relay A off track occupied
      delaytime = (millis() + delayvalue);
      trackbusy = 1;
    }
    if (millis() > delaytime)  {
      digitalWrite(relayA, LOW); //Turn relay A on Line is clear
      trackbusy = 0;
    }
    oldexittrack = exittrack;
    linkinstate  = digitalRead(linkin);
    if (linkinstate  == LOW)    {
      // turn relay B off, signal ahead  RED
      digitalWrite(relayB, HIGH);
      exittrack = 0;
    }
    else
    {
      //Turn relay B on, signal ahead CLEAR (YELLOW or GREEN).
      digitalWrite(relayB, LOW);
      exittrack = 1;
    }
    bluetoothop();
    speedtest();
    lamptest ();
    ftime = micros ();
    cycletime = (ftime - stime);
  }
  void flashled()
  {
    if (ledflashflag > 0) // flash requested
    { unsigned long temp = millis();
      if (flashtime < temp)
      {
        flashtime = (temp + flashperiod);
        if (flashstate > 0)
        {
          (flashstate = 0);
          digitalWrite (led1, LOW);
        }
        else
        {
          flashstate = 1;
          digitalWrite (led1, HIGH);
        }
      }
    }
    else                 //Flash turned off
    {
      digitalWrite (led1, LOW);
    }
  }
  void bluetoothop ()

  {
    btt = millis();
    if ( btt > (bttold + 4000))
    {
      bttold = millis();

      float volts = (trackcircuitstate / 204.8);
      if (dilacc1 == 0 )BTserial.print("SP10 - Sec AB length ");
      if (dilacc1 == 1 )BTserial.print("SP11 - Sec AC length ");
      if (dilacc1 == 2 )BTserial.print("SP12 - Sec AD length ");
      if (dilacc1 == 3 )BTserial.print("SP13 - Sec AE length ");
      BTserial.print (tracklength);
      BTserial.println ("ft");
      BTserial.print("trackvolts ");
      // Serial.println (tracktargetvalue);
      BTserial.print (volts);
      BTserial.print(",");
      BTserial.print("TRIP VOLTS ");
      float tripv = (tracktargetvalue / 204.8);
      BTserial.print(tripv);
      BTserial.print(";");
      BTserial.print(" SP ");
      BTserial.print(sspeed);
      Serial.print (sspeed);
      BTserial.println(";");
      Serial.println (transittime);
      idkeys();
    }
  }
  void speedtest()
  {
    if ((trackbusy == 1) && (oldtrackbusy == 0)) //start timing
    {
      Serial.println ("start");
      transittimestart = millis();
      //oldtrackbusy = 1;
    }
    if ((exittrack == 1) && (oldexittrack == 0))
    {
      Serial.println("stop");
      transittime = (millis() - transittimestart);
      transittime = transittime / 1000;
      sspeed = ((tracklength / transittime) / (1.46));
    }
  }
  void idkeys()
  {
    dilacc1 = 0;
    if ((digitalRead(dil0) == 0))dilacc1 = dilacc1 + 1;
    if ((digitalRead(dil1) == 0))dilacc1 = dilacc1 + 2;
    if ((digitalRead(dil2) == 0))dilacc1 = dilacc1 + 4;
    Serial.println(dilacc1);
    if (dilacc1 == 0 )
    {
      Serial.println("SP10");
      tracklength = 224.0;
    }
    if (dilacc1 == 1 )
    {
      Serial.println("SP11");
      tracklength = 199.0;
    }
    if (dilacc1 == 2 )
    {
      Serial.println("SP12");
      tracklength = 177.0;
    }
    if (dilacc1 == 3 )
    {
      Serial.println("SP13");
      tracklength = 145.0;
    }
    if (dilacc1 == 4 )
    {
      Serial.println("u4");
      tracklength = 145.0;
    }
    if (dilacc1 == 5 )
    {
      Serial.println("u5");
      tracklength = 145.0;
    }
    if (dilacc1 == 6 )
    {
      Serial.println("u6");
      tracklength = 145.0;
    }
    if (dilacc1 == 7 )
    {
      Serial.println("u7");
      tracklength = 145.0;
    }
  }
  void lamptest()
  {
    int lptime = (5000);
    while ((digitalRead(dil3) == 0))
    {
      digitalWrite (relayA, HIGH);      // A relay off
      Serial.println ("red on");
      BTserial.println ("red on");      // A off = RED signal
      lampstop ();
      delay (lptime);
      digitalWrite (relayA, LOW);       // A relay off
      Serial.println ("red off");
      BTserial.println ("red off");
      Serial.println ("yellow  on");    /* A on and B off
                                        = YELLOW signal
*/
      BTserial.println ("yellow  on");
      lampstop ();
      delay (lptime);
      digitalWrite (relayB, LOW);      // B relay on
      Serial.println ("yellow off");
      BTserial.println ("yellow off");
      Serial.println ("green on");      /* A on and B on
                                        = GREEN signal */
      BTserial.println ("green on");
      lampstop ();
      delay (lptime);
      digitalWrite (relayB, HIGH);      // B relay off
      Serial.println ("green off");
      BTserial.println ("green off");
    }
  }

  void lampstop()
    
    {
    Serial.println ((digitalRead(dil4)));
    while ((digitalRead(dil4) == 0))
    {
      delay (500);
    }
  }
 
