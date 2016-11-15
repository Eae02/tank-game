#include "imguiinterface.h"
#include "graphics/gl/shaderprogram.h"
#include "graphics/gl/shadermodule.h"
#include "graphics/gl/texture2d.h"
#include "graphics/gl/vertexarray.h"
#include "utils/memory/stackobject.h"
#include "utils/ioutils.h"

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW\glfw3native.h>
#endif

namespace TankGame
{
namespace ImGuiInterface
{
	static StackObject<Texture2D> theFontTexture;
	static StackObject<ShaderProgram> theGuiShader;
	static StackObject<VertexArray> theGuiVAO;
	
	static int theTextureUniformLoc = 0;
	static int theProjectionMatrixUniformLoc = 0;
	static unsigned int theGuiVBO = 0;
	static unsigned int theGuiIBO = 0;
	
	static bool isMouseCaptured = false;
	static bool isKeyboardCaptured = false;
	
	static float mouseWheelChange = 0.0f;
	
	static std::string iniFilePath;
	
	static void RenderDrawLists(ImDrawData* drawData);
	
	void Init(GLFWwindow* window)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
		
		io.RenderDrawListsFn = RenderDrawLists;
		
		iniFilePath = (GetDataDirectory() / "imgui.ini").string();
		io.IniFilename = iniFilePath.c_str();
		
		io.SetClipboardTextFn = [] (void* data, const char* text)
		{
			glfwSetClipboardString(reinterpret_cast<GLFWwindow*>(data), text);
		};
		io.GetClipboardTextFn = [] (void* data)
		{
			return glfwGetClipboardString(reinterpret_cast<GLFWwindow*>(data));
		};
		io.ClipboardUserData = window;
		
#ifdef _WIN32
		io.ImeWindowHandle = glfwGetWin32Window(window);
#endif
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
		
		theFontTexture.Construct(width, height, 1, GL_RGBA8);
		glTextureSubImage2D(theFontTexture->GetID(), 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		
		io.Fonts->TexID = reinterpret_cast<void*>(static_cast<intptr_t>(theFontTexture->GetID()));
	}
	
	static void CreateDeviceObjects()
	{
		auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "imgui.vs.glsl", GL_VERTEX_SHADER);
		auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "imgui.fs.glsl", GL_FRAGMENT_SHADER);
		
		theGuiShader.Construct<std::initializer_list<const ShaderModule*>>({ &vs, &fs });
		
		theTextureUniformLoc = theGuiShader->GetUniformLocation("Texture");
		theProjectionMatrixUniformLoc = theGuiShader->GetUniformLocation("ProjMtx");
		
		glGenBuffers(1, &theGuiVBO);
		glGenBuffers(1, &theGuiIBO);
		
		theGuiVAO.Construct();
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
		
		theGuiVAO.Destroy();
		
		theGuiShader.Destroy();
		theFontTexture.Destroy();
		
		ImGui::Shutdown();
	}
	
	static void RenderDrawLists(ImDrawData* drawData)
	{
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
	
	void NewFrame(GLFWwindow* window, float dt)
	{
		if (theGuiShader.IsNull())
			CreateDeviceObjects();
		if (theFontTexture.IsNull())
			CreateFontsTexture();
		
		ImGuiIO& io = ImGui::GetIO();
		
		isMouseCaptured = io.WantCaptureMouse;
		isKeyboardCaptured = io.WantCaptureKeyboard;
		
		io.DeltaTime = dt;
		
		int windowWidth, windowHeight;
		int displayWidth, displayHeight;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
		
		io.DisplaySize = { static_cast<float>(windowWidth), static_cast<float>(windowHeight) };
		io.DisplayFramebufferScale.x = windowWidth > 0 ? (static_cast<float>(displayWidth) / windowWidth) : 0;
		io.DisplayFramebufferScale.y = windowHeight > 0 ? (static_cast<float>(displayHeight) / windowHeight) : 0;
		
		if (glfwGetWindowAttrib(window, GLFW_FOCUSED))
		{
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			
			io.MousePos = { static_cast<float>(mouseX), static_cast<float>(mouseY) };
		}
		else
		{
			io.MousePos = { -1.0f, -1.0f };
		}
		
		for (int i = 0; i < 3; i++)
		{
			io.MouseDown[i] = glfwGetMouseButton(window, i) != 0;
		}
		
		io.MouseWheel = mouseWheelChange;
		mouseWheelChange = 0.0f;
		
		glfwSetInputMode(window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
		
		ImGui::NewFrame();
	}
	
	void HandleKeyEvent(int key, int action)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[key] = action == GLFW_PRESS;
		
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
	}
	
	void HandleCharEvent(unsigned int c)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (c > 0 && c < 0x10000)
			io.AddInputCharacter(static_cast<unsigned short>(c));
	}
	
	void HandleScrollEvent(double yOffset)
	{
		mouseWheelChange += static_cast<float>(yOffset);
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
}
