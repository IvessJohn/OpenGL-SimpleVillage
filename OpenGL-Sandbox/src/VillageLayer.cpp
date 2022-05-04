#include "VillageLayer.h"

using namespace GLCore;
using namespace GLCore::Utils;

VillageLayer::VillageLayer()
{
}

VillageLayer::~VillageLayer()
{
}

void VillageLayer::OnAttach()
{
	EnableGLDebugging();

	// Init here
}

void VillageLayer::OnDetach()
{
	// Shutdown here
}

void VillageLayer::OnEvent(Event& event)
{
	// Events here
}

void VillageLayer::OnUpdate(Timestep ts)
{
	// Render here
}

void VillageLayer::OnImGuiRender()
{
	// ImGui here
}
