#define GLEW_STATIC	
#define BUFFER_OFFSET(i) ((void*)(i)) //Define this to more easily get VBO offsets.

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


const char* vertex_Shader = 
"#version 330\n"
"in vec3 vPosition;"
"in vec3 vColor;"
"out vec3 color;"
"void main() {"
"	gl_Position = vec4(vPosition, 1.0);"
"	color = vColor;"
"}";

const char* fragment_Shader =
"#version 330\n"
"in vec3 color;"
"out vec4 frag_color;"
"void main() {"
"	frag_color = vec4(color, 1.0);"
"}";

GLuint shaderProgram = 0;


GLfloat vertexPositions[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f, 1.0f, 0.0f
};

GLfloat vertexColors[] = {
	1.0f, 0.0f, 0.0f, //Red
	0.0f, 1.0f, 0.0f, //Blue
	0.0f, 0.0f, 1.0f  //Green
};

GLuint shaderInit(const char* source, GLenum type)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar**)&source, NULL);
	glCompileShader(shader);
	return shader;
}

GLuint initShaders(const char* vs, const char* fs)
{	//GPU SHADER PROGRAM CREATION
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, shaderInit(vs, GL_VERTEX_SHADER));
	glAttachShader(shaderProgram, shaderInit(fs, GL_FRAGMENT_SHADER));
	glLinkProgram(shaderProgram);
	/* link and error check */
	GLint linked;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
	if (!linked) {
		std::cerr << "Shader failed to link!\n";
		GLint logSize = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logSize);

		char* infoLog = new char[logSize];
		glGetProgramInfoLog(shaderProgram, logSize, NULL, infoLog);
		std::cout << infoLog << std::endl;
		delete(infoLog);
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "Shaders link successful!\n";
	}

	return shaderProgram;
}
int main()
{

	//SETTING UP GLFW - CREATING A WINDOW
	//-----------------------------------------------------------------------

	GLFWwindow* window;

	if (!glfwInit())
	{
		std::cout << "ERROR: Failed to Initialize GLFW!" << std::endl;
		return -1;
	}
	
	//Create the Glfw window 
	window = glfwCreateWindow(600, 600, "Hello Triangle!", NULL, NULL);

	//Error-check window creation
	if (window == NULL)
	{
		std::cout << "ERROR: Failed to Create Window!" << std::endl;
		return -1;
	}

	//State that we're drawing in the window.
	glfwMakeContextCurrent(window);  //OpenGL is a state machine. An OpenGL context holds that state. All drawing happens 
									//in the current context. So, we're saying all drawing happens in the window we just created.



	//SETTING UP GLEW
	//-----------------------------------------------------------------------
	glewExperimental = GL_TRUE; // Bug in GLEW. glewExperimental is a variable that is already defined by GLEW.
								// Make sure to set this to GL_TRUE.

	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR: Failed to Initialize GLEW!" << std::endl;
		return -1;

	}



	//SETTING UP SHADERS
	//------------------------------------------------------------------------
	GLuint shaderProgram = initShaders(vertex_Shader, fragment_Shader);
	glUseProgram(shaderProgram);

	GLuint VAO;
	GLuint VBO;

	//Initialize Vertex Array Object 
	glGenVertexArrays(1, &VAO);	//Generating 1 vertex array object.
	glBindVertexArray(VAO);		//make the VAO active.


	//Initialize Vertex Buffer Object
	glGenBuffers(1, &VBO);	//Generating 2 buffers and puts the resulting buffers in VBO. 

	//Setting up Position Data Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//First Buffer is bound to the GL_ARRAY_BUFFER target. This simply means that we intend to use this buffer for vertex attribute data.
											//We are activating the vertex attribute buffer.
											//This will be used to hold position data.

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions) + sizeof(vertexColors), NULL, GL_STATIC_DRAW); //Gives our vertex positions to openGL. This code tells OpenGL that 
																					//that the buffer size is the size of the PositionData and it gives it
																					//the address of the first value. This will be stored in GPU memory I believe.

	//Two sub buffers. Position attributes all go in first. Then all color attributes go in. NOT INTERLEAVED.
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexPositions), vertexPositions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertexPositions), sizeof(vertexColors), vertexColors);


	glUseProgram(shaderProgram); //Tell OpenGL that you want to use your shaders in the shader program

	//Map shader variables to main variable.
	GLint vPosition = glGetAttribLocation(shaderProgram, "vPosition");

	if (vPosition < 0)
	{
		std::cout << "ERROR: couldn't find vPosition in shader" << std::endl;
	}

	//Turn on the pipeline
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));





	//Setting attribute format for vertex shader.

	GLint vColor = glGetAttribLocation(shaderProgram, "vColor");

	if (vColor < 0)
	{
		std::cout << "couldn't find vColor in shader" << std::endl;
	}



	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertexPositions)));


	

	//Render Loop
	while (!glfwWindowShouldClose(window))
	{
		//Background Color
		glClearColor(1.0, 1.0, 1.0, 1.0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear the screen with the color described by glClearColor() 


		glDrawArrays(GL_TRIANGLES, 0, 3);

		//glfwPollEvents();   //This would be for continuous rendering, like in games.
		// 
		glfwWaitEvents();	//Only render with external events, like interactive mouse-clicks.
							//Waaaaay more efficient for GPU

		glfwSwapBuffers(window); //Need this

	}


	return 0;
}