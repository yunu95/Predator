/// 2024. 05. 30 김상준
/// Chess Object 를 Boss 등 소환할 경우에 사용할 Chess Pool

#pragma once
#include "YunutyEngine.h"
#include "Interactable_ChessPawn.h"
#include "Interactable_ChessRook.h"
#include "Interactable_ChessBishop.h"
#include "DebugMeshes.h"
#include "PermanentObservee.h"

/// 실제 사용자가 사용하게 될 Pool 입니다.
/// 내부적으로 Chess Pool 을 통해서 Object 를 관리합니다.
class Interactable_ChessPool
	: public SingletonComponent<Interactable_ChessPool>, public Component, public PermanentObservee
{
public:
	Interactable_ChessPawn* SummonPawn(const Vector3d& pos);
	Interactable_ChessRook* SummonRook(const Vector3d& pos);
	Interactable_ChessBishop* SummonBishop(const Vector3d& pos);
	virtual void OnContentsPlay() override;
	virtual void OnContentsStop() override;
	virtual Component* GetComponent() override { return this; }
};
