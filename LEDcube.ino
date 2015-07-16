
//THE CONSTANT SIZE OF THE CUBE
const int CUBE_SIZE_X = 2;
const int CUBE_SIZE_Y = 2;
const int CUBE_SIZE_Z = 2;

//The 3D array that is mapped to the cube
int dataSource[CUBE_SIZE_X][CUBE_SIZE_Y][CUBE_SIZE_Z];

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


//holder for infromation you're going to pass to shifting function
word data = 0;

//The constants used to map the array to the cube
const int sizeOfCubeFace = 4;
const int numberOfLayers = 2;
//Needs to store a 16 bit number
unsigned int ledAddress[sizeOfCubeFace];
int layerAddress[numberOfLayers];

void setup() {
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

  //These are all the pins being used to control the ground to control layers
  pinMode(ground1, OUTPUT);
  pinMode(ground2, OUTPUT);

}

void loop() {
  //update the function being dispayed by the cube
  //update the cube to be synced to the datasource

  applyPattern(1);
  updateCubeWithDataSource();
  counter++;
  if (counter > 1) {
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
      testPattern();
      break;
    case 1:
      simpleBounce();
      break;
    default:
      break;
  }
}

void simpleBounce() {
  writeValueToEntireDataSource(0);
  if (counter == 0) {
    dataSource[0][0][0] = 1;
    dataSource[0][0][1] = 1;
  } else if (counter == 1) {
    dataSource[1][0][0] = 1;
    dataSource[1][0][1] = 1;
  }
}

void lightAllCols() {
  writeValueToEntireDataSource(0);
  if (counter == 0) {
    dataSource[0][0][0] = 1;
    dataSource[0][1][0] = 1;
  }
  if (counter == 1) {
    dataSource[1][0][0] = 1;
    dataSource[1][1][0] = 1;
  }
  if (counter == 2) {
    dataSource[1][0][1] = 1;
    dataSource[1][1][1] = 1;
  }
  if (counter == 3) {
    dataSource[1][0][1] = 1;
    dataSource[1][1][1] = 1;
  }
}

void testPattern() {
  //use the timer to provide a value to calculate stuff with
  writeLayerToCube(counter);
  data = 1 + 4 + 8;
  delay(200);
}

//This function write data to a chosen layer of the cube
void writeLayerToCube(int layer) {
  //First we need to drop the latch and clock low
  //Dropping the latch low makes the shift register take in data
  //Putting the clock low makes sure that no data is accidentally written to the register.
  digitalWrite(latchPin, 0);
  digitalWrite(clockPin, 0);
  
  //Next we need to select the correct layer by picking the write ground
  //Match the layer value with the correct pin stored in the layerAddress array
  for (int var = 0; var < numberOfLayers; var++) {
    if (var == layer) {
      digitalWrite(layerAddress[var], 0);
    }
    else {
      digitalWrite(layerAddress[var], 1);
    }
  }
  
  //Write a face of data to the cube
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  //Close the latch so it can update
  digitalWrite(latchPin, 1);
  //Clear the data
  data = 0;
}


//basically this needs to happen
/*
  we have cube which we read from. and set lights on or off based on those values
  we modify this cube with math.
  we update the led cube from the matrix cube.
  We needs to amake sure that the cube is saynced to the data source in order to minimize the flickering of the lights when everything is being typed and use an extrernal timer in order to do the pattern
  because using the delay function in the mian program loop stops the updating and if we are no updating constanly it looks like on of them turns off.
*/

void updateCubeWithDataSource() {
  for (int k = 0; k < CUBE_SIZE_Z; k++) {
    for (int i = 0; i < CUBE_SIZE_X; i++) {
      for (int j = 0; j < CUBE_SIZE_Y; j++) {
        //This loop will go through every value of the dataSource
        //Check if the value needs to be written to
        if (dataSource[i][j][k] == 1) {
          //If it needs to be written to the cube add it to the data
          //We can add it to the data because each LED is mapped to a power of 2 and we are essentially doing an 'or' operation on what to light up
          data = data + ledAddress[i];
        }
      }
    }
    //At this point we have gone through one entire layer and can write that layer to the cube
    writeLayerToCube(k);
    Serial.println("_________");
    printDataLayer(k);
  }

}

///Helper functions
void makeData() {
  for (int i = 0; i < sizeOfCubeFace; i++) {
    ledAddress[i] = 1 << i;
  }
  //Set up the ground array to correct pins only hard coding needed
  layerAddress[0] = 3;
  layerAddress[1] = 2;

}

