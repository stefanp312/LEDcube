
//THE CONSTANT SIZE OF THE CUBE
const int CUBE_SIZE = 4;


//The 3D array that is mapped to the cube
bool dataSource[CUBE_SIZE][CUBE_SIZE][CUBE_SIZE];

//Stupid variable that counts
int counter;

//millis timer
volatile long unsigned int millis_timer;

//Pin connected to ST_CP of 74HC595
const int latchPin = 8;
//Pin connected to SH_CP of 74HC595
const int clockPin = 12;
////Pin connected to DS of 74HC595
const int dataPin = 11;

//ground for layer one and ground for layer 2
const int ground1 = 2;
const int ground2 = 3;
const int ground3 = 4;
const int ground4 = 5;

//holders for infromation you're going to pass to shifting function
byte data1 = 0;
byte data2 = 0;

//Needs to store a 16 bit number
unsigned int ledAddress[CUBE_SIZE][CUBE_SIZE];
int layerAddress[CUBE_SIZE];

unsigned int timerStuff=0;

void setup() {
  
  //Serial monitor for debug
  Serial.begin(9600);

  millis_timer = 0;
  
  noInterrupts();           // disable all interrupts
  TCCR2B = 0;
  TCCR2A = 0;
  
  //Set up the timer being used to control the patterns on cube
  TCCR2B |= (_BV(CS22)); //Set Timer2 prescaling factor to 64
  TCCR2B &= ~(_BV(CS20) | _BV(CS21));
  TIMSK2 |= (_BV(TOIE2));   // enable timer2 overflow interrupt
  interrupts();             // enable all interrupts
     
  //makes the array we are using to address the bits.
  makeData();
   
  //The latch pin used to update the shift registers
  pinMode(latchPin, OUTPUT);
  //The clock pin pulses when new data is fed to the shift register
  pinMode(clockPin, OUTPUT);
  //This is the pin that data is fed to the shift regsisters
  pinMode(dataPin, OUTPUT);

  //These are all the pins being used to control the ground to control layers
  pinMode(ground1, OUTPUT);
  pinMode(ground2, OUTPUT);
  pinMode(ground3, OUTPUT);
  pinMode(ground4, OUTPUT);
  
  //This initializes the data source to zeros
  writeValueToEntireDataSource(0);
  //This updates the data source
  updateCubeWithDataSource();
  
  //initiliazes the counter variable for seeding things temporaly
  counter = 0;
}

void loop() {
  //update the function being dispayed by the cube
  //update the cube to be synced to the datasource
  applyPattern(0);
  updateCubeWithDataSource();
}


//This goes through every value in the matrix and writes a value.
void writeValueToEntireDataSource(bool value) {
  for (int i = 0; i < CUBE_SIZE; i++) {
    for (int j = 0; j < CUBE_SIZE; j++) {
      for (int k = 0; k < CUBE_SIZE; k++) {
        dataSource[i][j][k] = value;
      }
    }
  }
}

//This prints one layer of the cube to the serial monitor
void printDataLayer(int layer) {
  for (int i = 0; i < CUBE_SIZE; i++) {
    for (int j = 0; j < CUBE_SIZE; j++) {
      Serial.print(dataSource[i][j][layer]);
    }
    Serial.println("");
  }
}

//This will be main pattern selection method
void applyPattern(int pattern) {
  switch (pattern) {
    case 0:
      powerUp();
      break;
    case 1:
      testPattern();
      break;
    default:
      break;
  }
}

//This makes a square from the two ranges passed in on the layer provided with the value passed in
void makeSquareOnDataSourceLayer(int x1, int x2, int y1, int y2, int z, int value){
  for (int i = x1; i < x2; i++) {
    for (int j = y1; j < y2; j++) {
      dataSource[i][j][z] = value;
    }    
  }
}
void functionALL() {
  writeValueToEntireDataSource(1);
}

//This function is responsible for changing the state of the current animation that is being displayed on the screen
//It does this by determining if a set amount of time has occured and incrementing a global counter variable
void stateChange(int timeInMilliSeconds){
  
  if (Millis() >= timeInMilliSeconds){ 
    //increment the state variable

    counter++;
    millis_timer = 0;
    
  }
}

//Unoptimized pattern
void powerUp() {
  //This changes the state every 300 milliseconds AKA inc counter every 300 milliseconds

  stateChange(500);
 
  if (counter==0){
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0,CUBE_SIZE,0,CUBE_SIZE,0,1);
  }
  if (counter==1){
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0,CUBE_SIZE,0,CUBE_SIZE,1,1);
  }
  if (counter==2){
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0,CUBE_SIZE,0,CUBE_SIZE,2,1);
  }
  if (counter==3){
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0,CUBE_SIZE,0,CUBE_SIZE,3,1);
  }
  if (counter==4){
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0,CUBE_SIZE,0,CUBE_SIZE,2,1);
  }
  if (counter==5){
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0,CUBE_SIZE,0,CUBE_SIZE,1,1);
  }
  if (counter==6){
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0,CUBE_SIZE,0,CUBE_SIZE,0,1);
    
    //In your last state reset that state counter
    counter = 0;
  }  
}

void testPattern() {

    dataSource[0][0][0]=1;
    dataSource[1][0][1]=1;
    dataSource[2][0][2]=1;
    dataSource[3][0][3]=1;
    
    dataSource[0][0][3]=1;
    dataSource[1][0][2]=1;
    dataSource[2][0][1]=1;
    dataSource[3][0][0]=1;
}

void raiseAllGrounds(){
  for (int var = 0; var < CUBE_SIZE; var++) {
    digitalWrite(layerAddress[var], 1);
  }
}
//This function write data to a chosen layer of the cube
void writeLayerToCube(int layer) {
  //First we need to drop the latch and clock low
  //Dropping the latch low makes the shift register take in data
  //Putting the clock low makes sure that no data is accidentally written to the register.
  digitalWrite(latchPin, 0);
  digitalWrite(clockPin, 0);
  
  //Write a face of data to the cube
  shiftOut(dataPin, clockPin, MSBFIRST, data2);
  shiftOut(dataPin, clockPin, MSBFIRST, data1);
  //Close the latch so it can update
  digitalWrite(latchPin, 1);
  
  //Next we need to select the correct layer by picking the right ground
  //Match the layer value with the correct pin stored in the layerAddress array
  if (data1!=0 || data2!=0){
    raiseAllGrounds();
    digitalWrite(layerAddress[layer], 0);
  }
  //Clear the data
  data1 = 0;
  data2 = 0;
}

//This is the main function that ensures that the LED cube is kept updated with the datasource
void updateCubeWithDataSource() {
  for (int k = 0; k < CUBE_SIZE; k++) {
    for (int i = 0; i < CUBE_SIZE; i++) {
      for (int j = 0; j < CUBE_SIZE; j++) {
        //This loop will go through every value of the dataSource
        //Check if the value needs to be written to
        if (dataSource[i][j][k] == 1) {
          //If it needs to be written to the cube add it to the data
          //We can add it to the data because each LED is mapped to a power of 2 and we are essentially doing an 'or' operation on what to light up
          //The check to j is to decicide which byte to write to
          if (j<2){
            data1 = data1 + ledAddress[i][j];
          }else{
            data2 = data2 + ledAddress[i][j];
          }
        }
      }
    }
    //At this point we have gone through one entire layer and can write that layer to the cube
    writeLayerToCube(k);
  }
}

///Helper functions
void makeData() {
  
  //First Byte
  ledAddress[0][0] = 1;
  ledAddress[1][0] = 4; //Wiring mistake
  ledAddress[2][0] = 2; //These are flipped in wires
  ledAddress[3][0] = 8;
  ledAddress[0][1] = 128;
  ledAddress[1][1] = 64;
  ledAddress[2][1] = 32;
  ledAddress[3][1] = 16;
  
  //Second Byte
  ledAddress[0][2] = 8;
  ledAddress[1][2] = 4;
  ledAddress[2][2] = 2;
  ledAddress[3][2] = 1;
  ledAddress[0][3] = 128;
  ledAddress[1][3] = 64;
  ledAddress[2][3] = 32;
  ledAddress[3][3] = 16;
  
  //Set up the ground array to correct pins only hard coding needed
  layerAddress[0] = 2;
  layerAddress[1] = 3;
  layerAddress[2] = 4;
  layerAddress[3] = 5;

}

ISR(TIMER2_OVF_vect)    
{
  millis_timer ++;
  
}
unsigned int Millis()
{
  int m = millis_timer;
  return m;
}


