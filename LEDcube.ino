
//THE CONSTANT SIZE OF THE CUBE
const int CUBE_SIZE_X = 4;
const int CUBE_SIZE_Y = 4;
const int CUBE_SIZE_Z = 4;

//The 3D array that is mapped to the cube
bool dataSource[CUBE_SIZE_X][CUBE_SIZE_Y][CUBE_SIZE_Z];

//Stupid variable that counts
int counter;

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

const int tester = 6;


//holder for infromation you're going to pass to shifting function
word data = 0;

//The constants used to map the array to the cube
const int sizeOfCubeFace = 16;
const int numberOfLayers = 4;

//Needs to store a 16 bit number
unsigned int ledAddress[sizeOfCubeFace];
int layerAddress[numberOfLayers];



void setup() {
  //Set up the timer being used to control the patterns on cube
  TCCR0B |= (_BV(CS01)| _BV(CS00)); //Set Timer0 prescaling factor
  TCCR0B &= ~(_BV(CS02));
  
  //Serial monitor for debug
  Serial.begin(9600);
  //This initializes the data source to zeros
  writeValueToEntireDataSource(0);
  //This updates the data source
  updateCubeWithDataSource();
  
  //initiliazes the counter variable for seeding things temporaly
  counter = 0;
  
  //makes the array we are using to address the bits.
  makeData();
  
  //The latch pin used to update the shift registers
  pinMode(latchPin, OUTPUT);
  //The clock pin pulses when new data is fed to the shift register
  pinMode(clockPin, OUTPUT);
  //This is the pin that data is fed to the shift regsisters
  pinMode(dataPin, OUTPUT);

  pinMode(tester,INPUT);

  //These are all the pins being used to control the ground to control layers
  pinMode(ground1, OUTPUT);
  pinMode(ground2, OUTPUT);
  pinMode(ground3, OUTPUT);
  pinMode(ground4, OUTPUT);

}

void loop() {
  //update the function being dispayed by the cube
  //update the cube to be synced to the datasource

  applyPattern(0);
  updateCubeWithDataSource();
  counter++;
  if (counter > 1000) {
    counter = 0;
  }
}


//This goes through every value in the matrix and writes a value.
void writeValueToEntireDataSource(int value) {
  for (int i = 0; i < CUBE_SIZE_X; i++) {
    for (int j = 0; j < CUBE_SIZE_Y; j++) {
      for (int k = 0; k < CUBE_SIZE_Z; k++) {
        dataSource[i][j][k] = value;
      }
    }
  }
}

//This prints one layer of the cube to the serial monitor
void printDataLayer(int layer) {
  for (int i = 0; i < CUBE_SIZE_X; i++) {
    for (int j = 0; j < CUBE_SIZE_Y; j++) {
      Serial.print(dataSource[i][j][layer]);
    }
    Serial.println("");
  }
}

//This will be main pattern selection method
void applyPattern(int pattern) {
  switch (pattern) {
    case 0:
      //functionOne();
      //functionTwo();
      powerUp();
      break;
    case 1:
      simpleBounce();
      break;
    default:
      break;
  }
}

void makeSquareOnDataSourceLayer(int x1, int x2, int y1, int y2, int z){
  for (int i = x1; i < x2; i++) {
    for (int j = y1; j < y2; j++) {
      dataSource[i][j][z] = 1;
    }    
  }
}
void functionALL() {
  writeValueToEntireDataSource(1);
}
void powerUp() {
  if (counter==0){
    writeValueToEntireDataSource(1);
  }
  if (counter==500){
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0,CUBE_SIZE_X,0,CUBE_SIZE_Y,2);
    makeSquareOnDataSourceLayer(0,CUBE_SIZE_X,0,CUBE_SIZE_Y,1);
        makeSquareOnDataSourceLayer(0,CUBE_SIZE_X,0,CUBE_SIZE_Y,0);
  }
  
}
void functionOne() {
  if (counter==0){
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0,CUBE_SIZE_X,0,CUBE_SIZE_Y,0);
  }
  if (counter==500){
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0,CUBE_SIZE_X,0,CUBE_SIZE_Y,1);
  }
}
void functionTwo() {
  if (counter==0){
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0,CUBE_SIZE_X,0,1,0);
  }
  if (counter==300){
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0,CUBE_SIZE_X,0,1,1);
  }
  if (counter==600){
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0,1,0,CUBE_SIZE_X,1);
  }
  if (counter==900){
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0,1,0,CUBE_SIZE_X,0);
  }
}

void simpleBounce() {
    //
    if (counter==0){
      writeValueToEntireDataSource(0);
      dataSource[0][0][0] = 1;
      dataSource[1][0][1] = 1;  
    }
    if (counter==300){
      writeValueToEntireDataSource(0);
      dataSource[1][0][0] = 1;
      dataSource[0][1][1] = 1;  
    }
    if (counter==500){
      writeValueToEntireDataSource(0);
      dataSource[0][1][0] = 1;
      dataSource[1][1][1] = 1;  
    }
    if (counter==700){
      writeValueToEntireDataSource(0);
      dataSource[1][1][0] = 1;
      dataSource[0][0][1] = 1;  
    }
    
}

void testPattern() {

}

void raiseAllGrounds(){
  for (int var = 0; var < numberOfLayers; var++) {
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
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  //Close the latch so it can update
  digitalWrite(latchPin, 1);
  
  //Next we need to select the correct layer by picking the write ground
  //Match the layer value with the correct pin stored in the layerAddress array
  if (data!=0){
    raiseAllGrounds();
    digitalWrite(layerAddress[layer], 0);
  }
  //Clear the data
  Serial.println(digitalRead(tester));
  data = 0;
}

void updateCubeWithDataSource() {
  int countForMe = 0;
  for (int k = 0; k < CUBE_SIZE_Z; k++) {
    for (int i = 0; i < CUBE_SIZE_X; i++) {
      for (int j = 0; j < CUBE_SIZE_Y; j++) {
        //This loop will go through every value of the dataSource
        //Check if the value needs to be written to
        if (dataSource[j][i][k] == 1) {
          //If it needs to be written to the cube add it to the data
          //We can add it to the data because each LED is mapped to a power of 2 and we are essentially doing an 'or' operation on what to light up
          data = data + ledAddress[countForMe];
          //Serial.println("This data got added");
          //Serial.print(ledAddress[countForMe]);
        }
        countForMe++;
      }
    }
    countForMe=0;
    //At this point we have gone through one entire layer and can write that layer to the cube
    writeLayerToCube(k);
    //Serial.println("_________");
    //printDataLayer(k);
  }
}

///Helper functions
void makeData() {
  for (int i = 0; i < sizeOfCubeFace; i++) {
    ledAddress[i] = 1 << i;
  }
  //Set up the ground array to correct pins only hard coding needed
  layerAddress[0] = 2;
  layerAddress[1] = 3;
  layerAddress[2] = 4;
  layerAddress[3] = 5;

}

