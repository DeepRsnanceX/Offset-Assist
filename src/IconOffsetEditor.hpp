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

class IconOffsetEditorPopup : public Popup<>, public geode::ColorPickPopupDelegate {
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

    CCMenu* m_colorPickerMenu = nullptr;
    std::string m_currentColorSettingId;
    CCSprite* m_currentColorButtonSprite = nullptr;
    ccColor3B m_previewColor1;
    ccColor3B m_previewColor2;
    ccColor3B m_previewGlowColor;

    CCMenuItemSpriteExtra* m_glowToggler = nullptr;
    CCLabelBMFont* m_rotationSpeedLabel = nullptr;
    Slider* m_cubeOpacitySlider = nullptr;
    CCLabelBMFont* m_cubeOpacityLabel = nullptr;
    CCMenu* m_animButtonsMenu = nullptr;
    Slider* m_rotationSpeedSlider = nullptr;
    CCLabelBMFont* m_iconNameLabel = nullptr;
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

    std::stringstream m_logStream;
    
    void updatePreviewPlayer();
    void onPartSelected(CCObject* sender);
    void onUpdateOffsets(CCObject* sender);
    void onToggleGlow(CCObject* sender);
    void onInfoButton(CCObject* sender);
    void onWhy(CCObject* sender); // :sob:
    void onPlayAnimation(CCObject* sender);
    void onPlayBallRotation(CCObject* sender);
    void onStopBallRotation(CCObject* sender);
    void onRotationSpeedChanged(CCObject* sender);
    void onCubeOpacityChanged(CCObject* sender);
    void onRenderIcon(CCObject* sender);
    void onModSettings(CCObject* sender);
    void updateInputFields();
    void drawHitbox();
    void onToggleHitbox(CCObject* sender);
    void onSavePlist(CCObject* sender);
    void onOpenRendersFolder(CCObject* sender);
    void highlightSelectedButton();
    bool isUnsupportedIconType();
    void mapRobotSpiderSprites(CCNode* node);
    void applyOffsetToAllMatchingSprites(CCNode* node, const std::string& frameName, CCPoint offset);
    void addToLog(const std::string& logMsg, int newLines);
    void processPlistSave(bool remapNames);
    CCSprite* getCurrentSelectedSprite();
    CCImage* getIconImage();
    std::string getCurrentRealFrameName();

    void onColorPicker(CCObject* sender);
    void updateColor(cocos2d::ccColor4B const& color) override;
    void applyPreviewColors();
    CCMenuItemSpriteExtra* createColorPickerButton(const std::string& colorId, ccColor3B currentColor);
    
public:
    static IconOffsetEditorPopup* create();
};
