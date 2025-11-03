#include "IconOffsetEditor.hpp"
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/SimplePlayer.hpp>
#include <hiimjustin000.more_icons/include/MoreIcons.hpp>

// a girl gotta be honest
// ty to copilot for helping me figure out how tf to do the auto plist editing feature LOL
// modding be so fun then BAM
// u have to write to a file ...

void updatePreviewIcon(SimplePlayer* player, IconType iconType) {
    auto manager = GameManager::sharedState();
	int typeAsInt = static_cast<int>(iconType);
    
    switch(iconType) {
        case IconType::Cube:
            player->updatePlayerFrame(manager->getPlayerFrame(), IconType::Cube);
            break;
        case IconType::Ship:
            player->updatePlayerFrame(manager->getPlayerShip(), IconType::Ship);
            break;
        case IconType::Ball:
            player->updatePlayerFrame(manager->getPlayerBall(), IconType::Ball);
            break;
        case IconType::Ufo:
            player->updatePlayerFrame(manager->getPlayerBird(), IconType::Ufo);
            break;
        case IconType::Wave:
            player->updatePlayerFrame(manager->getPlayerDart(), IconType::Wave);
            break;
        case IconType::Robot:
            player->updatePlayerFrame(manager->getPlayerRobot(), IconType::Robot);
            break;
        case IconType::Spider:
            player->updatePlayerFrame(manager->getPlayerSpider(), IconType::Spider);
            break;
        case IconType::Swing:
            player->updatePlayerFrame(manager->getPlayerSwing(), IconType::Swing);
            break;
        case IconType::Jetpack:
            player->updatePlayerFrame(manager->getPlayerJetpack(), IconType::Jetpack);
            break;
        default:
            player->updatePlayerFrame(manager->getPlayerFrame(), IconType::Cube);
            break;
    }
    
    MoreIcons::updateSimplePlayer(player, iconType);
    
    player->setColor(manager->colorForIdx(manager->getPlayerColor()));
    player->setSecondColor(manager->colorForIdx(manager->getPlayerColor2()));
    player->setGlowOutline(manager->colorForIdx(manager->getPlayerGlowColor()));
    player->enableCustomGlowColor(manager->colorForIdx(manager->getPlayerGlowColor()));
    
    if (!manager->getPlayerGlow()) {
        player->disableGlowOutline();
    }
}

CCSize getHitboxSizeForIconType(IconType iconType) {
    switch(iconType) {
        case IconType::Cube:
        case IconType::Ship:
        case IconType::Ball:
        case IconType::Ufo:
        case IconType::Robot:
        case IconType::Swing:
        case IconType::Jetpack:
            return {30.f * 2.f, 30.f * 2.f};
        case IconType::Wave:
            return {10.f * 2.f, 10.f * 2.f};
        case IconType::Spider:
            return {27.f * 2.f, 27.f * 2.f};
        default:
            return {30.f, 30.f};
    }
}

std::string getRealFrameName(const std::string& fullFrameName) {
    std::string result = fullFrameName;
    
    const std::string moreIconsPrefix = "hiimjustin000.more_icons/";
    if (utils::string::startsWith(result, moreIconsPrefix)) {
        result = result.substr(moreIconsPrefix.length());
    }
    
    size_t colonPos = result.find(':');
    if (colonPos != std::string::npos) {
        result = result.substr(colonPos + 1);
    }
    
    return result;
}

IconOffsetEditorPopup* IconOffsetEditorPopup::create() {
    auto ret = new IconOffsetEditorPopup();
    if (ret->initAnchored(280.0f, 175.0f, "GJ_square01.png")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool IconOffsetEditorPopup::setup() {
    this->setTitle("Icon Offset Editor");

    auto manager = GameManager::sharedState();
    m_currentIconType = manager->m_playerIconType;
    IconInfo* icInfo = MoreIcons::getIcon(m_currentIconType);
    auto size = this->m_mainLayer->getContentSize();
    auto popupSize = this->getContentSize();
    bool isRobotOrSpider = (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider);

    const float midX = size.width / 2.f;
    const float midY = size.height / 2.f;
    const float inputX = midX - 40.0f;
    const float inputYTop = midY + 25.0f;
    const float lowerMenuX = midX - 55.f;
    const float lowerMenuBaseY = midY - 52.5f;

    // -----------------------
    // INFO BUTTON
    // -----------------------
    auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoSpr->setScale(0.8f);
    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoSpr,
        this,
        menu_selector(IconOffsetEditorPopup::onInfoButton)
    );

    auto infoBtnMenu = CCMenu::create();
    infoBtnMenu->addChild(infoBtn);
    infoBtnMenu->setPosition({size.width - 15.f, size.height - 15.f});
    this->m_mainLayer->addChild(infoBtnMenu);
    
    // -----------------------
    // DISABLE FOR VANILLA ICONS (L bozo sorry it's easier to work with MI)
    // -----------------------
    if (!icInfo) {
        auto warningLabel = CCLabelBMFont::create("Vanilla icons are not supported!\nPlease load your icons via\nMore Icons instead.", "bigFont.fnt");
        warningLabel->setPosition({size.width / 2.0f, size.height / 2.0f});
        warningLabel->setScale(0.4f);
        warningLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
        warningLabel->setColor({255, 0, 0});
        this->m_mainLayer->addChild(warningLabel);
        return true;
    }

    m_mainLayer->setPosition({m_mainLayer->getPositionX() - 60.f, m_mainLayer->getPositionY()});
    
    // -----------------------
    // SETUP SIMPLEPLAYER
    // -----------------------
    m_previewPlayer = SimplePlayer::create(0);
    m_previewPlayer->setPosition({midX + 75.0f, midY});
    m_previewPlayer->setScale(2.f);
    m_previewPlayer->setZOrder(2);
    updatePreviewPlayer();
    
    this->m_mainLayer->addChild(m_previewPlayer);

    // -----------------------
    // PREVIEW CONTROLS MENU (Glow + Hitbox)
    // -----------------------
    auto previewMenu = CCMenu::create();
    previewMenu->setPosition({midX + 75.0f, midY - 60.f});
    previewMenu->setContentSize({80.f, 30.f});
    previewMenu->setLayout(
        RowLayout::create()
            ->setGap(6.f)
            ->setAxisAlignment(AxisAlignment::Center)
    );
    this->m_mainLayer->addChild(previewMenu);

    auto glowLabel = CCLabelBMFont::create("Glow", "bigFont.fnt");
    glowLabel->setScale(0.35f);
    auto glowSpr = CircleButtonSprite::create(glowLabel, CircleBaseColor::Pink, CircleBaseSize::Small);
    glowSpr->setScale(0.7f);
    m_glowToggler = CCMenuItemSpriteExtra::create(
        glowSpr,
        this,
        menu_selector(IconOffsetEditorPopup::onToggleGlow)
    );
    previewMenu->addChild(m_glowToggler);

    auto hitboxLabel = CCLabelBMFont::create("Hitbox", "bigFont.fnt");
    hitboxLabel->setScale(0.35f);
    auto hitboxSpr = CircleButtonSprite::create(hitboxLabel, CircleBaseColor::Cyan, CircleBaseSize::Small);
    hitboxSpr->setScale(0.7f);
    m_hitboxToggler = CCMenuItemSpriteExtra::create(
        hitboxSpr,
        this,
        menu_selector(IconOffsetEditorPopup::onToggleHitbox)
    );
    previewMenu->addChild(m_hitboxToggler);

    previewMenu->updateLayout();

    if (isRobotOrSpider) {
        auto robotSprite = (m_currentIconType == IconType::Robot) ? 
            m_previewPlayer->m_robotSprite : nullptr;
        auto spiderSprite = (m_currentIconType == IconType::Spider) ? 
            m_previewPlayer->m_spiderSprite : nullptr;
        
        if (robotSprite && robotSprite->m_glowSprite) {
            robotSprite->m_glowSprite->setVisible(true);
        }
        if (spiderSprite && spiderSprite->m_glowSprite) {
            spiderSprite->m_glowSprite->setVisible(true);
        }
    } else {
        if (m_previewPlayer->m_outlineSprite) m_previewPlayer->m_outlineSprite->setVisible(true);
    }

    m_hitboxDrawNode = CCDrawNode::create();
    m_hitboxDrawNode->setZOrder(10);
    m_hitboxDrawNode->setVisible(false);
    if (m_currentIconType == IconType::Ship) m_hitboxDrawNode->setPosition({0.f, 10.f});
    this->m_mainLayer->addChild(m_hitboxDrawNode);

    drawHitbox();

    // -----------------------
    // ROBOT/SPIDER ANIMATIONS MENU
    // -----------------------
    if (isRobotOrSpider) {
        m_animButtonsMenu = CCMenu::create();
        m_animButtonsMenu->setPosition({lowerMenuX, lowerMenuBaseY - 5.f});
        m_animButtonsMenu->setScale(0.6f);
        m_animButtonsMenu->setContentSize({200.f, 40.f});
        m_animButtonsMenu->setLayout(
            RowLayout::create()
                ->setGap(4.f)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setAxisReverse(false)
        );
        this->m_mainLayer->addChild(m_animButtonsMenu);

        auto animDescLabel = CCLabelBMFont::create("Test Animations", "goldFont.fnt");
        animDescLabel->setPosition({lowerMenuX, lowerMenuBaseY + 15.f});
        animDescLabel->setScale(0.4f);
        this->m_mainLayer->addChild(animDescLabel);
        
        if (m_currentIconType == IconType::Robot) {
            auto createAnimBtn = [this](const char* label, const char* animName) {
                auto lbl = CCLabelBMFont::create(label, "bigFont.fnt");
                lbl->setScale(0.4f);
                auto btn = CCMenuItemSpriteExtra::create(
                    CircleButtonSprite::create(lbl, CircleBaseColor::Gray, CircleBaseSize::Small),
                    this,
                    menu_selector(IconOffsetEditorPopup::onPlayAnimation)
                );
                btn->setUserObject(CCString::create(animName));
                return btn;
            };
            
            m_animButtonsMenu->addChild(createAnimBtn("idle", "idle"));
            m_animButtonsMenu->addChild(createAnimBtn("idle01", "idle01"));
            m_animButtonsMenu->addChild(createAnimBtn("run", "run"));
            m_animButtonsMenu->addChild(createAnimBtn("run2", "run2"));
            m_animButtonsMenu->addChild(createAnimBtn("run3", "run3"));
        } else {
            auto createAnimBtn = [this](const char* label, const char* animName) {
                auto lbl = CCLabelBMFont::create(label, "bigFont.fnt");
                lbl->setScale(0.4f);
                auto btn = CCMenuItemSpriteExtra::create(
                    CircleButtonSprite::create(lbl, CircleBaseColor::Gray, CircleBaseSize::Small),
                    this,
                    menu_selector(IconOffsetEditorPopup::onPlayAnimation)
                );
                btn->setUserObject(CCString::create(animName));
                return btn;
            };
            
            m_animButtonsMenu->addChild(createAnimBtn("idle", "idle"));
            m_animButtonsMenu->addChild(createAnimBtn("idle01", "idle01"));
            m_animButtonsMenu->addChild(createAnimBtn("walk", "walk"));
            m_animButtonsMenu->addChild(createAnimBtn("run", "run"));
            m_animButtonsMenu->addChild(createAnimBtn("run2", "run2"));
        }
        
        m_animButtonsMenu->updateLayout();
    }

    // -----------------------
    // BALL ROTATION PREVIEW CONTROLS
    // -----------------------
    if (m_currentIconType == IconType::Ball) {
        m_animButtonsMenu = CCMenu::create();
        m_animButtonsMenu->setPosition({lowerMenuX, lowerMenuBaseY - 18.f});
        m_animButtonsMenu->setContentSize({55.f, 40.f});
        m_animButtonsMenu->setLayout(
            RowLayout::create()
                ->setGap(6.f)
                ->setAxisAlignment(AxisAlignment::Center)
        );
        m_animButtonsMenu->setScale(0.7f);
        this->m_mainLayer->addChild(m_animButtonsMenu);
        
        auto playLbl = CCLabelBMFont::create("Spin", "bigFont.fnt");
        playLbl->setScale(0.35f);
        auto playBtn = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::create(playLbl, CircleBaseColor::Green, CircleBaseSize::Small),
            this,
            menu_selector(IconOffsetEditorPopup::onPlayBallRotation)
        );
        
        auto stopLbl = CCLabelBMFont::create("Stop", "bigFont.fnt");
        stopLbl->setScale(0.35f);
        auto stopBtn = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::create(stopLbl, CircleBaseColor::Gray, CircleBaseSize::Small),
            this,
            menu_selector(IconOffsetEditorPopup::onStopBallRotation)
        );
        
        m_animButtonsMenu->addChild(playBtn);
        m_animButtonsMenu->addChild(stopBtn);
        m_animButtonsMenu->updateLayout();
        
        // rotation speed slider
        auto speedLabel = CCLabelBMFont::create("Full Spin Duration:", "bigFont.fnt");
        speedLabel->setPosition({lowerMenuX - 15.f, lowerMenuBaseY + 15.f});
        speedLabel->setScale(0.3f);
        this->m_mainLayer->addChild(speedLabel);
        
        m_rotationSpeedSlider = Slider::create(
            this, 
            menu_selector(IconOffsetEditorPopup::onRotationSpeedChanged), 
            0.6f
        );
        m_rotationSpeedSlider->setValue(0.5f);
        m_rotationSpeedSlider->m_sliderBar->setContentSize({80.f, m_rotationSpeedSlider->m_sliderBar->getContentSize().height});
        
        auto sliderMenu = CCMenu::create();
        sliderMenu->addChild(m_rotationSpeedSlider);
        sliderMenu->setPosition({lowerMenuX, lowerMenuBaseY});
        this->m_mainLayer->addChild(sliderMenu);
        
        m_rotationSpeedLabel = CCLabelBMFont::create("1.0", "bigFont.fnt");
        m_rotationSpeedLabel->setPosition({lowerMenuX + 50.f, lowerMenuBaseY + 15.f});
        m_rotationSpeedLabel->setScale(0.3f);
        this->m_mainLayer->addChild(m_rotationSpeedLabel);
    }

    // -----------------------
    // CUBE PREVIEW FOR RIDER GAMEMODES
    // -----------------------
    if (m_currentIconType == IconType::Ship || m_currentIconType == IconType::Ufo || m_currentIconType == IconType::Jetpack) {        
        m_cubePreview = CCSprite::create("exampleCube.png"_spr);
        
        // welcome back icon preview
        if (m_currentIconType == IconType::Ship) {
            m_cubePreview->setPosition({
                m_previewPlayer->getPositionX(),
                m_previewPlayer->getPositionY() + 20.f
            });
            m_cubePreview->setScale(1.1f);
        } else if (m_currentIconType == IconType::Ufo) {
            m_cubePreview->setPosition({
                m_previewPlayer->getPositionX(),
                m_previewPlayer->getPositionY() + 12.f
            });
            m_cubePreview->setScale(1.1f);
        } else if (m_currentIconType == IconType::Jetpack) {
            m_cubePreview->setPosition({
                m_previewPlayer->getPositionX() + 12.f,
                m_previewPlayer->getPositionY() + 8.f
            });
            m_cubePreview->setScale(1.2f);
        }
        
        this->m_mainLayer->addChild(m_cubePreview);
        
        // cube opacity slider
        auto opacityLabel = CCLabelBMFont::create("Cube Opacity:", "goldFont.fnt");
        opacityLabel->setPosition({lowerMenuX, lowerMenuBaseY + 15.f});
        opacityLabel->setScale(0.35f);
        this->m_mainLayer->addChild(opacityLabel);
        
        m_cubeOpacitySlider = Slider::create(
            this,
            menu_selector(IconOffsetEditorPopup::onCubeOpacityChanged),
            0.6f
        );
        m_cubeOpacitySlider->setValue(1.0f);
        m_cubeOpacitySlider->m_sliderBar->setContentSize({60.f, m_cubeOpacitySlider->m_sliderBar->getContentSize().height});
        
        auto opacityMenu = CCMenu::create();
        opacityMenu->addChild(m_cubeOpacitySlider);
        opacityMenu->setPosition({lowerMenuX, lowerMenuBaseY});
        this->m_mainLayer->addChild(opacityMenu);
        
        m_cubeOpacityLabel = CCLabelBMFont::create("100%", "bigFont.fnt");
        m_cubeOpacityLabel->setPosition({lowerMenuX, lowerMenuBaseY - 10.f});
        m_cubeOpacityLabel->setScale(0.25f);
        m_cubeOpacityLabel->setOpacity(150);
        this->m_mainLayer->addChild(m_cubeOpacityLabel);
    }

    // -----------------------
    // MAIN OFFSET INPUTS SETUP
    // -----------------------
    
    // x offset
    m_labelX = CCLabelBMFont::create("Offset X:", "bigFont.fnt");
    m_labelX->setPosition({inputX - 80.0f, inputYTop});
    m_labelX->setScale(0.4f);
    m_labelX->setAnchorPoint({0.0f, 0.5f});
    this->m_mainLayer->addChild(m_labelX);
    
    m_inputX = geode::TextInput::create(80.0f, "0.0", "bigFont.fnt");
    m_inputX->setPosition({inputX + 20.0f, inputYTop});
    m_inputX->setScale(0.7f);
    m_inputX->setFilter("0123456789.-");
    this->m_mainLayer->addChild(m_inputX);
    
    // y offset
    m_labelY = CCLabelBMFont::create("Offset Y:", "bigFont.fnt");
    m_labelY->setPosition({inputX - 80.0f, inputYTop - 40.0f});
    m_labelY->setScale(0.4f);
    m_labelY->setAnchorPoint({0.0f, 0.5f});
    this->m_mainLayer->addChild(m_labelY);
    
    m_inputY = geode::TextInput::create(80.0f, "0.0", "bigFont.fnt");
    m_inputY->setPosition({inputX + 20.0f, inputYTop - 40.0f});
    m_inputY->setScale(0.7f);
    m_inputY->setFilter("0123456789.-");
    this->m_mainLayer->addChild(m_inputY);
    
    // action buttons hi
    auto updateBtnSpr = ButtonSprite::create("Update", "goldFont.fnt", "GJ_button_01.png", 0.7f);
    m_updateButton = CCMenuItemSpriteExtra::create(
        updateBtnSpr,
        this,
        menu_selector(IconOffsetEditorPopup::onUpdateOffsets)
    );

    auto savePlistSpr = ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_01.png", 0.7f);
    auto savePlistBtn = CCMenuItemSpriteExtra::create(
        savePlistSpr,
        this,
        menu_selector(IconOffsetEditorPopup::onSavePlist)
    );
    
    auto buttonMenu = CCMenu::create();
    buttonMenu->setPosition({midX, -20.f});
    buttonMenu->addChild(m_updateButton);
    buttonMenu->addChild(savePlistBtn);
    buttonMenu->setLayout(
        RowLayout::create()
            ->setGap(4.f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setAxisReverse(false)
    );
    
    this->m_mainLayer->addChild(buttonMenu);

    int partCount = 4;
    
    // -----------------------
    // BUNCH OF BULLSHIT IDK BRO LOL
    // THIS (somehow) WORKS DON'T TOUCH IT
    // -----------------------
    if (isRobotOrSpider) {
        if (!icInfo->frameNames.empty()) {
            m_frameNames = icInfo->frameNames;
            partCount = m_frameNames.size();
            
            /*
            for (const auto& frameName : m_frameNames) {
                auto frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());
                if (frame) {
                    m_storedOffsets[frameName] = frame->getOffsetInPixels();
                }
            }
            */
            
            auto targetNode = (m_currentIconType == IconType::Robot) ? 
                static_cast<CCNode*>(m_previewPlayer->m_robotSprite) : 
                static_cast<CCNode*>(m_previewPlayer->m_spiderSprite);
            
            if (targetNode) {
                mapRobotSpiderSprites(targetNode);
                //log::info("mapped {} frames", m_robotSpiderSprites.size());
            }
            
            if (!m_frameNames.empty()) {
                m_currentFrameName = m_frameNames[0];
            }
        } else {
            log::error("couldn't get MoreIcons frameNames");
            return true;
        }
    }

    //float bgHeight = partCount * 40.f + 25.f;
    float bgWidth = (isRobotOrSpider) ? 90.f : 60.f;
    auto partBg = CCScale9Sprite::create("GJ_square01.png");
    partBg->setAnchorPoint({0.f, 0.5f});
    partBg->setContentSize({bgWidth, 200.f});
    partBg->setPosition({size.width + 25.f, midY});
    partBg->setOpacity(255);
    this->m_mainLayer->addChild(partBg, -1);
    
    m_partSelectMenu = CCMenu::create();
    if (isRobotOrSpider) {
        m_partSelectMenu->setLayout(
            ColumnLayout::create()
                ->setGap(4.0f)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setAxisReverse(true)
                ->setGrowCrossAxis(true)
        );
    } else {
        m_partSelectMenu->setLayout(
            ColumnLayout::create()
                ->setGap(4.0f)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setAxisReverse(true)
        );
    }
    
    
    float lowerBy = (m_currentIconType == IconType::Ufo) ? 10.f : 0.f;
    float moveBy = (isRobotOrSpider) ? 45.f : 30.f;
    m_partSelectMenu->setPosition({partBg->getPositionX() + moveBy, midY - lowerBy});
    m_partSelectMenu->setContentSize({20.0f, 105.f});
    m_partSelectMenu->setScale(1.75f);
    this->m_mainLayer->addChild(m_partSelectMenu);
    
    if (isRobotOrSpider) {
        for (int i = 0; i < m_frameNames.size(); i++) {
            const auto& frameName = m_frameNames[i];
            auto frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());
            if (!frame) {
                log::warn("couldn't find frame: {}", frameName);
                continue;
            }
            
            auto buttonSprite = CCSprite::createWithSpriteFrame(frame);
            buttonSprite->setColor({255, 255, 255});
            buttonSprite->setScale(0.5f);
            
            auto button = CCMenuItemSpriteExtra::create(
                buttonSprite,
                this,
                menu_selector(IconOffsetEditorPopup::onPartSelected)
            );
            button->setTag(i);
            button->setUserObject(CCString::create(frameName));

            auto btnSize = button->getContentSize();
            if (btnSize.width <= 5.f || btnSize.height <= 5.f) {
                button->setContentSize({14.f, 14.f});
                buttonSprite->setPosition({7.f, 7.f});
            } else if (btnSize.width >= 90.f || btnSize.height >= 90.f) {
                button->setContentSize({20.f, 20.f});
                //buttonSprite->setScale(0.35f);
                buttonSprite->setPosition({10.f, 10.f});
                m_partSelectMenu->updateLayout();
            }
            
            m_frameButtons[frameName] = button;
            m_partSelectMenu->addChild(button);
        }
    } else {
        auto createPartButton = [this](SelectedSpritePart part, CCSprite* sprite, const char* tooltip) {
            if (!sprite || !sprite->isVisible()) return;
            
            auto buttonSprite = CCSprite::createWithSpriteFrame(sprite->displayFrame());
            buttonSprite->setColor({255, 255, 255});
            buttonSprite->setScale(0.5f);
            
            auto button = CCMenuItemSpriteExtra::create(
                buttonSprite,
                this,
                menu_selector(IconOffsetEditorPopup::onPartSelected)
            );
            button->setTag(static_cast<int>(part));
            button->setUserObject(CCString::create(tooltip));

            auto btnSize = button->getContentSize();
            if (btnSize.width <= 5.f || btnSize.height <= 5.f) {
                button->setContentSize({14.f, 14.f});
                buttonSprite->setPosition({7.f, 7.f});
            } else if (btnSize.width >= 90.f || btnSize.height >= 90.f) {
                button->setContentSize({20.f, 20.f});
                //buttonSprite->setScale(0.35f);
                buttonSprite->setPosition({10.f, 10.f});
                m_partSelectMenu->updateLayout();
            }
            
            m_partButtons[part] = button;
            m_partSelectMenu->addChild(button);
        };
        
        createPartButton(SelectedSpritePart::FirstLayer, m_previewPlayer->m_firstLayer, "First Layer");
        createPartButton(SelectedSpritePart::SecondLayer, m_previewPlayer->m_secondLayer, "Second Layer");
        createPartButton(SelectedSpritePart::Outline, m_previewPlayer->m_outlineSprite, "Outline");
        createPartButton(SelectedSpritePart::Detail, m_previewPlayer->m_detailSprite, "Detail");
        
        if (m_currentIconType == IconType::Ufo && m_previewPlayer->m_birdDome) {
            createPartButton(SelectedSpritePart::Dome, m_previewPlayer->m_birdDome, "Dome");
        }
    }
    
    m_partSelectMenu->updateLayout();
    
    updateInputFields();
    highlightSelectedButton();
    
    return true;
}

void IconOffsetEditorPopup::updatePreviewPlayer() {
    if (!m_previewPlayer) return;
    updatePreviewIcon(m_previewPlayer, m_currentIconType);
}

CCSprite* IconOffsetEditorPopup::getCurrentSelectedSprite() {
    if (!m_previewPlayer) return nullptr;
    
    if (m_currentIconType != IconType::Robot && m_currentIconType != IconType::Spider) {
        switch(m_selectedPart) {
            case SelectedSpritePart::FirstLayer:
                return m_previewPlayer->m_firstLayer;
            case SelectedSpritePart::SecondLayer:
                return m_previewPlayer->m_secondLayer;
            case SelectedSpritePart::Outline:
                return m_previewPlayer->m_outlineSprite;
            case SelectedSpritePart::Detail:
                return m_previewPlayer->m_detailSprite;
            case SelectedSpritePart::Dome:
                return m_previewPlayer->m_birdDome;
            default:
                return nullptr;
        }
    }
    
    auto frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(m_currentFrameName.c_str());
    if (frame) {
        return nullptr;
    }
    
    return nullptr;
}

std::string IconOffsetEditorPopup::getCurrentRealFrameName() {
    if (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider) {
        return getRealFrameName(m_currentFrameName);
    }
    
    auto sprite = getCurrentSelectedSprite();
    if (!sprite) {
        log::warn("getCurrentSelectedSprite returned nullptr");
        return "";
    }
    
    auto frame = sprite->displayFrame();
    if (!frame) {
        log::warn("sprite->displayFrame() returned nullptr");
        return "";
    }
    
    auto texture = frame->getTexture();
    if (!texture) {
        log::warn("Frame has null texture");
        return "";
    }
    
    auto rect = frame->getRect();
    
    auto icInfo = MoreIcons::getIcon(m_currentIconType);
    if (!icInfo || icInfo->frameNames.empty()) {
        log::warn("Couldn't get IconInfo or frameNames is empty");
        return "";
    }
    
    for (const auto& fullFrameName : icInfo->frameNames) {
        auto cachedFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(fullFrameName.c_str());
        
        if (!cachedFrame) {
            continue;
        }
        
        auto cachedTexture = cachedFrame->getTexture();
        if (!cachedTexture) {
            continue;
        }
        
        auto cachedRect = cachedFrame->getRect();
        
        bool textureMatches = (texture == cachedTexture);
        bool rectMatches = (rect.equals(cachedRect));
        
        if (textureMatches && rectMatches) {
            std::string realName = getRealFrameName(fullFrameName);
            log::info("Found matching frame: {} -> {}", fullFrameName, realName);
            return realName;
        }
    }
    
    log::warn("No matching frame found for current sprite");
    return "";
}

void IconOffsetEditorPopup::updateInputFields() {
    std::string realFrameName = getCurrentRealFrameName();
    
    if (realFrameName.empty()) {
        log::warn("Couldn't get real frame name");
        return;
    }
    
    if (m_modifiedOffsets.count(realFrameName)) {
        auto offset = m_modifiedOffsets[realFrameName];
        m_inputX->setString(fmt::format("{:.1f}", offset.x));
        m_inputY->setString(fmt::format("{:.1f}", offset.y));
        return;
    }
    
    if (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider) {
        auto frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(m_currentFrameName.c_str());
        if (frame) {
            auto offset = frame->getOffsetInPixels();
            m_inputX->setString(fmt::format("{:.1f}", offset.x));
            m_inputY->setString(fmt::format("{:.1f}", offset.y));
        }
    } else {
        auto sprite = getCurrentSelectedSprite();
        if (sprite) {
            auto frame = sprite->displayFrame();
            if (frame) {
                auto offset = frame->getOffsetInPixels();
                m_inputX->setString(fmt::format("{:.1f}", offset.x));
                m_inputY->setString(fmt::format("{:.1f}", offset.y));
            }
        }
    }
}

void IconOffsetEditorPopup::onPartSelected(CCObject* sender) {
    auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
    if (!button) return;
    
    if (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider) {
        auto frameNameObj = dynamic_cast<CCString*>(button->getUserObject());
        if (frameNameObj) {
            m_currentFrameName = frameNameObj->getCString();
        }
    } else {
        m_selectedPart = static_cast<SelectedSpritePart>(button->getTag());
    }
    
    updateInputFields();
    highlightSelectedButton();
}

void IconOffsetEditorPopup::onInfoButton(CCObject* sender) {
    FLAlertLayer::create(
        "Offset Assist",
        "This is the Icon Offset Editor Menu!\nIn here, you can modify the sprite offsets of each individual Icon Sprite.\nOnce you've made some changes, click on the Refresh button on the lower right corner to refresh the Preview and check how those new offsets look!\nThere's also some buttons to preview Robot/Spider animations, Ball rolling, Cube pos, anything u may need!",
        "OK"
    )->show();
}

void IconOffsetEditorPopup::onToggleGlow(CCObject* sender) {

    if (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider) {
        auto robotSprite = (m_currentIconType == IconType::Robot) ? 
            m_previewPlayer->m_robotSprite : nullptr;
        auto spiderSprite = (m_currentIconType == IconType::Spider) ? 
            m_previewPlayer->m_spiderSprite : nullptr;
        
        if (robotSprite && robotSprite->m_glowSprite) {
            robotSprite->m_glowSprite->setVisible(!robotSprite->m_glowSprite->isVisible());
        }
        if (spiderSprite && spiderSprite->m_glowSprite) {
            spiderSprite->m_glowSprite->setVisible(!spiderSprite->m_glowSprite->isVisible());
        }
    } else {
        if (m_previewPlayer->m_outlineSprite) m_previewPlayer->m_outlineSprite->setVisible(!m_previewPlayer->m_outlineSprite->isVisible());
    }
}

void IconOffsetEditorPopup::drawHitbox() {
    if (!m_hitboxDrawNode || !m_previewPlayer) return;
    
    m_hitboxDrawNode->clear();
    
    auto hitboxSize = getHitboxSizeForIconType(m_currentIconType);
    
    auto playerPos = m_previewPlayer->getPosition();
    
    CCRect hitboxRect = {
        playerPos.x - hitboxSize.width / 2.f,
        playerPos.y - hitboxSize.height / 2.f,
        hitboxSize.width,
        hitboxSize.height
    };
    
    ccColor4F borderColor = {1.0f, 0.0f, 0.0f, 1.0f};
    ccColor4F fillColor = {1.0f, 0.0f, 0.0f, 0.15f}; 
    float borderSize = 0.5f;
    
    std::array<CCPoint, 4> vertices = {
        CCPoint{hitboxRect.getMinX(), hitboxRect.getMinY()},
        CCPoint{hitboxRect.getMinX(), hitboxRect.getMaxY()},
        CCPoint{hitboxRect.getMaxX(), hitboxRect.getMaxY()},
        CCPoint{hitboxRect.getMaxX(), hitboxRect.getMinY()}
    };
    
    m_hitboxDrawNode->drawPolygon(vertices.data(), 4, fillColor, borderSize, borderColor);
}

void IconOffsetEditorPopup::onToggleHitbox(CCObject* sender) {
    m_showHitbox = !m_showHitbox;
    
    if (m_hitboxDrawNode) {
        m_hitboxDrawNode->setVisible(m_showHitbox);
    }
}

void IconOffsetEditorPopup::onPlayAnimation(CCObject* sender) {
    auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
    auto animName = dynamic_cast<CCString*>(btn->getUserObject());
    if (!animName) return;
    
    if (m_currentIconType == IconType::Robot && m_previewPlayer->m_robotSprite) {
        m_previewPlayer->m_robotSprite->runAnimation(animName->getCString());
    } else if (m_currentIconType == IconType::Spider && m_previewPlayer->m_spiderSprite) {
        m_previewPlayer->m_spiderSprite->runAnimation(animName->getCString());
    }
}

void IconOffsetEditorPopup::onPlayBallRotation(CCObject* sender) {
    if (!m_previewPlayer) return;
    
    m_previewPlayer->stopAllActions();
    m_previewPlayer->setRotation(0);
    
    float speed = 0.1f + (m_rotationSpeedSlider->getValue() * 1.9f);
    
    auto rotate = CCRotateBy::create(speed, 360.f);
    auto repeat = CCRepeatForever::create(rotate);
    m_previewPlayer->runAction(repeat);
    
    m_isRotating = true;
}

void IconOffsetEditorPopup::onStopBallRotation(CCObject* sender) {
    if (!m_previewPlayer) return;
    
    m_previewPlayer->stopAllActions();
    m_previewPlayer->setRotation(0);
    m_isRotating = false;
}

void IconOffsetEditorPopup::onRotationSpeedChanged(CCObject* sender) {
    float speed = 0.1f + (m_rotationSpeedSlider->getValue() * 1.9f);
    
    if (m_rotationSpeedLabel) m_rotationSpeedLabel->setString(fmt::format("{:.1f}s", speed).c_str());
    if (m_isRotating) onPlayBallRotation(nullptr);
}

void IconOffsetEditorPopup::onCubeOpacityChanged(CCObject* sender) {
    if (!m_cubePreview) return;
    
    float opacity = m_cubeOpacitySlider->getValue();
    m_cubePreview->setOpacity(static_cast<GLubyte>(opacity * 255));
    
    if (m_cubeOpacityLabel) m_cubeOpacityLabel->setString(fmt::format("{}%", static_cast<int>(opacity * 100)).c_str());
}

void IconOffsetEditorPopup::highlightSelectedButton() {
    bool isRobotOrSpider = (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider);
    
    if (isRobotOrSpider) {
        // spider/robot buttons
        for (auto& [frameName, button] : m_frameButtons) {
            auto sprite = static_cast<CCSprite*>(button->getNormalImage());
            sprite->stopAllActions();
            sprite->setColor({255, 255, 255});
        }
        
        if (m_frameButtons.count(m_currentFrameName)) {
            auto selectedButton = m_frameButtons[m_currentFrameName];
            auto sprite = static_cast<CCSprite*>(selectedButton->getNormalImage());
            
            auto tintToGray = CCTintTo::create(0.35f, 90, 90, 90);
            auto tintToWhite = CCTintTo::create(0.35f, 255, 255, 255);
            auto sequence = CCSequence::create(tintToGray, tintToWhite, nullptr);
            auto repeat = CCRepeatForever::create(sequence);
            
            sprite->runAction(repeat);
        }
    } else {
        // normal buttons
        for (auto& [part, button] : m_partButtons) {
            auto sprite = static_cast<CCSprite*>(button->getNormalImage());
            sprite->stopAllActions();
            sprite->setColor({255, 255, 255});
        }
        
        if (m_partButtons.count(m_selectedPart)) {
            auto selectedButton = m_partButtons[m_selectedPart];
            auto sprite = static_cast<CCSprite*>(selectedButton->getNormalImage());
            
            auto tintToGray = CCTintTo::create(0.35f, 90, 90, 90);
            auto tintToWhite = CCTintTo::create(0.35f, 255, 255, 255);
            auto sequence = CCSequence::create(tintToGray, tintToWhite, nullptr);
            auto repeat = CCRepeatForever::create(sequence);
            
            sprite->runAction(repeat);
        }
    }
}

void IconOffsetEditorPopup::onUpdateOffsets(CCObject* sender) {
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    
    try {
        std::string xStr = m_inputX->getString();
        std::string yStr = m_inputY->getString();
        
        if (!xStr.empty()) offsetX = std::stof(xStr);
        if (!yStr.empty()) offsetY = std::stof(yStr);
    } catch (const std::exception& e) {
        log::error("Failed to parse offset values: {}", e.what());
        return;
    }
    
    CCPoint newOffset = {offsetX, offsetY};
    std::string realFrameName = getCurrentRealFrameName();
    
    if (realFrameName.empty()) {
        log::error("Couldn't determine frame name");
        return;
    }
    
    m_modifiedOffsets[realFrameName] = newOffset;
    
    if (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider) {
        auto robotSprite = (m_currentIconType == IconType::Robot) ? 
            m_previewPlayer->m_robotSprite : nullptr;
        auto spiderSprite = (m_currentIconType == IconType::Spider) ? 
            m_previewPlayer->m_spiderSprite : nullptr;
        
        CCNode* targetNode = robotSprite ? static_cast<CCNode*>(robotSprite) : static_cast<CCNode*>(spiderSprite);
        
        if (targetNode) {
            applyOffsetToAllMatchingSprites(targetNode, m_currentFrameName, newOffset);
        }
    } else {
        auto sprite = getCurrentSelectedSprite();
        if (sprite) {
            auto frame = sprite->displayFrame();
            if (frame) {
                frame->setOffsetInPixels(newOffset);
                sprite->setDisplayFrame(frame);
            }
        }
    }
    
    log::info("Updated offset for frame '{}' to ({}, {})", realFrameName, offsetX, offsetY);
}

void IconOffsetEditorPopup::onSavePlist(CCObject* sender) {
    if (m_modifiedOffsets.empty()) {
        FLAlertLayer::create("No Changes", "You haven't modified any offsets yet!", "OK")->show();
        return;
    }
    
    auto icInfo = MoreIcons::getIcon(m_currentIconType);
    if (!icInfo) {
        FLAlertLayer::create("Error", "Couldn't get icon info!", "OK")->show();
        return;
    }
    
    std::string plistPath = icInfo->sheetName;
    
    if (plistPath.empty()) {
        FLAlertLayer::create("Error", "Couldn't get the icon's plist!\nPlease make sure your Texture Pack path is valid, and you're not trying to edit a zipped texture pack.", "OK")->show();
        return;
    }
    
    // Read the plist file using Geode's utilities
    auto readResult = utils::file::readString(plistPath);
    if (!readResult) {
        FLAlertLayer::create("Error", 
            fmt::format("Couldn't open plist file for reading.\nError: {}", readResult.unwrapErr()),
            "OK")->show();
        return;
    }
    
    std::string plistContent = readResult.unwrap();
    int updatedCount = 0;
    
    log::info("=== Starting plist save process ===");
    log::info("Modified offsets to save:");
    for (const auto& [name, offset] : m_modifiedOffsets) {
        log::info("  - '{}': ({}, {})", name, offset.x, offset.y);
    }
    
    // Update each modified offset in the plist content
    for (const auto& [frameName, offset] : m_modifiedOffsets) {
        log::info("Processing frame: '{}'", frameName);
        
        // Find the frame section
        std::string frameKey = fmt::format("<key>{}</key>", frameName);
        size_t framePos = plistContent.find(frameKey);
        
        if (framePos == std::string::npos) {
            log::warn("Couldn't find frame key '{}' in plist", frameKey);
            continue;
        }
        
        log::info("Found frame at position {}", framePos);
        
        // Find the next <dict> tag (start of frame data)
        size_t frameDictStart = plistContent.find("<dict>", framePos);
        if (frameDictStart == std::string::npos) {
            log::warn("Couldn't find opening <dict> after frame key");
            continue;
        }
        
        // Find the closing </dict> for this frame
        size_t frameDictEnd = plistContent.find("</dict>", frameDictStart);
        if (frameDictEnd == std::string::npos) {
            log::warn("Couldn't find closing </dict> for frame");
            continue;
        }
        
        // Now search for spriteOffset within this frame's dict
        size_t searchStart = frameDictStart;
        size_t searchEnd = frameDictEnd;
        
        std::string frameSection = plistContent.substr(searchStart, searchEnd - searchStart);
        size_t offsetKeyPos = frameSection.find("<key>spriteOffset</key>");
        
        if (offsetKeyPos == std::string::npos) {
            log::warn("Couldn't find spriteOffset key for frame '{}'", frameName);
            continue;
        }
        
        // Convert back to absolute position
        offsetKeyPos += searchStart;
        
        log::info("Found spriteOffset at position {}", offsetKeyPos);
        
        // Find the string tag after the key
        size_t stringStart = plistContent.find("<string>", offsetKeyPos);
        size_t stringEnd = plistContent.find("</string>", offsetKeyPos);
        
        // Make sure we're still within the frame's dict
        if (stringStart == std::string::npos || stringEnd == std::string::npos || 
            stringStart > frameDictEnd || stringEnd > frameDictEnd) {
            log::warn("Couldn't find offset string tags within frame boundaries");
            continue;
        }
        
        // Replace the offset value
        std::string newOffsetStr = fmt::format("{{{},{}}}", 
            static_cast<int>(std::round(offset.x)), 
            static_cast<int>(std::round(offset.y)));
        
        log::info("Replacing offset with: {}", newOffsetStr);
        
        plistContent.replace(
            stringStart + 8,  // Length of "<string>"
            stringEnd - (stringStart + 8),
            newOffsetStr
        );
        
        updatedCount++;
        log::info("Successfully updated offset for '{}'", frameName);
    }
    
    log::info("=== Plist save complete: {} updates ===", updatedCount);
    
    if (updatedCount == 0) {
        FLAlertLayer::create("Warning", "No offsets were updated in the plist.\nCheck the logs for details.", "OK")->show();
        return;
    }
    
    // Write the modified content back using Geode's utilities
    auto writeResult = utils::file::writeString(plistPath, plistContent);
    if (!writeResult) {
        FLAlertLayer::create("Error", 
            fmt::format("Couldn't write to plist file.\nError: {}", writeResult.unwrapErr()),
            "OK")->show();
        return;
    }
    
    FLAlertLayer::create(
        "Success!",
        fmt::format("Updated plist file with {} changes!\nPath: <cy>{}</c>", updatedCount, plistPath),
        "OK"
    )->show();
    log::info("Successfully edited plist '{}' with {} changes", plistPath, updatedCount);
    
    // Clear modified offsets after successful save
    m_modifiedOffsets.clear();
}

void IconOffsetEditorPopup::mapRobotSpiderSprites(CCNode* node) {
    if (!node) return;
    
    static int depth = 0;
    depth++;
    
    std::string indent(depth * 2, ' ');
    
    if (auto sprite = dynamic_cast<CCSprite*>(node)) {
        auto frame = sprite->displayFrame();
        if (frame) {
            auto texture = frame->getTexture();
            if (!texture) {
                log::debug("{}Sprite has frame but null texture, skipping", indent);
                depth--;
                return;
            }
            
            auto rect = frame->getRect();
            
            bool matched = false;
            for (const auto& frameName : m_frameNames) {
                auto cachedFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());
                
                if (cachedFrame) {
                    auto cachedTexture = cachedFrame->getTexture();
                    if (!cachedTexture) {
                        log::warn("{}Cached frame has null texture: {}", indent, frameName);
                        continue;
                    }
                    
                    auto cachedRect = cachedFrame->getRect();
                    
                    bool textureMatches = (texture == cachedTexture);
                    bool rectMatches = (rect.equals(cachedRect));
                    
                    if (textureMatches && rectMatches) {
                        m_robotSpiderSprites[frameName].push_back(sprite);
                        matched = true;
                        break;
                    }
                }
            }
        }
    }
    
    if (auto spritePart = dynamic_cast<CCSpritePart*>(node)) {
        auto frame = spritePart->displayFrame();
        if (frame) {
            auto texture = frame->getTexture();
            if (!texture) {
                log::warn("{}SpritePart has frame but null texture! skipping, but please check ur icon...", indent);
                depth--;
                return;
            }
            
            auto rect = frame->getRect();
            
            bool matched = false;
            for (const auto& frameName : m_frameNames) {
                auto cachedFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());
                
                if (cachedFrame) {
                    auto cachedTexture = cachedFrame->getTexture();
                    if (!cachedTexture) {
                        log::warn("{}Cached frame has null texture: {} - skippin, but please check ur icon...", indent, frameName);
                        continue;
                    }
                    
                    auto cachedRect = cachedFrame->getRect();
                    
                    bool textureMatches = (texture == cachedTexture);
                    bool rectMatches = (rect.equals(cachedRect));
                    
                    if (textureMatches && rectMatches) {
                        m_robotSpiderSprites[frameName].push_back(static_cast<CCSprite*>(spritePart));
                        matched = true;
                        break;
                    }
                }
            }
        }
    }
    
    auto children = node->getChildren();
    if (children && children->count() > 0) {
        CCObject* child;
        CCARRAY_FOREACH(children, child) {
            if (auto childNode = dynamic_cast<CCNode*>(child)) {
                mapRobotSpiderSprites(childNode);
            }
        }
    }
    
    depth--;
}

void IconOffsetEditorPopup::applyOffsetToAllMatchingSprites(CCNode* node, const std::string& frameName, CCPoint offset) {
    if (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider) {
        if (m_robotSpiderSprites.count(frameName)) {
            auto& sprites = m_robotSpiderSprites[frameName];
            //log::info("applying offset to {} sprites with frame: {}", sprites.size(), frameName);
            
            for (auto* sprite : sprites) {
                if (sprite) {
                    auto frame = sprite->displayFrame();
                    if (frame) {
                        frame->setOffsetInPixels(offset);
                        sprite->setDisplayFrame(frame);
                    }
                }
            }
        } else {
            log::warn("no sprites using: {}", frameName);
        }
    }
}

bool IconOffsetEditorPopup::isUnsupportedIconType() {
    return false;
}

class $modify(OffsetEditorGarageLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init()) return false;
        
        //auto editorSprite = CCSprite::createWithSpriteFrameName("GJ_editBtn_001.png");
		auto editorSprite = CircleButtonSprite::create(CCSprite::create("offsetIndicatorBtn.png"_spr), CircleBaseColor::Green, CircleBaseSize::Medium);
        auto editorButton = CCMenuItemSpriteExtra::create(
            editorSprite,
            this,
            menu_selector(OffsetEditorGarageLayer::onOpenOffsetEditor)
        );
        
        auto menu = this->getChildByID("shards-menu");
        if (!menu) {
            menu = CCMenu::create();
            menu->setID("offset-editor-menu");
            menu->setPosition({20.0f, 100.0f});
            this->addChild(menu);
        }
        
        editorButton->setID("offset-editor-btn");
        editorButton->setPosition({-180.0f, 120.0f});
        menu->addChild(editorButton);
        
        return true;
    }
    
    void onOpenOffsetEditor(CCObject* sender) {
        IconOffsetEditorPopup::create()->show();
    }
};