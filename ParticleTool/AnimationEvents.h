/// 2024. 04. 28 김상준
/// Animator 에 적절한 AnimationEvent 를 등록할 수 있도록 돕는 중간 데이터 양식들의 집합입니다.
/// 해당 양식들이 저장되고, 이를 해석하여 functor 가 등록됩니다.

#pragma once

#include "YunutyEngine.h"
#include "ImCurveEdit.h"

#define GET_TYPE(Class) \
virtual AnimationEventType GetType() override \
{ \
	return AnimationEventType::##Class; \
}

struct RampEdit 
	: public ImCurveEdit::Delegate
{
	enum class Transform
	{
		Position_X,
		Position_Y,
		Position_Z,
		Rotation_X,
		Rotation_Y,
		Rotation_Z,
		Scale_X,
		Scale_Y,
		Scale_Z,
	};

	RampEdit()
	{
		mPointCount[(int)Transform::Position_X] = 0;
		mPointCount[(int)Transform::Position_Y] = 0;
		mPointCount[(int)Transform::Position_Z] = 0;
		mPointCount[(int)Transform::Rotation_X] = 0;
		mPointCount[(int)Transform::Rotation_Y] = 0;
		mPointCount[(int)Transform::Rotation_Z] = 0;
		mPointCount[(int)Transform::Scale_X] = 0;
		mPointCount[(int)Transform::Scale_Y] = 0;
		mPointCount[(int)Transform::Scale_Z] = 0;

		mbVisible[(int)Transform::Position_X] = true;
		mbVisible[(int)Transform::Position_Y] = true;
		mbVisible[(int)Transform::Position_Z] = true;
		mbVisible[(int)Transform::Rotation_X] = true;
		mbVisible[(int)Transform::Rotation_Y] = true;
		mbVisible[(int)Transform::Rotation_Z] = true;
		mbVisible[(int)Transform::Scale_X] = true;
		mbVisible[(int)Transform::Scale_Y] = true;
		mbVisible[(int)Transform::Scale_Z] = true;
		mMax = ImVec2(1.f, 1.f);
		mMin = ImVec2(0.f, 0.f);
	}

	size_t GetCurveCount()
	{
		return 9;
	}

	bool IsVisible(size_t curveIndex)
	{
		return mbVisible[curveIndex];
	}

	size_t GetPointCount(size_t curveIndex)
	{
		return mPointCount[curveIndex];
	}

	uint32_t GetCurveColor(size_t curveIndex)
	{
		uint32_t cols[] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF0000FF, 0xFF00FF00, 0xFFFF0000 };
		return cols[curveIndex];
	}

	ImVec2* GetPoints(size_t curveIndex)
	{
		return &(*mPts[curveIndex].begin());
	}

	virtual ImCurveEdit::CurveType GetCurveType(size_t curveIndex) const { return ImCurveEdit::CurveSmooth; }
	
	virtual int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value)
	{
		mPts[curveIndex][pointIndex] = ImVec2(value.x, value.y);
		SortValues(curveIndex);
		for (size_t i = 0; i < GetPointCount(curveIndex); i++)
		{
			if (mPts[curveIndex][i].x == value.x)
				return (int)i;
		}
		return pointIndex;
	}

	virtual void AddPoint(size_t curveIndex, ImVec2 value)
	{
		mPts[curveIndex].push_back(value);
		mPointCount[curveIndex]++;
		SortValues(curveIndex);
	}

	void ErasePoint(size_t curveIndex, unsigned int index)
	{
		if (index > mPointCount[curveIndex])
		{
			return;
		}
		mPts[curveIndex].erase(mPts[curveIndex].begin() + index);
		mPointCount[curveIndex]--;
		SortValues(curveIndex);
	}

	virtual ImVec2& GetMax() { return mMax; }
	virtual ImVec2& GetMin() { return mMin; }
	virtual unsigned int GetBackgroundColor() { return 0; }
	std::vector<ImVec2> mPts[9];
	size_t mPointCount[9];
	bool mbVisible[9];
	ImVec2 mMin;
	ImVec2 mMax;

private:
	void SortValues(size_t curveIndex)
	{
		auto b = std::begin(mPts[curveIndex]);
		auto e = std::begin(mPts[curveIndex]) + GetPointCount(curveIndex);
		std::sort(b, e, [](ImVec2 a, ImVec2 b) { return a.x < b.x; });
	}
};

namespace application
{
	enum class AnimationEventType
	{
		GameObject_ActivateEvent,
		GameObject_DisabledEvent,
		GameObject_TransformEditEvent,
		Sound_PlayOnceEvent,
		Sound_PlayLoopEvent
	};

	struct AnimationEvent
	{
		virtual ~AnimationEvent() = default;
		virtual AnimationEventType GetType() = 0;
	
		std::string fbxName;
		std::string animationName;
		float frame;
	};

	/// 우선은 Object Name 으로 관리하도록 합니다.
	struct GameObject_ActivateEvent
		: public AnimationEvent
	{
		GET_TYPE(GameObject_ActivateEvent)

		std::string objName;
	};

	struct GameObject_DisabledEvent
		: public AnimationEvent
	{
		GET_TYPE(GameObject_DisabledEvent)

		std::string objName;
	};

	struct GameObject_TransformEditEvent
		: public AnimationEvent
	{
		GET_TYPE(GameObject_TransformEditEvent)

		std::string objName;
		RampEdit editData;
	};

	struct Sound_PlayOnceEvent
		: public AnimationEvent
	{
		GET_TYPE(Sound_PlayOnceEvent)

		std::string rscPath;
	};

	struct Sound_PlayLoopEvent
		: public AnimationEvent
	{
		GET_TYPE(Sound_PlayLoopEvent)

		std::string rscPath;
	};
}
