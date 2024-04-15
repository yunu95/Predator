#pragma once
#include "YunutyEngine.h"
#include "UIButton.h"
#include "Storable.h"
/// <summary>
/// UIButton 객체들을 관리하는 컴포넌트.
/// 역할
/// 1. 모든 UIButton 객체들을 저장 및 관리
/// 2. button 멤버 UIImage의 Layer를 비교하여 클릭 함수 재정의
/// 3. 최소 하나의 button에 마우스가 올라가 있는가? 를 항상 판별.
/// </summary>

class UIManager : public Component, public SingletonComponent<UIManager>
{
private:
    struct JsonUIData;
    // 특별한 로직이 적용되어야 하는 경우 참, 그렇지 않으면 거짓을 반환합니다.
    bool ImportDealWithSpecialCases(const JsonUIData& uiData);
    // JsonUIData만으로 UI를 생성합니다.
    void ImportDefaultAction(const JsonUIData& uiData);
    // 아래 두 함수들을 응용해 UI들이 다 생성되고 난 후 추가적인 작업을 수행합니다.
    bool ImportDealWithSpecialCases_Post(const JsonUIData& uiData);
    void ImportDefaultAction_Post(const JsonUIData& uiData);
    int uiImportingPriority{ 0 };
    struct JsonUIData;
    std::list<UIButton*> m_currentSelectedButtonList;

    std::function<void(Vector3d pos)> m_beforeUIEnterFunction;

    int m_currentHighestLayer = 0;
    UIButton* m_highestPriorityButton;

    bool isButtonActiviated = false;
    std::unordered_map<std::string, GameObject*> uisByName;
    std::unordered_map<std::string, JsonUIData> uidatasByName;

public:
    struct JsonUIData
    {
        //public string imageName;
        string uiname;
        // 부모가 없다면 부모를 1920 * 1080 크기의 스크린 스페이스로 가정하게 된다.
        string parentUIName;
        string imagePath;
        // 만약 플래그에 openingButton이 있다면 버튼을 눌렀을 때 활성화시킬 UI 창을 의미한다.
        string openTarget;
        std::vector<double> pivot;
        std::vector<double> anchor;
        std::vector<double> anchoredPosition;
        double rotation;
        double width;
        double height;
        // 임의로 사용하게 될 사용자 플래그
        int customFlags;
        FROM_JSON(JsonUIData)
    };
    enum class UIExportFlag
    {
        None = 0,
        // 마우스가 올라갔을 때, 마우스가 클릭했을 때 등의 상호작용을 받습니다.
        IsButton = 1 << 0,
        // 창을 닫는 용도로 쓰입니다.
        CloseButton = 1 << 1,
        // 특정 창을 여는 용도로 쓰입니다.
        OpeningButton = 1 << 2,
        // 비활성화시 그레이 스케일 적용된 이미지를 출력합니다.
        GrayScaleDisable = 1 << 3,
        // 활성화될때 팝업 효과를 줍니다.
        IsPoppingUp = 1 << 4,
        // 버튼 위에 마우스 포인터가 올라왔을 때 자식 오브젝트들을 보여줍니다.
        IsIncludingTooltips = 1 << 5,
    };
    void LoadUITextures() const;
    void ReportButtonOnMouse(UIButton* p_btn);
    void ReportMouseExitButton(UIButton* p_btn);

    bool IsMouseOnButton();
    void ImportUI(const char* path);

    virtual void Update() override;
};

