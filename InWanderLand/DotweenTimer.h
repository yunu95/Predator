#pragma once
#include <iostream>
#include <functional>
#include "Easing.h"

/// <summary>
/// Dotween의 반복호출을 막기위한 클래스.
/// elasped? OnExpiriation?
/// Dotween 객체가 가지고 있으면 딱인 것 같다...
/// </summary>

class DotweenTimer
{
private:


public:
	double m_xValue;
	double m_yValue;
	double m_zValue;
	double m_wValue;

	double easeTime;
	double elapsed;
	// 타이머가 시작된 후 경과된 시간
	double duration;
	double delay;

	// 현재 진행 거리
	double movingDistanceX;
	double movingDistanceY;
	double movingDistanceZ;
	double movingDistanceW;
	// 속도
	double speed;

	double normalizedDistanceX;
	double normalizedDistanceY;
	double normalizedDistanceZ;
	double normalizedDistanceW;

	double finalXvalue;
	double finalYvalue;
	double finalZvalue;
	double finalWvalue;

	double accumulated;		// 누적 회전량

	bool isActive;
	bool isRepeat;
	bool isDone;			// 타이머 Vector에서 빼도 되는지에 대한 메시지 전달용

	easing_functions m_ease;

	std::function<void()> onUpdate = []() {};			
	std::function<void()> onCompleteFunction = []() {};		// duration 도달했을 때 실행시킬 함수.
	std::function<void()> onExpiration = []() {};			// dotween 내부에서 설정해줄 수 있는 실행 완료 시 호출 함수

	DotweenTimer();
	void Start();
	void Update();

	void Init();


};

