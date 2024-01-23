#include "InWanderLand.h"
#include "DebugMeshes.h"
#include "UnitInstance.h"

namespace application::editor::palette
{
    void UnitInstance::Start()
    {
        PaletteInstance::Start();
        mesh = AttachDebugMesh(GetGameObject()->AddGameObject(), DebugMeshType::Cube, yunuGI::Color{1, 1, 1}, false);
    }

    void UnitInstance::ApplyInstance()
    {
        // 유닛 생성과 관련된 코드를 여기 넣으면 됨.
    }
}
