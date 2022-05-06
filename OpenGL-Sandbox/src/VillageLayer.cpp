#include "VillageLayer.h"

#include <vector>

using namespace GLCore;
using namespace GLCore::Utils;

struct Vec2 { float x, y; Vec2(float x, float y) : x(x), y(y) {} };
struct Vec3 { float x, y, z; Vec3(float x, float y, float z) : x(x), y(y), z(z) {} };
struct Vec4 { float x, y, z, w; Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {} };

struct Vertex
{
	Vec3 Position;
	Vec4 Color;
	Vec2 TexCoords;
	float TexID;
	Vertex(
		Vec3 pos = { 0.0f, 0.0f, 0.0f }, 
		Vec4 color = { 0.0f, 0.0f, 0.0f, 0.0f }, 
		Vec2 texCoords = { 0.0f, 0.0f }, 
		float texID = 0.0f)
		: Position(pos), Color(color), TexCoords(texCoords), TexID(texID) {}
};

VillageLayer::VillageLayer()
	: m_CameraController(16.0f / 9.0f)
{
	m_CameraController.GetCamera().SetProjection(0.0f, 1280.0f, 720.0f, 0.0f);
}

VillageLayer::~VillageLayer()
{
}

void VillageLayer::OnAttach()
{
	EnableGLDebugging();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Shader = Shader::FromGLSLTextFiles(
		"assets/shaders/test.vert.glsl",
		"assets/shaders/test.frag.glsl"
	);

	glUseProgram(m_Shader->GetRendererID());

	const size_t MaxQuadCount = 24;
	const size_t MaxVertexCount = MaxQuadCount * 4;
	const size_t MaxIndexCount = MaxQuadCount * 6;

	glCreateVertexArrays(1, &m_QuadVA);
	glBindVertexArray(m_QuadVA);

	glCreateBuffers(1, &m_QuadVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferData(GL_ARRAY_BUFFER, MaxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	// Define position (3 floats)
	glEnableVertexArrayAttrib(m_QuadVB, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));
	// Define color (4 floats)
	glEnableVertexArrayAttrib(m_QuadVB, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Color));
	// Define texture position (2 floats)
	glEnableVertexArrayAttrib(m_QuadVB, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoords));
	// Define texture ID (1 float)
	glEnableVertexArrayAttrib(m_QuadVB, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexID));

	uint32_t indices[MaxIndexCount];
	uint32_t offset = 0;
	for (size_t i = 0; i < MaxIndexCount; i += 6)
	{
		indices[i + 0] = 0 + offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;

		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = 0 + offset;

		offset += 4;
	}
	glCreateBuffers(1, &m_QuadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void VillageLayer::OnDetach()
{
	glDeleteVertexArrays(1, &m_QuadVA);
	glDeleteBuffers(1, &m_QuadVB);
	glDeleteBuffers(1, &m_QuadIB);
}

void VillageLayer::OnEvent(Event& event)
{
	// Events here
}

static void SetUniformMat4(uint32_t shader, const char* name, const glm::mat4& matrix)
{
	int location = glGetUniformLocation(shader, name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

static void SetUniformVec4(uint32_t shader, const char* name, const glm::vec4& vec)
{
	int location = glGetUniformLocation(shader, name);
	glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

static Vertex* CreateQuad(
	Vertex* target,
	const Vec2& pos,
	const float textureID,
	const Vec2& size = { 1.0f, 1.0f },
	const Vec4& color1 = { 0.8f, 0.2f, 0.3f, 1.0f },
	const Vec4& color2 = { 0.8f, 0.2f, 0.3f, 1.0f },
	const Vec4& color3 = { 0.8f, 0.2f, 0.3f, 1.0f },
	const Vec4& color4 = { 0.8f, 0.2f, 0.3f, 1.0f }
)
{
	target->Position = { pos.x, pos.y, 0.0f };
	target->Color = color1;
	target->TexCoords = { 0.0f, 0.0f };
	target->TexID = textureID;
	target++;

	target->Position = { pos.x + size.x, pos.y, 0.0f };
	target->Color = color2;
	target->TexCoords = { 1.0f, 0.0f };
	target->TexID = textureID;
	target++;

	target->Position = { pos.x + size.x, pos.y + size.y, 0.0f };
	target->Color = color3;
	target->TexCoords = { 1.0f, 1.0f };
	target->TexID = textureID;
	target++;

	target->Position = { pos.x, pos.y + size.y, 0.0f };
	target->Color = color4;
	target->TexCoords = { 0.0f, 1.0f };
	target->TexID = textureID;
	target++;

	return target;
}

static float KeepLocationWithinBounds(float& val, float min, float max)
{
	if (val > max)
	{
		val = min;
		return min;
	}
	else if (val < min)
	{
		val = max;
		return max;
	}
	return val;
}

void VillageLayer::OnUpdate(Timestep ts)
{
	m_CameraController.OnUpdate(ts);
	
	// Vertices.
	// Keep in mind that the window is 1280x720 (0.0, 0.0 is TOP LEFT corner)
	float vertices[] = {
		// Tree
		// Leaves - Square 1 (Rotated)
		125.0f,	490.0f,	0.0f, 0.20f, 0.60f, 0.18f, 1.0f, 0.0f, 0.0f, 0.0f,
		195.0f,	415.0f,	0.0f, 0.20f, 0.60f, 0.18f, 1.0f, 0.0f, 0.0f, 0.0f,
		125.0f,	340.0f,	0.0f, 0.20f, 0.60f, 0.18f, 1.0f, 0.0f, 0.0f, 0.0f,
		55.0f,	415.0f,	0.0f, 0.20f, 0.60f, 0.18f, 1.0f, 0.0f, 0.0f, 0.0f,
		// Leaves - Square 2 (Straight)
		80.0f,	470.0f,	0.0f, 0.20f, 0.60f, 0.18f, 1.0f, 0.0f, 0.0f, 0.0f,
		170.0f,	470.0f,	0.0f, 0.20f, 0.60f, 0.18f, 1.0f, 0.0f, 0.0f, 0.0f,
		170.0f,	380.0f,	0.0f, 0.20f, 0.60f, 0.18f, 1.0f, 0.0f, 0.0f, 0.0f,
		80.0f,	380.0f,	0.0f, 0.20f, 0.60f, 0.18f, 1.0f, 0.0f, 0.0f, 0.0f,
		// Trunk
		110.0f,	540.0f,	0.0f, 0.70f, 0.50f, 0.30f, 1.0f, 0.0f, 0.0f, 0.0f,
		140.0f,	540.0f,	0.0f, 0.70f, 0.50f, 0.30f, 1.0f, 0.0f, 0.0f, 0.0f,
		140.0f,	470.0f,	0.0f, 0.70f, 0.50f, 0.30f, 1.0f, 0.0f, 0.0f, 0.0f,
		110.0f,	470.0f,	0.0f, 0.70f, 0.50f, 0.30f, 1.0f, 0.0f, 0.0f, 0.0f,

		// The house
		// The door
		830.0f,	550.0f,	0.0f, 0.70f, 0.50f, 0.30f, 1.0f, 0.0f, 0.0f, 0.0f,
		910.0f,	550.0f,	0.0f, 0.70f, 0.50f, 0.30f, 1.0f, 0.0f, 0.0f, 0.0f,
		910.0f,	430.0f,	0.0f, 0.70f, 0.50f, 0.30f, 1.0f, 0.0f, 0.0f, 0.0f,
		830.0f,	430.0f,	0.0f, 0.70f, 0.50f, 0.30f, 1.0f, 0.0f, 0.0f, 0.0f,
		// Window - front
		940.0f,	490.0f,	0.0f, 0.40f, 0.45f, 0.60f, 1.0f, 0.0f, 0.0f, 0.0f,
		995.0f,	490.0f,	0.0f, 0.40f, 0.45f, 0.60f, 1.0f, 0.0f, 0.0f, 0.0f,
		995.0f,	440.0f,	0.0f, 0.40f, 0.45f, 0.60f, 1.0f, 0.0f, 0.0f, 0.0f,
		940.0f,	440.0f,	0.0f, 0.40f, 0.45f, 0.60f, 1.0f, 0.0f, 0.0f, 0.0f,
		// Window - side
		1060.0f,500.0f,	0.0f, 0.40f, 0.45f, 0.60f, 1.0f, 0.0f, 0.0f, 0.0f,
		1110.0f,500.0f,	0.0f, 0.40f, 0.45f, 0.60f, 1.0f, 0.0f, 0.0f, 0.0f,
		1110.0f,410.0f,	0.0f, 0.40f, 0.45f, 0.60f, 1.0f, 0.0f, 0.0f, 0.0f,
		1060.0f,410.0f,	0.0f, 0.40f, 0.45f, 0.60f, 1.0f, 0.0f, 0.0f, 0.0f,
		// Wall - front
		720.0f,	550.0f,	0.0f, 0.85f, 0.85f, 0.85f, 1.0f, 0.0f, 0.0f, 0.0f,
		1020.0f,550.0f,	0.0f, 0.85f, 0.85f, 0.85f, 1.0f, 0.0f, 0.0f, 0.0f,
		1020.0f,350.0f,	0.0f, 0.85f, 0.85f, 0.85f, 1.0f, 0.0f, 0.0f, 0.0f,
		720.0f,	350.0f,	0.0f, 0.85f, 0.85f, 0.85f, 1.0f, 0.0f, 0.0f, 0.0f,
		// Wall - side
		1020.0f,550.0f,	0.0f, 0.55f, 0.55f, 0.55f, 1.0f, 0.0f, 0.0f, 0.0f,
		1150.0f,550.0f,	0.0f, 0.55f, 0.55f, 0.55f, 1.0f, 0.0f, 0.0f, 0.0f,
		1150.0f,350.0f,	0.0f, 0.55f, 0.55f, 0.55f, 1.0f, 0.0f, 0.0f, 0.0f,
		1020.0f,350.0f,	0.0f, 0.55f, 0.55f, 0.55f, 1.0f, 0.0f, 0.0f, 0.0f,
		// Roof - front
		720.0f,	350.0f,	0.0f, 0.40f, 0.45f, 0.60f, 1.0f, 0.0f, 0.0f, 0.0f,
		1020.0f,350.0f,	0.0f, 0.40f, 0.45f, 0.60f, 1.0f, 0.0f, 0.0f, 0.0f,
		1090.0f,240.0f,	0.0f, 0.40f, 0.45f, 0.60f, 1.0f, 0.0f, 0.0f, 0.0f,
		790.0f,	240.0f,	0.0f, 0.40f, 0.45f, 0.60f, 1.0f, 0.0f, 0.0f, 0.0f,
		// Roof - front (it's a triangle, but the last vertex is duplicated
		// to not break the format)
		1020.0f,350.0f,	0.0f, 0.25f, 0.25f, 0.35f, 1.0f, 0.0f, 0.0f, 0.0f,
		1150.0f,350.0f,	0.0f, 0.25f, 0.25f, 0.35f, 1.0f, 0.0f, 0.0f, 0.0f,
		1090.0f,240.0f,	0.0f, 0.25f, 0.25f, 0.35f, 1.0f, 0.0f, 0.0f, 0.0f,
		1090.0f,240.0f,	0.0f, 0.25f, 0.25f, 0.35f, 1.0f, 0.0f, 0.0f, 0.0f,

		// Ground
		0.0f,	720.0f,	0.0f, 0.5f, 0.75f, 0.3f, 1.0f, 0.0f, 0.0f, 0.0f,
		1280.0f,720.0f,	0.0f, 0.5f, 0.75f, 0.3f, 1.0f, 0.0f, 0.0f, 0.0f,
		1280.0f,480.0f,	0.0f, 0.5f, 0.75f, 0.3f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f,	480.0f,	0.0f, 0.5f, 0.75f, 0.3f, 1.0f, 0.0f, 0.0f, 0.0f,

		// Far ground
		0.0f,	720.0f,	0.0f,	0.8f, 0.9f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		1280.0f,720.0f,	0.0f,	0.8f, 0.9f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		1280.0f,312.0f,	0.0f,	0.8f, 0.9f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f,	312.0f,	0.0f,	0.8f, 0.9f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,

		// Cloud - Small
		m_SmallCloudOffset[0] + 0.0f, m_SmallCloudOffset[1] + 120.0f, 0.0f, 0.95f, 0.95f, 0.95f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_SmallCloudOffset[0] + 165.0f, m_SmallCloudOffset[1] + 120.0f, 0.0f, 0.95f, 0.95f, 0.95f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_SmallCloudOffset[0] + 165.0f, m_SmallCloudOffset[1] + 75.0f, 0.0f, 0.95f, 0.95f, 0.95f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_SmallCloudOffset[0] + 0.0f, m_SmallCloudOffset[1] + 75.0f, 0.0f, 0.95f, 0.95f, 0.95f, 1.0f, 0.0f, 0.0f, 0.0f,

		// Birds
		// Bird 1
		m_BirdsOffset[0] + 165.0f, m_BirdsOffset[1] + 120.0f, 0.0f, 0.95f, 0.47f, 0.43f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_BirdsOffset[0] + 165.0f, m_BirdsOffset[1] + 70.0f, 0.0f, 0.99f, 0.50f, 0.47f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_BirdsOffset[0] + 190.0f, m_BirdsOffset[1] + 90.0f, 0.0f, 0.95f, 0.47f, 0.43f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_BirdsOffset[0] + 105.0f, m_BirdsOffset[1] + 90.0f, 0.0f, 0.95f, 0.47f, 0.43f, 1.0f, 0.0f, 0.0f, 0.0f,
		// Bird 3
		m_BirdsOffset[0] + 115.0f, m_BirdsOffset[1] + 71.0f, 0.0f, 0.95f, 0.47f, 0.43f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_BirdsOffset[0] + 115.0f, m_BirdsOffset[1] + 35.0f, 0.0f, 0.99f, 0.50f, 0.47f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_BirdsOffset[0] + 135.0f, m_BirdsOffset[1] + 50.0f, 0.0f, 0.95f, 0.47f, 0.43f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_BirdsOffset[0] + 85.0f, m_BirdsOffset[1] + 50.0f, 0.0f, 0.95f, 0.47f, 0.43f, 1.0f, 0.0f, 0.0f, 0.0f,
		// Bird 2
		m_BirdsOffset[0] + 40.0f, m_BirdsOffset[1] + 90.0f, 0.0f, 0.95f, 0.47f, 0.43f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_BirdsOffset[0] + 40.0f, m_BirdsOffset[1] + 55.0f, 0.0f, 0.99f, 0.50f, 0.47f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_BirdsOffset[0] + 60.0f, m_BirdsOffset[1] + 70.0f, 0.0f, 0.95f, 0.47f, 0.43f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_BirdsOffset[0] + 0.0f, m_BirdsOffset[1] + 70.0f, 0.0f, 0.95f, 0.47f, 0.43f, 1.0f, 0.0f, 0.0f, 0.0f,

		// Cloud - Big
		m_BigCloudOffset[0] + 0.0f, m_BigCloudOffset[1] + 100.0f, 0.0f, 0.95f, 0.95f, 0.95f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_BigCloudOffset[0] + 245.0f, m_BigCloudOffset[1] + 100.0f, 0.0f, 0.95f, 0.95f, 0.95f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_BigCloudOffset[0] + 245.0f, m_BigCloudOffset[1] + 20.0f, 0.0f, 0.95f, 0.95f, 0.95f, 1.0f, 0.0f, 0.0f, 0.0f,
		m_BigCloudOffset[0] + 0.0f, m_BigCloudOffset[1] + 20.0f, 0.0f, 0.95f, 0.95f, 0.95f, 1.0f, 0.0f, 0.0f, 0.0f,

		// Sun - straight
		72.0f, 40.0f, 0.0f, 1.00f, 0.83f, 0.47f, 1.0f, 0.0f, 0.0f, 0.0f,
		152.0f, 40.0f, 0.0f, 1.00f, 0.83f, 0.47f, 1.0f, 0.0f, 0.0f, 0.0f,
		152.0f, 120.0f, 0.0f, 1.00f, 0.83f, 0.47f, 1.0f, 0.0f, 0.0f, 0.0f,
		72.0f, 120.0f, 0.0f, 1.00f, 0.83f, 0.47f, 1.0f, 0.0f, 0.0f, 0.0f,

		// Sky
		0.0f,	720.0f,	0.0f, 0.73f, 0.84f, 0.83f, 1.0f, 0.0f, 0.0f, 0.0f,
		1280.0f,720.0f,	0.0f, 0.73f, 0.84f, 0.83f, 1.0f, 0.0f, 0.0f, 0.0f,
		1280.0f,0.0f,	0.0f, 0.73f, 0.84f, 0.83f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f,	0.0f,	0.0f, 0.73f, 0.84f, 0.83f, 1.0f, 0.0f, 0.0f, 0.0f,
	};

	m_BigCloudOffset[0] += m_BigCloudSpeed;
	KeepLocationWithinBounds(m_BigCloudOffset[0], m_Borders[0], m_Borders[1]);
	m_SmallCloudOffset[0] += m_SmallCloudSpeed;
	KeepLocationWithinBounds(m_SmallCloudOffset[0], m_Borders[0], m_Borders[1]);
	m_BirdsOffset[0] += m_BirdSpeed;
	KeepLocationWithinBounds(m_BirdsOffset[0], m_Borders[0], m_Borders[1]);

	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Blue BG <- MAKE IT BLUE
	//glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Grey BG
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_Shader->GetRendererID());

	SetUniformMat4(m_Shader->GetRendererID(), "u_ViewProjection", m_CameraController.GetCamera().GetViewProjectionMatrix());

	glBindVertexArray(m_QuadVA);
	glDrawElements(GL_TRIANGLES, 128, GL_UNSIGNED_INT, nullptr);
}

void VillageLayer::OnImGuiRender()
{
	ImGui::Begin("Controls");
	ImGui::SliderFloat2("Bird offset", m_BirdsOffset, m_Borders[0], m_Borders[1]);
	ImGui::SliderFloat2("Small cloud offset", m_SmallCloudOffset, m_Borders[0], m_Borders[1]);
	ImGui::SliderFloat2("Big cloud offset", m_BigCloudOffset, m_Borders[0], m_Borders[1]);

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::SliderFloat("Bird speed", &m_BirdSpeed, -5.0f, 5.0f, "%.2f");
	ImGui::SliderFloat("Small cloud speed", &m_SmallCloudSpeed, -5.0f, 5.0f, "%.2f");
	ImGui::SliderFloat("Big cloud speed", &m_BigCloudSpeed, -5.0f, 5.0f, "%.2f");

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}
