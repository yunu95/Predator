#include <cmath>
#include <map>

#include "Easing.h"

#ifndef PI
#define PI 3.1415926545
#endif

#ifndef E
#define E 2.71828
#endif

double easeInSine(double t) {
	return 1 - cos((t * PI) / 2);
}

double easeOutSine(double t) {
	return sin((t * PI) / 2);;
}

double easeInOutSine(double t) {
	return -1 * (cos(PI * t) - 1) / 2;
}

double easeInQuad(double t) {
	return t * t;
}

double easeOutQuad(double t) {
	return 1 - (1 - t) * (1 - t);
}

double easeInOutQuad(double t) {
	return t < 0.5 ? 2 * t * t : 1 - pow(-2 * t + 2, 2) / 2;
}

double easeInCubic(double t) {
	return pow(t, 3);
}

double easeOutCubic(double t) {
	return 1 - pow(1 - t, 3);
}

double easeInOutCubic(double t) {
	return t < 0.5 ? 4 * t * t * t : 1 - pow(-2 * t + 2, 3) / 2;
}

double easeInQuart(double t) {
	return pow(t, 4);
}

double easeOutQuart(double t) {
	return 1 - pow(1 - t, 4);
}

double easeInOutQuart(double t) {
	return t < 0.5 ? 8 * pow(t, 4) : 1 - pow(-2 * t + 2, 4) / 2;
}

double easeInQuint(double t) {
	return pow(t, 5);
}

double easeOutQuint(double t) {
	double t2 = (--t) * t;
	return 1 + t * t2 * t2;
}

double easeInOutQuint(double t) {
	double t2;
	if (t < 0.5) {
		t2 = t * t;
		return 16 * t * t2 * t2;
	}
	else {
		t2 = (--t) * t;
		return 1 + 16 * t * t2 * t2;
	}
}

double easeInExpo(double t) {
	return (pow(2, 8 * t) - 1) / 255;
}

double easeOutExpo(double t) {
	return t == 1 ? 1 : 1 - pow(2, -10 * t);
}

double easeInOutExpo(double t) {
	return t == 0 ? 0 : t == 1 ? 1 : t < 0.5 ? pow(2, 20 * t - 10) / 2
		: (2 - pow(2, -20 * t + 10)) / 2;
}

double easeInCirc(double t) {
	return 1 - sqrt(1 - pow(t, 2));
}

double easeOutCirc(double t) {
	return sqrt(1 - pow(t - 1, 2));
}

double easeInOutCirc(double t) {
	return t < 0.5
		? (1 - sqrt(1 - pow(2 * t, 2))) / 2
		: (sqrt(1 - pow(-2 * t + 2, 2)) + 1) / 2;
}

double easeInBack(double t) {
	const double c1 = 1.70158;
	const double c3 = c1 + 1;

	return c3 * t * t * t - c1 * t * t;
}

double easeOutBack(double t) {
	const double c1 = 1.70158;
	const double c3 = c1 + 1;

	return 1 + c3 * pow(t - 1, 3) + c1 * pow(t - 1, 2);
}

double easeInOutBack(double t) {
	const double c1 = 1.70158;
	const double c2 = c1 * 1.525;

	return t < 0.5
		? (pow(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2
		: (pow(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2;
}

double easeInElastic(double t) {
	const double c4 = (2 * PI) / 3;

	return t == 0
		? 0
		: t == 1
		? 1
		: -pow(2, 10 * t - 10) * sin((t * 10 - 10.75) * c4);
}

double easeOutElastic(double t) {
	const double c4 = (2 * PI) / 3;

	return t == 0
		? 0
		: t == 1
		? 1
		: pow(2, -10 * t) * sin((t * 10 - 0.75) * c4) + 1;
}

double easeInOutElastic(double t) {
	const double c5 = (2 * PI) / 4.5;

	return t == 0
		? 0
		: t == 1
		? 1
		: t < 0.5
		? -(pow(2, 20 * t - 10) * sin((20 * t - 11.125) * c5)) / 2
		: (pow(2, -20 * t + 10) * sin((20 * t - 11.125) * c5)) / 2 + 1;
}

double easeOutBounce(double t) {
	const double n1 = 7.5625;
	const double d1 = 2.75;

	if (t < 1 / d1) {
		return n1 * t * t;
	}
	else if (t < 2 / d1) {
		return n1 * (t -= 1.5 / d1) * t + 0.75;
	}
	else if (t < 2.5 / d1) {
		return n1 * (t -= 2.25 / d1) * t + 0.9375;
	}
	else {
		return n1 * (t -= 2.625 / d1) * t + 0.984375;
	}
}

double easeInBounce(double t) {
	return 1 - easeOutBounce(1 - t);
}

double easeInOutBounce(double t) {
	return t < 0.5
		? (1 - easeOutBounce(1 - 2 * t)) / 2
		: (1 + easeOutBounce(2 * t - 1)) / 2;
}

double easeBasic(double t)
{
	return t;
}

easingFunction getEasingFunction(easing_functions function)
{
	static std::map< easing_functions, easingFunction > easingFunctions;
	if (easingFunctions.empty())
	{
		easingFunctions.insert(std::make_pair(EaseInSine, easeInSine));
		easingFunctions.insert(std::make_pair(EaseOutSine, easeOutSine));
		easingFunctions.insert(std::make_pair(EaseInOutSine, easeInOutSine));
		easingFunctions.insert(std::make_pair(EaseInQuad, easeInQuad));
		easingFunctions.insert(std::make_pair(EaseOutQuad, easeOutQuad));
		easingFunctions.insert(std::make_pair(EaseInOutQuad, easeInOutQuad));
		easingFunctions.insert(std::make_pair(EaseInCubic, easeInCubic));
		easingFunctions.insert(std::make_pair(EaseOutCubic, easeOutCubic));
		easingFunctions.insert(std::make_pair(EaseInOutCubic, easeInOutCubic));
		easingFunctions.insert(std::make_pair(EaseInQuart, easeInQuart));
		easingFunctions.insert(std::make_pair(EaseOutQuart, easeOutQuart));
		easingFunctions.insert(std::make_pair(EaseInOutQuart, easeInOutQuart));
		easingFunctions.insert(std::make_pair(EaseInQuint, easeInQuint));
		easingFunctions.insert(std::make_pair(EaseOutQuint, easeOutQuint));
		easingFunctions.insert(std::make_pair(EaseInOutQuint, easeInOutQuint));
		easingFunctions.insert(std::make_pair(EaseInExpo, easeInExpo));
		easingFunctions.insert(std::make_pair(EaseOutExpo, easeOutExpo));
		easingFunctions.insert(std::make_pair(EaseInOutExpo, easeInOutExpo));
		easingFunctions.insert(std::make_pair(EaseInCirc, easeInCirc));
		easingFunctions.insert(std::make_pair(EaseOutCirc, easeOutCirc));
		easingFunctions.insert(std::make_pair(EaseInOutCirc, easeInOutCirc));
		easingFunctions.insert(std::make_pair(EaseInBack, easeInBack));
		easingFunctions.insert(std::make_pair(EaseOutBack, easeOutBack));
		easingFunctions.insert(std::make_pair(EaseInOutBack, easeInOutBack));
		easingFunctions.insert(std::make_pair(EaseInElastic, easeInElastic));
		easingFunctions.insert(std::make_pair(EaseOutElastic, easeOutElastic));
		easingFunctions.insert(std::make_pair(EaseInOutElastic, easeInOutElastic));
		easingFunctions.insert(std::make_pair(EaseInBounce, easeInBounce));
		easingFunctions.insert(std::make_pair(EaseOutBounce, easeOutBounce));
		easingFunctions.insert(std::make_pair(EaseInOutBounce, easeInOutBounce));
		easingFunctions.insert(std::make_pair(EaseBasic, easeBasic));

	}

	auto it = easingFunctions.find(function);
	return it == easingFunctions.end() ? nullptr : it->second;
}
