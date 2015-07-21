//THE CONSTANT SIZE OF THE CUBE
const int CUBE_SIZE = 4;

//The 3D array that is mapped to the cube
bool dataSource[CUBE_SIZE][CUBE_SIZE][CUBE_SIZE];

//This value is used for a pattern
bool writeValue;
int fakeTime;

//Variables used to track states
volatile int counter;
volatile int count;
//Millis timer
volatile long unsigned int millis_timer;

//Pin connected to ST_CP of 74HC595
const int latchPin = 8;
//Pin connected to SH_CP of 74HC595
const int clockPin = 12;
//Pin connected to DS of 74HC595
const int dataPin = 11;

//Constants for ground
const int ground1 = 3;
const int ground2 = 4;
const int ground3 = 5;
const int ground4 = 6;

//Holders for infromation you're going to pass to shifting function
//Need two bytes because two shift registers exist
byte data1 = 0;
byte data2 = 0;

//What we use to map the cube to a matrix
int ledAddress[CUBE_SIZE][CUBE_SIZE];
int layerAddress[CUBE_SIZE];


void setup() {

  //Sets the initial pattern state
  count = 0;
  //Used to seed the random function
  randomSeed(analogRead(0));
  //Setting up custom millis
  millis_timer = 0;
  //Disable all interupts while writing to registers
  noInterrupts();
  TCCR2B = 0;
  TCCR2A = 0;

  //Set up the timer being used to control the patterns on cube
  TCCR2B |= (_BV(CS22)); //Set Timer2 prescaling factor to 64
  TCCR2B &= ~(_BV(CS20) | _BV(CS21));
  TIMSK2 |= (_BV(TOIE2));   // Enable timer2 overflow interrupt
  interrupts();             // Enable all interrupts again

  //Makes the array we are using to address the bits.
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
  //Used to provide power to the button
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  //Interupt to handle button presses
  attachInterrupt(0, event_Handler, RISING);
  //This initializes the data source to zeros
  writeValueToEntireDataSource(0);
  //This updates the data source
  updateCubeWithDataSource();

  //Set initial pattern state
  counter = 0;

  //Value for a pattern
  writeValue = 1;
  fakeTime=0;
}

void loop() {
  //update the function being dispayed by the cube
  //update the cube to be synced to the datasource
  //Serial.println(count);
  applyPattern(count);

  updateCubeWithDataSource();
}

/*-----------------------------------------------------------------------------------------------------/
--Functions that are used to modify the matrix directly------------------------------------------------/
-------------------------------------------------------------------------------------------------------/
-------------------------------------------------------------------------------------------------------/
------------------------------------------------------------------------------------------------------*/

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

//This is the main pattern selection method
void applyPattern(int pattern) {

  switch (pattern) {
    case 0:
      powerUp();
      break;
    case 1:
      RandomLights();
      break;
    case 2:
      rotatingRectangles();
      break;
    case 3:
      lineSweep();
      break;
    case 4:
      testPattern();
      break;
    case 5:
      lightCubeFace();
      break;
    case 6:
      BlinkyRand();
      break;
    case 7:
      testFunction();
      break;
    case 8:
      RandomLightsGrowAndShrink();
      break;
    case 9:
      rotatingRectanglesTakeTwo();
      break;
    default:
      count = 0;
      break;
  }
}

//This makes a square from the two ranges passed in on the layer provided with the value passed in
void makeSquareOnDataSourceLayer(int x1, int x2, int y1, int y2, int z, int value) {
  for (int i = x1; i < x2; i++) {
    for (int j = y1; j < y2; j++) {
      dataSource[i][j][z] = value;
    }
  }
}

void makeCubeOnDataSourceLayer(int x1, int x2, int y1, int y2, int z1, int z2, int value) {
  for (int i = x1; i < x2; i++) {
    for (int j = y1; j < y2; j++) {
      for (int z = z1; z < z2; z++) {
        dataSource[i][j][z] = value;
      }
    }
  }
}

//This goes through every value in the matrix and checks for a value. Returns true if all lit or not
bool checkValueInEntireDataSource(bool value) {
  for (int i = 0; i < CUBE_SIZE; i++) {
    for (int j = 0; j < CUBE_SIZE; j++) {
      for (int k = 0; k < CUBE_SIZE; k++) {
        if (dataSource[i][j][k] == value) {
          return 0;
        }
      }
    }
  }
  return 1;
}

/*-----------------------------------------------------------------------------------------------------/
--Functions that are patterns to modify the matrix-----------------------------------------------------/
-------------------------------------------------------------------------------------------------------/
-------------------------------------------------------------------------------------------------------/
------------------------------------------------------------------------------------------------------*/

//This function is responsible for changing the state of the current animation that is being displayed on the screen
//It does this by determining if a set amount of time has occured and incrementing a global counter variable
void stateChange(int timeInMilliSeconds) {

  if (Millis() >= timeInMilliSeconds) {
    //increment the state variable
    counter++;
    millis_timer = 0;

  }
}

void RandomLights() {
  stateChange(100);
  if (counter == 0) {
    writeValueToEntireDataSource(0);
    int x = random(4);
    int y = random(4);
    int z = random(4);
    Serial.println( x );
    dataSource[x][y][z] = 1;
    counter++;
  }
  if (counter == 1) {
  }
  if (counter == 2) {
    counter = 0;
  }
}
void RandomLightsGrow() {
  stateChange(100);
  if (counter == 0) {

    int x = 0;
    int y = 0;
    int z = 0;
    do {
      x = random(4);
      y = random(4);
      z = random(4);
    }
    while (dataSource[x][y][z] == 1);

    dataSource[x][y][z] = 1;
    counter++;
  }
  if (counter == 1) {
  }
  if (counter == 2) {
    if (checkValueInEntireDataSource(0) == 1) {
      writeValueToEntireDataSource(0);
    }
    counter = 0;
  }
}

void RandomLightsGrowAndShrink() {
  stateChange(100);

  if (counter == 0) {

    int x = 0;
    int y = 0;
    int z = 0;
    do {
      x = random(4);
      y = random(4);
      z = random(4);
    }
    while (dataSource[x][y][z] == writeValue);

    dataSource[x][y][z] = writeValue;
    counter++;
  }
  if (counter == 1) {
  }
  if (counter == 2) {
    if (checkValueInEntireDataSource(0) == 1 ) {
      writeValue = 0;
    } else if (checkValueInEntireDataSource(1) == 1 ) {
      writeValue = 1;
    }

    counter = 0;
  }
}

void testFunction() {
  stateChange(250);
  if (counter == 0)
  {
    writeValueToEntireDataSource(0);
    int x = 0;
    int y = 0;
    int z = 0;
    x = random(3)+1;
    y = random(3)+1;
    z = (x + y) % CUBE_SIZE;
    for (int i = 0; i <= x; i++) {
      for (int j = 0; j <= y; j++) {
        for (int k = 0; k <= z; k++)
          dataSource[i][j][k] = 1;
      }
    }
    counter++;
  }
  if (counter == 1) {
  }
  if (counter == 2) {
    counter = 0;
  }
}

void testFunctionMore() {
  stateChange(250);
  if (counter == 0)
  {
    writeValueToEntireDataSource(0);
    int x = 0;
    int y = 0;
    int z = 0;
    x = random(3)+1;
    y = random(3)+1;
    makeSquareOnDataSourceLayer(0, x, 0, y,fakeTime, 1);
    counter++;
  }
  if (counter == 1) {
  }
  if (counter == 2) {
    counter = 0;
  }
}

void BlinkyRand() {
  stateChange(350);
  if (counter == 0) {
    writeValueToEntireDataSource(0);
    int x = random(4);
    int y = random(4);
    int z = random(4);
    dataSource[x][y][z] = 1;
  }
  if (counter == 1) {
  }
  if (counter == 2) {
    counter = 0;
  }
}

void powerUp() {
  stateChange(200);

  if (counter == 0) {
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 0, CUBE_SIZE, 0, 1);
  }
  if (counter == 1) {
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 0, CUBE_SIZE, 1, 1);
  }
  if (counter == 2) {
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 0, CUBE_SIZE, 2, 1);
  }
  if (counter == 3) {
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 0, CUBE_SIZE, 3, 1);
  }
  if (counter == 4) {
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 0, CUBE_SIZE, 2, 1);
  }
  if (counter == 5) {
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 0, CUBE_SIZE, 1, 1);
  }
  if (counter == 6) {
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 0, CUBE_SIZE, 0, 1);
  }
  if (counter == 7) {
    writeValueToEntireDataSource(0);
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 0, CUBE_SIZE, 0, 1);

    //In your last state reset that state counter
    counter = 0;
  }
}

void rotatingRectangles() {
  stateChange(100);

  if (counter == 0) {
    writeValueToEntireDataSource(0);
    makeCubeOnDataSourceLayer(0, 2, 0, 2, 0, CUBE_SIZE, 1);
  }
  if (counter == 1) {
    writeValueToEntireDataSource(0);
    makeCubeOnDataSourceLayer(1, 3, 0, 2, 0, CUBE_SIZE, 1);
  }
  if (counter == 2) {
    writeValueToEntireDataSource(0);
    makeCubeOnDataSourceLayer(2, 4, 0, 2, 0, CUBE_SIZE, 1);
  }
  if (counter == 3) {
    writeValueToEntireDataSource(0);
    makeCubeOnDataSourceLayer(2, 4, 1, 3, 0, CUBE_SIZE, 1);
  }
  if (counter == 4) {
    writeValueToEntireDataSource(0);
    makeCubeOnDataSourceLayer(2, 4, 2, 4, 0, CUBE_SIZE, 1);
  }
  if (counter == 5) {
    writeValueToEntireDataSource(0);
    makeCubeOnDataSourceLayer(1, 3, 2, 4, 0, CUBE_SIZE, 1);
  }
  if (counter == 6) {
    writeValueToEntireDataSource(0);
    makeCubeOnDataSourceLayer(0, 2, 2, 4, 0, CUBE_SIZE, 1);
  }

  if (counter == 7) {
    writeValueToEntireDataSource(0);
    makeCubeOnDataSourceLayer(0, 2, 1, 3, 0, CUBE_SIZE, 1);
  }
  if (counter == 8) {
    writeValueToEntireDataSource(0);
    makeCubeOnDataSourceLayer(0, 2, 1, 3, 0, CUBE_SIZE, 1);

    //In your last state reset that state counter
    counter = 0;

  }
}

void rotatingRectanglesTakeTwo() {
  stateChange(170);

  if (counter == 0) {
    writeValueToEntireDataSource(0);
    makeCubeOnDataSourceLayer(0, 2, 0, 2, 0, CUBE_SIZE, 1);
    makeCubeOnDataSourceLayer(2, 4, 2, 4, 0, CUBE_SIZE, 1);
  }
  if (counter == 1) {
    writeValueToEntireDataSource(0);
    makeCubeOnDataSourceLayer(1, 3, 0, 2, 0, CUBE_SIZE, 1);
    makeCubeOnDataSourceLayer(1, 3, 2, 4, 0, CUBE_SIZE, 1);
  }
  if (counter == 2) {
    writeValueToEntireDataSource(0);
    makeCubeOnDataSourceLayer(2, 4, 0, 2, 0, CUBE_SIZE, 1);
    makeCubeOnDataSourceLayer(0, 2, 2, 4, 0, CUBE_SIZE, 1);
  }
  if (counter == 3) {
    writeValueToEntireDataSource(0);
    makeCubeOnDataSourceLayer(2, 4, 1, 3, 0, CUBE_SIZE, 1);
    makeCubeOnDataSourceLayer(0, 2, 1, 3, 0, CUBE_SIZE, 1);

  }
  if (counter == 4) {
    writeValueToEntireDataSource(0);
    makeCubeOnDataSourceLayer(2, 4, 1, 3, 0, CUBE_SIZE, 1);
    makeCubeOnDataSourceLayer(0, 2, 1, 3, 0, CUBE_SIZE, 1);
    counter = 0 ;
  }
}

void Xpat ( int z, int val)
{
  dataSource[0][z][0] = val;
  dataSource[1][z][1] = val;
  dataSource[2][z][2] = val;
  dataSource[3][z][3] = val;

  dataSource[0][z][3] = val;
  dataSource[1][z][2] = val;
  dataSource[2][z][1] = val;
  dataSource[3][z][0] = val;
}

void testPattern() {

  stateChange(100);

  if (counter == 0) {
    writeValueToEntireDataSource(0);
    Xpat(0, 1);
  }
  if (counter == 1) {
    writeValueToEntireDataSource(0);
    Xpat(1, 1);
  }
  if (counter == 2) {
    writeValueToEntireDataSource(0);
    Xpat(2, 1);
  }
  if (counter == 3) {
    writeValueToEntireDataSource(0);
    Xpat(3, 1);
  }
  if (counter == 4) {
    writeValueToEntireDataSource(0);
    Xpat(3, 1);
    counter = 0;
  }
}

void lineSweep() {
  stateChange(100);
  writeValueToEntireDataSource(0);

  if (counter == 0) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 0, 1, 0, 1);
  }
  if (counter == 1) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 1, 2, 0, 1);
  }
  if (counter == 2) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 2, 3, 0, 1);
  }
  if (counter == 3) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 3, 4, 0, 1);
  }
  if (counter == 4) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 3, 4, 1, 1);
  }
  if (counter == 5) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 2, 3, 1, 1);
  }
  if (counter == 6) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 1, 2, 1, 1);
  }
  if (counter == 7) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 0, 1, 1, 1);
  }
  if (counter == 8) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 0, 1, 2, 1);
  }
  if (counter == 9) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 1, 2, 2, 1);
  }
  if (counter == 10) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 2, 3, 2, 1);
  }
  if (counter == 11) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 3, 4, 2, 1);
  }
  if (counter == 12) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 3, 4, 3, 1);
  }
  if (counter == 13) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 2, 3, 3, 1);
  }
  if (counter == 14) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 1, 2, 3, 1);
  }
  if (counter == 15) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 0, 1, 3, 1);
  }
  if (counter == 16) {
    makeSquareOnDataSourceLayer(0, CUBE_SIZE, 0, 1, 3, 1);
    counter = 0;
  }
}

void lightCubeFace() {
  stateChange(300);
  writeValueToEntireDataSource(0);

  if (counter == 0) {
    makeCubeOnDataSourceLayer(0, CUBE_SIZE, 0, CUBE_SIZE, 0, 1, 1);
  }
  if (counter == 1) {
    makeCubeOnDataSourceLayer(0, CUBE_SIZE, 0, 1, 0, CUBE_SIZE, 1);
  }
  if (counter == 2) {
    makeCubeOnDataSourceLayer(0, CUBE_SIZE, 0, CUBE_SIZE, 3, 4, 1);
  }
  if (counter == 3) {
    makeCubeOnDataSourceLayer(0, 4, 3, 4, 0, 4, 1);
  }
  if (counter == 4) {
    makeCubeOnDataSourceLayer(0, 4, 3, 4, 0, 4, 1);
    counter = 0;
  }
}

/*-----------------------------------------------------------------------------------------------------/
--Functions that map matrix to cube -------------------------------------------------------------------/
-------------------------------------------------------------------------------------------------------/
-------------------------------------------------------------------------------------------------------/
------------------------------------------------------------------------------------------------------*/

void raiseAllGrounds() {
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
  if (data1 != 0 || data2 != 0) {
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
          if (j < 2) {
            data1 = data1 + ledAddress[i][j];
          }
          else {
            data2 = data2 + ledAddress[i][j];
          }
        }
      }
    }
    //At this point we have gone through one entire layer and can write that layer to the cube
    writeLayerToCube(k);
  }
}


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
  layerAddress[0] = 3;
  layerAddress[1] = 4;
  layerAddress[2] = 5;
  layerAddress[3] = 6;

}

//Interupts
ISR(TIMER2_OVF_vect)
{
  millis_timer ++;
}
unsigned int Millis()
{
  int m = millis_timer;
  return m;
}

void event_Handler()
{
  count++;
  counter = 0;
}
