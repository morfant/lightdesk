#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>
#include <wiringShift.h>

#define SHIFT_REG_DELAY 0 

// define pins
// VCC control
#define lPin_v 28
#define cPin_v 29
#define dPin_v 27

// GND control
#define lPin_g 24
#define cPin_g 25
#define dPin_g 23


void printBin(int num);
int setX(int nBit);
void setY(int nBit);
void setYbits(int ix, int iy);
void clrX(int nBit);
void clrY(int nBit);
void clrYbits(int ix, int iy);
void updateX();
void updateY(int yBits);

int xbit_1 = 0;
int xbit_2 = 0;
int ybit_1 = 0;

int yBits[11] = {0, 0, 0, 0,  0, 0, 0, 0,  0, 0 ,0 };

int main ()
{
  int fd;
  int count = 0;
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

      int i;
      char x = 0;
      char y = 0;
      char val = 0;

 //     x = serialGetchar(fd);
//      y = serialGetchar(fd);
//      val = serialGetchar(fd);

//      printf("\n%d : %d : %d \n", x, y, val );
     
      while (serialDataAvail (fd)){

          count++;
//          printf("Serial data coming..\n");
          x = serialGetchar (fd);
          y = serialGetchar (fd);
          val = serialGetchar (fd);

          printf("\ncount: %d, x: %d, y: %d, val: %d \n", count, x, y, val );
          
//          fflush (stdout) ;
      }

  //    char x = 0;
   //   char y = 0;
    //  char val = 0;

 
      // make ybit_1 '0xxxxxxx'
      ybit_1 &= 0x7f;
      
      if (val == 1){
          setYbits(x, y);
      }else if (val == 0) {
 //         clrX(ix);
//          clrY(iy);
          clrYbits(x, y);
      }

      // check bit set/clr
//      printf("xbit_1: %d, xbit_2: %d\n", xbit_1, xbit_2); 
//      printf("\n");
//      printf("ix: %d, yBits: %d\n", ix, yBits[ix]); 
 
 // printf("mt is running...\n");    
 
//      xbit_1 = 8;
//      xbit_2 = 3;
//      ybit_1 = 5; 

 // 595 loop
  int curTime = millis();
 for (i = 0; i < 11; i++){
    
     if (i > 0){
        clrX(i - 1);
     }else if (i == 0) {
         clrX(10);
     }


     int xbits = setX(i);

/*
     printf("X: ");
     printBin(xbits);
     printf("\n");

     printf("\tY: ");
     printBin(yBits[i]);
     printf("\n");
 */    
     updateX();
     updateY(yBits[i]);
     //     delay(2);
//     printf("in 595 loop(%d).\n", i);
 }   
// printf("elasped time for loop: %d\n", millis() - curTime);

  }

  return 0;
}

void printBin(int num)
{
    int i = 0;
    for (i = 0; i < 16; i++){
        int bit_value = num & (0x8000 >> i); //0x8000 = 1000 0000 0000 0000

        if (bit_value == 0) printf("0");
        else printf("1");

        if (i == 7) printf(" ");
    }
}

int setX(int nBit)
{
    if (nBit < 8){
        xbit_1 |= 1 << nBit;
    }else{
        xbit_2 |= 1 << (nBit - 8);
    }

    int xbit_sum = 0;
    xbit_sum |= xbit_1 << 8;
    xbit_sum |= xbit_2;

   return xbit_sum; 
//    int bit_sum = (xbit_1 << 8) | xbit_2;
}

void setY(int nBit)
{
    ybit_1 |= 1 << nBit;
}

void setYbits(int ix, int iy)
{
    yBits[ix] |= 1 << iy;
}

void clrX(int nBit)
{
    /*
    int i;
    // make xbit_2 '00000xxx'
    for (i = 3; i < 8; i++) {
        xbit_2 &= ~(1 << i);
    };
    */
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

void clrYbits(int ix, int iy)
{
    yBits[ix] &= ~(1 << iy);
}

void updateX()
{
    digitalWrite(lPin_v, 0);
    shiftOut(dPin_v, cPin_v, MSBFIRST, xbit_2);
    shiftOut(dPin_v, cPin_v, MSBFIRST, xbit_1);
    digitalWrite(lPin_v, 1);
    delay(SHIFT_REG_DELAY);
}

void updateY(int bits)
{
    digitalWrite(lPin_g, 0);
    shiftOut(dPin_g, cPin_g, MSBFIRST, bits);
    digitalWrite(lPin_g, 1);
    delay(SHIFT_REG_DELAY);
}
