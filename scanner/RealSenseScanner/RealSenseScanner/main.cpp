#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <future>

#include <windows.h>

#include <pxcsensemanager.h>
#include <pxcmetadata.h>

#include <boost/filesystem.hpp>
#include <boost/cast.hpp>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/common.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/common.hpp>
#include <glm/gtx/extend.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/normal.hpp>

#include "util_cmdline.h"
#include "util_render.h"
#include "Model.h"

#include <nanogui/nanogui.h>


using namespace std;
using namespace nanogui;

pxcCHAR meshFileName[MAX_PATH];

//struct verticies_object{
//	float Position_x;
//	float Position_y;
//	float	Position_z;
//	float	Normal_x;
//	float	Normal_y;
//	float	Normal_z;
//	float	TexCoords_x;
//	float	TexCoords_y;
//	//float	Tangent_x;
//	//float	Tangent_y;
//	//float	Tangent_z;
//	//float	Bitangent_x;
//	//float	Bitangent_y;
//	//float	Bitangent_z;
//
//};
//
//struct textures_object{
//	int id;
//	const char* type;
//};
//
//struct indicies_object{
//	int index;
//};

int scanner(){
	pxcStatus result = PXC_STATUS_NO_ERROR;
	static int SCANNING_FRAMES = 200;
	int minFramesBeforeScanStart = 200;
	int totalFramesBeforeScanStart = 200;
	pxcI32 scanningFramesRemaining = SCANNING_FRAMES;
	pxcI32 frame = 0;
	pxcI32 firstScanningFrame = 0;
	PXCSenseManager* pSenseManager = PXCSenseManager::CreateInstance();
	PXCMetadata * md = pSenseManager->QuerySession()->QueryInstance<PXCMetadata>();
	result = pSenseManager->Enable3DScan();
	result = pSenseManager->Init();
	PXCCapture::DeviceInfo device_info;
	pSenseManager->QueryCaptureManager()->QueryCapture()->QueryDeviceInfo(0, &device_info);
	wprintf_s(L"Camera: %s \nFirmware: %d.%d.%d.%d \n", device_info.name, device_info.firmware[0], device_info.firmware[1], device_info.firmware[2], device_info.firmware[3]);
	PXC3DScan* pScanner = pSenseManager->Query3DScan();
	PXCVideoModule::DataDesc videoProfile;
	result = pScanner->QueryInstance<PXCVideoModule>()->QueryCaptureProfile(PXCBase::WORKING_PROFILE, &videoProfile);
	if (result < PXC_STATUS_NO_ERROR) return result;
	else wprintf_s(L"Color: %dx%dx%0.f \nDepth: %dx%dx%0.f \n",
		videoProfile.streams.color.sizeMax.width, videoProfile.streams.color.sizeMax.height,
		videoProfile.streams.color.frameRate.max,
		videoProfile.streams.depth.sizeMax.width, videoProfile.streams.depth.sizeMax.height,
		videoProfile.streams.depth.frameRate.max);
	UtilRender window(L"Scanning");
	wprintf_s(L"Initializing...\n");
	pSenseManager->QueryCaptureManager()->QueryDevice()->SetColorAutoExposure(true);
	PXC3DScan::Configuration config = pScanner->QueryConfiguration();
	config.mode = PXC3DScan::OBJECT_ON_PLANAR_SURFACE_DETECTION;
	result = pScanner->SetConfiguration(config);
	while (scanningFramesRemaining)
	{
		if (pSenseManager->AcquireFrame(true) < PXC_STATUS_NO_ERROR) break;
		frame++;
		PXCImage* image = pScanner->AcquirePreviewImage();
		pSenseManager->ReleaseFrame();
		if (minFramesBeforeScanStart == 0){
			PXC3DScan::Configuration configAgain = pScanner->QueryConfiguration();
			config.options = config.options | PXC3DScan::TEXTURE;
			config.options = config.options | PXC3DScan::SOLIDIFICATION;
			pxcBool scanFlag = true;
			config.startScan = scanFlag;
			result = pScanner->SetConfiguration(config);
		}
		if (frame <= totalFramesBeforeScanStart || !pScanner->IsScanning() || minFramesBeforeScanStart != 0)
		{
			UtilRender::RenderProgressBar(image, frame, totalFramesBeforeScanStart, 2);
			minFramesBeforeScanStart--;
		}
		if (pScanner->IsScanning())
		{
			scanningFramesRemaining--;
			if (!firstScanningFrame)
			{
				firstScanningFrame = frame;
				wprintf_s(L"Scanning...\n");
				pSenseManager->QueryCaptureManager()->QueryDevice()->SetColorAutoExposure(false);
			}
			UtilRender::RenderProgressBar(image, frame - firstScanningFrame + 1, SCANNING_FRAMES, 1);
		}
		bool success = window.RenderFrame(image);
		image->Release();
		if (!success || GetAsyncKeyState(VK_ESCAPE)) break;
	}
	pSenseManager->QueryCaptureManager()->QueryDevice()->SetColorAutoExposure(true);
	result = PXC_STATUS_NO_ERROR;
	bool bMeshSaved = false;
	PXC3DScan::FileFormat meshFormat = PXC3DScan::OBJ;
	if (pScanner->IsScanning())
	{
		size_t unused;
		WCHAR* pUserProfilePath;
		_wdupenv_s(&pUserProfilePath, &unused, L"USERPROFILE");
		meshFormat = PXC3DScan::OBJ;
		const char* env_p = std::getenv("MESH_PATH");
		const char* model_file_name = "mesh.obj";
		char str[250];
		strcpy(str, env_p);
		strcat(str, "\\");
		strcat(str, model_file_name);
		swprintf_s(meshFileName, MAX_PATH, window.GetWC(str), pUserProfilePath, PXC3DScan::FileFormatToString(meshFormat));
		wprintf_s(L"Generating %s...\n", meshFileName);
		result = pScanner->Reconstruct(meshFormat, meshFileName);
		PXCImage* image = pScanner->AcquirePreviewImage();
		if (result >= PXC_STATUS_NO_ERROR)
		{
			bMeshSaved = true;
			wprintf_s(L"done.\n");
		}
		else if (result == PXC_STATUS_FILE_WRITE_FAILED)
		{
			wprintf_s(L"the file could not be created using the provided path. Aborting.\n");
		}
		else if (result == PXC_STATUS_ITEM_UNAVAILABLE || result == PXC_STATUS_DATA_UNAVAILABLE)
		{
			wprintf_s(L"no scan data found. Aborting.\n");
		}
		else if (result < PXC_STATUS_NO_ERROR) wprintf_s(L"error (%d)\n", result);
	}
	else wprintf_s(L"Exited before scan start.\n");
	pSenseManager->Release();
	const char* env_p = std::getenv("MESH_PATH");
	const char* model_file_name = "mesh.obj";
	char str_mesh[250];
	strcpy(str_mesh, env_p);
	strcat(str_mesh, "\\");
	strcat(str_mesh, model_file_name);
	Model obj(str_mesh);
	//Creating the lock file to be detected by the juggler applicaiton

	const char* lock_file_name = "lock.txt";
	char str[250];
	strcpy(str, env_p);
	strcat(str, "\\");
	strcat(str, lock_file_name);
	puts(str);
	HANDLE h = CreateFile(str, GENERIC_EXECUTE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (h)
	{
		std::cout << "CreateFile() succeeded\n";
		CloseHandle(h);
	}
	else
	{
		std::cerr << "CreateFile() failed:" << GetLastError() << "\n";
	}
}
//
//void create_binary(){
//	const char* env_p = std::getenv("MESH_PATH");
//	const char* model_file_name = "mesh.obj";
//	char str[250];
//	strcpy(str, env_p);
//	strcat(str, "\\");
//	strcat(str, model_file_name);
//	//char str_dat[250];
//	//strcpy(str_dat, env_p);
//	//strcat(str_dat, "\\");
//	//strcat(str_dat, "mesh.dat");
//	//char str_desc[250];
//	//strcpy(str_desc, env_p);
//	//strcat(str_desc, "\\");
//	//strcat(str_desc, "describe.txt");
//	//vector<Mesh> meshes;
//	Model obj(str);
//
//	////std::ofstream ofs(str_dat, std::ios::binary);
//	//std::ofstream out;
//	//std::ofstream desc_file;
//	//out.open(str_dat, std::ios::binary);
//	//desc_file.open(str_desc);
//	//vector<Mesh> meshes_vector = obj.meshes;
//	//	for (vector<Mesh>::iterator it = meshes_vector.begin(); it != meshes_vector.end(); ++it) {
//	//		vector<Vertex> v = it->vertices;
//	//		vector<GLuint> i = it->indices;
//	//		vector<Texture> t = it->textures;
//	//		verticies_object vertex_obj;
//	//		indicies_object index_obj;
//	//		textures_object textures_obj;
//	//		int vertex_size;
//	//		int texture_size;
//	//		int index_id;
//	//		for (std::vector<Vertex>::iterator it = v.begin(); it != v.end(); ++it) {
//	//			//ofs << it->Position.x << "," << it->Position.y << "," << it->Position.z << "," << it->Normal.x << "," << it->Normal.y << "," << it->Normal.z << "," << it->TexCoords.x << "," << it->TexCoords.y << "," << it->Tangent.x << "," << it->Tangent.y << "," << it->Tangent.z << "," << it->Bitangent.x << "," << it->Bitangent.y << "," << it->Bitangent.z << ",";
//	//			vertex_obj.Position_x = it->Position.x;
//	//			vertex_obj.Position_y = it->Position.y;
//	//			vertex_obj.Position_z = it->Position.z;
//	//			vertex_obj.Normal_x = it->Normal.x;
//	//			vertex_obj.Normal_y = it->Normal.y;
//	//			vertex_obj.Normal_z = it->Normal.z;
//	//			vertex_obj.TexCoords_x = it->TexCoords.x;
//	//			vertex_obj.TexCoords_y = it->TexCoords.y;
//	//			//data_object.Tangent_x = it->Tangent.x;
//	//			//data_object.Tangent_y = it->Tangent.y;
//	//			//data_object.Tangent_z = it->Tangent.z;
//	//			//data_object.Bitangent_x = it->Bitangent.x;
//	//			//data_object.Bitangent_y = it->Bitangent.y;
//	//			//data_object.Bitangent_z = it->Bitangent.z;
//	//			out.write(reinterpret_cast<char*>(&vertex_obj), sizeof(verticies_object));
//	//			vertex_size++;
//	//		}
//	//		for (std::vector<Texture>::iterator it = t.begin(); it != t.end(); ++it) {
//	//			//ofs << it->id << "," << it->type << "," << it->path.data << ",";
//	//			textures_obj.id = it->id;
//	//			textures_obj.type = it->type.c_str();
//	//			out.write(reinterpret_cast<char*>(&textures_obj), sizeof(textures_object));
//	//			texture_size++;
//	//		}
//	//		for (std::vector<GLuint>::iterator it = i.begin(); it != i.end(); ++it) {
//	//			//ofs << *it << endl;
//	//			index_obj.index = *it;
//	//			out.write(reinterpret_cast<char*>(&index_obj), sizeof(indicies_object));
//	//			index_id++;
//	//		}
//	//		desc_file << vertex_size << "," << texture_size << "," << index_id;
//	//		//out.write(reinterpret_cast<char*>(&data_object), sizeof(OBJECT));
//	//		
//	//	}
//	//	out.close();
//	//	desc_file.close();
//}

void start_scanner(){
	nanogui::init();
	Screen *screen = new Screen(Vector2i(200, 200), "Scanner Controller");
	bool enabled = true;
	FormHelper *gui = new FormHelper(screen);
	nanogui::ref<Window> window = gui->addWindow(Eigen::Vector2i(50, 100), "Control Panel");
	gui->addButton("Scan", [](){
		std::async(std::launch::async, &scanner);
	});

	screen->setVisible(true);
	screen->performLayout();
	window->center();
	nanogui::mainloop();
	nanogui::shutdown();
}

int main(int argc, char* argv[]){
	start_scanner();
	return 0;
}