Mandelbrot Fractal

Features

Real-Time Rendering: Leverages GPU shaders for efficient fractal computation and rendering.

Controls: Zoom, and move to explore the fractal.

High Precision: Emulates double precision using split arithmetic for extreme zoom levels

Technologies Used

C++: Core

OpenGL

GLUT

GLEW

Prerequisites

Ensure you have the following installed:

OpenGL

GLUT

GLEW

A C++ compiler

Building the Project

Usage

Keyboard Controls

Zoom In: Press E

Zoom Out: Press Q

Pan Up: Press W

Pan Down: Press S

Pan Left: Press A

Pan Right: Press D

Console Outputs

Frame Time: Time taken to render each frame (ms).

Scale: Current zoom level.

Iterations: Maximum iterations for fractal computation.

Code Structure

Shaders

Vertex Shader: Computes vertex positions and passes texture coordinates to the fragment shader.

Fragment Shader: Computes the Mandelbrot set for each pixel and maps iteration counts to gradient colors.

Core Methods

compileShaders: Compiles and links the vertex and fragment shaders.

loadPalette: Generates the color gradient.

computeIterations: Dynamically adjusts iteration count based on the zoom.

display: Renders the fractal using OpenGL.

processInput: Handles user input for zooming and moving.

initBuffers: Sets up OpenGL buffers.

Customization

Color Palette

Edit the loadPalette function to define a custom gradient. Edit the red, green, and blue components:

paletteData[i * 3] = red
paletteData[i * 3 + 1] = green
paletteData[i * 3 + 2] = blue

Performance Tuning

Adjust the multiplier in computeIterations to increase or decrease detail.
Modify the resolution in glutInitWindowSize for a higher or lower rendering resolution.

References

Shader Concepts
https://www.brennengreen.dev/blog/posts/1/

Mandelbrot and OpenGL
https://physicspython.wordpress.com/2020/02/16/visualizing-the-mandelbrot-set-using-opengl-part-1/

Acknowledgments

Inspired by the mathematical beauty of the Mandelbrot set.

(ReadMe created by Chat-GPT 4o)

Kaitlyn, Chase, Nick
