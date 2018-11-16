# stevolve
OEE Artificial Life Platform

This is a platform for artificial life that is intended to be open-ended and extendable.   
- plugin classes for program-based or neuralnet-based cells, or create your own
- world classes for toroidal, gravity-based or test world, or create your own
- there is no attempt to define a "plant" or "animal" organism, they are expected to evolve into their own purpose
- organisms can Move/Look/Spawn/Eat(Share)

![Screenshot](/screenshot1.jpg)
- The yellow/brown areas are "land", the rest is "water". It takes much more energy to move on land. The yellow area takes more energy to move than the brown areas. The intention is to encourage diversity.
- For each of the property radio buttons at the top, except for 'instruction' the color of the cell represents its species and the brightness of the color represents the amount of the selected property.
- For the 'instruction' property, the color represents the last instruction the cell executed: green = move, red = eat, blue = spawn

**Directions:**
- Build the app.
- Press the 'new' button to generate a new world. It will generate cells with random dna and a large amount of energy. It may take a minute or two for the first tick of the clock.
- Change the environment parameters using the 'settings' button.
- Observe.

**Test World:**
If you want to create your own Cell class, you can use the "test" world to see how it performs.  The test world creates 2 cells and an environment of food randomly scattered around them to see which one finds it first.  It makes a copy of the winner, mutates it, and runs the test again.

**Observing:**
When creating this, I expected to see the following behaviors: plants, grazers, hunters. What I got was:
- plants: just stay still and soak up energy from the sun as it passes over, maybe retaining the energy during dark periods 
- grazers: move in a straight line and eat anything in its path
- hunters: a lot of movement going out and searching for prey
- ambushers: stay still until something approaches them
- cocoon builders: build a cocoon of its children around itself

**Cell Viewer:**
To the right side of the world is the visual representation of the currently selected cell. (works only in the test world, for now)

**ToDo:**
- The neural network based cells show more promise and I would like to expand on other type of artificial intelligence and deep learning techniques.
- Add momentum and physics to the movement of cells.
- Multi-cellular organisms.
