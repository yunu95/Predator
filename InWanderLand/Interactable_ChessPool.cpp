#include "Interactable_ChessPool.h"

class Interactable_ChessPawnPool :
	public GameObjectPool<Interactable_ChessPawn>, public SingletonComponent<Interactable_ChessPawnPool>, public Component, public PermanentObservee
{
public:
	virtual void ObjectInitializer(Interactable_ChessPawn* comp) override;
	virtual void OnContentsPlay() override;
	virtual void OnContentsStop() override;
	virtual Component* GetComponent() override { return this; }
};

class Interactable_ChessRookPool :
	public GameObjectPool<Interactable_ChessRook>, public SingletonComponent<Interactable_ChessRookPool>, public Component, public PermanentObservee
{
public:
	virtual void ObjectInitializer(Interactable_ChessRook* comp) override;
	virtual void OnContentsPlay() override;
	virtual void OnContentsStop() override;
	virtual Component* GetComponent() override { return this; }
};

class Interactable_ChessBishopPool :
	public GameObjectPool<Interactable_ChessBishop>, public SingletonComponent<Interactable_ChessBishopPool>, public Component, public PermanentObservee
{
public:
	virtual void ObjectInitializer(Interactable_ChessBishop* comp) override;
	virtual void OnContentsPlay() override;
	virtual void OnContentsStop() override;
	virtual Component* GetComponent() override { return this; }
};
