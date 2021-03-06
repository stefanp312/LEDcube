# LEDcube

Ever wanted to build a cube of individually addressable LEDs? Well I did. Here's the code that controls the cubes as well as documentation explaining how and why we built things certain ways.

Note that this project was tailored to fit the specifications for a group project at school. We needed to include: timers, a custom millis function, an Arduino, interrupts, and complex hardware.


![](https://github.com/stefanp312/LEDcube/blob/master/imagesForReadMe/Collage.jpg)

## Prototype

An iteractive design process was used in order to complete this project. A smaller prototype cube was first created so that we could familiarize ourselves with the assembly process, shift registers and so that software for the cube could be developed concurrently to speed up the over all project completion time.

> Baby Chirstmas Cube
![](https://github.com/stefanp312/LEDcube/blob/master/imagesForReadMe/BabyCube.JPG)


## The hardware.

### Cliff notes:
  
*Part list* (quantities bolded): 
  1. 8-bit Shift Registers [(Part #: 74HC595)](https://www.sparkfun.com/products/733)  **x 2**
  2. Red Diffused LEDs (We bought 70 in case any were broken) **x 64**
  3. Wire/Jumper Cable **LOTS*
  4. Arduino
  5. Soldering Iron / Solder
  6. Breadboard
  7. Push Button 
  8. Scrap Wood (One square foot 1/4 inch thick)
  
*Tips:*

- Cube Size: 4x4x4 (Expect around 10 hours of soldering time)

- Use scrap wood to make a template for soldering.

- Use diffused LEDs so the cube seems brighter.

- Find a shift register that has built-in resistors.

### The full story

The first and one of the most important decisions you will make when building an LED cube is the size. My group initially thought that bigger is always better but soon realized the number of LEDs and the amount of time required to solder the cube increases dramatically. Since we were on a limited time budget due to our course load at school and the project deadline we decided to build a 4x4x4 cube.

The first problem we had to tackle was how do we address all these LEDs individually. The Arduino Uno that was being used did not have enough output pins to directly control the number of LEDs we wanted to use. We began researching on the internet and saw that larger cubes connected the LEDs in a specific way. A single layer of the cube would share a ground connection (cathode) and each column would share a voltage pin. This allows the software to address any LED on the cube by using toggling specific voltage pins and providing a ground connection to certain layers. This decreases the number of output pins to 20 (16 anodes and 4 cathodes).

**Top View**

x---x---x---x

x---x---x---x

x---x---x---x

x---x---x---x

> Each "x" represents a column of LEDs which would share a common voltage source (anode).
>All of these LEDs would share a common ground (cathode).

Going from 64 output pins to 20 is a huge upgrade. However, we could do better. Using two 8-bit shift registers we were able to decrease that number to just 3. We chained the shift registers so that using a clock, data and latch pin we were able to control 16 outputs.

> Shift registers are a type of integrated circuit (IC) that are used to transform serial data into parallel data. They can be thought of as a storage tank that uses one main pipeline (data pin) to let 1 bit of data per clock cycle (clock pin). When the tank is full (latch pin), the tank releases all the data down multiple pipelines (output pins). 

There was a trade off to using shift registers as they increased software complexity and also added a very small insignificant latency to the update time on the cube. There is also additional wiring necessary to connect the two shift registers together.

> It would now take a minimum of 16 processor cycles to update the cube. However, the Arduino Uno runs at 16 Mhz which means that a cycle would last 0.0625 milli-seconds and the total delay added would be 1 milli-second.
> This is not perceivable to a human so the coolness factor of using shift registers won out.

A piece of scrap wood was acquired from the student machine shop so that a template for the cube could be constructed. The length of the LEDs legs were measured so that the spacing between the holes on the template could be determined. There was a safety factor of 2 mm on the LED leg measurements so that soldering could be made easier. LEDs were put into the template holes head first and alligned to fit the guidelines drawn. The template helped ensure that the layers made for the cube were as orthogonal as possible.

> 8 hours later...
> ![](https://github.com/stefanp312/LEDcube/blob/master/imagesForReadMe/CreatingLayers.jpg)

> 2 hours later!!!
> ![](https://github.com/stefanp312/LEDcube/blob/master/imagesForReadMe/FinalProduct.jpg)

A push button was installed on a bread board to be used as an interrupt source so that the pattern displayed on the cube could be changed. The reason we chose to use diffused LEDs was that they provided a uniform light from any angle. This would make the cube seem brighter since every LED is seen from a different angle and multiple locations. We chose red because we thought it would look cool in the dark.

Every column of LEDs also has a resistor attached to it so that none of the LEDs burn out. I would recommend finding a shift register with built-in resistors to make the wiring of the cube look nicer. Due to the added complexity of adding another shift register to control which layer would receive a ground connection, it was decided that we would just use 4 separate pins to control it.

## Software Tingz

### Cliff Notes.

  - A 3D array was mapped to the LEDs on the physical cube.
  
  - Uses persistence of vision to light up the cube layer by layer and look normal.
  
  - Patterns were created as various functions which manipulated a 3D matrix.
  
  - MVC Design.
  
  
### The full story

There are a few functions which seem random and unnecessary but were project requirements. The design I used is similar to the MVC used in iOS. The model is the 3D array which contains the data to be displayed. The view is the physical cube which acts as a sort of display. The controller would then be the update function which is responsible for making sure the cube looks like what the datasources describes.

**Mapping The Cube**

In order to control the LED cube I needed a way to model the cube in code and then update the cube with the data from the model. I used a boolean array with 3 dimensions that represented the X,Y,Z coordinates of an LED on the cube. Now when creating patterns for the cube, only the datasource needed to be modified thus simplifying the pattern making process.

I could only update the cube one layer at a time due to the way the LEDs were wired however this problem solved by using the persistence of human vision. If we updated each layer successively and fast enough it would appear to an observer that the entire cube was illuminated at the same time. The only time that this illusion could break down is if the update rate of the cube is decreased.

Thus, I only needed to be able to address 16 LEDs at a time using the shift registers. This meant that I would need to shift out 2 bytes of data when updating one layer of the cube. These bytes of data were created by iterating of the datasource and adding values to them from an array made when the Arduino first starts up. Each LED on a face is mapped to a power of two, thus when you add these values together you can illuminate multiple LEDs. The way it works is the first byte gets sent into the first register initially, then when the next bye is shifted out it takes the place of the first byte and pushes it into the second register.

**Update Function**
```c
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
          //The check to j is to decide which byte to write to
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
```

**How To Shift**

```C
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
```
**Creating Patterns**

Due to the design of the software designing patterns is quite simple. There exists only the restriction that each state of the animation has the same duration. However different animations can have different durations so it would be technically possible to chain multiple animation functions together in order to have different state durations.

Making an animation consists of calling the *stateChange* function with the duration of each animation frame. Then you need a conditional statement with the global variable *counter*. Inside that statement you modify the datasource as necessary. The *stateChange* function will automatically increment the counter variable so make sure that in the last state of the animation *counter* is set to 0.

```C
void stateChange(int timeInMilliSeconds) {
  if (Millis() >= timeInMilliSeconds) {
    //increment the state variable
    counter++;
    millis_timer = 0;
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
```

**Custom Millis**

The built-in millis function was rewritten by tracking the number of overflow interrupted that occur on hardware timer 2 of the Arduino. A timer prescaler value of 64 was used as it brought the duration closest to 1 millisecond. There was some roundoff error which was ignored since no more than a second is being timed before reseting the timer register. In one second, the maximum possible error is 24 milliseconds which is negligible in this use case.

