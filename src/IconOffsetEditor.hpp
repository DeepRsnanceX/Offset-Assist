#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

enum class SelectedSpritePart {
    FirstLayer,
    SecondLayer,
    Outline,
    Detail,
    Dome
};

class IconOffsetEditorPopup : public Popup<> {
protected:
    bool setup() override;
    
    SimplePlayer* m_previewPlayer = nullptr;
    geode::TextInput* m_inputX = nullptr;
    geode::TextInput* m_inputY = nullptr;
    CCLabelBMFont* m_labelX = nullptr;
    CCLabelBMFont* m_labelY = nullptr;
    CCMenu* m_partSelectMenu = nullptr;
    CCMenuItemSpriteExtra* m_updateButton = nullptr;
    
    SelectedSpritePart m_selectedPart = SelectedSpritePart::FirstLayer;
    IconType m_currentIconType;
    
    std::map<std::string, std::vector<CCSprite*>> m_robotSpiderSprites;
    std::map<std::string, CCMenuItemSpriteExtra*> m_frameButtons;
    std::map<SelectedSpritePart, CCMenuItemSpriteExtra*> m_partButtons;
    std::map<std::string, CCPoint> m_storedOffsets;
    std::vector<std::string> m_frameNames;
    std::string m_currentFrameName;
    
    void updatePreviewPlayer();
    void onPartSelected(CCObject* sender);
    void onUpdateOffsets(CCObject* sender);
    void onToggleGlow(CCObject* sender);
    void updateInputFields();
    CCSprite* getCurrentSelectedSprite();
    void highlightSelectedButton();
    bool isUnsupportedIconType();
    void mapRobotSpiderSprites(CCNode* node);
    void applyOffsetToAllMatchingSprites(CCNode* node, const std::string& frameName, CCPoint offset); // holy fucking long ass function
    
public:
    static IconOffsetEditorPopup* create();
};