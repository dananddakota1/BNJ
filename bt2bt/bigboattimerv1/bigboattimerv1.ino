/* Boat monitor v1 26 October 2016 BNJ
  This displays six 3 column columns each seperated buy 4 blank columns
  UPDATE HISTORY

*/

//include the LC library
#include "LedControl.h"

/*
  LedControl Pins
  pin 12 is connected to the DataIn
  pin 11 is connected to the CLK
  pin 10 is connected to LOAD
  single MAX72XX.
*/
// modify here for boat number to be displayed 12,11,10,6
LedControl lc = LedControl(12, 11, 10, 6);

#define inpin1  4
#define inpin2  5
#define inpin3  6
#define inpin4  7
#define inpin5  8
#define inpin6  9
int disNum = 0;
int nextBoat = 0;
int bestBoat = 0;
int ipflag [6] = {0, 0, 0, 0, 0, 0};
int oldipflag [6] = {0, 0, 0, 0, 0, 0};
int firstcol[6] =   {4, 5, 6, 7,  0, 1};
int secondCol[6] =  {5, 6, 7, 0, 1, 2};
int thirdcol[6] =   {6, 7, 0, 1, 2, 3 };
float stepTime = 10;
int onTime = 80;
//          test values to give shorter runs - comment out as required
int testFlag = 1;
float runtime [6] = {0, 0, 0, 0, 0, 0};
float oldruntime [6] = {0, 0, 0, 0, 0, 0};
float oldsystime;
float cycletime;
float inctime;
float xt;
int cyclelength = 1000; // milliseconds
int boatlu[3][6] = {0, 1, 2, 3, 3, 4,
                    0, 1, 2, 2, 3, 4,
                    0, 1, 1, 2, 3 , 4
                   };
int barlvl[6] = {0, 0, 0, 0, 0, 0};
int oldbarlvl[6] = {1, 1, 1, 1, 1, 1};
int state = 0;
int clearflag[6] = {0, 0, 0, 0, 0, 0};
//                                              Digit definition array
byte numberDef [6][8] = {B00011100, B00001000, B00001000, B00001000, B00001000, B00001100, B00001000, B00000000,
                         B00111110, B00000100, B00001000, B00010000, B00100000, B00100010, B00011100, B00000000,
                         B00011100, B00100010, B00100000, B00010000, B00001000, B00010000, B00111110, B00000000,
                         B00010000, B00010000, B00111110, B00010010, B00010100, B00011000, B00010000, B00000000,
                         B00011100, B00100010, B00100000, B00100000, B00011110, B00000010, B00111110, B00000000,
                         B00011100, B00100010, B00100010, B00011110, B00000010, B00000100, B00011000, B00000000
                        };

byte letterDef [4][8] = {B00111110, B01000001, B01000001, B01110001, B00000001, B00000001, B01000010, B00111100,
                         B01000001, B01000001, B01000001, B01000001, B01001001, B01010101, B01100011, B01000001,
                         B01111110, B00000010, B00000010, B00000010, B00011110, B00000010, B00000010, B01111110,
                         B00111100, B01000010, B01000000, B00110000, B00001100, B00000010, B01000010, B00111100
                        };
byte andMask [8] = {B00000001, B00000011, B00000111, B00001111, B00011111, B00111111, B01111111, B11111111
                   };
byte opBuffer [4][8] = {  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
                          B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
                          B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
                          B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000
                       };
int panIndex;
int colIndex;
int rowIndex;

void setup() {
  /* we always wait a bit between updates of the display */
  // brightness setting set low for battery demos= 0;
  int disLit = 4;
  /*
    The MAX72XX is in power
    -saving mode on startup,
    we have to do a wakeup call
  */
  // using the 4 panel display disNum indicates the panel to be used
  for (int disnum = 0 ; disnum < 8 ; disnum++)
  {

    lc.shutdown(disnum, false);
    /* Set the brightness to a medium values */
    lc.setIntensity(disnum, disLit);
    /* and clear the display */
    lc.clearDisplay(disnum);
  }
  // enable use of serial monitor
  Serial.begin(115200);
  /*
    each  panel displays two 'fuel gauge'  columns for 2 boats a 3 pixel column for each boat

    When the fuel gauge reaches is minimum the lowest level bar stays on until the external tx line goes low.

    The boat number of the  boat with the lowest fuel level is shown on the right hand panel.
  */
  pinMode(inpin1, INPUT);
  pinMode(inpin2, INPUT);
  pinMode(inpin3, INPUT);
  pinMode(inpin4, INPUT);
  pinMode(inpin5, INPUT);
  pinMode(inpin6, INPUT);
  if (testFlag > 0)
  {
   // stepTime = 39;
   // onTime = 312;
  }
}
// Control loop
void loop() {
  /* timming control assumes a promgram cycle time of 1 second varying the delay time to compensate
    for varitations in the actual cycle time.  Note we could move to a two  second cycle time if required. */

  oldsystime = millis();

  //                                    check input states.
  for (int x = 0; x < 6; x++) {
    ipflag[x] = digitalRead (x + 4);
    if (ipflag[x] == 1)
    {
      //                                setup the boatnumber on the display
      chnext ();
      //                                calculate the runtime in milliseconds
      runtime[x] = runtime[x] + 1;
      //                                limit the runtime too permitted on time.
      if (runtime[x] > onTime) runtime[x] = onTime;
      //                             generate the bar display initialising if required
      if (ipflag[x] != oldipflag[x])      initdisplay (x);
      oldipflag[x] = ipflag[x];
      bardisplay(x);
    }
    else
    {
      cleardis(x);
      barlvl[x] = 0;
      runtime[x] = 0;
      oldipflag [x] = 0;
      nextBoat = 0;
      if ( bestBoat == x)
      {
        //       +-+_+_+_+_+_ reinstate the following line
          lc.clearDisplay(5);
        bestBoat = 0;
      }
    }
  }
  //          workout the code execution time and adjust to give a 1 second cycle time
  if (ipFlagchk() < 1) scrnSave();
  xt = millis();
  cycletime = (xt - oldsystime);
  Serial.print("cycle length = ");
  Serial.println(cycletime);
  Serial.print("   ");
  Serial.print(ipFlagchk());
  Serial.print("    ");


  if (cycletime < cyclelength)  delay(cyclelength - cycletime);

}

//                          USER FUCTIONS

//                              Check time left and update display
void bardisplay(int boatid)
{
  if (runtime[boatid] < onTime)
  {
    calclvl(boatid);
    xupdate(boatid, firstcol[boatid], barlvl[boatid]);
  }
}
//                                                  delete a bar
void xupdate(int boat, int col , int level)
{
  {
    level = 8 - level;
    int x = firstcol[boat];
    int y = secondCol[boat];
    int z = thirdcol[boat];
    lc.setLed(boatlu[0][boat], level, x, state);
    delay (25);
    lc.setLed(boatlu[1][boat], level, y, state);
    delay (25);
    lc.setLed(boatlu[2][boat], level, z, state);
    delay (25);
  }
  //                                       calculate number of bars required
}
void calclvl(int boatid)
{
  int x = runtime[boatid];
  barlvl[boatid] = x / stepTime;
}
//                                            clear a column of 8 bars
void cleardis(int boatid)
{
  int x;
  if (clearflag[boatid] > 0 )
  {
    x = firstcol[boatid];
    int y = secondCol[boatid];
    int z = thirdcol[boatid];
    for (int p = 0; p < 8; p++) {
      lc.setLed(boatlu[0][boatid], p, x, false);
      delay(25);
      lc.setLed(boatlu[1][boatid], p, y, false);
      delay(25);
      lc.setLed(boatlu[2][boatid], p, z, false);
      delay(25);
      clearflag[boatid] = 0;
    }
  }
}
//                                              set up  a column of 8 bars
void initdisplay(int boatid)
{
  int x = firstcol[boatid];
  int y = secondCol[boatid];
  int z = thirdcol[boatid];
  for (int p = 0; p < 8; p++) {
    lc.setLed(boatlu[0][boatid], p, x, true);
    delay(30);
    lc.setLed(boatlu[1][boatid], p, y, true);
    delay(30);
    lc.setLed(boatlu[2][boatid], p, z, true);
    delay(30);
    clearflag[boatid] = 1;
  }
}
//                                             Select next boat too finish
void chnext()
{
  if (nextBoat < 1) {
    for (int x = 0; x < 6; x++) {
      if (runtime[x] > nextBoat && (ipflag[x] > 0))
      { nextBoat = runtime[x];
        bestBoat = x;
        Serial.print(nextBoat);
        Serial.print("     ");
        Serial.println(x);
        Serial.println(ipFlagchk());
      }
    }
    boatNum(bestBoat);
  }
}
//                                              display boat number
void boatNum (int boatid)
{
  //#-#-#-#-#-#-#-#--# remove /* */ around the next two lines and delete this
   for (int row = 0; row < 8; row++)
     lc.setRow(5, row, numberDef [boatid] [row]);
}
int ipFlagchk()
{
  int aCc = 0 ;
  //int result ;
  for (int x = 0 ; x < 6; x++)
  {
    aCc = aCc + ipflag [x];
  }
  //  Serial.println(aCc);
  return aCc;
}
//                      screen saver  random single dot
void scrnSave() {
  //logoscn();
  //}

  // note (dTime x dCount) should = 925 or there abouts
  int  dTime = 44;
  int dCount = 18;
  int x = random(8);
  int y = random(8);
  //'-'-'-'-'--'-'-'-'-'-'-'-'-'-'-'-change p = 8 to include boat number display
  int p = random(8);
  //                          use a fixed P to select a single panel  0 1 2 3
  // int p = 3;
  for (int d = 0 ; d < dCount; d++)
  {

    lc.setLed(p, y, x, true);
    delay(dTime);
   // for (int c = 0;  c < 5; c++);
  //  {
  //    x = random(8);
   //   y = random(8);
      //  p = random (5);
      lc.setLed(p, y, x, false);
      delay(dTime);
  //  }
    x = random(8);
    y = random(8);
    //'-'-'-'-'--'-'-'-'-'-'-'-'-'-'-'-change p = 6 to include boat number display
      p = random (8);
    // p = 3;
  }
}
void logoscn ()
{
  Serial.print("Hi");

  for (panIndex = 3; panIndex = 0; panIndex--)
  { Serial.print("Hi");
    for (int colIndex = 0; colIndex < 8; colIndex++)
    { Serial.print("Hi");
      opBuffer[panIndex][colIndex] = (letterDef[colIndex] && andMask[colIndex]);
      lc.setRow(panIndex, colIndex, opBuffer);
      delay(50);
      Serial.print(panIndex);
      Serial.println("hello");
    }


  }
}

