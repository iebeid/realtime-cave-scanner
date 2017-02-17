
#include "App.h"

using namespace vrj;

bool initiateScan = false;

void MeshApp::bufferPreDraw(){
	const char* env_p = std::getenv("MESH_PATH");
	const char* lock_file_name = "lock.txt";
	const char* mesh_file_name = "mesh.obj";
	const char* mtl_file_name = "mesh.mtl";
	const char* jpg_file_name = "meshImage1.png";
	
	char lock_file_name_str[250];
	char mesh_file_name_str[250];
	char mtl_file_name_str[250];
	char jpg_file_name_str[250];
	
	strcpy(lock_file_name_str, env_p);
	strcat(lock_file_name_str, "\\");
	strcat(lock_file_name_str, lock_file_name);
	//puts(lock_file_name_str);

	strcpy(mesh_file_name_str, env_p);
	strcat(mesh_file_name_str, "\\");
	strcat(mesh_file_name_str, mesh_file_name);
	//puts(mesh_file_name_str);

	strcpy(mtl_file_name_str, env_p);
	strcat(mtl_file_name_str, "\\");
	strcat(mtl_file_name_str, mtl_file_name);
	//puts(mtl_file_name_str);

	strcpy(jpg_file_name_str, env_p);
	strcat(jpg_file_name_str, "\\");
	strcat(jpg_file_name_str, jpg_file_name);
	//puts(jpg_file_name_str);

	if (boost::filesystem::exists(lock_file_name_str)){ initiateScan = true; }
	if (initiateScan){
		if (boost::filesystem::exists(mesh_file_name_str) && boost::filesystem::exists(mtl_file_name_str) && boost::filesystem::exists(jpg_file_name_str)){
			cout << "Loading Scanned Object" << endl;
			Model createdObj(mesh_file_name_str);
			//v.push_back(createdObj);
			current_model = createdObj;
			v_updated = true;
		}
		boost::filesystem::remove(lock_file_name_str);
		boost::filesystem::remove(mesh_file_name_str);
		boost::filesystem::remove(mtl_file_name_str);
		boost::filesystem::remove(jpg_file_name_str);
		initiateScan = false;
	}
	UpdateTimer();
	//if (v_updated){ UpdateX(); }
	//updateGrabbing();
	//updateNavigation();
}

void MeshApp::draw()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, g_fb);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	
	vrj::opengl::DrawManager* drawMgr = boost::polymorphic_downcast<vrj::opengl::DrawManager*>(getDrawManager());
	vrj::opengl::UserData* udata = drawMgr->currentUserData();
	vrj::ProjectionPtr const  proj = udata->getProjection();
	vrj::Frustum const& frust = proj->getFrustum();

	//for (std::vector<Model>::iterator it = v.begin(); it != v.end(); ++it) {
		//Model model_in_use = *it;
		Model model_in_use = current_model;
		glm::mat4 matView = glm::make_mat4x4(proj->getViewMatrix().getData());
		glm::mat4 matProj = glm::frustum(frust[vrj::Frustum::VJ_LEFT], frust[vrj::Frustum::VJ_RIGHT], frust[vrj::Frustum::VJ_BOTTOM], frust[vrj::Frustum::VJ_TOP], frust[vrj::Frustum::VJ_NEAR], frust[vrj::Frustum::VJ_FAR]);
		
		glm::mat4 normalMatrix = glm::inverse(model_in_use.model * matView);
		model_in_use.model = glm::translate(model_in_use.model, glm::vec3(x_position, 0.0f, 0.0f));
		model_in_use.model = glm::rotate(model_in_use.model, timer010 * 360.0f, glm::vec3(0.5f, 1.0f, 1.0f));
		model_in_use.model = glm::scale(model_in_use.model, glm::vec3(10.0f, 10.0f, 10.0f));
		shader->Use();
		glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model_in_use.model));
		glUniformMatrix4fv(glGetUniformLocation(shader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(matProj));
		glUniformMatrix4fv(glGetUniformLocation(shader->Program, "view"), 1, GL_FALSE, glm::value_ptr(matView));
		glUniformMatrix4fv(glGetUniformLocation(shader->Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
		model_in_use.Draw(*shader);
	//}

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//post_shader->Use();
	//glBindVertexArray(g_ss_quad_vao);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, g_fb_tex);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
}

void MeshApp::updateGrabbing()
{
	glm::mat4 vw_M_w;
	vw_M_w = glm::inverse(current_model.model);
	glm::mat4 mWandGLM = glm::make_mat4(mWand->getData().mData);
	glm::mat4 wand_matrix = vw_M_w * mWandGLM;
	glm::vec3 wand_point = glm::make_vec3(glm::value_ptr(wand_matrix));
	//for (std::vector<Model>::iterator it = v.begin(); it != v.end(); ++it) {
		//Model current_model = *it;
		current_model.mModelIsect = current_model.isInVolume(wand_point);
		if (mGrabButton->getData() == gadget::Digital::ON)
		{
			if (current_model.mModelIsect || current_model.mModelSelected)
			{
				current_model.mModelSelected = true;
				current_model.model = glm::translate(current_model.model, wand_point);
			}
			else
			{
				current_model.mModelSelected = false;
			}
		}
		else if (mGrabButton->getData() == gadget::Digital::TOGGLE_OFF)
		{
			current_model.mModelSelected = false;
		}
	//}
}

void MeshApp::updateNavigation()
{
	glm::mat4 wand_matrix = glm::make_mat4(mWand->getData().mData);
	float velocity(0.05f);
	float rotation(0.0f);
	if (mForwardButton->getData())
	{
		glm::vec3 z_dir = glm::vec3(0.0f, 0.0f, velocity);
		glm::vec4 v(z_dir, 0);
		glm::vec4 result = wand_matrix * v;
		current_model.model = glm::translate(current_model.model, glm::vec3(result.x, result.y, result.z));
	}
	if (mRotateButton->getData())
	{
		glm::quat q = glm::quat_cast(wand_matrix);
		glm::vec3 euler = glm::eulerAngles(q) * 3.14159f / 180.f;
		glm::mat4 transform1 = glm::eulerAngleYXZ(euler.y, euler.x, euler.z);
		current_model.model = current_model.model * transform1;
	}
	if (mForwardButton->getData() && mRotateButton->getData())
	{
		this->reset();
	}
}

void MeshApp::reset(){
	//for (std::vector<Model>::iterator it = v.begin(); it != v.end(); ++it) {
		//Model current_model = *it;
		current_model.mModelIsect = false;
		current_model.mModelSelected = false;
	//}
}

void MeshApp::UpdateTimer()
{
	time1 = clock();
	float delta = static_cast<float>(static_cast<double>(time1 - time0) / static_cast<double>(CLOCKS_PER_SEC));
	delta = delta / 1500;
	time0 = clock();
	if (bUp)
	{
		timer010 += delta;
		if (timer010 >= 1.0f) { timer010 = 1.0f; bUp = false; }
	}
	else
	{
		timer010 -= delta;
		if (timer010 <= 0.0f) { timer010 = 0.0f; bUp = true; }
	}
}

void MeshApp::UpdateX()
{
	float delta = 0.2;
	float max = 5.0;
	if (x_position <= max){
		x_position = x_position + delta;
	}
}

void MeshApp::initGLState()
{
	glewExperimental = GL_TRUE;
	glewInit();
	glEnable(GL_MULTISAMPLE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);
	std::string vertex = "shader.vs";
	std::string fragment = "shader.frag";
	std::string post_vertex = "post.vs";
	std::string post_fragment = "post.frag";
	v_updated = false;
	shader = new Shader(vertex.c_str(), fragment.c_str());
	post_shader = new Shader(post_vertex.c_str(), post_fragment.c_str());
	mFramesToSleep = 25;
	time0 = clock();
	timer010 = 0.0f;
	x_position = 0.0f;
	bUp = true;
	string const path = "objects/Table.obj";
	Model ourTable(path);
	current_model = ourTable;
	//v.push_back(ourTable);
	//current_model = ourTable;

	////////FrameBuffer
	//vrj::opengl::DrawManager* drawMgr = boost::polymorphic_downcast<vrj::opengl::DrawManager*>(getDrawManager());
	//vrj::opengl::UserData* udata = drawMgr->currentUserData();
	//vrj::opengl::WindowPtr window = udata->getGlWindow();
	//window.get()->getOriginSize(x_origin, y_origin, width, height);
	//glGenFramebuffers(1, &g_fb);

	//glGenTextures(1, &g_fb_tex);
	//glBindTexture(GL_TEXTURE_2D, g_fb_tex);
	//glTexImage2D(
	//	GL_TEXTURE_2D,
	//	0,
	//	GL_RGBA,
	//	width,
	//	height,
	//	0,
	//	GL_RGBA,
	//	GL_UNSIGNED_BYTE,
	//	NULL
	//	);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//glBindFramebuffer(GL_FRAMEBUFFER, g_fb);
	//glFramebufferTexture2D(
	//	GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_fb_tex, 0
	//	);

	//GLuint rb = 0;
	//glGenRenderbuffers(1, &rb);
	//glBindRenderbuffer(GL_RENDERBUFFER, rb);
	//glRenderbufferStorage(
	//	GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height
	//	);

	//glFramebufferRenderbuffer(
	//	GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb
	//	);

	//GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, draw_bufs);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//GLfloat ss_quad_pos[] = {
	//	-1.0, -1.0,
	//	1.0, -1.0,
	//	1.0, 1.0,
	//	1.0, 1.0,
	//	-1.0, 1.0,
	//	-1.0, -1.0
	//};

	//glGenVertexArrays(1, &g_ss_quad_vao);
	//glBindVertexArray(g_ss_quad_vao);
	//GLuint vbo;
	//glGenBuffers(1, &vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(
	//	GL_ARRAY_BUFFER,
	//	sizeof(ss_quad_pos),
	//	ss_quad_pos,
	//	GL_STATIC_DRAW
	//	);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(0);
	//////////////////////////
}
