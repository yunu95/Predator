#pragma once
#include "YunutyEngine.h"
#include "UIButton.h"
#include "Storable.h"
#include "UIEnumID.h"
#include "JsonUIData.h"
#include "ContentsObservee.h"
/// <summary>
/// UIButton 객체들을 관리하는 컴포넌트.
/// 역할
/// 1. 모든 UIButton 객체들을 저장 및 관리
/// 2. button 멤버 UIImage의 Layer를 비교하여 클릭 함수 재정의
/// 3. 최소 하나의 button에 마우스가 올라가 있는가? 를 항상 판별.
/// 4. 유니티 에디터에서 만든 UI 레이아웃을 불러와서 생성
/// </summary>
class SpriteAnimation;
class UIElement;
class UIManager : public Component, public SingletonComponent<UIManager>, public PermanentObservee
{
private:
    void ClearDialogueInfos();
    // JsonUIData만으로 UI를 생성합니다.
    void ImportDefaultAction(const JsonUIData& uiData, UIElement* element);
    // 이 함수는 ImportDealwithSpecialCases_Post 함수와 더불어 같은 element에 대해 여러번 호출될 수 있다.
    // 고로 그 특성을 감안하고 작성해야됨.
    void ImportDefaultAction_Post(const JsonUIData& uiData, UIElement* element);
    // 특별한 로직이 적용되어야 하는 경우 참, 그렇지 않으면 거짓을 반환합니다.
    bool ImportDealWithSpecialCases(const JsonUIData& uiData, UIElement* element);
    // 아래 두 함수들을 응용해 UI들이 다 생성되고 난 후 추가적인 작업을 수행합니다.
    bool ImportDealWithSpecialCases_Post(const JsonUIData& uiData, UIElement* element);

    void SetUIElementWithEnum(UIEnumID uiEnumID, UIElement* ui);
    void SetUIElementWithIndex(int index, UIElement* ui);
    void SetUIDataWithIndex(int index, const JsonUIData& uiData);
    coroutine::Coroutine StartGameAfterFadeOutCoro();
    coroutine::Coroutine ReturnToTitleAfterFadeOutCoro();
    virtual Component* GetComponent() { return this; };
    virtual void OnContentsStop();
    virtual void OnContentsPlay();

    SpriteAnimation* moveToSpriteAnim{ nullptr };
    SpriteAnimation* attackMoveToSpriteAnim{ nullptr };
    int uiImportingPriority{ 0 };
    struct ButtonCompare
    {
        bool operator()(const UIButton* lhs, const UIButton* rhs) const
        {
            return lhs->GetLayer() > rhs->GetLayer();
        }
    };
    std::map<UIButton*, std::weak_ptr<UIButton>, ButtonCompare> m_selectedButtons;

    std::function<void(Vector3d pos)> m_beforeUIEnterFunction;

    int m_currentHighestLayer = 0;
    UIButton* m_highestPriorityButton;
    UIElement* localContext{ nullptr };

    bool isButtonActiviated = false;
    std::vector<UIElement*> rootUIs;
    std::unordered_map<int, UIElement*> uisByIndex;
    std::unordered_map<UIEnumID, UIElement*> uisByEnumID;
    std::unordered_map<int, JsonUIData> uidatasByIndex;
    std::unordered_map<UIElement*, array<yunuGI::ITexture*, 10>> digitFonts;

    std::unordered_map<std::string, UIElement*> dialogueTimed;
    std::vector<std::string> dialogueTimed_KeyStrings;
    std::unordered_map<std::string, UIElement*> dialogueManual;
    std::vector<std::string> dialogueManual_KeyStrings;
    std::unordered_map<std::string, UIElement*> scriptUI;
    std::vector<std::string> scriptUI_KeyStrings;
    std::unordered_map<std::wstring, std::weak_ptr<graphics::VideoPlayer>> videoPlayers;
    //std::unordered_map<std::wstring, std::weak_ptr<graphics::VideoPlayer>> videoPlayersScaled;
public:
    const std::vector<std::string>& GetDialogueTimed_KeyStrings();
    const std::vector<std::string>& GetDialogueManual_KeyStrings();
    const std::vector<std::string>& GetScriptUI_KeyStrings();
    UIElement* GetDialogueTimed(const std::string& keyString);
    UIElement* GetDialogueManual(const std::string& keyString);
    UIElement* GetScriptUI(const std::string& keyString);
    void StartGameAfterFadeOut();
    void ReturnToTitleAfterFadeOut();
    UIButton* GetHighlightedButton();
    static constexpr UIEnumID comboNumbers[6]
    {
        UIEnumID::Ingame_Combo_TargetNumFinished1,
        UIEnumID::Ingame_Combo_TargetNumFinished2,
        UIEnumID::Ingame_Combo_TargetNumFinished3,
        UIEnumID::Ingame_Combo_TargetNumUnfinished1,
        UIEnumID::Ingame_Combo_TargetNumUnfinished2,
        UIEnumID::Ingame_Combo_TargetNumUnfinished3,
    };
    static constexpr UIEnumID comboFinishedImgs[3]
    {
        UIEnumID::Ingame_Combo_DescriptionImageFinished1,
        UIEnumID::Ingame_Combo_DescriptionImageFinished2,
        UIEnumID::Ingame_Combo_DescriptionImageFinished3
    };
    static constexpr UIEnumID comboUnFinishedImgs[3]
    {
        UIEnumID::Ingame_Combo_DescriptionImageUnfinished1,
        UIEnumID::Ingame_Combo_DescriptionImageUnFinished2,
        UIEnumID::Ingame_Combo_DescriptionImageUnFinished3
    };
    static constexpr UIEnumID comboCheckImgs[3]
    {
        UIEnumID::Ingame_Combo_Check1,
        UIEnumID::Ingame_Combo_Check2,
        UIEnumID::Ingame_Combo_Check3
    };

    void Clear();
    void FadeOutRight(float duration = 1);
    void FadeOutLeft(float duration = 1);
    void FadeOutTop(float duration = 1);
    void FadeOutBottom(float duration = 1);
    void FadeIn(float duration = 1);
    void SetIngameUIVisible(bool visible);
    void SetLetterBoxVisible(bool visible);
    void ReportButtonOnMouse(UIButton* p_btn);
    void ReportMouseExitButton(UIButton* p_btn);
    void ShowComboObjectives();
    void HideComboObjectvies();
    void SetPortraitsClickable(bool clickable);
    // 버튼 중 가장 높은 우선순위를 가진 버튼으로 highestpriorityButton을 재설정합니다.
    // 현재의 highestprirorityButton은 Exit 이벤트 함수가 호출되며, 새로운 highestpriorityButton은 Enter 이벤트 함수가 호출됩니다.
    // 만약 현재의 highestPirorityButton이 여전히 가장 높은 우선순위를 가지고 있다면, 아무 일도 벌어지지 않습니다.
    void UpdateHighestPriorityButton();

    void SummonMoveToFeedback(const Vector3d& worldPos, bool attackMove = false);
    Vector3d GetUIPosFromWorld(Vector3d worldPosition);
    bool IsMouseOnButton();
    weak_ptr<UIElement> DuplicateUIElement(UIElement* ui);
    UIElement* GetUIElementWithIndex(int index);
    JsonUIData GetUIDataWithIndex(int index);
    UIElement* GetUIElementByEnum(UIEnumID uiEnumID);
    void ImportUI(const char* path);

    virtual void Update() override;
};
