#include<iostream>

#include "VertexArray.h"
//#include "Shader.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Camera.h"
#include "ShaderModel.h"

#include"ModelMesh.h"

#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/ext/matrix_clip_space.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "openCV2/core.hpp"
#include <opencv2/highgui.hpp>
//#include <GL/glew.h>

enum Attrib_IDs { vPosition = 0, vColor = 1, vCoord = 2, vNormal = 3, vMatId = 4 };

int TriNum;
int window_width, window_height;

Shader shader2;// ("shadowMap.vert", "shadowMap.frag");
Shader shader3; //("shadowMapScene.vert", "ShadowMapScene.frag");

float fov = glm::radians(45.f);
float aspect_ratio = 4.0f / 3.0f;
float zNear = 0.1f;
float zFar = 100.f;
glm::vec3 lookFrom(0.f, 1.f, 5.f);
glm::vec3 lookAt(0.f, 1.f, 0.f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

GLuint depth_fbo, depth_texture, color_fbo, color_texture;
GLuint ground_vao, ground_vbo;

Camera cam;

glm::mat4 projMatrix = cam.getProjMatrix();
glm::mat4 viewMatrix = cam.getViewMatrix();

glm::mat4 ModelS(1.0f);
glm::mat4 ModelR(1.0f);
glm::mat4 ModelT(1.0f);
glm::mat4 Model(1.0f);

glm::mat4 MVPMatrix;
glm::mat4 MVMatrix;

glm::mat3 normalMatrix(1.0f);

glm::vec3 Ambient(1.0f, 1.0f, 1.0f);
glm::vec3 LightColor(5.0f, 5.0f, 5.0f);

glm::vec3 LightPosition(2.0f, 1.0f, -5.0f);

GLuint vao3_3;
const glm::vec3 EyeDirection = cam.lookFrom - cam.lookAt;
glm::vec3 ConeDirection(-1.0f, -0.3f, 1.0f);
const float Shininess = 5.f;
const float Strength = 1.0f;
//const float ConstantAttenuation = 0.0f;
//const float LinearAttenuation = 0.0f;
//const float QuadraticAttenuation = 0.3f;
//const float SpotCosCutoff = cos(glm::radians(75.f));
//const float SpotExponent = 1.0f;

//LightProperties light(true, true, true, Ambient, LightColor, LightPosition, glm::vec3(0, 0, 0),
//	ConeDirection, SpotCosCutoff, SpotExponent,
//	ConstantAttenuation, LinearAttenuation, QuadraticAttenuation);
glm::vec3 emission(0.1f, 0.2f, 0.0f);
glm::vec3 MatAmbient(0.2f, 0.2f, 0.6f);
glm::vec3 MatDiffuse(0.3f, 0.8f, 0.2f);
glm::vec3 MatSpecular(1.0f, 1.0f, 1.0f);
float MatShininess = 10.0f;
MaterialProperities material(emission, MatAmbient, MatDiffuse, MatSpecular, MatShininess);

void test() {
	ModelMesh modelMesh;
	modelMesh.readObjStf("bunny.obj");

	int vtxNum = modelMesh.vtx.size();
	int trisNum = modelMesh.tris.size();
	int NrmNum = modelMesh.vtxNrm.size();

	TriNum = trisNum;

	Vertex* vtx = new Vertex[vtxNum + 4];
	Tri* tris = new Tri[trisNum + 2];
	Normal* Nrm = new Normal[NrmNum + 4];

	std::copy(modelMesh.vtx.begin(), modelMesh.vtx.end(), vtx);
	std::copy(modelMesh.tris.begin(), modelMesh.tris.end(), tris);
	std::copy(modelMesh.vtxNrm.begin(), modelMesh.vtxNrm.end(), Nrm);

	vtx[vtxNum] = Vertex(-50.f, -0.35f, -50.f);
	vtx[vtxNum + 1] = Vertex(-50.f, -0.35f, 50.f);
	vtx[vtxNum + 2] = Vertex(50.f, -0.35f, 50.f);
	vtx[vtxNum + 3] = Vertex(50.f, -0.35f, -50.f);

	tris[trisNum] = Tri(vtxNum, vtxNum + 1, vtxNum + 2);
	tris[trisNum + 1] = Tri(vtxNum, vtxNum + 2, vtxNum + 3);

	Nrm[NrmNum] = Normal(0.f, 1.f, 0.f);
	Nrm[NrmNum + 1] = Normal(0.f, 1.f, 0.f);
	Nrm[NrmNum + 2] = Normal(0.f, 1.f, 0.f);
	Nrm[NrmNum + 3] = Normal(0.f, 1.f, 0.f);

	glm::vec4* cols = new glm::vec4[vtxNum + 4];
	fill(cols, cols + vtxNum, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	fill(cols + vtxNum, cols + vtxNum + 4, glm::vec4(1.0f));

	VertexArray vao3;
	vao3_3 = vao3.vao;
	VertexBuffer vb_position(vtx, (vtxNum + 4) * sizeof(Vertex));
	VertexBuffer vb_color(cols, (vtxNum + 4) * sizeof(glm::vec4));
	VertexBuffer vb_Nrm(Nrm, (NrmNum + 4) * sizeof(Normal));

	//GLint* MatIndex = new GLint[vtxNum];
	//fill(MatIndex, MatIndex + vtxNum, 0);
	//VertexBuffer vb_MatId(MatIndex, vtxNum * sizeof(GLint));

	VertexBufferElement v_pos((GLuint)vPosition, GL_FLOAT, 3, GL_FALSE);
	VertexBufferElement v_col((GLuint)vColor, GL_FLOAT, 4, GL_FALSE);
	VertexBufferElement v_nrm((GLuint)vNormal, GL_FLOAT, 3, GL_FALSE);
	//VertexBufferElement v_mat((GLuint)vMatId, GL_INT, 1, GL_FALSE);

	vao3.AddBuffer(vb_position, v_pos);
	vao3.AddBuffer(vb_color, v_col);
	vao3.AddBuffer(vb_Nrm, v_nrm);
	//vao3.AddBuffer(vb_MatId, v_mat);

	IndexBuffer ib(tris, trisNum + 2);
	ib.Bind();

	cam = Camera(fov, aspect_ratio, zNear, zFar, lookFrom, lookAt, up);

	projMatrix = cam.getProjMatrix();
	viewMatrix = cam.getViewMatrix();

	ModelS = glm::scale(ModelS, glm::vec3(1.f, 1.f, 1.f));
	ModelR = glm::rotate(ModelR, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
	ModelT = glm::translate(ModelT, glm::vec3(0.0f, 0.0f, 0.0f));
	Model = ModelS * ModelR * ModelT;

	MVPMatrix = projMatrix * viewMatrix * Model;
	MVMatrix = viewMatrix * Model;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			normalMatrix[i][j] = MVPMatrix[i][j];
		}
	}
	normalMatrix = glm::transpose(glm::inverse(normalMatrix));

	shader2 = Shader("shadowMap.vert", "shadowMap.frag");
	shader3 = Shader("shadowMapScene.vert", "shadowMapScene.frag");

	//glUseProgram(shader2.program);
	glUseProgram(shader3.program);
	glUniform1i(glGetUniformLocation(shader3.program, "depth_texture"), 0);

	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
		window_width, window_height, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &depth_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)depth_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
	glDrawBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		cout << "error2\n";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	static const float ground_vertices[] =
	{
		-500.0f, -0.5f, -500.0f, 1.0f,
		-500.0f, -0.5f,  500.0f, 1.0f,
		 500.0f, -0.5f,  500.0f, 1.0f,
		 500.0f, -0.5f, -500.0f, 1.0f,
	};

	static const float ground_colors[] = {
		0.0f, 0.3f, 0.3f,
		0.0f, 0.3f, 0.3f,
		0.0f, 0.3f, 0.3f,
		0.0f, 0.3f, 0.3f,
	};

	static const float ground_normals[] =
	{
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &ground_vao);
	glGenBuffers(1, &ground_vbo);
	glBindVertexArray(ground_vao);
	glBindBuffer(GL_ARRAY_BUFFER, ground_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices) + sizeof(ground_normals) + sizeof(ground_colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ground_vertices), ground_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(ground_vertices), sizeof(ground_colors), ground_colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(ground_vertices) + sizeof(ground_colors), sizeof(ground_normals), ground_normals);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)sizeof(ground_vertices));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(sizeof(ground_vertices) + sizeof(ground_colors)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(3);
}

float t = 0.0f;

void shadowDisplay() {
	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const float white[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	glClearBufferfv(GL_COLOR, 0, black);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	t += 0.001f;
	static float q = 0.0f;
	static const glm::vec3 X(1.0f, 0.0f, 0.0f);
	static const glm::vec3 Y(0.0f, 1.0f, 0.0f);
	static const glm::vec3 Z(0.0f, 0.0f, 1.0f);

	glm::vec3 light_position = glm::vec3(sinf(t * 6.0f * 3.141592f) * 30.0f, 20.0f, cosf(t * 4.0f * 3.141592f) * 30.0f);
	//cout << "light_position:    ";
	//for (int i = 0; i < 3; i++) {
	//	cout << light_position[i] << ",";
	//}
	//cout << endl;
	//cout << light_position[0] << endl;
	//light_position = glm::vec3(15.0f, 18.0f, 0.0f);

	// Setup
	const glm::mat4 scale_bias_matrix = glm::mat4(
		glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	// Matrices used when rendering from the light's position
	glm::mat4 light_view_matrix = glm::lookAt(light_position, glm::vec3(0.0f), Y);
	glm::mat4 light_projection_matrix(glm::perspective(glm::radians(60.f), aspect_ratio, zNear, zFar));
	glm::mat4 light_model_matrix(1.0f);

	// Now we render from the light's position into the depth buffer.
	// Select the appropriate program

	glUseProgram(shader2.program);
	glUniformMatrix4fv(glGetUniformLocation(shader2.program, "MVPMatrix"),
		1, GL_FALSE, glm::value_ptr(light_projection_matrix * light_view_matrix * Model));

	glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
	glViewport(0, 0, window_width, window_height);

	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	glFrontFace(GL_CCW);

	glPolygonOffset(1.0, 1.0);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0f, 4.0f);

	glBindVertexArray(vao3_3);
	glDrawElements(GL_TRIANGLES, 3 * TriNum, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glBindVertexArray(ground_vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);
	glDisable(GL_POLYGON_OFFSET_FILL);

	//cv::Mat M(window_height, window_width, CV_8UC3, cv::Scalar::all(255));
	//GLfloat* pix = new GLfloat[window_width * window_height];
	////glReadBuffer(GL_DEPTH_BUFFER);
	//glReadPixels(0, 0, window_width, window_height, GL_DEPTH_COMPONENT, GL_FLOAT, pix);
	//int index = 0;
	//while (index < window_width * window_height) {
	//	GLfloat b = pix[index];
	//	//cout << b << endl;
	//	GLubyte bb = b * 255.9;
	//	M.at<cv::Vec3b>(window_height - 1 - index / window_width, index % window_width) = cv::Vec3b(bb, bb, bb);
	//	//M.at<cv::Vec3b>(window_height - 1 - index / window_width, index % window_width) = cv::Vec3b(pix[4 * index + 2], pix[4 * index + 1], pix[4 * index]);
	//	index++;
	//}
	//cv::imshow("test", M);
	//cv::waitKey(60000);
	//cv::destroyAllWindows();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window_width, window_height);

	glUseProgram(shader3.program);
	//glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUniformMatrix4fv(glGetUniformLocation(shader3.program, "model_matrix"), 1, GL_FALSE, glm::value_ptr(Model));
	glUniformMatrix4fv(glGetUniformLocation(shader3.program, "view_matrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader3.program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader3.program, "shadow_matrix"), 1, GL_FALSE, glm::value_ptr(scale_bias_matrix * light_projection_matrix * light_view_matrix));
	glUniform3fv(glGetUniformLocation(shader3.program, "light_position"), 1, glm::value_ptr(light_position));
	glUniform3fv(glGetUniformLocation(shader3.program, "material_ambient"), 1, glm::value_ptr(MatAmbient));
	glUniform3fv(glGetUniformLocation(shader3.program, "material_diffuse"), 1, glm::value_ptr(MatDiffuse));
	glUniform3fv(glGetUniformLocation(shader3.program, "material_specular"), 1, glm::value_ptr(MatSpecular));
	glUniform1f(glGetUniformLocation(shader3.program, "material_specular_power"), MatShininess);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindVertexArray(vao3_3);
	glDrawElements(GL_TRIANGLES, 3 * TriNum, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glBindVertexArray(ground_vao);
	//glUseProgram(shader3.program);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);
}
int main() {
	glfwInit();
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_MULTISAMPLE);

	glfwWindowHint(GLFW_SAMPLES, 8);
	GLFWwindow* window = glfwCreateWindow(640, 480, "Triangles", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//OpenGL主版本号 4
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);//OpenGL副版本号 .5
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//OpenGL模式 OpenGL核心模式

	gl3wInit();

	//loadModel();
	//init();
	glEnable(GL_DEPTH_TEST);//深度测试
	glDepthFunc(GL_LEQUAL);//深度比较函数，深度小或相等的时候也渲染。

	GLint scissor_box[4];
	glEnable(GL_SCISSOR_TEST);
	glGetIntegerv(GL_SCISSOR_BOX, scissor_box);
	window_width = scissor_box[2];
	window_height = scissor_box[3];

	cout << "window_width:" << window_width << endl;
	cout << "window_height:" << window_height << endl;

	test();

	//cout << "1\n";

	while (!glfwWindowShouldClose(window)) {
		//cout << "2\n";
		shadowDisplay();
		//cout << "3\n";
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}