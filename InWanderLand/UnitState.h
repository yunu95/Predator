enum class UnitState
{
    None,
    // 가만히 있는 상태, 홀드와 다를바 없다.
    Stop,
    // 적들의 조우 같은건 다 무시하고 목표지로만 이동중인 상태
    Move,
    // 특정 대상을 공격 상태로 지정한 상태
    Attack,
    // 어택땅, 특정 지점까지 돌격모드가 박힌 상태
    AttackMove,
    // 스킬을 사용중인 상태
    Skill,
    // 사망한 상태
    Death,
    // 정지상태. 경직, 넉백, 트리거상 정지가 모두 이 상태에 속한다. 디폴트로 Idle 애니메이션을 재생함.
    Paused,
    // enum class의 끝을 알리는 상태
    End
};
