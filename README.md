# LEDcube

[Image here]
Ever wanted to build a cube of individually addressable LEDs? Well I did. Here's the code that controls the cubes as well as documentation explaining how and why we built things certain ways.

Note that this project was tailored to fit the specifications for a group project at school. We needed to include: timers, a custom millis function, an arduino, interupts, and complex hardware.

## The hardware.

### Cliff notes:
  Cube Size: 4x4x4. (Around 10 hours of soldering time)
  
  Part list (quantities bolded): 
  1. 8-bit Shift Registers [(Part #: 74HC595)](https://www.sparkfun.com/products/733)  **x 2**
  2. Red Diffused LEDs (We bought 70 incase any were broken) **x 64**
  3. Wire/Jumper Cable **LOTS*
  4. Arduino
  5. Soldering Iron / Solder
  6. Breadboard
  7. Scrap Wood (One square foot 1/4 inch thick)

### The full story

The first and one of the most important decisions you will make when building an LED cube is the size. We initially thought that bigger is always better but soon realized the amount of time required to solder increases dramatically. We were on a limited time budget due to our courseload at school and the project deadline so we decided to build a 4x4x4.

The first problem we had to tackle was how do we address all these LEDs indiviudally. We began researching on the internet and saw that larger cubes connected the LEDs in a specific way. A single layer of the cube all shared a common ground (cathode)

**Top View**

> x  x   x   x

> x x x x   All of these LEDs would share a common ground

> x x x x   Each "x" represents a coloumn of LEDs which would share a common voltage source. (anode)

> x x x x

[2x2x2 image]

It was very important to build a prototype so that we could iterate on our design/assembly process. The prototype cube could also be used to develop the software for the project while the main hardware was assembled. 

Around 10 hours were spent soldering all the joints for the completed cube. 
