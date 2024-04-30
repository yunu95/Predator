#pragma once

class ContentsObservee
{
public:
	ContentsObservee();				/// Observer에 등록
	virtual void PlayFunction() = 0;
	virtual void StopFunction() = 0;
	bool isSingletonComponent{ false };
	bool isOncePaused{ false };

private:
};

