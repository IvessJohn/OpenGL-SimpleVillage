#include "GLCore.h"
#include "VillageLayer.h"

using namespace GLCore;

class Sandbox : public Application
{
public:
	Sandbox()
	{
		PushLayer(new VillageLayer());
	}
};

int main()
{
	std::unique_ptr<Sandbox> app = std::make_unique<Sandbox>();
	app->Run();
}