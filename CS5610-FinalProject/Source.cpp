#include <Windows.h>
#include <GL/glew.h>
#include <iostream>
#include <cmath>
#include <GL/freeglut.h>

#include "../cyCodeBase/cyTriMesh.h"
#include "../cyCodeBase/cyMatrix.h"
#include "../cyCodeBase/cyGL.h"
#include "../lodepng/lodepng.h"
#include "../lodepng/lodepng.cpp"

// programs
cy::GLSLProgram objProgram;
cy::GLSLProgram planeProgram;

// .obj stuff
cy::TriMesh mesh; // .obj mesh
std::vector<cy::Vec3f> vertices;
std::vector<cy::Vec3f> normals;
std::vector<cy::Vec3f> textures;
GLuint objVao;
std::vector<unsigned char> image; //the raw pixels
unsigned texWidth, texHeight;

// plane stuff
GLuint planeVao;
GLuint planeBuffer;

// controls
float yRot = 60;
float xRot = 0;
float prevX = 0;
float prevY = 0;
float scale = 11;
bool leftMouseDown = false;
bool rightMouseDown = false;
float inc = 0;

cyVec3f cameraPos = cyVec3f(0, 2, 1);

double Deg2Rad(double degree)
{
	double pi = 3.14159265359;
	return (degree * (pi / 180));
}

void createMatrices() {
	// create obj matrix
	cy::Matrix3f rotMatrix = cy::Matrix3f::RotationXYZ(Deg2Rad(0), Deg2Rad(0), Deg2Rad(yRot));
	cy::Matrix4f projMatrix =
		cy::Matrix4f::Perspective(scale,
			float(1) / float(1),
			1.0f,
			100.0f);
	cy::Matrix4f viewMatrix = cy::Matrix4f::View(cameraPos, cyVec3f(0, 0, 0), cyVec3f(0, 1, 0));
	cy::Matrix4f modelMatrix = cy::Matrix4f(1.0f);

	cy::Matrix4f mvp = projMatrix * viewMatrix * modelMatrix * rotMatrix;
	cy::Matrix3f mv = cy::Matrix3f(viewMatrix) * cy::Matrix3f(modelMatrix);
	cy::Matrix4f mv4 = viewMatrix * modelMatrix;
	mv = mv.GetInverse();
	mv = mv.GetTranspose();

	objProgram["mvp"] = mvp;
	objProgram["mv"] = mv;
	objProgram["mv4"] = mv4;

	planeProgram["mvp"] = mvp;
}

void createLighting() {
	std::cout << yRot << std::endl;
	cyVec3f lightPos = cyVec3f(yRot * 0.05, 3, 0);
	cyVec3f white = cyVec3f(0.7, 0.7, 0.7);
	cyVec3f ambient = cyVec3f(0.3, 0.3, 0.3);
	objProgram["lightPos"] = lightPos;
	objProgram["white"] = white;
	objProgram["ambient"] = ambient;
	glutPostRedisplay();
}

void createPlane() {
	static const GLfloat plane[] =
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};
	// create vertex array object for plane
	glGenVertexArrays(1, &planeVao);
	glBindVertexArray(planeVao);

	// create buffer for plane
	glGenBuffers(1, &planeBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, planeBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
}

void createObj() {
	// Create Vertex buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer); // bind buffer, following operations will use it
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(cy::Vec3f) * vertices.size(),
		&vertices[0], // access mesh data inside object, pointer for array of vertex positions
		GL_STATIC_DRAW);

	objProgram.SetAttribBuffer("pos", objVao, 3);

	// generate normal buffer
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(cy::Vec3f), &normals[0], GL_STATIC_DRAW);
	objProgram.SetAttribBuffer("norm", normalbuffer, 3);

	// generate textures buffer
	GLuint textureBuffer;
	glGenBuffers(1, &textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(cy::Vec3f), &textures[0], GL_STATIC_DRAW);
	objProgram.SetAttribBuffer("tex", textureBuffer, 3);

	cyGLTexture2D texID;
	texID.Initialize();
	texID.SetImage(&image[0], 4, texWidth, texHeight);
	texID.BuildMipmaps();
	texID.Bind(0);
	objProgram["tex"] = 0;
}

void displayObj() {
	objProgram.Bind();
	glBindVertexArray(objVao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

void displayPlane() {
	planeProgram.Bind();
	// planeProgram["tex"] = 3;
	glBindVertexArray(planeVao);
	glEnableVertexAttribArray(0);
	// glBindBuffer(GL_ARRAY_BUFFER, planeBuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}


void myDisplay() {
	// clear the viewport
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(objProgram.GetID());

	// OpenGL drawing calls here
	glClearColor(0, 0, 0, 0); // set background color

	createLighting();

	createMatrices();

	displayObj();

	displayPlane();

	// swap buffers
	glutSwapBuffers();
}

void myKeyboard(unsigned char key, int x, int y) {
	// Handle keyboard input here
	switch (key) {
	case 27: // ESC
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

void keyCallback(int key, int x, int y) {
	switch (key) {
	case 100: // left
		inc -= 0.1f;
		break;
	case 102: // right
		inc += 0.1f;
		break;
	}
	std::cout << inc << std::endl;
	glutPostRedisplay();
	myDisplay();
}

void myMouse(int button, int state, int x, int y) {
	// Handle mouse buttons here
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		leftMouseDown = true;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		leftMouseDown = false;
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		rightMouseDown = true;
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		rightMouseDown = false;
}
void myMouseMotion(int x, int y) {
	if (leftMouseDown == true) {
		// update MVP matrix
		if (y > prevY) xRot += 2;
		if (y < prevY) xRot -= 2;
		if (x > prevX) yRot += 2;
		if (x < prevX) yRot -= 2;

		createMatrices();

		glutPostRedisplay();
		prevX = x;
		prevY = y;
	}
	else if (rightMouseDown == true) {
		// update MVP matrix
		if (y > prevY) scale += 0.01;
		if (y < prevY) scale -= 0.01;
		if (x > prevX) scale += 0.01;
		if (x < prevX) scale -= 0.01;
		createMatrices();

		glutPostRedisplay();
		prevX = x;
		prevY = y;
	}
}

int main(int argc, char** argv) {
	// Initializations
	glutInit(&argc, argv); // Initialize GLUT
	glutInitContextFlags(GLUT_DEBUG);
	// Create a window
	glutInitWindowSize(1080, 1080);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutCreateWindow("Final Project: Watercolor Shading/ Silhouette Shading");
	glEnable(GL_DEPTH_TEST);

	glewInit();
	CY_GL_REGISTER_DEBUG_CALLBACK;

	// load teapot
	bool success = mesh.LoadFromFileObj("lion/Loewe_C.obj");

	// cy::Vec3f textureColor = cy::Vec3f(mesh.M(0).Kd[0], mesh.M(0).Kd[1], mesh.M(0).Kd[2]);
	// objProgram["color"] = textureColor;

	// decode
	if (lodepng::decode(image, texWidth, texHeight, "wood.png"))
		return 0;

	if (!objProgram.BuildFiles("shader.vert", "shader.frag"))
		return 0;

	if (!planeProgram.BuildFiles("shader_plane.vert", "shader_plane.frag"))
		return 0;

	// Create Vertex Array Object
	glGenVertexArrays(1, &objVao);
	glBindVertexArray(objVao);



	int numFaces = mesh.NF();
	for (int i = 0; i < numFaces; i++) {
		cy::TriMesh::TriFace face = mesh.F(i);
		cy::TriMesh::TriFace normalFace = mesh.FN(i);
		cy::TriMesh::TriFace textureFace = mesh.FT(i);
		for (int i = 0; i < 3; i++) {
			cy::Vec3f v = mesh.V(face.v[i]);
			cy::Vec3f n = mesh.VN(normalFace.v[i]);
			cy::Vec3f t = mesh.VT(textureFace.v[i]);
			vertices.push_back(v);
			normals.push_back(n);
			textures.push_back(t);
		}
	}

	mesh.ComputeNormals();

	createMatrices();
	createLighting();
	createObj();
	createPlane();

	createMatrices();

	objProgram.Bind();

	// register display callback function
	glutDisplayFunc(myDisplay);
	// register keyboard callback function
	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(keyCallback);
	// register mouse callback function
	glutMouseFunc(myMouse);
	// register mouse motion callback
	glutMotionFunc(myMouseMotion);
	// set background color
	glClearColor(0, 0, 0, 0);
	// call main loop
	glutMainLoop();

	return 0;
}