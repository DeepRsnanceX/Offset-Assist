#include "IconOffsetEditor.hpp"
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/SimplePlayer.hpp>
#include <hiimjustin000.more_icons/include/MoreIcons.hpp>

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

IconOffsetEditorPopup* IconOffsetEditorPopup::create() {
    auto ret = new IconOffsetEditorPopup();
    if (ret->initAnchored(280.0f, 160.0f, "GJ_square01.png")) {
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
    
    if (!icInfo) {
        auto size = this->m_mainLayer->getContentSize();
        auto warningLabel = CCLabelBMFont::create("Vanilla icons are not supported!\nPlease load your icons\nvia More Icons instead.", "bigFont.fnt");
        warningLabel->setPosition({size.width / 2.0f, size.height / 2.0f});
        warningLabel->setScale(0.4f);
        warningLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
        this->m_mainLayer->addChild(warningLabel);
        return true;
    }

    m_mainLayer->setPosition({m_mainLayer->getPositionX() - 60.f, m_mainLayer->getPositionY()});
    
    auto size = this->m_mainLayer->getContentSize();
    auto popupSize = this->getContentSize();
    const float midX = size.width / 2.f;
    const float midY = size.height / 2.f;
    
    m_previewPlayer = SimplePlayer::create(0);
    m_previewPlayer->setPosition({midX + 75.0f, midY});
    m_previewPlayer->setScale(2.f);
    updatePreviewPlayer();
    
    if (m_previewPlayer->m_outlineSprite) m_previewPlayer->m_outlineSprite->setVisible(true);
    
    this->m_mainLayer->addChild(m_previewPlayer);
    
    const float inputX = midX - 40.0f;
    const float inputYTop = midY + 25.0f;
    
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
    
    auto updateBtnSprite = ButtonSprite::create("Update", "goldFont.fnt", "GJ_button_01.png", 0.8f);
    m_updateButton = CCMenuItemSpriteExtra::create(
        updateBtnSprite,
        this,
        menu_selector(IconOffsetEditorPopup::onUpdateOffsets)
    );
    
    auto buttonMenu = CCMenu::create();
    buttonMenu->setPosition({0.f, 0.f});
    buttonMenu->addChild(m_updateButton);
    m_updateButton->setPosition({midX, 0.f});
    
    this->m_mainLayer->addChild(buttonMenu);

    bool isRobotOrSpider = (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider);
    int partCount = 4;
    
    if (isRobotOrSpider) {
        if (!icInfo->frameNames.empty()) {
            m_frameNames = icInfo->frameNames;
            partCount = m_frameNames.size();
            
            for (const auto& frameName : m_frameNames) {
                auto frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());
                if (frame) {
                    m_storedOffsets[frameName] = frame->getOffsetInPixels();
                }
            }
            
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

void IconOffsetEditorPopup::updateInputFields() {
    if (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider) {
        if (m_storedOffsets.count(m_currentFrameName)) {
            auto offset = m_storedOffsets[m_currentFrameName];
            m_inputX->setString(std::to_string(offset.x));
            m_inputY->setString(std::to_string(offset.y));
        }
        return;
    }
    
    auto sprite = getCurrentSelectedSprite();
    if (!sprite) return;
    
    auto frame = sprite->displayFrame();
    if (!frame) return;
    
    auto offset = frame->getOffsetInPixels();
    
    m_inputX->setString(std::to_string(offset.x));
    m_inputY->setString(std::to_string(offset.y));
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
        log::error("failed to parse offset values: {}", e.what());
        return;
    }
    
    CCPoint newOffset = {offsetX, offsetY};
    
    if (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider) {
        m_storedOffsets[m_currentFrameName] = newOffset;
        
        auto robotSprite = (m_currentIconType == IconType::Robot) ? 
            m_previewPlayer->m_robotSprite : nullptr;
        auto spiderSprite = (m_currentIconType == IconType::Spider) ? 
            m_previewPlayer->m_spiderSprite : nullptr;
        
        CCNode* targetNode = robotSprite ? static_cast<CCNode*>(robotSprite) : static_cast<CCNode*>(spiderSprite);
        
        if (targetNode) {
            applyOffsetToAllMatchingSprites(targetNode, m_currentFrameName, newOffset);
        }
        
        log::info("updated offset for frame {} to ({}, {})", m_currentFrameName, offsetX, offsetY);
        return;
    }
    
    auto sprite = getCurrentSelectedSprite();
    if (!sprite) {
        log::warn("no sprite selected/sprite is null");
        return;
    }
    
    auto frame = sprite->displayFrame();
    if (!frame) {
        log::warn("selected sprite has no display frame");
        return;
    }
    
    frame->setOffsetInPixels(newOffset);
    sprite->setDisplayFrame(frame);
    
    log::info("updated offset for {} to ({}, {})", 
        static_cast<int>(m_selectedPart), offsetX, offsetY);
}

void IconOffsetEditorPopup::mapRobotSpiderSprites(CCNode* node) {
    if (!node) {
        log::debug("mapRobotSpiderSprites: node is null");
        return;
    }
    
    static int depth = 0;
    depth++;
    
    std::string indent(depth * 2, ' ');
    
    if (auto sprite = dynamic_cast<CCSprite*>(node)) {
        auto frame = sprite->displayFrame();
        if (frame) {            
            auto texture = frame->getTexture();
            auto rect = frame->getRect();
            
            bool matched = false;
            for (const auto& frameName : m_frameNames) {
                auto cachedFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());
                
                if (cachedFrame) {
                    auto cachedTexture = cachedFrame->getTexture();
                    auto cachedRect = cachedFrame->getRect();
                    
                    bool textureMatches = (texture == cachedTexture);
                    bool rectMatches = (rect.equals(cachedRect));
                    
                    if (textureMatches && rectMatches) {
                        m_robotSpiderSprites[frameName].push_back(sprite);
                        matched = true;
                        break;
                    }
                } else {
                    log::warn("{}couldn't get cached frame for: {}", indent, frameName);
                }
            }
            
            if (!matched) {
                log::debug("{}couldn't find any frame matches", indent);
            }
        } else {
            log::debug("{}sprite has no display frame????? ok", indent);
        }
    }
    
    if (auto spritePart = dynamic_cast<CCSpritePart*>(node)) {
        auto frame = spritePart->displayFrame();
        if (frame) {
            
            auto texture = frame->getTexture();
            auto rect = frame->getRect();
            
            bool matched = false;
            for (const auto& frameName : m_frameNames) {
                auto cachedFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());
                
                if (cachedFrame) {
                    auto cachedTexture = cachedFrame->getTexture();
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
            
            if (!matched) {
                log::debug("{}couldn't find any frame matches", indent);
            }
        } else {
            log::debug("{}sprite has no display frame????? ok 2", indent);
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
    } else {
        log::debug("{}el nodo: amor perdi al niño", indent);
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