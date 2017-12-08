#include <string>
#include <algorithm>
#include <GL/glew.h>	// GLEW 
#include <GLFW/glfw3.h> // GLFW
// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// Image loading Lib
#include <SOIL.h> 

using namespace std;

// Properties
GLuint screenWidth = 1000, screenHeight = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

GLuint loadTexture(GLchar* path);
GLuint loadCubemap(vector<const GLchar*> faces);
void RenderQuad(); 
bool CheckCollision(glm::vec3 &one, glm::vec3 &two);
// Camera
Camera camera(glm::vec3(-4.9f, 3.7f, 5.7f));
// Light position
glm::vec3 lightPos(0.0f, 0.0f, 6.0f);

// Ball position and Player1 &2 position
glm::vec3 Player1_pos(1.0f, 0.0f, 0.0f);
glm::vec3 Player2_pos(0.0f, 0.0f, -3.3f);
glm::vec3 Ball_pos (0.0f, 0.0f, 0.0f);
float player1_angle = 0.0f;
float player2_angle = 0.0f;
glm::vec3 step = glm::vec3(0.1f, 0.0f, 0.0);

bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

bool normalMapping = true;

int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "CPRE557", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Setup and compile our shaders
	Shader modelShader("./shaders/Model.vs", "./shaders/Model.frag");
	Shader skyboxShader("./shaders/skybox.vs", "./shaders/skybox.frag");
	Shader floorShader("./shaders/grass.vs", "./shaders/grass.frag");
	Shader Normalshader("./shaders/normal_mapping.vs", "./shaders/normal_mapping.frag");
	 
	// Load textures
	GLuint diffuseMap = loadTexture("brickwall.jpg");
	GLuint normalMap = loadTexture("brickwall_normal.jpg");
	GLuint floorTexture = loadTexture("Grass 00 seamless.jpg");

	// Load models 
	Model ourModel1("./Models/Soccerball2/Ball.obj");
	Model ourModel2("./Models/Mario and Luigi/Luigi_obj.obj");
	Model ourModel3("./Models/Mario and Luigi/mario_obj.obj");
	Model ourModel4("./Models/oildrum/oildrum.obj"); 

	Model Light_model("./Models/Light Pole/Light Pole.obj");


	// Set texture units 
	Normalshader.Use();
	glUniform1i(glGetUniformLocation(Normalshader.Program, "diffuseMap"), 0);
	glUniform1i(glGetUniformLocation(Normalshader.Program, "normalMap"), 1);

	//////////////////// Skybox and Floor setup /////////////////////////////////
#pragma region "object_initialization"
	// Setup skybox VAO
	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
	// Cubemap (Skybox)
	vector<const GLchar*> faces;
	faces.push_back("./sky/xpos.png");
	faces.push_back("./sky/xneg.png");
	faces.push_back("./sky/ypos.png");
	faces.push_back("./sky/yneg.png");
	faces.push_back("./sky/zpos.png");
	faces.push_back("./sky/zneg.png");
	GLuint cubemapTexture = loadCubemap(faces);


	// Setup floor plane VAO
	GLfloat floorVertices[] = {
		// Positions          
		5.0f,  0.0f,  5.0f,  10.0f, 0.0f,
		-5.0f, 0.0f,  5.0f,  0.0f, 0.0f,
		-5.0f, 0.0f, -5.0f,  0.0f, 10.0f,
		5.0f,  0.0f,  5.0f,  10.0f, 0.0f,
		-5.0f, 0.0f, -5.0f,  0.0f, 10.0f,
		5.0f,  0.0f, -5.0f,  10.0f, 10.0f
	};

	GLuint floorVAO, floorVBO;
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

#pragma endregion
	/////////////////////////////////////////////////////////////////////////////
	
	// Draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	int ball_movement_cnt=0;
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		Do_Movement();

		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		/////////////////////////////////////////////////////////////////////////////////
		// Draw skybox first
		glDepthMask(GL_FALSE);// Remember to turn depth writing off
		skyboxShader.Use();
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
		/////////////////////////////////////////////////////////////////////////////////

		// Floor
		floorShader.Use();
		glm::mat4 model;
		view = camera.GetViewMatrix();
		projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(floorShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(floorShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		
		glBindVertexArray(floorVAO);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		model = glm::mat4();
		glUniformMatrix4fv(glGetUniformLocation(floorShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0); 
		 
		///////////////////////////////////
		// Configure view/projection matrices
		Normalshader.Use(); 
		view = camera.GetViewMatrix();
		projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(Normalshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(Normalshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		
		model = glm::mat4();
		// Render normal-mapped quad 
		glUniformMatrix4fv(glGetUniformLocation(Normalshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(glGetUniformLocation(Normalshader.Program, "lightPos"), 1, &lightPos[0]);
		glUniform3fv(glGetUniformLocation(Normalshader.Program, "viewPos"), 1, &camera.Position[0]);
		glUniform1i(glGetUniformLocation(Normalshader.Program, "normalMapping"), normalMapping);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		RenderQuad(); 


		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
		glUniformMatrix4fv(glGetUniformLocation(Normalshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		RenderQuad(); 
		model = glm::mat4();
		model = glm::rotate(model, (GLfloat)1.5708, glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		glUniformMatrix4fv(glGetUniformLocation(Normalshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		RenderQuad();
		model = glm::mat4();
		model = glm::rotate(model, (GLfloat)-1.5708, glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		glUniformMatrix4fv(glGetUniformLocation(Normalshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		RenderQuad();
		 
		modelShader.Use();
		// Set texture units 
		//shader.Use();
		glUniform1i(glGetUniformLocation(modelShader.Program, "diffuseMap"), 0);
		glUniform1i(glGetUniformLocation(modelShader.Program, "normalMap"), 1);

		view = camera.GetViewMatrix();

		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


		// Render the Ball model
		glm::mat4 model1;
		model1 = glm::translate(model1, Ball_pos); // Translate it down a bit so it's at the center of the scene
		model1 = glm::scale(model1, glm::vec3(0.0009f, 0.0009f, 0.0009f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model1));
		ourModel1.Draw(modelShader);


		// Render the First Player model
		glm::mat4 model2;
		model2 = glm::translate(model2, Player1_pos); // Translate it down a bit so it's at the center of the scene
		model2 = glm::scale(model2, glm::vec3(0.009f, 0.009f, 0.009f));	// It's a bit too big for our scene, so scale it down
		model2 = glm::rotate(model2, (GLfloat)glm::radians(player1_angle), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model2));
		ourModel2.Draw(modelShader);


		// Render the Second Player model
		glm::mat4 model3;
		model3 = glm::translate(model3, Player2_pos); // Translate it down a bit so it's at the center of the scene
		model3 = glm::scale(model3, glm::vec3(0.009f, 0.009f, 0.009f));	// It's a bit too big for our scene, so scale it down 
		model3 = glm::rotate(model3, (GLfloat)glm::radians(player2_angle), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model3));
		ourModel3.Draw(modelShader);



		// Render the Four oildrum for goals model 		
		// Number 1
		glm::mat4 model4;
		model4 = glm::translate(model4, glm::vec3(1.0f, 0.0f, 4.7f)); // Translate it down a bit so it's at the center of the scene
		model4 = glm::scale(model4, glm::vec3(0.7f, 0.7f, 0.7f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model4));
		ourModel4.Draw(modelShader);
		// Number 2
		model4 = glm::mat4();
		model4 = glm::translate(model4, glm::vec3(-1.0f, 0.0f, 4.7f)); // Translate it down a bit so it's at the center of the scene
		model4 = glm::scale(model4, glm::vec3(0.7f, 0.7f, 0.7f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model4));
		ourModel4.Draw(modelShader);
		// Number 3
		model4 = glm::mat4();
		model4 = glm::translate(model4, glm::vec3(1.0f, 0.0f, -4.7f)); // Translate it down a bit so it's at the center of the scene
		model4 = glm::scale(model4, glm::vec3(0.7f, 0.7f, 0.7f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model4));
		ourModel4.Draw(modelShader);
		// Number 4
		model4 = glm::mat4();
		model4 = glm::translate(model4, glm::vec3(-1.0f, 0.0f, -4.7f)); // Translate it down a bit so it's at the center of the scene
		model4 = glm::scale(model4, glm::vec3(0.7f, 0.7f, 0.7f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model4));
		ourModel4.Draw(modelShader);
		 
		// Render the Four Lights for goals model 		
		glm::mat4 Light;
		Light = glm::translate(Light, glm::vec3(4.7f, 1.0f, 4.7f)); // Translate it down a bit so it's at the center of the scene
		Light = glm::scale(Light, glm::vec3(0.09f, 0.09f, 0.09f));	// It's a bit too big for our scene, so scale it down
		Light = glm::rotate(Light, (GLfloat)glm::radians(45.0), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(Light));
		Light_model.Draw(modelShader);
		 
		Light = glm::mat4();
		Light = glm::translate(Light, glm::vec3(4.7f, 1.0f, -4.7f)); // Translate it down a bit so it's at the center of the scene
		Light = glm::scale(Light, glm::vec3(0.09f, 0.09f, 0.09f));	// It's a bit too big for our scene, so scale it down
		Light = glm::rotate(Light, (GLfloat)glm::radians(135.0), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(Light));
		Light_model.Draw(modelShader);

		Light = glm::mat4();
		Light = glm::translate(Light, glm::vec3(-4.7f, 1.0f, 4.7f)); // Translate it down a bit so it's at the center of the scene
		Light = glm::scale(Light, glm::vec3(0.09f, 0.09f, 0.09f));	// It's a bit too big for our scene, so scale it down
		Light = glm::rotate(Light, (GLfloat)glm::radians(-45.0), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(Light));
		Light_model.Draw(modelShader);

		Light = glm::mat4();
		Light = glm::translate(Light, glm::vec3(-4.7f, 1.0f, -4.7f)); // Translate it down a bit so it's at the center of the scene
		Light = glm::scale(Light, glm::vec3(0.09f, 0.09f, 0.09f));	// It's a bit too big for our scene, so scale it down
		Light = glm::rotate(Light, (GLfloat)glm::radians(-135.0), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(Light));
		Light_model.Draw(modelShader);
		// Check for collision between players and the ball 
		if (CheckCollision(Player1_pos, Ball_pos))
		{
			Ball_pos = Ball_pos - 2.0f *(Player1_pos - Ball_pos);
			cout << "Collision1" << endl;
		} else if(CheckCollision(Player2_pos, Ball_pos))
		{
			Ball_pos = Ball_pos - 2.0f *(Player2_pos - Ball_pos);
			cout << "Collision2" << endl;
		}

		if(Player2_pos.x >= 1.5)
		{
			step = glm::vec3(-0.1f, 0.0f, 0.0f);
			cout << "cond1" << endl;
		}
		else if(Player2_pos.x <= -1.5)
		{
			step = glm::vec3(0.1f, 0.0f, 0.0f);
			cout << "cond2" << endl;

		}

		Player2_pos = Player2_pos+ step;
	 

		//cout << Player2_pos.x << "\t" << Player2_pos.y << "\t" << Player2_pos.z << endl;

		cout << camera.Position.x << "\t" << camera.Position.y << "\t" << camera.Position.z << endl;
		cout << camera.Pitch << "\t" << camera.Yaw  << endl;

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}



// Loads a cubemap texture from 6 individual texture faces
// Order should be: +X (right) -X (left) +Y (top) -Y (bottom) +Z (front) -Z (back)
GLuint loadCubemap(vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

GLuint loadTexture(GLchar* path)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}

bool CheckCollision(glm::vec3 &one, glm::vec3 &two) // AABB - AABB collision
{
	float obj_size = 0.5;


	// Collision x-axis?
	bool collisionX = one.x + obj_size >= two.x && two.x + obj_size >= one.x;
	// Collision y-axis?
	bool collisionY = one.z + obj_size >= two.z && two.z + obj_size >= one.z;
	// Collision only if on both axes
	return collisionX && collisionY; 
}


// RenderQuad() Renders a 1x1 quad in NDC
GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions       // Normals          // Texture Coords 
			-5.0f, 1.0f, 5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			-5.0f, 0.0f, 5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			5.0f, 1.0f, 5.0f, 0.0f, 0.0f, 1.0f, 10.0f, 1.0f,
			5.0f, 0.0f, 5.0f, 0.0f, 0.0f, 1.0, 10.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
 
#pragma region "User input"
// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_RIGHT])
	{
		Player1_pos = Player1_pos + glm::vec3(0.1f, 0.0f, 0.0f);
		player1_angle = 90;
	}
	if (keys[GLFW_KEY_LEFT])
	{
		Player1_pos = Player1_pos + glm::vec3(-0.1f, 0.0f, 0.0f);
		player1_angle = 270;
	}
	if (keys[GLFW_KEY_DOWN])
	{
		Player1_pos = Player1_pos + glm::vec3(0.0f, 0.0f, 0.1f);
		player1_angle = 0;
	}

	if (keys[GLFW_KEY_UP])
	{
		Player1_pos = Player1_pos + glm::vec3(0.0f, 0.0f, -0.1f);
		player1_angle = 180;

	}

	if (keys[GLFW_KEY_R])
	{
		Player1_pos = glm::vec3(1.0f, 0.0f, 0.0f);
		Player2_pos = glm::vec3(-1.0f, 0.0f, -3.3f);
		Ball_pos = glm::vec3(0.0f, 0.0f, 0.0f);

	}


}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

#pragma endregion

