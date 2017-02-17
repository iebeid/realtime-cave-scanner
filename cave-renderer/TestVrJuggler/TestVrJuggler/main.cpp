#include <iostream>

#include <vrj/Kernel/Kernel.h>

#include "App.h"

using namespace std;
using namespace vrj;

int main(int argc, char* argv[])
{	
	Kernel* kernel = Kernel::instance();
	MeshApp* application = new MeshApp();
	kernel->init(argc, argv);
	for (int i = 1; i < argc; ++i)
	{
		kernel->loadConfigFile(argv[i]);
	}
	kernel->start();
	kernel->setApplication(application);
	kernel->waitForKernelStop();
	delete application;
	return EXIT_SUCCESS;
}
