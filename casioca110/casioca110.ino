// Pin Definitions
// Rows are connected to

int fsr1 = 7;   // Colonna 1 FSR
int fsr2 = 6;   // Colonna 2 FSR
int fsr3 = 12;  // Colonna 3 FSR
  
const int s0 = 4; 
const int s1 = 3; 
const int s2 = 2; 

const int Clock = 8; // ShiftOut
const int latch = 9; // ShiftOut
const int data = 11; // ShiftOut
int octave = 1;

void velodown(){
  if(octave == 2){   octave = octave - 1;  
  }   
}
void veloup(){  
  if(octave == 1){      
    octave = octave + 1;   
  }   
}

void scanColumn(int value) {
  digitalWrite(latch, LOW); //Pulls the chips latch low
  shiftOut(data, Clock, MSBFIRST, value); //Shifts out the 8 bits to the shift register
  digitalWrite(latch, HIGH); //Pulls the latch high displaying the data
}


boolean stan1 = false;
boolean stan2 = false;

int r0 = 0;      //value of select pin at the 4051 (s0)
int r1 = 0;      //value of select pin at the 4051 (s1)
int r2 = 0;      //value of select pin at the 4051 (s2)

int bits[] = { B00000001, B00000010, B00000100, B00001000, B00010000, B00100000, B01000000, B10000000 };

int keyToMIDI0[8][6] = {
{37,38,36,39,41,40},
{55,56,54,57,59,58},
{49,50,48,51,53,52},
{43,44,42,45,47,46},
{31,32,30,33,35,34},
{25,26,24,27,29,28},
{19,20,18,21,23,22},
{13,14,12,15,17,16},
};

boolean keyPressed[8][6] = {
{false,false,false,false,false,false},
{false,false,false,false,false,false},
{false,false,false,false,false,false},
{false,false,false,false,false,false},
{false,false,false,false,false,false},
{false,false,false,false,false,false},
{false,false,false,false,false,false},
{false,false,false,false,false,false},  
};

boolean FSRKeys[3][2] = {
{false,false},
{false,false},
{false,false},
};

int FSRMidi[3][2]{
{0,1}, 
{250,252},
{2,3},
};

void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}

void setup() {
  
pinMode(s0, OUTPUT);    // s0 multiplexer pinout
pinMode(s1, OUTPUT);    // s1 multiplexer pinout
pinMode(s2, OUTPUT);    // s2 multiplexer pinout

pinMode(data, OUTPUT);    // ShiftOut data pinout
pinMode(Clock, OUTPUT);    // ShiftOut clock pinout
pinMode(latch, OUTPUT);    // ShiftOut latch pinout

pinMode(fsr1, OUTPUT);    // Colonna 1 FSR
pinMode(fsr2, OUTPUT);    // Colonna 2 FSR
pinMode(fsr3, OUTPUT);    // Colonna 3 FSR
  
Serial.begin(9600);

}


void loop() {
  FSRMatrix(); 
  for (int col = 0; col < 8; col++) {
    scanColumn(bits[col]);
    int rows[6] = {0,1,3,5,6,7};
    for (int row = 0; row < 6; row++) {
      int real_row = rows[row];
      r0 = bitRead(real_row,0);      
      r1 = bitRead(real_row,1);  
      r2 = bitRead(real_row,2);  
      digitalWrite(s0, r0);
      digitalWrite(s1, r1);
      digitalWrite(s2, r2);
      int value = analogRead(A0);
      if(value > 300){
        if(!keyPressed[col][row] ){
          if(octave == 2){
            noteOn(144,keyToMIDI0[col][row] + 48,127);
          }else{
            noteOn(144,keyToMIDI0[col][row],127);
          }
          keyPressed[col][row] = true;
        }else{
          if(keyPressed[col][row]){   
            if(octave == 2){
              noteOn(128,keyToMIDI0[col][row] + 48,127);
            }else{
              noteOn(128,keyToMIDI0[col][row],127);
            }
            keyPressed[col][row] = false;
          }
        }
      }    
    } 
  }  
}

void FSRMatrix() {
  int cols[3] = {7,6,12};
  int rows[2] = {1,2};
  for(int i=0;i < 3;i++){
    digitalWrite(cols[i], HIGH);
    for(int j = 0;j < 2;j++){
      int value = analogRead(rows[j]);
      if(value > 300){
        if(!FSRKeys[i][j] ){
          if(i==2&&j==0){  
            velodown();  
          } else if(i==2&&j==1){ 
            veloup(); 
          } else{
            Serial.write(FSRMidi[i][j]);
          }
          FSRKeys[i][j] = true;
        }                   
      } else{
        if(FSRKeys[i][j]){   
          FSRKeys[i][j] = false;
        }
      }
    }
    digitalWrite(cols[i], LOW);
  }
 
  
}





