#pragma once 

enum easing_functions
{
	Linear,
	EaseInSine,
	EaseOutSine,
	EaseInOutSine,
	EaseInQuad,
	EaseOutQuad,
	EaseInOutQuad,
	EaseInCubic,
	EaseOutCubic,
	EaseInOutCubic,
	EaseInQuart,
	EaseOutQuart,
	EaseInOutQuart,
	EaseInQuint,
	EaseOutQuint,
	EaseInOutQuint,
	EaseInExpo,
	EaseOutExpo,
	EaseInOutExpo,
	EaseInCirc,
	EaseOutCirc,
	EaseInOutCirc,
	EaseInBack,
	EaseOutBack,
	EaseInOutBack,
	EaseInElastic,
	EaseOutElastic,
	EaseInOutElastic,
	EaseInBounce,
	EaseOutBounce,
	EaseInOutBounce,
	EaseBasic
};

typedef double(*easingFunction)(double);

easingFunction getEasingFunction(easing_functions function);

// 0~1까지 norm 값을 넣었을 때 0~0.5는 firstHalf 값이, 0.5~ 1은 1-lastHalf값이 나오는 함수
double EasingBackFunction(easing_functions firstHalf,easing_functions lastHalf,double t);
