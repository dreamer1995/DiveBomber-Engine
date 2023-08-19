#pragma once
#include <string>

namespace DiveBomber::DEScene
{
	class Scene final
	{
	public:
		Scene();
		~Scene();

		void LoadSceneFromFile(const std::wstring name);
	private:
		const std::wstring name;
	};
}