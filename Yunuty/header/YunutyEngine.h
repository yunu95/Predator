#pragma once
#include <iostream>
#include <chrono>
#include <stack>
#include <vector>
#include <queue>
#include <cassert>
//#include <d2d1.h>
//#include <d2d1_1.h>
#include <iostream>
#include <sstream>
#include <string>

#include "Component.h"
#include "EKeycode.h"
#include "GameObject.h"
#include "GlobalComponent.h"
#include "IGameObjectParent.h"
#include "Input.h"
#include "Object.h"
#include "Rect.h"
#include "RectTransform.h"
#include "Scene.h"
#include "Transform.h"
#include "Vector.h"
#include "Vector2.h"
#include "Vector3.h"
#include "YunutyCycle.h"
#include "YunutyTime.h"
#include "Time.h"
#include "WinAPIBitmapImage.h"
#include "RigidBody2D.h"
#include "Collider2D.h"
#include "Collision2D.h"
#include "BoxCollider2D.h"
#include "CircleCollider2D.h"
#include "GameObjectPool.h"
#include "SoundSystem.h"
#include "SoundChannel.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Animator.h"
#include "AnimationEventController.h"
#include "SingletonComponent.h"
#include "ScopedDestroyer.h"

// 수학 라이브러리들
#include "YunutyMathConstants.h"
#include "Random.h"
#include "Interval.h"
#include "Curve.h"

#include "CanvasRenderSpace.h"

#include "NavigationField2D.h"
#include "NavigationUnit2D.h"
#include "YunutyPixel.h"
#include "YunutyPixelInfos.h"

// 그래픽스
#include "YunuGraphicsInterface.h"
#include "GraphicsRenderer.h"
#include "Camera.h"
#include "YunutyGraphicsEventHandler.h"
#include "IYunuGIDebugCube.h"
#include "IYunuGIUIImage.h"
#include "StaticMeshRenderer.h"
#include "YunutySkinnedMesh.h"
#include "ParticleRenderer.h"
#include "VideoPlayer.h"
#include "DebugCube.h"
#include "UIImage.h"
#include "UIText.h"
#include "yunutyCoroutine.h"
#include "YunutyYieldInstruction.h"
#include "YunutyForSeconds.h"
#include "YunutyWaitForSeconds.h"

// 네비게이션
#include "YunutyNavigationField.h"
#include "YunutyNavigationAgent.h"
#include "YunutyNavigationObstacle.h"

// 물리
#include "YunutyCollider.h"
#include "YunutyBoxCollider.h"
#include "YunutyCapsuleCollider.h"
#include "YunutySphereCollider.h"
#include "YunutyRigidBody.h"

#include "YunutyGenericEqual.h"
#include "YunutyGenericHash.h"
