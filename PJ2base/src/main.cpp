/* Lab 6 base code - transforms using matrix stack built on glm
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

#include <iostream>
#include <glad/glad.h>
#include <cmath>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;

	// Shape to be used (from obj file)
	shared_ptr<Shape> shape;
	shared_ptr<Shape> spaceShip;
	shared_ptr<Shape> ground;
	shared_ptr<Shape> spitfire;
	shared_ptr<Shape> tree;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	float lightTrans = 0;
	float lightY = 0;
	float gRot = 0;
	float gTrans=0;

	float planeRotate=0;
	float planeDescent = 0;

	float backR=0.52;
	float backG = 0.8;
	float backB = 0.92;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		// CONTROLLS
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
		if (key == GLFW_KEY_O && action == GLFW_RELEASE) {
			lightTrans--;
		}
		if (key == GLFW_KEY_P && action == GLFW_RELEASE) {
			lightTrans++;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
			gRot+= 0.16;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
			gRot-= 0.16;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
			gTrans++;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
			gTrans--;
		}
		if (key == GLFW_KEY_Q && action == GLFW_RELEASE) {
			planeDescent--;
		}
		if (key == GLFW_KEY_E && action == GLFW_RELEASE) {
			planeDescent++;
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("MatShine");
		prog->addUniform("lightP");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
	}
	//initGeom Helper function
	void initMesh(shared_ptr<Shape> &mesh, std::string location) {
		mesh = make_shared<Shape>();
		mesh->loadMesh(location);
		mesh->resize();
		mesh->init();

	}

	void initGeom(const std::string& objectDirectory)
	{
		// Initialize mesh.
		shape = make_shared<Shape>();
		spaceShip = make_shared<Shape>();
		initMesh(ground,objectDirectory + "/newTerrain.obj");
		initMesh(spitfire, objectDirectory + "/spitfirev6.obj");
		initMesh(tree, objectDirectory + "/house.obj");
		//shape->loadMesh(resourceDirectory + "/SmoothSphere.obj");
		//shape->loadMesh(resourceDirectory + "/sphere.obj");
		shape->loadMesh(objectDirectory + "/bunny.obj");
		spaceShip->loadMesh(objectDirectory + "/Low_poly_UFO.obj");


		shape->resize();
		shape->init();
		spaceShip->resize();
		spaceShip->init();
	}


		void SetMaterial(int i) {

    		prog->bind();
    		switch (i) {
    		case 0: //shiny blue plastic
    			glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.04, 0.2);
    			glUniform3f(prog->getUniform("MatDif"), 0.0, 0.16, 0.9);
    			glUniform3f(prog->getUniform("MatSpec"), 0.14, 0.2, 0.8);
    			glUniform1f(prog->getUniform("MatShine"), 27.0);
    		break;
    		case 1: // RAF plane metal
    			glUniform3f(prog->getUniform("MatAmb"), 0.349, 0.271, 0.125);
    			glUniform3f(prog->getUniform("MatDif"), 0.773, 0.72, 0.62);
    			glUniform3f(prog->getUniform("MatSpec"), 1, 0.898, 0.706);
    			glUniform1f(prog->getUniform("MatShine"),50);
    		break;
    		case 2: // LFTWF plane metal
    			glUniform3f(prog->getUniform("MatAmb"), 0.157, 0.165, 0.118);
    			glUniform3f(prog->getUniform("MatDif"), 0.953, 0.773, 0.18);
    			glUniform3f(prog->getUniform("MatSpec"), 1, 0, 0);
    			glUniform1f(prog->getUniform("MatShine"),100);
    		break;
			case 3://mountain
				glUniform3f(prog->getUniform("MatAmb"), 0.33, 0.529, 0.204);
				glUniform3f(prog->getUniform("MatDif"), .494, 0.784, 0.314);
				glUniform3f(prog->getUniform("MatSpec"), 0.557, 0.871,0.302);
				glUniform1f(prog->getUniform("MatShine"),4);

			break;
			case 4://pastel house
				glUniform3f(prog->getUniform("MatAmb"),.204,.525, 0.769);
				glUniform3f(prog->getUniform("MatDif"), .298, 0.694, 1);
				glUniform3f(prog->getUniform("MatSpec"), 0.886, 0.894, 0.804);
				glUniform1f(prog->getUniform("MatShine"), 32);

			break;
  		}
	}

	void render()
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(backR, backG, backB, 1);
		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto View = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);

		// View is identity - for now
		View->pushMatrix();
		View->translate(vec3(0,-.3, -3 + gTrans));
		

		// Draw a stack of cubes with indiviudal transforms
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniform3f(prog->getUniform("lightP"), lightTrans+0,lightY+ 4, 5);

		// draw the spheres
		Model->pushMatrix();

		  Model->pushMatrix();
		  //GROUND START

		  Model->rotate(gRot, vec3(0, 1, 0));
		  Model->translate(vec3(0, 0, 0));
		  Model->scale(vec3(2.5, 2.5, 2.5));
		  SetMaterial(3);
		  glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		  ground->draw(prog);

		  planeRotate += .003;

		  Model->pushMatrix();
		  //raf plane
		  Model->rotate(planeRotate * 2, vec3(0, 1, 0));
		  Model->translate(vec3(0, 0.3+ planeDescent * .01, -.5));
		  Model->rotate(90, vec3(0, 1, 0));
		  Model->rotate(20, vec3(0, 0, 1));
		  Model->scale(vec3(0.1, 0.1, 0.1));
		  SetMaterial(1);
		  glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		  spitfire->draw(prog);
		  Model->popMatrix();

		 

		  Model->pushMatrix();
		  //lftw plane
		  Model->rotate(planeRotate*2, vec3(0, 1, 0));
		  Model->translate (vec3(0, 0.3 + planeDescent* .01, .5));
		  Model->rotate(-90, vec3(0, 1, 0));
		  Model->rotate(20, vec3(0, 0, 1));
		  Model->scale(vec3(0.1, 0.1, 0.15));
		  SetMaterial(2);
		  glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		  spitfire->draw(prog);
		  Model->popMatrix();



		  //house :)
		  Model->pushMatrix();
		  Model->translate(vec3(0,-0.050,0.75));
		  Model->scale(vec3(0.05,0.05, 0.05));
		  Model->rotate(-170, vec3(0, 1, 0));
		  SetMaterial(4);
		  glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		  tree->draw(prog);
		  Model->popMatrix();




		  //GROUND END

		  Model->popMatrix();
		  lightY = (-1.0 / 20.0) * lightTrans * lightTrans + 8;



		Model->popMatrix();

		prog->unbind();

		// Pop matrix stacks.
		Projection->popMatrix();
		View->popMatrix();

	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";
	std::string objectDir = "../objects";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(objectDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
