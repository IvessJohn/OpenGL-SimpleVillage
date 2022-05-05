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

	const size_t MaxQuadCount = 16;
	const size_t MaxVertexCount = MaxQuadCount * 4;
	const size_t MaxIndexCount = MaxQuadCount * 6;

	glCreateVertexArrays(1, &m_QuadVA);
	glBindVertexArray(m_QuadVA);

	// Vertices.
	// Keep in mind that the window is 1280x720
	float vertices[] = {
		// Sun

		// Cloud - Small
		375.0f, 120.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		540.0f, 120.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		540.0f, 75.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		375.0f, 75.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,

		// Cloud - Big
		505.0f, 100.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		750.0f, 100.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		750.0f, 20.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		505.0f, 20.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,

		// Birds

		// Far ground
		0.0f,	720.0f,	0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		1280.0f,720.0f,	0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		1280.0f,312.0f,	0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f,	312.0f,	0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,

		// Ground
		0.0f,	720.0f,	0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		1280.0f,720.0f,	0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		1280.0f,480.0f,	0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f,	480.0f,	0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f,

		// The house

		// Tree1
	};

	glCreateBuffers(1, &m_QuadVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferData(GL_ARRAY_BUFFER, MaxVertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);

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

void VillageLayer::OnUpdate(Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	uint32_t indexCount = 0;

	std::array<Vertex, 64> vertices;
	Vertex* buffer = vertices.data();
	//buffer = CreateQuad(buffer, )


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
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}
