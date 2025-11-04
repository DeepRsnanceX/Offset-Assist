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

struct FrameOffsetData {
    std::string frameName;
    CCPoint offset;
};

class IconOffsetEditorPopup : public Popup<> {
protected:
    bool setup() override;
    
    SimplePlayer* m_previewPlayer = nullptr;
    CCNode* m_iconContainerNode = nullptr;
    CCSprite* m_cubePreview = nullptr;
    geode::TextInput* m_inputX = nullptr;
    geode::TextInput* m_inputY = nullptr;
    CCLabelBMFont* m_labelX = nullptr;
    CCLabelBMFont* m_labelY = nullptr;
    CCMenu* m_partSelectMenu = nullptr;
    CCMenuItemSpriteExtra* m_updateButton = nullptr;

    CCMenuItemSpriteExtra* m_glowToggler = nullptr;
    CCLabelBMFont* m_rotationSpeedLabel = nullptr;
    Slider* m_cubeOpacitySlider = nullptr;
    CCLabelBMFont* m_cubeOpacityLabel = nullptr;
    CCMenu* m_animButtonsMenu = nullptr;
    Slider* m_rotationSpeedSlider = nullptr;
    bool m_isRotating = false;

    CCDrawNode* m_hitboxDrawNode = nullptr;
    CCMenuItemSpriteExtra* m_hitboxToggler = nullptr;
    bool m_showHitbox = false;
    
    SelectedSpritePart m_selectedPart = SelectedSpritePart::FirstLayer;
    IconType m_currentIconType;
    
    std::map<std::string, std::vector<CCSprite*>> m_robotSpiderSprites;
    std::map<std::string, CCMenuItemSpriteExtra*> m_frameButtons;
    std::map<SelectedSpritePart, CCMenuItemSpriteExtra*> m_partButtons;
    std::map<std::string, CCPoint> m_modifiedOffsets;
    
    std::vector<std::string> m_frameNames;
    std::string m_currentFrameName;
    
    void updatePreviewPlayer();
    void onPartSelected(CCObject* sender);
    void onUpdateOffsets(CCObject* sender);
    void onToggleGlow(CCObject* sender);
    void onInfoButton(CCObject* sender);
    void onPlayAnimation(CCObject* sender);
    void onPlayBallRotation(CCObject* sender);
    void onStopBallRotation(CCObject* sender);
    void onRotationSpeedChanged(CCObject* sender);
    void onCubeOpacityChanged(CCObject* sender);
    void onRenderIcon(CCObject* sender);
    void updateInputFields();
    void drawHitbox();
    void onToggleHitbox(CCObject* sender);
    void onSavePlist(CCObject* sender);
    void onOpenRendersFolder(CCObject* sender);
    void highlightSelectedButton();
    bool isUnsupportedIconType();
    void mapRobotSpiderSprites(CCNode* node);
    void applyOffsetToAllMatchingSprites(CCNode* node, const std::string& frameName, CCPoint offset);
    CCSprite* getCurrentSelectedSprite();
    CCImage* getIconImage();
    std::string getCurrentRealFrameName();
    
public:
    static IconOffsetEditorPopup* create();
};