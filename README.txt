Name: Cliff Boyce, Aidan Miller
Assignment: Final Project - Compute Shader and Water Simulation

1. Description
	Uses the compute shader in an OpenGL 4.3 context to simulate real-time waves in a water simulation. The compute shader 
	essentially writes to a texture according to a series of sine waves, which is read in the vertex pipeline and the vertices
	are transformed accordingly.

2. Usage
	Keys:
		1 - Scrolls through the different sine waves that were implemented.
		2 - Toggles noise on the waves
		3 - Resets random value of which waves are generated on
		q/ESC - Quit

3. Compilation Instructions
	Requires OpenGL 4.3 for compute shader. Compile with make using makefile.

4. Bugs/Implementation Details
	Although we used the exact blinn-phong code from Dr. Paone's A1 project, the lighting does not look correct. Initially, 
	you see no lighting, and if you spin around, the lighting gets very intense.

5. Length
	10+ hours.

6. Fun
	10/10. Very fun to experiment with new technologies and shader programs. Learning about and using compute shaders was very
	cool. Water simulation looks very nice, so we are proud of what we made.