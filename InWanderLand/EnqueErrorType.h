#pragma once

enum class EnqueErrorType
{
    NONE,
    NotEnoughMana,
    QueueFull,
    UseSkill,
    NotReady,
    Executing,
	Success,
};