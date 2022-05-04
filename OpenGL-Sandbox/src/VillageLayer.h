#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>

class VillageLayer : public GLCore::Layer
{
public:
	VillageLayer();
	virtual ~VillageLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:
};