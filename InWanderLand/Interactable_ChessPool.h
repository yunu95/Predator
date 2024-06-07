/// 2024. 05. 30 김상준
/// Chess Object 를 Boss 등 소환할 경우에 사용할 Chess Pool 입니다.

#pragma once
#include "YunutyEngine.h"
#include "DebugMeshes.h"
#include "PermanentObservee.h"

class Interactable_ChessPawn;
class Interactable_ChessRook;
class Interactable_ChessBishop;

/// 실제 사용자가 사용하게 될 Pool 입니다.
/// 내부적으로 Chess Pool 을 통해서 Object 를 관리합니다.
class Interactable_ChessPool
    : public SingletonComponent<Interactable_ChessPool>, public Component, public PermanentObservee
{
public:
    virtual void Start() override;
    std::weak_ptr<Interactable_ChessPawn> BorrowPawn();
    std::weak_ptr<Interactable_ChessRook> BorrowRook();
    std::weak_ptr<Interactable_ChessBishop> BorrowBishop();
    void Return(std::weak_ptr<Interactable_ChessPawn> chess);
    void Return(std::weak_ptr<Interactable_ChessRook> chess);
    void Return(std::weak_ptr<Interactable_ChessBishop> chess);
    virtual Component* GetComponent() override { return this; }

private:
    int initContainerSize = 0;
};
