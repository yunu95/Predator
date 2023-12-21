/// 2023. 11. 01 김상준
/// Unit, Terrain 등 대단위에 대한 편집 시 옵션을 제공하는 팔레트 패널

#pragma once

#include "Singleton.h"
#include "EditorPanel.h"

#include <memory>

namespace application
{
	namespace editor
	{
		class PalettePanel
			: public Panel, public Singleton<PalettePanel>
		{
		public:
			PalettePanel();
			virtual ~PalettePanel();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;
		};
	}
}
