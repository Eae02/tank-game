#pragma once

namespace TankGame
{
	class LobbyMenu
	{
	public:
		LobbyMenu();
		
		void OnResize(int newWidth, int newHeight);
		
		void Update(const class UpdateInfo& updateInfo);
		void Draw(const class UIRenderer& uiRenderer) const;
		
		void OnOpen();
		
	private:
		
	};
}
