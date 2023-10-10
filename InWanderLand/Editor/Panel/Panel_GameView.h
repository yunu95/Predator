/// 2023. 10. 04 김상준
/// 게임에서 사용하는 MainCamera 시점에서 확인할 수 있는 화면을 보여주는 패널

#pragma once

#include "EditorPanel.h"

namespace Editor
{
	class GameViewPanel
		: public Panel
	{
	public:
		virtual ~GameViewPanel();

		virtual void Initialize() override;
		virtual void Update(float ts) override;
		virtual void DrawPanel() override;
		virtual void Finalize() override;
		
		static GameViewPanel& GetInstance();

	protected:
		GameViewPanel();
		GameViewPanel(const GameViewPanel& copy) = delete;
		GameViewPanel& operator=(const GameViewPanel& copy) = delete;

		static GameViewPanel* instance;
	};
}
