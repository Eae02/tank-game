#include "imguiinterface.h"
#include "graphics/gl/shaderprogram.h"
#include "graphics/gl/shadermodule.h"
#include "graphics/gl/texture2d.h"
#include "graphics/gl/vertexarray.h"
#include "utils/ioutils.h"
#include "keyboard.h"
#include "mouse.h"
#include "updateinfo.h"

#include <memory>

namespace TankGame::ImGuiInterface
{
	static std::unique_ptr<Texture2D> theFontTexture;
	static std::unique_ptr<ShaderProgram> theGuiShader;
	static std::unique_ptr<VertexArray> theGuiVAO;
	
	static int theTextureUniformLoc = 0;
	static int theProjectionMatrixUniformLoc = 0;
	static unsigned int theGuiVBO = 0;
	static unsigned int theGuiIBO = 0;
	
	static bool isMouseCaptured = false;
	static bool isKeyboardCaptured = false;
	
	static std::string iniFilePath;
	
	void Init()
	{
		ImGui::CreateContext();
		
		ImGuiIO& io = ImGui::GetIO();
		io.KeyMap[ImGuiKey_Tab]        = (int)Key::Tab;
		io.KeyMap[ImGuiKey_LeftArrow]  = (int)Key::ArrowLeft;
		io.KeyMap[ImGuiKey_RightArrow] = (int)Key::ArrowRight;
		io.KeyMap[ImGuiKey_UpArrow]    = (int)Key::ArrowUp;
		io.KeyMap[ImGuiKey_DownArrow]  = (int)Key::ArrowDown;
		io.KeyMap[ImGuiKey_PageUp]     = (int)Key::PageUp;
		io.KeyMap[ImGuiKey_PageDown]   = (int)Key::PageDown;
		io.KeyMap[ImGuiKey_Home]       = (int)Key::Home;
		io.KeyMap[ImGuiKey_End]        = (int)Key::End;
		io.KeyMap[ImGuiKey_Space]      = (int)Key::Space;
		io.KeyMap[ImGuiKey_Delete]     = (int)Key::Delete;
		io.KeyMap[ImGuiKey_Backspace]  = (int)Key::Backspace;
		io.KeyMap[ImGuiKey_Enter]      = (int)Key::Enter;
		io.KeyMap[ImGuiKey_Escape]     = (int)Key::Escape;
		io.KeyMap[ImGuiKey_C]          = (int)Key::C;
		io.KeyMap[ImGuiKey_D]          = (int)Key::D;
		io.KeyMap[ImGuiKey_E]          = (int)Key::E;
		io.KeyMap[ImGuiKey_A]          = (int)Key::A;
		io.KeyMap[ImGuiKey_S]          = (int)Key::S;
		io.KeyMap[ImGuiKey_V]          = (int)Key::V;
		io.KeyMap[ImGuiKey_W]          = (int)Key::W;
		io.KeyMap[ImGuiKey_X]          = (int)Key::X;
		io.KeyMap[ImGuiKey_Y]          = (int)Key::Y;
		io.KeyMap[ImGuiKey_Z]          = (int)Key::Z;
		
		ImGui::StyleColorsDark(&ImGui::GetStyle());
		
		iniFilePath = (GetDataDirectory() / "imgui.ini").string();
		io.IniFilename = iniFilePath.c_str();
	}
	
	static void CreateFontsTexture()
	{
		ImGuiIO& io = ImGui::GetIO();
		unsigned char* pixels;
		int width, height;
		
		std::string uiFontFilePath = (GetResDirectory() / "fonts" / "ui.ttf").string();
		io.Fonts->AddFontFromFileTTF(uiFontFilePath.c_str(), 16);
		
		std::string monoFontFilePath = (GetResDirectory() / "fonts" / "mono.ttf").string();
		io.Fonts->AddFontFromFileTTF(monoFontFilePath.c_str(), 16);
		
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		
		theFontTexture = std::make_unique<Texture2D>(width, height, 1, GL_RGBA8);
		glTextureSubImage2D(theFontTexture->GetID(), 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		
		io.Fonts->TexID = reinterpret_cast<void*>(static_cast<intptr_t>(theFontTexture->GetID()));
	}
	
	static void CreateDeviceObjects()
	{
		auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "imgui.vs.glsl", GL_VERTEX_SHADER);
		auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "imgui.fs.glsl", GL_FRAGMENT_SHADER);
		
		theGuiShader.reset(new ShaderProgram{ &vs, &fs });
		
		theTextureUniformLoc = theGuiShader->GetUniformLocation("Texture");
		theProjectionMatrixUniformLoc = theGuiShader->GetUniformLocation("ProjMtx");
		
		glGenBuffers(1, &theGuiVBO);
		glGenBuffers(1, &theGuiIBO);
		
		theGuiVAO = std::make_unique<VertexArray>();
		theGuiVAO->Bind();
		glBindBuffer(GL_ARRAY_BUFFER, theGuiVBO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, pos));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, uv));
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, col));
	}
	
	void Close()
	{
		if (theGuiVBO)
			glDeleteBuffers(1, &theGuiVBO);
		if (theGuiIBO)
			glDeleteBuffers(1, &theGuiIBO);
		theGuiVBO = theGuiIBO = 0;
		
		theGuiVAO = nullptr;
		
		theGuiShader = nullptr;
		theFontTexture = nullptr;
	}
	
	void EndFrame()
	{
		ImGui::Render();
		
		ImDrawData* drawData = ImGui::GetDrawData();
		if (drawData->TotalIdxCount == 0)
			return;
		
		ImGuiIO& io = ImGui::GetIO();
		
		GLsizei fbWidth = static_cast<GLsizei>(io.DisplaySize.x * io.DisplayFramebufferScale.x);
		GLsizei fbHeight = static_cast<GLsizei>(io.DisplaySize.y * io.DisplayFramebufferScale.y);
		if (fbWidth == 0 || fbHeight == 0)
			return;
		
		drawData->ScaleClipRects(io.DisplayFramebufferScale);
		
		GLint oldViewport[4];
		glGetIntegerv(GL_VIEWPORT, oldViewport);
		
		glEnable(GL_SCISSOR_TEST);
		
		glViewport(0, 0, fbWidth, fbHeight);
		const float ortho_projection[4][4] =
		{
			{ 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
			{ 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
			{ 0.0f,                  0.0f,                  -1.0f, 0.0f },
			{-1.0f,                  1.0f,                   0.0f, 1.0f }
		};
		
		theGuiShader->Use();
		glUniform1i(theTextureUniformLoc, 0);
		glUniformMatrix4fv(theProjectionMatrixUniformLoc, 1, GL_FALSE, &ortho_projection[0][0]);
		theGuiVAO->Bind();
		
		constexpr GLenum INDEX_TYPE = sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
		
		for (int n = 0; n < drawData->CmdListsCount; n++)
		{
			const ImDrawList* commandList = drawData->CmdLists[n];
			const ImDrawIdx* indexBufferOffset = 0;
			
			glBindBuffer(GL_ARRAY_BUFFER, theGuiVBO);
			glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)commandList->VtxBuffer.Size * sizeof(ImDrawVert),
			             reinterpret_cast<GLvoid*>(commandList->VtxBuffer.Data), GL_STREAM_DRAW);
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theGuiIBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)commandList->IdxBuffer.Size * sizeof(ImDrawIdx),
			             reinterpret_cast<GLvoid*>(commandList->IdxBuffer.Data), GL_STREAM_DRAW);
			
			for (int i = 0; i < commandList->CmdBuffer.Size; i++)
			{
				const ImDrawCmd* pcmd = &commandList->CmdBuffer[i];
				if (pcmd->UserCallback)
				{
					pcmd->UserCallback(commandList, pcmd);
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(reinterpret_cast<intptr_t>(pcmd->TextureId)));
					glScissor(static_cast<int>(pcmd->ClipRect.x), static_cast<int>(fbHeight - pcmd->ClipRect.w),
					          static_cast<GLsizei>(pcmd->ClipRect.z - pcmd->ClipRect.x),
					          static_cast<GLsizei>(pcmd->ClipRect.w - pcmd->ClipRect.y));
					
					glDrawElements(GL_TRIANGLES, pcmd->ElemCount, INDEX_TYPE, indexBufferOffset);
				}
				indexBufferOffset += pcmd->ElemCount;
			}
		}
		
		glDisable(GL_SCISSOR_TEST);
		
		glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
	}
	
	void NewFrame(const UpdateInfo& updateInfo, float dt)
	{
		if (theGuiShader == nullptr)
			CreateDeviceObjects();
		if (theFontTexture == nullptr)
			CreateFontsTexture();
		
		ImGuiIO& io = ImGui::GetIO();
		
		isMouseCaptured = io.WantCaptureMouse;
		isKeyboardCaptured = io.WantCaptureKeyboard;
		
		io.DeltaTime = dt;
		
		io.DisplaySize.x = static_cast<float>(updateInfo.m_windowWidth);
		io.DisplaySize.y = static_cast<float>(updateInfo.m_windowHeight);
		io.DisplayFramebufferScale.x = 1;
		io.DisplayFramebufferScale.y = 1;
		
		if (updateInfo.m_windowHasFocus)
		{
			io.MousePos.x = updateInfo.m_mouse.pos.x;
			io.MousePos.y = updateInfo.m_windowHeight - updateInfo.m_mouse.pos.y;
		}
		else
		{
			io.MousePos = { -1.0f, -1.0f };
		}
		
		for (int i = 0; i < 3; i++)
		{
			io.MouseDown[i] = updateInfo.m_mouse.GetButtonState() & (1 << i);
		}
		
		io.MouseWheel = updateInfo.m_mouse.GetDeltaScroll();
		
		for (int key = 0; key <= MAX_KEY_ID; key++)
			io.KeysDown[key] = updateInfo.m_keyboard.IsDown((Key)key, true);
		io.KeyCtrl  = updateInfo.m_keyboard.IsAnyDown(KEY_MASK_CONTROL, true);
		io.KeyShift = updateInfo.m_keyboard.IsAnyDown(KEY_MASK_SHIFT, true);
		io.KeyAlt   = updateInfo.m_keyboard.IsAnyDown(KEY_MASK_ALT, true);
		io.KeySuper = updateInfo.m_keyboard.IsAnyDown(KEY_MASK_SUPER, true);
		
		//glfwSetInputMode(window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
		
		ImGui::NewFrame();
	}
	
	bool IsMouseCaptured()
	{
		return isMouseCaptured;
	}
	
	bool IsKeyboardCaptured()
	{
		return isKeyboardCaptured;
	}
}
