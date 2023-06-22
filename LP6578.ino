/*
This code is not clean and far from perfect, that's just
a reference to extract ideas and adapt to your solution.
you can replace the BIN values with HEX... I leave it in BIN
because it is easier to relate the segment number with
  the position of the bit in BIN.
Of course, a library can be created for this purpose! But I won't 
take the time to do that, I'll leave it up to you!
*/

void send_char(unsigned char a);
void send_data(unsigned char a);
void segments();
void buttonReleasedInterrupt();  

#define LCD_in 8  // This is the pin number 8 on Arduino UNO
#define LCD_clk 9 // This is the pin number 9 on Arduino UNO
#define LCD_CE 10 // This is the pin number 10 on Arduino UNO

//unsigned int numberSeg = 0;  // Variable to supporte the number of segment
//unsigned int numberByte = 0; // Variable to supporte the number byte 
unsigned int shiftBit=0;
unsigned int nBitOnBlock=0; // Used to count number of bits and split to 8 bits... (number of byte)
unsigned int nByteOnBlock=0; 
unsigned int sequencyByte=0x00;
byte Aa,Ab,Ac,Ad,Ae,Af,Ag,Ah,Ai,Aj,Ak,Al,Am,An,Ap,Ao;
byte blockBit =0x00;


// constants won't change. They're used here to set pin numbers:
//const int buttonPin = 7;  // the number of the pushbutton pin
const int ledPin = 12;    // the number of the LED pin

#define BUTTON_PIN 2 //Att check wich pins accept interrupts... Uno is 2 & 3
volatile byte buttonReleased = false;

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

bool forward = false;
bool backward = false;
bool isRequest = true;
bool allOn=false;
bool cycle=false;
/*
#define BIN(x) \
( ((0x##x##L & 0x00000001L) ? 0x01 : 0) \
| ((0x##x##L & 0x00000010L) ? 0x02 : 0) \
| ((0x##x##L & 0x00000100L) ? 0x04 : 0) \
| ((0x##x##L & 0x00001000L) ? 0x08 : 0) \
| ((0x##x##L & 0x00010000L) ? 0x10 : 0) \
| ((0x##x##L & 0x00100000L) ? 0x20 : 0) \
| ((0x##x##L & 0x01000000L) ? 0x40 : 0) \
| ((0x##x##L & 0x10000000L) ? 0x80 : 0))
*/

//ATT: On the Uno and other ATMEGA based boards, unsigned ints (unsigned integers) are the same as ints in that they store a 2 byte value.
//Long variables are extended size variables for number storage, and store 32 bits (4 bytes), from -2,147,483,648 to 2,147,483,647.

//*************************************************//
void setup() {
  pinMode(LCD_clk, OUTPUT);
  pinMode(LCD_in, OUTPUT);
  pinMode(LCD_CE, OUTPUT);

  pinMode(13, OUTPUT);
  
// initialize the LED pin as an output:
//pinMode(ledPin, OUTPUT);
// initialize the pushbutton pin as an input:
//pinMode(buttonPin, INPUT);  //Next line is the attach of interruption to pin 2
pinMode(BUTTON_PIN, INPUT);

 attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),
                  buttonReleasedInterrupt,
                  FALLING);

//Dont insert any print inside of interrupt function!!!
//If you run the search function, please active the terminal to be possible print lines,
//Other way the run will be blocked!
//
  Serial.begin(115200);
  
  /*CS12  CS11 CS10 DESCRIPTION
  0        0     0  Timer/Counter1 Disabled 
  0        0     1  No Prescaling
  0        1     0  Clock / 8
  0        1     1  Clock / 64
  1        0     0  Clock / 256
  1        0     1  Clock / 1024
  1        1     0  External clock source on T1 pin, Clock on Falling edge
  1        1     1  External clock source on T1 pin, Clock on rising edge
 */
  
// Note: this counts is done to a Arduino 1 with Atmega 328... Is possible you need adjust
// a little the value 62499 upper or lower if the clock have a delay or advnce on hours.

  digitalWrite(LCD_CE, LOW);
  delayMicroseconds(5);
  digitalWrite(13, LOW);
  delay(500);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
  digitalWrite(13, HIGH);
  delay(500);
}
void send_char(unsigned char a){
 unsigned char transmit = 15; //define our transmit pin
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
  data=a;
  // the validation of data happen when clk go from LOW to HIGH.
  // This lines is because the clk have one advance in data, see datasheet of sn74HC595
  // case don't have this signal instead of "." will se "g"
  digitalWrite(LCD_CE, LOW); // When strobe is low, all output is enable. If high, all output will be set to low.
  delayMicroseconds(5);
  digitalWrite(LCD_clk,LOW);// need invert the signal to allow 8 bits is is low only send 7 bits
  delayMicroseconds(5);
        for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
        digitalWrite(LCD_clk,LOW);// need invert the signal to allow 8 bits is is low only send 7 bits
        delayMicroseconds(5);
                  if (data & mask){ // if bitwise AND resolves to true
                    digitalWrite(LCD_in, HIGH);
                    //Serial.print(1);
                  }
                  else{ //if bitwise and resolves to false
                    digitalWrite(LCD_in, LOW);
                    //Serial.print(0);
                  }
          digitalWrite(LCD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
          delayMicroseconds(1);
          //
        }
  delayMicroseconds(1);
  digitalWrite(LCD_CE, HIGH); // When strobe is low, all output is enable. If high, all output will be set to low.
  delayMicroseconds(1);
}
// I h've created 3 functions to send bit's, one with strobe, other without strobe and one with first byte with strobe followed by remaing bits.
void send_char_without(unsigned char a){
 //
 unsigned char data = 0x00; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
  data=a;
  //Serial.println(":");
        for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
        digitalWrite(LCD_clk, LOW);
        delayMicroseconds(1);
              if (data & mask){ // if bitwise AND resolves to true
                digitalWrite(LCD_in, HIGH);
                //Serial.print(1);
              }
              else{ //if bitwise and resolves to false
                digitalWrite(LCD_in, LOW);
                //Serial.print(0);
              }
          digitalWrite(LCD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
          delayMicroseconds(1);
        }
}
void send_char_8bit_stb(unsigned char a){
 unsigned char data = 0x00; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
 int i = 0;
  data=a;
  //Serial.println(":");
  digitalWrite(LCD_CE, LOW);
  delayMicroseconds(1);
  // This lines is because the clk have one advance in data, see datasheet of sn74HC595
  // case don't have this signal instead of "." will se "g"
          for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
           i++;
           digitalWrite(LCD_clk, LOW);
           delayMicroseconds(1);
                      if (data & mask){ // if bitwise AND resolves to true
                        digitalWrite(LCD_in, HIGH);
                        //Serial.print(1);
                      }
                      else{ //if bitwise and resolves to false
                        digitalWrite(LCD_in, LOW);
                        //Serial.print(0);
                      }
            digitalWrite(LCD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
            delayMicroseconds(1);
    //            if (i==8){
    //            //Serial.println(i);
    //            delayMicroseconds(1); 
    //            }
          }
     digitalWrite(LCD_CE, HIGH);
     delayMicroseconds(1);
}
void controle(){
//PC1 TO PC4: GENERAL PURPOSE OUTPUT PORT CONTROL DATA BITS &&  CT0 TO CT3, CTC: DISPLAY CONTRAST CONTROL DATA BITS
//CTC State of the Display Contrast Adjustment Circuit
//SC SEGMENT ON/OFF CONTROL DATA BIT 
//BU: NORMAL/POWER SAVING MODE CONTROL DATA BIT
//DT1, DT2: DISPLAY TECHNIQUE CONTROL DATA BITS 
//Bit function: pc1, pc2, pc3, pc4, ct0, ct1, ct2, ct3, CTC, SC, BU, DT1, DT2, DD, DD, DD  this is the format of 2 bytes of controle
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      //0b00000001 Retro ilum ON; 0b00000010 LED front panel ON
          send_char_without(0B00000001);  send_char_without(0B10100000);
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); // 
}
void LED(){
//PC1 TO PC4: GENERAL PURPOSE OUTPUT PORT CONTROL DATA BITS &&  CT0 TO CT3, CTC: DISPLAY CONTRAST CONTROL DATA BITS
//CTC State of the Display Contrast Adjustment Circuit
//SC SEGMENT ON/OFF CONTROL DATA BIT 
//BU: NORMAL/POWER SAVING MODE CONTROL DATA BIT
//DT1, DT2: DISPLAY TECHNIQUE CONTROL DATA BITS 
//Bit function: pc1, pc2, pc3, pc4, ct0, ct1, ct2, ct3, CTC, SC, BU, DT1, DT2, DD, DD, DD  this is the format of 2 bytes of controle
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      //0b00000001 Retro ilum ON; 
      //0b00000010 LED front panel ON
          send_char_without(0B00000010); //Here we activate the port P1 to activate the LED's of panel! 
          send_char_without(0B10100000);
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); // 
}
void allON(){
//Bit function: 0, 0, 0, P0, P1, P2, P3, DR, SC, BU, DD, DD;
 for(int i=0; i<5;i++){   // Need send 5 burst of data with 120 bits more 8 of control.
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // On the 75853 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
          
          send_char_without(0B11111111);  send_char_without(0B11111111);  //   8:1     -16:9// 
          send_char_without(0B11111111);  send_char_without(0B11111111);  //  24:17    -32:25// 
          send_char_without(0B11111111);  send_char_without(0B11111111);  //  40:33    -48:41// 
          send_char_without(0B11111111);  send_char_without(0B11111111);  //  56:49    -64:57// 
          send_char_without(0B11111111);  send_char_without(0B11111111);  //  72:65    -80:73// 
          send_char_without(0B11111111);  send_char_without(0B11111111);  //  88:81    -96:89// 
          send_char_without(0B11111111);  send_char_without(0B11111111);  // 104:97   -112:105//
          send_char_without(0B11111111);   //  120:113   
           //the next switch send reamaining bits -41:48// 
              switch (i){ //Last 3 bits is "DD" data direction, and is used
                case 0: send_char_without(0B00000000); break;
                case 1: send_char_without(0B10000000); break;
                case 2: send_char_without(0B01000000); break;
                case 3: send_char_without(0B11000000); break;
                case 4: send_char_without(0B00100000); break;
              }
      // to mark the 5 groups of 136 bits, 00, 01, 10, 11, 101
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); // 
      }
      //controle();
}
void allOFF(){
//Bit function: 
 for(int i=0; i<5;i++){   // Need send 5 burst of data with 120 bits more 8 of control.
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // 
          send_char_without(0B00000000);  send_char_without(0B00000000);  //   8:1     -16:9//
          send_char_without(0B00000000);  send_char_without(0B00000000);  //  24:17    -32:25// 
          send_char_without(0B00000000);  send_char_without(0B00000000);  //  40:33    -48:41// 
          send_char_without(0B00000000);  send_char_without(0B00000000);  //  56:49    -64:57// 
          send_char_without(0B00000000);  send_char_without(0B00000000);  //  72:65    -80:73// 
          send_char_without(0B00000000);  send_char_without(0B00000000);  //  88:81    -96:89//  
          send_char_without(0B00000000);  send_char_without(0B00000000);  // 104:97   -112:105// 
          send_char_without(0B00000000);    //  24:17    -32:25//
           //the next switch send reamaining bits -41:48// 
              switch (i){ //Last 3 bits is "DD" data direction, and is used
                case 0: send_char_without(0B00000000); break;
                case 1: send_char_without(0B10000000); break;
                case 2: send_char_without(0B01000000); break;
                case 3: send_char_without(0B11000000); break;
                case 4: send_char_without(0B00100000); break;
              }
      // to mark the 4 groups of 36 bits, 00, 01, 10, 11.
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); // 
      }
      controle();
}
void searchOfSegments(){
// put your main code here, to run repeatedly:
  int group = 0x00;
  byte nBit =0x00;
  byte nMask = 0b00000001;
  unsigned int block =0;
  unsigned int nSeg=0x000;  // This variable need support more of 255 (600 segments)
  Serial.println();
  Serial.println("We start the test of segments!");
  for(block=0; block<5; block++){  //This is the last 2 bit's marked as DD, group: 0x00, 0x01, 0x10, 0x11;
for( group=0; group<15; group++){   // Do until n bits 15*8 bits
        //for(int nBit=0; nBit<8; nBit++){
          for (nMask = 0b00000001; nMask>0; nMask <<= 1){
            Aa=0x00; Ab=0x00; Ac=0x00; Ad=0x00; Ae=0x00; Af=0x00; Ag=0x00; Ah=0x00; Ai=0x00; Aj=0x00;Ak=0x00; Al=0x00; Am=0x00; An=0x00; Ao=0x00;
                  switch (group){
                    case 0: Aa=nMask; break;
                    case 1: Ab=nMask; break;//atoi(to integer)
                    case 2: Ac=nMask; break;
                    case 3: Ad=nMask; break;
                    case 4: Ae=nMask; break;
                    case 5: Af=nMask; break;
                    case 6: Ag=nMask; break;//atoi(to integer)
                    case 7: Ah=nMask; break;
                    case 8: Ai=nMask; break;
                    case 9: Aj=nMask; break;
                    case 10: Ak=nMask; break;
                    case 11: Al=nMask; break;//atoi(to integer)
                    case 12: Am=nMask; break;
                    case 13: An=nMask; break;
                    case 14: Ao=nMask; break;
                  }
            
           nSeg++;
           if((nSeg >=0) && (nSeg<121)){
            blockBit=0;
            }
            if((nSeg >=121) && (nSeg<241)){
            blockBit=1;
            }
            if((nSeg >=241) && (nSeg<361)){
            blockBit=2;
            }
            if((nSeg >=361) && (nSeg<481)){
            blockBit=3;
            }
            if((nSeg >=481) && (nSeg<601)){
            blockBit=4;
            }
            if (nSeg >=601){
              nSeg=0;
              group=0;
              block=0;
              break;
            }
          
      //This start the control of button to allow continue teste! 
                      while(1){
                            if(!buttonReleased){
                              delay(200);
                            }
                            else{
                              delay(15);
                               buttonReleased = false;
                               break;
                               }
                         }
               delay(50);
                     segments();
            Serial.print(nSeg, DEC); Serial.print(", group: "); Serial.print(group, DEC);Serial.print(", BlockBit: "); Serial.print(blockBit, HEX);Serial.print(", nMask: "); Serial.print(nMask, BIN);Serial.print("   \t");
            Serial.print(Ae, HEX);Serial.print(", ");Serial.print(Ad, HEX);Serial.print(", ");Serial.print(Ac, HEX);Serial.print(", ");Serial.print(Ab, HEX);Serial.print(", ");Serial.print(Aa, HEX); Serial.print("; ");
            
            Serial.println();
            delay (250);  
                }         
           }        
      }
  }
void segments(){
//Bit function: 
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // 
          send_char_without(Aa);  send_char_without(Ab);  //   8:1     -16:9// 
          send_char_without(Ac);  send_char_without(Ad);  //  24:17    -32:25//
          send_char_without(Ae);  send_char_without(Af);  //  40:33    -48:41// 
          send_char_without(Ag);  send_char_without(Ah);  //  56:49    -64:57// 
          send_char_without(Ai);  send_char_without(Aj);  //  72:65    -80:73// 
          send_char_without(Ak);  send_char_without(Al);  //  88:81    -96:89//  
          send_char_without(Am);  send_char_without(An);  //  104:97   -112:105//  
          send_char_without(Ao);  //  120:113   
          //The next switch finalize the burst of bits -41:48//  
              switch (blockBit){ //Last 2 bits is "DD" data direction, and is used to mark the 4 groups of 36 bits, 00, 01, 10, 11.                                 
                case 0: send_char_without(0B00000000); break; //Block 00
                case 1: send_char_without(0B10000000); break; //Block 01
                case 2: send_char_without(0B01000000); break; //Block 10
                case 3: send_char_without(0B11000000); break; //Block 11
                case 4: send_char_without(0B00100000); break; //Block 11
              }
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); //                   
}
//
void msg1_120(){
//Bit function:pc1, pc2, pc3, pc4, ct0, ct1, ct2, ct3, CTC, SC, BU, DT1, DT2, DD, DD, DD  this is the format of 2 bytes of controle 
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //   8:1     -16:9// 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  24:17    -32:25//
          send_char_without(0b01111111);  send_char_without(0b00001000);  //  40:33    -48:41// 
          send_char_without(0b00001000);  send_char_without(0b01111111);  //  56:49    -64:57// 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  72:65    -80:73// 
          send_char_without(0b01000001);  send_char_without(0b01111111);  //  88:81    -96:89// 
          send_char_without(0b01000001);  send_char_without(0b00000000);  // 104:97   -112:105//  
          send_char_without(0b00000000);  //  120:113    
                                            
              send_char_without(0B00000000); //Block 1º
                
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); //                   
}
void msg121_240(){
//Bit function: pc1, pc2, pc3, pc4, ct0, ct1, ct2, ct3, CTC, SC, BU, DT1, DT2, DD, DD, DD  this is the format of 2 bytes of controle
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  128:121    -136:129// 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  144:137    -152:145//
          send_char_without(0b01111111);  send_char_without(0b00001001);  //  160:153    -168:161// 
          send_char_without(0b00001001);  send_char_without(0b00000001);  //  176:169    -184:177// 
          send_char_without(0b00000000);  send_char_without(0b01111111);  //  192:185    -200:193// 
          send_char_without(0b01000001);  send_char_without(0b01000001);  //  208:201    -216:209// 
          send_char_without(0b01111111);  send_char_without(0b00000000);  //  224:217    -232:225//  
          send_char_without(0b01111111);  //  240:233    
              
               send_char_without(0B10000000);//Block 2º
               
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); //                   
}
void msg241_360(){
//Bit function: pc1, pc2, pc3, pc4, ct0, ct1, ct2, ct3, CTC, SC, BU, DT1, DT2, DD, DD, DD  this is the format of 2 bytes of controle
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // 
          send_char_without(0b01000000);  send_char_without(0b01000000);  //  248:241    -256:249// 
          send_char_without(0b01000000);  send_char_without(0b00000000);  //  264:257    -272:265//
          send_char_without(0b01111111);  send_char_without(0b00001000);  //  280:273    -288:281// 
          send_char_without(0b00010100);  send_char_without(0b01100011);  //  296:289    -304:297// 
          send_char_without(0b00000000);  send_char_without(0b01001111);  //  312:305    -320:313// 
          send_char_without(0b01001001);  send_char_without(0b01001001);  //  328:321    -336:329// 
          send_char_without(0b01111001);  send_char_without(0b00000000);  //  344:337    -352:345//  
          send_char_without(0b00000000);  //  360:353    
             
                send_char_without(0B01000000); //Block 3º
               
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); //                   
}
void msg361_480(){
//Bit function: pc1, pc2, pc3, pc4, ct0, ct1, ct2, ct3, CTC, SC, BU, DT1, DT2, DD, DD, DD  this is the format of 2 bytes of controle
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  368:361  -376:369// 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  384:377  -392:385//
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  400:393  -408:401// 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  416:409  -424:417// 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  432:425  -440:433// 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  448:441  -456:449// 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  464:457  -472:465//  
          send_char_without(0b00000000);  //  480:473    
              
                send_char_without(0B11000000); //Block 4º
               
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); //                   
}
void msg481_600(){
//Bit function: pc1, pc2, pc3, pc4, ct0, ct1, ct2, ct3, CTC, SC, BU, DT1, DT2, DD, DD, DD  this is the format of 2 bytes of controle
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  481:488  -489:496// 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  497:504    -505:512//
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  513:520    -521:528// 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  529:536    -537:544// 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  545:552    -553:560// 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  561:568    -569:576// 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  577:584    -585:592//  
          send_char_without(0b00000000);  //  600:593      
             
                send_char_without(0B00100000);  //Block 5º
            
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); //                   
}
//
void msgSymb1(){
//Bit function:pc1, pc2, pc3, pc4, ct0, ct1, ct2, ct3, CTC, SC, BU, DT1, DT2, DD, DD, DD  this is the format of 2 bytes of controle 
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // 
          send_char_without(0b11111111);  send_char_without(0b11111111);  //   8:1     -16:9// 
          send_char_without(0b11111111);  send_char_without(0b11111111);  //  24:17    -32:25//
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  40:33    -48:41// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  56:49    -64:57// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  72:65    -80:73// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  88:81    -96:89// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  // 104:97   -112:105//  
          send_char_without(0b10000000);  //  120:113    
                                            
              send_char_without(0B00000000); //Block 1º
                
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); //                   
}
void msgSymb2(){
//Bit function: pc1, pc2, pc3, pc4, ct0, ct1, ct2, ct3, CTC, SC, BU, DT1, DT2, DD, DD, DD  this is the format of 2 bytes of controle
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  128:121    -136:129// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  144:137    -152:145//
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  160:153    -168:161// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  176:169    -184:177// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  192:185    -200:193// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  208:201    -216:209// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  224:217    -232:225//  
          send_char_without(0b10000000);  //  240:233    
              
               send_char_without(0B10000000);//Block 2º
               
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); //                   
}
void msgSymb3(){
//Bit function: pc1, pc2, pc3, pc4, ct0, ct1, ct2, ct3, CTC, SC, BU, DT1, DT2, DD, DD, DD  this is the format of 2 bytes of controle
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  248:241    -256:249// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  264:257    -272:265//
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  280:273    -288:281// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  296:289    -304:297// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  312:305    -320:313// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  328:321    -336:329// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  344:337    -352:345//  
          send_char_without(0b10000000);  //  360:353    
             
                send_char_without(0B01000000); //Block 3º
               
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); //                   
}
void msgSymb4(){
//Bit function: pc1, pc2, pc3, pc4, ct0, ct1, ct2, ct3, CTC, SC, BU, DT1, DT2, DD, DD, DD  this is the format of 2 bytes of controle
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  368:361  -376:369// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  384:377  -392:385//
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  400:393  -408:401// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  416:409  -424:417// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  432:425  -440:433// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  448:441  -456:449// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  464:457  -472:465//  
          send_char_without(0b10000000);  //  480:473    
              
                send_char_without(0B11000000); //Block 4º
               
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); //                   
}
void msgSymb5(){
//Bit function: pc1, pc2, pc3, pc4, ct0, ct1, ct2, ct3, CTC, SC, BU, DT1, DT2, DD, DD, DD  this is the format of 2 bytes of controle
      digitalWrite(LCD_CE, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01001011); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  488:481    -496:489// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  504:497    -512:505//
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  520:513    -528:521// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  536:529    -544:537// 
          send_char_without(0b10000000);  send_char_without(0b10000000);  //  552:545    -560:553// 
          send_char_without(0b10000000);  send_char_without(0b00000000);  //  568:561    -576:569// 
          send_char_without(0b00000000);  send_char_without(0b00000000);  //  584:577    -592:585//  
          send_char_without(0b00000000);  //  600:593      
             
                send_char_without(0B00100000);  //Block 5º
            
      delayMicroseconds(1);
      digitalWrite(LCD_CE, LOW); //                   
}
//
void loop() {
long randNumber;
//buttonState = digitalRead(buttonPin);
//read the state of the pushbutton value:
//buttonState = digitalRead(buttonPin);
LED();
delay(1000);
allOFF(); // All off
msgSymb1();
msgSymb2();
msgSymb3();
msgSymb4();
msgSymb5();
delay(3000);
while(1){
          for(int s=0; s<5; s++){
              allON(); // All on
              delay(2000);
              allOFF(); // All off
              delay(1000);
              msg1_120();
              msg121_240();
              msg241_360();
              msg361_480();
              msg481_600();
              delay(3000);
          }
    //Uncomment this two lines to proceed identification of segments on this driver... adapt to other if necessary!
    //Please don't forget of activation of Serial Monitor of IDE Arduino, to allow printing of running correctley,other way will block!
    allOFF();
    searchOfSegments(); //Uncomment this line if you want run the find segments
    }    
}
//
void buttonReleasedInterrupt() {
  buttonReleased = true; // This is the line of interrupt button to advance one step on the search of segments!
}
