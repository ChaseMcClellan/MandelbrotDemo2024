#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>



/*
* Base of the shaders were found on a bolg site brennengreen.dev it explaines the shader code and what it does
*/
// https://www.brennengreen.dev/blog/posts/1/
/*
 * Vertex Shader
 * This shader processes the vertex and texture coordinates
 * Passes texture coordinates to the fragment shader and calculates
 * the position of each vertex
 */
const char* vertexShaderSource = R"(
#version 330 core
//postion
layout (location = 0) in vec3 aPos;

//Texture coords
layout (location = 1) in vec2 aTexCoord; 

//give frag shader the coords
out vec2 TexCoord; 

void main() {
    TexCoord = aTexCoord;
    //transformation
    gl_Position = vec4(aPos, 1.0);
}
)";

/*
* Math and mandelbrot concepts in opengl found here
* https://physicspython.wordpress.com/2020/02/16/visualizing-the-mandelbrot-set-using-opengl-part-1/
* 
*/

/*
 * Fragment Shader
 * This shader calculates the fractal for each pixel
 * using high-p floats. It determines
 * the number of iterations for the fractal
 * the result to a gradient color
 */
const char* fragmentShaderSource = R"(
#version 330 core

//use highp because we can't use double
precision highp float;

//texture coordinates from vertex shader
in vec2 TexCoord; 

//give to the frame buffer
out vec4 FragColor; 

//High & low precision part of the center
uniform vec2 centerHigh;  
uniform vec2 centerLow;

//Scale for zooming in
uniform float scale;         

//Max iterations for the fractal math
uniform int maxIterations;   

//the color palette for gradient
uniform sampler1D palette;   

void main() {
    vec2 c = centerHigh + centerLow + vec2(
        (TexCoord.x - 0.5) * scale,
        (TexCoord.y - 0.5) * scale
    );

    vec2 z = vec2(0.0, 0.0);
    int i;
    for (i = 0; i < maxIterations; ++i) {
        vec2 z_new = vec2(
            z.x * z.x - z.y * z.y + c.x,  //Real
            2.0 * z.x * z.y + c.y         //Imaginary
        );
        z = z_new;
        //tells when to break loop
        if (dot(z, z) > 4.0) break;
    }

    float colorIndex = float(i) / float(maxIterations);
    FragColor = texture(palette, colorIndex);
}
)";

//globals
double centerX = 0.0, centerY = 0.0;
double scale = 2.0;
int maxIterations = 500;

//glut objects
GLuint shaderProgram, VAO, paletteTexture;

//method compile shaders
//links the vertex and fragment shaders into OpenGL
GLuint compileShaders(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

//edit palette to change gradient
GLuint loadPalette() {
    std::vector<float> paletteData(1024*3);
    for (int i = 0; i < 1024; ++i) {
        float t = float(i) / 1023.0;
        //r
        paletteData[i *3] = t;
        //g
        paletteData[i *3 +1] = t * t;
        //b
        paletteData[i *3 +2] = sqrt(t);
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_1D, tex);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 1024, 0, GL_RGB, GL_FLOAT, paletteData.data());
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    return tex;
}

//method to compute the maxiterations based off scale
int computeIterations(){
    return 500 + log2(2.0 / scale) *200;
}

//gather everything in order to render it
void display(){
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    //hi && lo
    double centerHiX = floor(centerX);
    double centerLoX = centerX - centerHiX;
    double centerHiY = floor(centerY);
    double centerLoY = centerY - centerHiY;

    glUniform2f(glGetUniformLocation(shaderProgram, "centerHigh"), centerHiX, centerHiY);
    glUniform2f(glGetUniformLocation(shaderProgram, "centerLow"), centerLoX, centerLoY);
    glUniform1f(glGetUniformLocation(shaderProgram, "scale"), scale);
    glUniform1i(glGetUniformLocation(shaderProgram, "maxIterations"), maxIterations);

    glBindTexture(GL_TEXTURE_1D, paletteTexture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glutSwapBuffers();
    

}

//controls
void processInput(unsigned char key, int x, int y) {
    double zoomFactor = scale > 1e-5 ? 0.9 : 0.99;
    switch (key) {
    case 'e': 
        scale *= zoomFactor; 
        break;
    case 'q': 
        scale /= zoomFactor; 
        break;
    case 'w': 
        centerY += 0.1 * scale; 
        break;
    case 's': 
        centerY -= 0.1 * scale; 
        break;
    case 'a': 
        centerX -= 0.1 * scale; 
        break;
    case 'd': 
        centerX += 0.1 * scale; 
        break;
    }
    maxIterations = computeIterations();
    static int last = glutGet(GLUT_ELAPSED_TIME);
    int curr = glutGet(GLUT_ELAPSED_TIME);
    int deltaTime = curr - last;
    std::cout << "Frame time: " << deltaTime << " ms" << std::endl;
    std::cout << "Scale: " << scale << std::endl;
    std::cout << "Iterations " << maxIterations << std::endl;

    last = curr;
    glutPostRedisplay();
}

//get the opengl buffers
void initBuffers(){
    float vertices[] ={
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
    };

    unsigned int indices[] = { 0, 1, 3, 1, 2, 3 };

    GLuint VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    //res
    glutInitWindowSize(800, 800);
    glutCreateWindow("Mandelbrot");

    glewInit();
    //init shaders and other stuff
    shaderProgram = compileShaders(vertexShaderSource, fragmentShaderSource);
    paletteTexture = loadPalette();
    initBuffers();


    glutDisplayFunc(display);
    glutKeyboardFunc(processInput);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glutMainLoop();

    return 0;
}
