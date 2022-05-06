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
	GLCore::Utils::Shader* m_Shader;
	GLCore::Utils::OrthographicCameraController m_CameraController;

	GLuint m_QuadVA = 0, m_QuadVB = 0, m_QuadIB = 0;

	int m_Borders[2]{-320, 1600};
	float m_BirdsOffset[2]{0.0f, 0.0f}, m_BigCloudOffset[2]{ 0.0f, 0.0f }, m_SmallCloudOffset[2]{ 0.0f, 0.0f };

	float m_BigCloudSpeed = 0.1f;
	float m_SmallCloudSpeed = 0.2f;
	float m_BirdSpeed = 0.5f;
};