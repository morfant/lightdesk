#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>
#include <wiringShift.h>

// define pins
// VCC control
#define lPin_v 36
#define cPin_v 38
#define dPin_v 40

// GND control
#define lPin_g 33
#define cPin_g 35
#define dPin_g 37


void setX(int nBit);
void setY(int nBit);
void clrX(int nBit);
void clrY(int nBit);
void updateX();
void updateY();

char xbit_1 = 0;
char xbit_2 = 0;
char ybit_1 = 0;

int main ()
{
  int fd;
  // Init
  
  if ((fd = serialOpen ("/dev/ttyAMA0", 115200)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }

  if (wiringPiSetup () == -1)
  {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }

  // Pin mode

  pinMode(lPin_v, OUTPUT);
  pinMode(cPin_v, OUTPUT);
  pinMode(dPin_v, OUTPUT);
  
  pinMode(lPin_g, OUTPUT);
  pinMode(cPin_g, OUTPUT);
  pinMode(dPin_g, OUTPUT);


  // Loop
  for (;;)
  {

  // need to 3 serial valid check

      char x = serialGetchar (fd);
      char y = serialGetchar (fd);
      char val = serialGetchar (fd);

      if (val){
          setX(atoi(&x));
          setY(atoi(&y));
      }else{
          clrX(atoi(&x));
          clrY(atoi(&y));
      }

      updateX();
      updateY();
     
      printf("\n%d : %d : %d \n", atoi(&x), atoi(&y), atoi(&val) );
  }

  return 0 ;
}

void setX(int nBit)
{
    if (nBit < 8){
        xbit_1 |= 1 << nBit;
    }else{
        xbit_2 |= 1 << (nBit - 8);
    }
}

void setY(int nBit)
{
    ybit_1 |= 1 << nBit;
}

void clrX(int nBit)
{
    if (nBit < 8){
        xbit_1 &= ~(1 << nBit);
    }else{
        xbit_2 &= ~(1 << (nBit - 8));
    }
}

void clrY(int nBit)
{
    ybit_1 &= ~(1 << nBit);
}

void updateX()
{
    digitalWrite(lPin_v, 0);
    shiftOut(dPin_v, cPin_v, MSBFIRST, xbit_1);
    shiftOut(dPin_v, cPin_v, MSBFIRST, xbit_2);
    digitalWrite(lPin_v, 1);
}

void updateY()
{
    digitalWrite(lPin_g, 0);
    shiftOut(dPin_g, cPin_g, MSBFIRST, ybit_1);
    digitalWrite(lPin_g, 1);
}
