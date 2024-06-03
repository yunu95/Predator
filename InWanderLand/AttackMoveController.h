#include "UnitController.h"

class Unit;
class AttackMoveController : public UnitController
{
    weak_ptr<Unit> target;
};