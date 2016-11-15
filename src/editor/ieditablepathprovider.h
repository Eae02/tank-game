#pragma once

namespace TankGame
{
	class IEditablePathProvider
	{
	public:
		virtual class Path& GetEditPath() = 0;
		virtual void PathEditEnd() = 0;
		virtual bool IsClosedPath() const = 0;
		virtual const char* GetEditPathName() const = 0;
	};
}
