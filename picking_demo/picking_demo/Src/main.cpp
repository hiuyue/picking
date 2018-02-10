
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

#include "fbo.h"
#include "shader.h"

using namespace std;

// Global
int window_width = 512;
int window_height = 512;

glm::mat4 model_mat = glm::mat4(1.0);
glm::mat4 view_mat = glm::mat4(1.0);
glm::mat4 proj_mat = glm::mat4(1.0);


class Model;

// Picking 
GLuint picking_ModeldLocation;
GLuint picking_ViewdLocation;
GLuint picking_ProjLocation;
GLuint picking_IndexLocation;
GLuint picking_isbuttonLocation;

FBO* picking_fbo = NULL;
GLuint picking_programe;


// Scene
vector<Model*> models;
GLuint scene_programe = 0;

GLuint scene_ModeldLocation;
GLuint scene_ViewdLocation;
GLuint scene_ProjLocation;
GLuint scene_SelectedLocation;
GLuint scene_isbuttonLocation;

// Mouse
static bool mouse_down = false;
int mouse_key = -1;
int mouse_x = 0;
int mouse_y = 0;


const float pCubeVertices[] =
{
	-0.5, 0.5, -0.5, 0.5, -0.5, -0.5,
	-0.5, -0.5, -0.5, -0.5, 0.5, -0.5,
	0.5, 0.5, -0.5, 0.5, -0.5, -0.5,
	-0.5, 0.5, 0.5, -0.5, -0.5, -0.5,
	-0.5, -0.5, 0.5, -0.5, 0.5, 0.5,
	-0.5, 0.5, -0.5, -0.5, -0.5, -0.5,
	-0.5, 0.5, 0.5, 0.5, 0.5, -0.5,
	-0.5, 0.5, -0.5, -0.5, 0.5, 0.5,
	0.5, 0.5, 0.5, 0.5, 0.5, -0.5,
	0.5, 0.5, -0.5, 0.5, -0.5, 0.5,
	0.5, -0.5, -0.5, 0.5, 0.5, -0.5,
	0.5, 0.5, 0.5, 0.5, -0.5, 0.5,
	0.5, 0.5, 0.5, -0.5, -0.5, 0.5,
	0.5, -0.5, 0.5, 0.5, 0.5, 0.5,
	-0.5, 0.5, 0.5, -0.5, -0.5, 0.5,
	-0.5, -0.5, -0.5, 0.5, -0.5, 0.5,
	-0.5, -0.5, 0.5, -0.5, -0.5, -0.5,
	0.5, -0.5, -0.5, 0.5, -0.5, 0.5,
};

class Model
{
public:
	Model() 
		: selected_(false)
		, button_(false)
	{ 
		index_ = ++count;
	}

	virtual void Draw() = 0;
	virtual void SelectCB() = 0;

	int GetIndex() { return index_; }
	bool IsSelect() { return selected_; }
	bool IsButton() { return button_; }
	void SetGeometry(int x, int y, int w, int h)
	{
		geom_ = glm::vec4(x, y, w, h);
	}

protected:
	static int count;
	bool selected_;
	int index_;
	bool button_;
	GLuint vbo_;
	glm::vec4 geom_;
};

int Model::count = 0;

class Cube: public Model
{
public:

	Cube()
	{
		CreateVertexBuffer();
	}
	~Cube()
	{
		glDeleteBuffers(1, &vbo_);
	}
	virtual void Draw()
	{
		//update geometry
		int i, cubes;
		
		for (i = 0; i < models.size(); i++)
		{
			if (models[i]->GetIndex() == index_)
			{
				break;
			}
		}
		i -= 1;
		cubes = models.size() - 1;

		geom_.z = window_width / cubes;
		geom_.w = window_height / cubes;
		geom_.x = i*geom_.z;
		geom_.y = window_height / 2 - geom_.w / 2;

		glViewport(geom_.x, window_height - geom_.y - geom_.w, geom_.z, geom_.w);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDisableVertexAttribArray(0);

		glViewport(0, 0, window_width, window_height);
	}

	virtual void SelectCB()
	{
		selected_ = !selected_;
	}
private:

	void CreateVertexBuffer()
	{
		glm::vec3 Vertices[36];
		for (int i = 0; i < 36; i++)
		{
			Vertices[i] = glm::vec3(pCubeVertices[i * 3], pCubeVertices[i * 3 + 1], pCubeVertices[i * 3 + 2]);
		}

		glGenBuffers(1, &vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	}

};

class Button : public Model
{
public:

	Button(int x, int y, int w, int h)
	{		
		geom_ = glm::vec4(x, y, w, h);
		CreateVertexBuffer();
		button_ = true;
	}
	~Button()
	{
		glDeleteBuffers(1, &vbo_);
	}
	virtual void Draw()
	{

		glViewport(geom_.x, window_height - geom_.y - geom_.w, geom_.z, geom_.w);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
		if (selected_)
		{
			selected_ = !selected_;
		}
		glViewport(0, 0, window_width, window_height);
	}

	virtual void SelectCB()
	{
		selected_ = !selected_;
		models.push_back(new Cube());
	}
private:
	void CreateVertexBuffer()
	{
		glm::vec3 Vertices[6];
		Vertices[0] = glm::vec3(-1.0, 1.0, -1.0);
		Vertices[1] = glm::vec3(-1.0, -1.0, -1.0);
		Vertices[2] = glm::vec3(1.0, -1.0, -1.0);
		

		Vertices[3] = glm::vec3(-1.0, 1.0, -1.0);
		Vertices[4] = glm::vec3(1.0, -1.0, -1.0);
		Vertices[5] = glm::vec3(1.0, 1.0, -1.0);

		glGenBuffers(1, &vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	}

};

void UpdateMatrix()
{
	static double count = 0;
	count ++;

	model_mat = glm::rotate<float>(model_mat, 0.1, glm::vec3(1, 1, 1));
	view_mat = glm::lookAt(glm::vec3(0, 0, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	proj_mat = glm::perspective<float>(120.0, window_width/window_height, 0.1, 100.0f);
}

static void InitPicking(int w, int h)
{
	picking_fbo = new FBO();
	picking_fbo->Init(w, h);

	picking_programe = CompileShaders("picking.vs", "picking.fs");

	picking_ModeldLocation = glGetUniformLocation(picking_programe, "mp_M");
	picking_ViewdLocation = glGetUniformLocation(picking_programe, "mp_V");
	picking_ProjLocation = glGetUniformLocation(picking_programe, "mp_P");
	picking_isbuttonLocation = glGetUniformLocation(picking_programe, "isbutton");

	picking_IndexLocation = glGetUniformLocation(picking_programe, "uModelIndex");

}

static void DestroyPicking()
{
	if (picking_fbo)
	{
		delete picking_fbo;
		picking_fbo = NULL;
	}
}

static void RenderPicking()
{
	glUseProgram(picking_programe);

	picking_fbo->Bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < models.size(); i++)
	{
		glUniform1ui(picking_IndexLocation, models[i]->GetIndex());
		glUniform1ui(picking_isbuttonLocation, (int)models[i]->IsButton());
		glUniformMatrix4fv(picking_ModeldLocation, 1, GL_FALSE, &model_mat[0][0]);
		glUniformMatrix4fv(picking_ViewdLocation, 1, GL_FALSE, &view_mat[0][0]);
		glUniformMatrix4fv(picking_ProjLocation, 1, GL_FALSE, &proj_mat[0][0]);
		models[i]->Draw();
	}

	picking_fbo->Unbind();

	glUseProgram(0);
}

static void InitScene()
{
	if (!scene_programe)
	{	
		scene_programe = CompileShaders("scene.vs", "scene.fs");
		scene_SelectedLocation = glGetUniformLocation(scene_programe, "uSelected");
		scene_isbuttonLocation = glGetUniformLocation(scene_programe, "isbutton");

		scene_ModeldLocation = glGetUniformLocation(scene_programe, "mp_M");
		scene_ViewdLocation = glGetUniformLocation(scene_programe, "mp_V");
		scene_ProjLocation = glGetUniformLocation(scene_programe, "mp_P");


		glUseProgram(scene_programe);
	}
}

static void RrenderScene()
{
	glUseProgram(scene_programe);
	
	//glViewport(100, 100, 100, 100);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < models.size(); i++)
	{
		glUniform1ui(scene_SelectedLocation, (int)models[i]->IsSelect());
		glUniform1ui(scene_isbuttonLocation, (int)models[i]->IsButton());
		glUniformMatrix4fv(scene_ModeldLocation, 1, GL_FALSE, &model_mat[0][0]);
		glUniformMatrix4fv(scene_ViewdLocation, 1, GL_FALSE, &view_mat[0][0]);
		glUniformMatrix4fv(scene_ProjLocation, 1, GL_FALSE, &proj_mat[0][0]);

		models[i]->Draw();
	}

	glUseProgram(0);
}

static void RenderSceneCB()
{
	int delete_model_index = 0;
	if (mouse_down)
	{
		RenderPicking();

		int model_index = (int)picking_fbo->ReadPixel(mouse_x, window_height - mouse_y - 1);
		for (int i = 0; i < models.size(); i++)
		{
			if (models[i]->GetIndex() == model_index)
			{
				if (mouse_key == GLUT_LEFT_BUTTON)
				{
					models[i]->SelectCB();
				}
				else if (mouse_key == GLUT_RIGHT_BUTTON)
				{
					delete_model_index = i;
				}
			}
		}
		mouse_down = false;
	}
	
	RrenderScene();

	glutSwapBuffers();

	if (delete_model_index > 0)
	{
		models.erase(models.begin() + delete_model_index);
	}
	//printf("swap \n");
}

static void MouseCB(int Button, int State, int x, int y)
{
	if (State == GLUT_DOWN)
	{
		mouse_down = true;
		mouse_key = Button;
		mouse_x = x;
		mouse_y = y;
	}

	printf("mouse key:%d x:%d y:%d down:%d \n", Button, x, y, State);
}
void KeyCB(unsigned char, int, int)
{
	exit(1);
}
static void RenderSceneTimerCB(int in)
{
	UpdateMatrix();

	RenderSceneCB();

	glutTimerFunc(30, RenderSceneTimerCB, 1);
}

void resize(int width, int height) {
	// we ignore the params and do
	printf("window w:%d h:%d \n", width, height);
	window_width = width;
	window_height = height;

	DestroyPicking();
	InitPicking(window_width, window_height);
	InitScene();

	glViewport(0, 0, window_width, window_height);
	
}

static void InitializeGlutCallbacks()
{
	glutDisplayFunc(RenderSceneCB);
	//glutIdleFunc(RenderSceneCB);
	glutTimerFunc(33, RenderSceneTimerCB, 1);
	glutMouseFunc(MouseCB);
	glutReshapeFunc(resize);
	glutKeyboardFunc(KeyCB);
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	window_width = 1366;
	window_height = 768;
	glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Picking Demo");
	glutFullScreen();

    InitializeGlutCallbacks();

	// Must be done after glut is initialized!
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	printf("GL version: %s\n", glGetString(GL_VERSION));

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH);
	glEnable(GL_CULL_FACE);
		
	models.push_back(new Button(100, 100, 100, 50));
	models.push_back(new Cube());
	

	glutMainLoop();
    
    return 0;
}