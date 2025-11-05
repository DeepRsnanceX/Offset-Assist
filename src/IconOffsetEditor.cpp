#include "IconOffsetEditor.hpp"
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/SimplePlayer.hpp>
#include <hiimjustin000.more_icons/include/MoreIcons.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <random>

// a girl gotta be honest
// copilot helped me figure out how tf to do the auto plist editing feature LOL
// i was a little lost on some stuff and didn't wanna bother anyone else
// modding be so fun then BAM
// u have to write to a file ...
// lesson of the day
// use ai as a TOOL ppl
// not to make ur entire fucking job
// :thumbsup:

std::string infoStr = R"(# Icon Workbench
Welcome to the ***Icon Workbench Menu***!!
In here, you will find all the mod's tools to assist you with your Icon Creating process! You've probably already read the mod's info page, so i'll just give you some extra guidance on what everything here does!

In the Main Popup, you will find various things, such as the **Offset Input Fields**, which is where you can edit your Icon Sprite Offsets to your liking! 
To the right side of the text inputs you will see the icon you're currently editing. This is the **Live Preview**, where your offset changes will be applied live.
This **Live Preview** will also be where you'll be able to preview the Robot/Spider animations, ball rolling, and Cube position!

Right under the main popup you will find 2 buttons, labeled "Update" and "Apply":

- The "Update" button will apply any changes you've made to the Frame Offsets in the Popup's input fields. (Or, in the mod's settings, u can find a setting to enable automatically applying changes any time the Input fields changes! This is off by default due to stability concerns).
- The "Apply" button will automatically edit your Icon's plist file to apply your changed offsets!

At the right side of the mod's popup you will see a little *"Side-menu"*, this is where you can choose which Icon Sprite to edit! When you click on one, it'll be selected and show a small flashing animation to show it's the currently selected Sprite, and of course, once an icon is selected, the Text Inputs will edit that Sprite's offsets.

Under the Live Preview you will find 2 buttons, these essentially act as Togglers for the **Live Preview's Glow** and the **Hitbox Preview**.

At the left side of the menu, outside the Main Popup you will find 3 Buttons, these will allow you to change the **Live Preview Colors** directly from the Mod's popup!

At the top right corner of the main Popup there's a small menu (which you already saw because u clicked this button) where you will find 3 buttons:
- The first one shows this info popup (hi :3)
- The second one opens your chosen Renders Folder.
- The third one is the **Render Icon Button**!

The **Render Icon** button will immediately render your Live Preview right as it is in its current state to a PNG image! It'll be saved to your chosen Renders folder, which you can customize via the mod's settings menu. By default, they'll be saved to a "Renders" folder inside the mod's config folder.)";

std::string whyStr = R"(## Why aren't vanilla icons supported?

The **MoreIcons** mod provides simple and quick ways to know various things this mod heavily utilizes:

- Where is the .plist file for an Icon located
- A full list of frame names for an Icon
- The Icon's name
- When necessary, the Texture Pack's ID/Name

Fetching these things without MoreIcons for Vanilla Icons would get very annoying VERY quickly. Therefore, the mod relies on MoreIcons to function properly.

The mod's main target audience is Icon **Creators** anyway, so, most creators are probably using MoreIcons to load their icons without touching their vanilla icon list (Or should be, at least. Trust me, it's a whoooole 'nother world.)

In any case, if you're making a Vanilla Icon Pack, i'd recommend enabling MoreIcons' "Load from Traditional Icon Packs" setting, at least temporarily. This will load icons from Vanilla icon packs as if they were MoreIcons added Icons, and therefore, you SHOULD be able to edit them via Icon Construct. From my testing this DOES work, so it should for you too!)";

// this SHOULD'VE been a function to get the current time as a string
// but now it's just this
// thank u so much apple for being SOOO FUN to work with ... <3 fck u
int getRandomInt(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min, max);

    return distr(gen);
}

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
    if (utils::string::startsWith(result, moreIconsPrefix)) result = result.substr(moreIconsPrefix.length());
    
    size_t colonPos = result.find(':');
    if (colonPos != std::string::npos) result = result.substr(colonPos + 1);
    
    return result;
}

class AddValuePopup : public geode::Popup<std::function<void(float)>> {
protected:
    geode::TextInput* m_valueInput = nullptr;
    std::function<void(float)> m_callback;
    bool m_isXAxis = true;
    
    bool setup(std::function<void(float)> callback) override {
        m_callback = callback;
        
        auto winSize = this->m_mainLayer->getContentSize();
        
        this->setTitle(m_isXAxis ? "Add to Offset X" : "Add to Offset Y");
        
        // info
        auto infoLabel = CCLabelBMFont::create(
            "Enter a value to add to the current offset.\nPositive numbers move right/up.\nNegative numbers move left/down.",
            "chatFont.fnt"
        );
        infoLabel->setPosition({winSize.width / 2.0f, winSize.height / 2.0f + 23.0f});
        infoLabel->setScale(0.4f);
        infoLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
        this->m_mainLayer->addChild(infoLabel);
        
        // input
        m_valueInput = geode::TextInput::create(120.0f, "0.0", "bigFont.fnt");
        m_valueInput->setPosition({winSize.width / 2.0f, winSize.height / 2.0f - 10.0f});
        m_valueInput->setScale(0.8f);
        m_valueInput->setFilter("0123456789.-");
        this->m_mainLayer->addChild(m_valueInput);
        
        // buttons
        auto addBtnSpr = ButtonSprite::create("Add", "goldFont.fnt", "GJ_button_01.png", 0.8f);
        auto addBtn = CCMenuItemSpriteExtra::create(
            addBtnSpr,
            this,
            menu_selector(AddValuePopup::onAdd)
        );
        
        auto cancelBtnSpr = ButtonSprite::create("Cancel", "goldFont.fnt", "GJ_button_06.png", 0.8f);
        auto cancelBtn = CCMenuItemSpriteExtra::create(
            cancelBtnSpr,
            this,
            menu_selector(AddValuePopup::onClose)
        );
        
        // menu
        auto buttonMenu = CCMenu::create();
        buttonMenu->addChild(addBtn);
        buttonMenu->addChild(cancelBtn);
        buttonMenu->setLayout(
            RowLayout::create()
                ->setGap(8.0f)
                ->setAxisAlignment(AxisAlignment::Center)
        );
        buttonMenu->setPosition({winSize.width / 2.0f, winSize.height / 2.0f - 45.0f});
        buttonMenu->setContentSize({150.0f, 30.0f});
        buttonMenu->updateLayout();
        this->m_mainLayer->addChild(buttonMenu);
        
        return true;
    }
    
    void onAdd(CCObject* sender) {
        std::string valueStr = m_valueInput->getString();

        if (valueStr.empty()) {
            FLAlertLayer::create("Error", "Please enter a value!", "OK")->show();
            return;
        }

        try {
            float value = std::stof(valueStr);

            if (m_callback) m_callback(value);
            this->onClose(nullptr);

        } catch (const std::exception& e) {
            FLAlertLayer::create("Error", "Invalid number!", "OK")->show();
        }
    }
    
public:
    static AddValuePopup* create(std::function<void(float)> callback, bool isXAxis) {
        auto ret = new AddValuePopup();
        ret->m_isXAxis = isXAxis;
        if (ret->initAnchored(240.0f, 140.0f, callback)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

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
    this->setTitle("Icon Workbench");

    auto manager = GameManager::sharedState();
    m_currentIconType = manager->m_playerIconType;
    IconInfo* icInfo = MoreIcons::getIcon(m_currentIconType);
    auto size = this->m_mainLayer->getContentSize();
    auto popupSize = this->getContentSize();
    bool isRobotOrSpider = (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider);
    bool isRiderMode = (m_currentIconType == IconType::Ship || m_currentIconType == IconType::Ufo || m_currentIconType == IconType::Jetpack);

    const float midX = size.width / 2.f;
    const float midY = size.height / 2.f;
    const float inputX = midX - 40.0f;
    const float inputYTop = midY + 25.0f;
    const float lowerMenuX = midX - 55.f;
    const float lowerMenuBaseY = midY - 52.5f;

    // -----------------------
    // DISABLE FOR VANILLA ICONS (L bozo sorry it's easier to work with MI)
    // -----------------------
    if (!icInfo) {
        auto warningLabel = CCLabelBMFont::create("Vanilla icons are not supported!\nPlease load your icons via\nMore Icons instead.", "bigFont.fnt");
        warningLabel->setPosition({midX, midY});
        warningLabel->setScale(0.4f);
        warningLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
        warningLabel->setColor({255, 0, 0});

        auto whySpr = ButtonSprite::create("Why?", "goldFont.fnt", "GJ_button_01.png", 0.7f);
        auto whyBtn = CCMenuItemSpriteExtra::create(
            whySpr,
            this,
            menu_selector(IconOffsetEditorPopup::onWhy)
        );

        auto whyMenu = CCMenu::create();
        whyMenu->setPosition({0.f, 0.f});
        whyMenu->addChild(whyBtn);

        whyBtn->setPosition({midX, midY - 45.f});
        
        this->m_mainLayer->addChild(warningLabel);
        this->m_mainLayer->addChild(whyMenu);
        return true;
    }


	m_iconNameLabel = CCLabelBMFont::create(fmt::format("Editing: {}", icInfo->shortName).c_str(), "chatFont.fnt");
	m_iconNameLabel->setPosition({midX, size.height - 35.f});
	m_iconNameLabel->setScale(0.5f);
    m_iconNameLabel->setOpacity(135);
	this->m_mainLayer->addChild(m_iconNameLabel);

    // -----------------------
    // TOP RIGHT BUTTON MENU
    // -----------------------
    auto infoSpr = CircleButtonSprite::create(CCSprite::create("helpIcon.png"_spr), CircleBaseColor::Cyan, CircleBaseSize::Small);
    infoSpr->setScale(0.8f);
    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoSpr,
        this,
        menu_selector(IconOffsetEditorPopup::onInfoButton)
    );

    auto renderSpr = CircleButtonSprite::create(CCSprite::create("renderIcon.png"_spr), CircleBaseColor::Green, CircleBaseSize::Small);
    renderSpr->setScale(0.8f);
    auto renderBtn = CCMenuItemSpriteExtra::create(
        renderSpr,
        this,
        menu_selector(IconOffsetEditorPopup::onRenderIcon)
    );

    auto openFolderSpr = CircleButtonSprite::create(CCSprite::createWithSpriteFrameName("folderIcon_001.png"), CircleBaseColor::Green, CircleBaseSize::Small);
    openFolderSpr->setScale(0.8f);
    auto openFolderBtn = CCMenuItemSpriteExtra::create(
        openFolderSpr,
        this,
        menu_selector(IconOffsetEditorPopup::onOpenRendersFolder)
    );

    auto topRightMenu = CCMenu::create();
    topRightMenu->addChild(infoBtn);
    topRightMenu->addChild(openFolderBtn);
    topRightMenu->addChild(renderBtn);
    topRightMenu->setLayout(
        RowLayout::create()
            ->setGap(3.0f)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAxisReverse(true)
    );
	topRightMenu->setID("top-right-menu"_spr);
    topRightMenu->setScale(0.8f);
    topRightMenu->setAnchorPoint({1.f, 1.f});
    topRightMenu->setContentSize({60.f, 20.f});
    topRightMenu->updateLayout();
    topRightMenu->setPosition({size.width - 8.f, size.height - 13.f});
    this->m_mainLayer->addChild(topRightMenu);

    // -----------------------
    // SETTINGS BUTTON THINGY HI
    // -----------------------

    auto optionsBtn02Spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png");
    auto optionsBtn02Btn = CCMenuItemSpriteExtra::create(optionsBtn02Spr, this, menu_selector(IconOffsetEditorPopup::onModSettings));
    auto optionsBtnMenu = CCMenu::create();

    optionsBtn02Spr->setScale(0.8f);
    optionsBtnMenu->setPosition({0.f, 0.f});
    optionsBtnMenu->addChild(optionsBtn02Btn);
    optionsBtn02Btn->setPosition({0.f, 0.f});

	optionsBtnMenu->setID("options-menu"_spr);

    this->m_mainLayer->addChild(optionsBtnMenu);

    // -----------------------
    // LEFT SIDE MENU
    // -----------------------
    m_previewColor1 = manager->colorForIdx(manager->getPlayerColor());
    m_previewColor2 = manager->colorForIdx(manager->getPlayerColor2());
    m_previewGlowColor = manager->colorForIdx(manager->getPlayerGlowColor());

    m_colorPickerMenu = CCMenu::create();

    auto color1Btn = createColorPickerButton("color1", m_previewColor1);
    auto color2Btn = createColorPickerButton("color2", m_previewColor2);
    auto glowBtn = createColorPickerButton("glow", m_previewGlowColor);

    m_colorPickerMenu->addChild(color1Btn);
    m_colorPickerMenu->addChild(color2Btn);
    m_colorPickerMenu->addChild(glowBtn);

    m_colorPickerMenu->setLayout(
        ColumnLayout::create()
            ->setGap(4.0f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setAxisReverse(true)
    );
    m_colorPickerMenu->setAnchorPoint({0.5f, 0.5f});
    m_colorPickerMenu->setContentSize({20.f, 70.f});
    m_colorPickerMenu->updateLayout();
    m_colorPickerMenu->setPosition({-20.f, midY});
    this->m_mainLayer->addChild(m_colorPickerMenu);

    m_mainLayer->setPosition({m_mainLayer->getPositionX() - 60.f, m_mainLayer->getPositionY()});

    auto colLabelsNode = CCNode::create();
    colLabelsNode->setContentSize(m_colorPickerMenu->getContentSize());
    colLabelsNode->setPosition({m_colorPickerMenu->getPositionX() - 30.f, m_colorPickerMenu->getPositionY()});
    
    auto col1Label = CCLabelBMFont::create("Col 1", "bigFont.fnt");
    auto col2Label = CCLabelBMFont::create("Col 2", "bigFont.fnt");
    auto col3Label = CCLabelBMFont::create("Glow", "bigFont.fnt");

    col1Label->setOpacity(160);
    col2Label->setOpacity(160);
    col3Label->setOpacity(160);

    colLabelsNode->setLayout(
        ColumnLayout::create()
            ->setGap(4.0f)
            ->setAxisAlignment(AxisAlignment::Even)
            ->setAxisReverse(true)
            ->setAutoScale(false)
    );

    colLabelsNode->setAnchorPoint({0.5f, 0.5f});
    colLabelsNode->addChild(col1Label);
    colLabelsNode->addChild(col2Label);
    colLabelsNode->addChild(col3Label);
    col1Label->setScale(0.3f);
    col2Label->setScale(0.3f);
    col3Label->setScale(0.3f);
    colLabelsNode->updateLayout();

    this->m_mainLayer->addChild(colLabelsNode);
    
    // -----------------------
    // SETUP ICON CONTAINER NODE
    // -----------------------
    m_iconContainerNode = CCNode::create();
    m_iconContainerNode->setContentSize({50.f, 50.f});
    m_iconContainerNode->setAnchorPoint({0.5f, 0.5f});
    m_iconContainerNode->setPosition({midX + 75.f, midY});
    m_iconContainerNode->setScale(2.f);
    m_iconContainerNode->setZOrder(2);

    auto midContainerX = m_iconContainerNode->getContentSize().width / 2.f;
    auto midContainerY = m_iconContainerNode->getContentSize().height / 2.f;
    m_previewPlayer = SimplePlayer::create(0);
    m_previewPlayer->setZOrder(2);
    if (m_currentIconType == IconType::Ship) {
        m_previewPlayer->setPosition({midContainerX, midContainerY - 5.f});
    } else {
        m_previewPlayer->setPosition({midContainerX, midContainerY});
    }
    updatePreviewPlayer();
    
    m_iconContainerNode->addChild(m_previewPlayer);
    this->m_mainLayer->addChild(m_iconContainerNode);

    // -----------------------
    // CUBE PREVIEW FOR RIDER GAMEMODES
    // -----------------------
    if (m_currentIconType == IconType::Ship || m_currentIconType == IconType::Ufo || m_currentIconType == IconType::Jetpack) {        
        m_cubePreview = CCSprite::create("exampleCube.png"_spr);
        
        // welcome back icon preview
        if (m_currentIconType == IconType::Ship) {
            m_cubePreview->setPosition({midContainerX, midContainerY + 5.f});
            m_cubePreview->setScale(0.55f);
        } else if (m_currentIconType == IconType::Ufo) {
            m_cubePreview->setPosition({midContainerX, midContainerY + 6.f});
            m_cubePreview->setScale(0.55f);
        } else if (m_currentIconType == IconType::Jetpack) {
            m_cubePreview->setPosition({midContainerX + 6.f, midContainerY + 4.f});
            m_cubePreview->setScale(0.6f);
        }
        
        m_iconContainerNode->addChild(m_cubePreview);
        
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
    //if (m_currentIconType == IconType::Ship) m_hitboxDrawNode->setPosition({0.f, 10.f});
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
    if (Mod::get()->getSettingValue<bool>("update-offsets-live")) {
        m_inputX->setCallback([this](std::string const&) {
            this->onUpdateOffsets(nullptr);
        });
    }

    // add to x offset btn
    auto addXSpr = CCSprite::createWithSpriteFrameName("GJ_plus3Btn_001.png");
    addXSpr->setScale(0.5f);
    auto addXBtn = CCMenuItemSpriteExtra::create(
        addXSpr,
        this,
        menu_selector(IconOffsetEditorPopup::onAddToOffsetX)
    );

    auto addXMenu = CCMenu::create();
    addXMenu->addChild(addXBtn);
    addXMenu->setPosition({inputX + 55.0f, inputYTop});
    this->m_mainLayer->addChild(addXMenu);

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
    if (Mod::get()->getSettingValue<bool>("update-offsets-live")) {
        m_inputY->setCallback([this](std::string const&) {
            this->onUpdateOffsets(nullptr);
        });
    }

    // add to y offset btn
    auto addYSpr = CCSprite::createWithSpriteFrameName("GJ_plus3Btn_001.png");
    addYSpr->setScale(0.5f);
    auto addYBtn = CCMenuItemSpriteExtra::create(
        addYSpr,
        this,
        menu_selector(IconOffsetEditorPopup::onAddToOffsetY)
    );

    auto addYMenu = CCMenu::create();
    addYMenu->addChild(addYBtn);
    addYMenu->setPosition({inputX + 55.0f, inputYTop - 40.0f});
    this->m_mainLayer->addChild(addYMenu);
    
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
	buttonMenu->setContentSize({130.f, 30.f});
	buttonMenu->updateLayout();
	buttonMenu->setID("lower-button-menu"_spr);
    
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
            //log::info("Found matching frame: {} -> {}", fullFrameName, realName);
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
    MDPopup::create("Icon Construct Info", infoStr, "OK", nullptr, static_cast<std::function<void(bool)>>(nullptr))->show();
}

void IconOffsetEditorPopup::onWhy(CCObject* sender) {
    MDPopup::create("Icon Construct Info", whyStr, "OK", nullptr, static_cast<std::function<void(bool)>>(nullptr))->show();
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
    
    //auto playerPos = m_previewPlayer->getPosition();
    auto playerPos = m_iconContainerNode->getPosition();
    
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

CCMenuItemSpriteExtra* IconOffsetEditorPopup::createColorPickerButton(const std::string& colorId, ccColor3B currentColor) {
    auto colorSprite = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    colorSprite->setColor(currentColor);
    colorSprite->setScale(0.7f);
    
    auto button = CCMenuItemSpriteExtra::create(
        colorSprite,
        this,
        menu_selector(IconOffsetEditorPopup::onColorPicker)
    );
    
    button->setUserObject(CCString::create(colorId));
    button->setID((colorId + "-btn").c_str());
    
    return button;
}

void IconOffsetEditorPopup::onColorPicker(CCObject* sender) {
    auto menuItem = static_cast<CCMenuItemSpriteExtra*>(sender);
    if (!menuItem) return;
    
    auto userObj = menuItem->getUserObject();
    if (!userObj) return;
    
    auto colorIdStr = dynamic_cast<CCString*>(userObj);
    if (!colorIdStr) return;
    
    m_currentColorSettingId = std::string(colorIdStr->getCString());
    m_currentColorButtonSprite = dynamic_cast<CCSprite*>(menuItem->getNormalImage());
    
    ccColor3B currentColor;
    if (m_currentColorSettingId == "color1") {
        currentColor = m_previewColor1;
    } else if (m_currentColorSettingId == "color2") {
        currentColor = m_previewColor2;
    } else if (m_currentColorSettingId == "glow") {
        currentColor = m_previewGlowColor;
    }
    
    auto colorPopup = geode::ColorPickPopup::create(currentColor);
    colorPopup->setDelegate(this);
    colorPopup->show();
}

void IconOffsetEditorPopup::updateColor(cocos2d::ccColor4B const& color) {
    if (m_currentColorSettingId.empty()) return;
    
    ccColor3B color3B = {color.r, color.g, color.b};
    
    if (m_currentColorSettingId == "color1") {
        m_previewColor1 = color3B;
    } else if (m_currentColorSettingId == "color2") {
        m_previewColor2 = color3B;
    } else if (m_currentColorSettingId == "glow") {
        m_previewGlowColor = color3B;
    }
    
    if (m_currentColorButtonSprite) {
        m_currentColorButtonSprite->setColor(color3B);
    }
    
    applyPreviewColors();
    
    log::info("Updated {} to ({}, {}, {})", m_currentColorSettingId, color.r, color.g, color.b);
}

void IconOffsetEditorPopup::applyPreviewColors() {
    if (!m_previewPlayer) return;
    
    if (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider) {
        auto robotSprite = (m_currentIconType == IconType::Robot) ? 
            m_previewPlayer->m_robotSprite : nullptr;
        auto spiderSprite = (m_currentIconType == IconType::Spider) ? 
            m_previewPlayer->m_spiderSprite : nullptr;
        
        if (robotSprite) {
            robotSprite->updateColor01(m_previewColor1);
            robotSprite->updateColor02(m_previewColor2);
            robotSprite->updateGlowColor(m_previewGlowColor, true);
        }
        if (spiderSprite) {
            spiderSprite->updateColor01(m_previewColor1);
            spiderSprite->updateColor02(m_previewColor2);
            spiderSprite->updateGlowColor(m_previewGlowColor, true);
        }
    } else {
        m_previewPlayer->setColor(m_previewColor1);
        m_previewPlayer->setSecondColor(m_previewColor2);
        m_previewPlayer->setGlowOutline(m_previewGlowColor);
        m_previewPlayer->enableCustomGlowColor(m_previewGlowColor);
    }
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
    processPlistSave(false);
}

void IconOffsetEditorPopup::processPlistSave(bool remapNames) {
    bool backupWasMade;

    if (m_modifiedOffsets.empty()) {
        FLAlertLayer::create("No Changes", "<cr>You haven't changed any offsets yet!</c> There's nothing to modify in your plist.\nMake some changes before applying!", "OK")->show();
        return;
    }

    auto icInfo = MoreIcons::getIcon(m_currentIconType);
    if (!icInfo) {
        FLAlertLayer::create("Error", "<cr>Couldn't get icon info!</c>", "OK")->show();
        return;
    }

    std::string plistPath = icInfo->sheetName;

    if (plistPath.empty()) {
        FLAlertLayer::create("Error", "<cr>Couldn't get the icon's plist!</c>\nPlease make sure your Texture Pack path is valid, and you're not trying to edit a zipped texture pack.", "OK")->show();
        return;
    }

    m_logStream.str("");
    addToLog("## Plist Editing Process", 0);
    
    if (remapNames) {
        addToLog("<cy>**Note:** Edits made while using frame name remapping workaround.</c>", 2);
    }

    std::string backupPath = plistPath + ".bak";
    auto readResult = utils::file::readString(plistPath);
    if (!readResult) {
        std::string errorMsg = fmt::format("**<cr>Error:</c>** Couldn't read contents of plist file.\n\n**Details:**\n```\n{}\n```", readResult.unwrapErr());

        geode::MDPopup::create(
            "Plist Edit Failed",
            errorMsg,
            "OK", nullptr,
            [](bool) {}
        )->show();
        return;
    }

    std::string plistContent = readResult.unwrap();

    if (!remapNames) {
        auto backupResult = utils::file::writeString(backupPath, plistContent);
        if (!backupResult) {
            backupWasMade = false;
            addToLog(fmt::format("<cy>Warning:</c> Failed to create plist backup at {}.", backupPath), 2);
        } else {
            backupWasMade = true;
        }
    } else {
        backupWasMade = std::filesystem::exists(backupPath);
    }

    addToLog("---", 2);
    addToLog("### Modified Offsets:", 2);

    int updatedCount = 0;
    int notFoundCount = 0;
    std::vector<std::string> successfulUpdates;
    std::vector<std::string> failedUpdates;
    std::vector<std::string> notFoundFrames;

    std::string iconShortName = icInfo->shortName;
    std::string internalFrameName = "";

    if (remapNames && !iconShortName.empty()) {
        addToLog("<cy>[REMAPPING]</c> Trying to find internal frame name in plist...", 1);
        
        std::string searchPattern = "_001.png"; // oh my GOD GEODE IT'S NOT A SPRITE I'M TRYING TO USE
        size_t searchPos = plistContent.find(searchPattern);
        
        if (searchPos != std::string::npos) {
            size_t keyStart = plistContent.rfind("<key>", searchPos);
            if (keyStart != std::string::npos) {
                size_t keyEnd = plistContent.find("</key>", keyStart);
                if (keyEnd != std::string::npos) {
                    std::string fullFrameName = plistContent.substr(keyStart + 5, keyEnd - (keyStart + 5));
                    
                    if (fullFrameName.length() > searchPattern.length()) {
                        internalFrameName = fullFrameName.substr(0, fullFrameName.length() - searchPattern.length());
                        addToLog(fmt::format("<cg>[REMAPPING]</c> Found internal frame name: `{}`", internalFrameName), 1);
                        addToLog(fmt::format("<cy>[REMAPPING]</c> Replacing `{}` with `{}` to find frame names in plist.", iconShortName, internalFrameName), 1);
                    }
                }
            }
        }
        
        if (internalFrameName.empty()) {
            addToLog("<cr>[ERROR]</c> Could not detect internal frame name from plist!", 0);
            addToLog("The plist may not contain any frames with the standard naming pattern.", 0);
            
            std::string errorMsg = "## <cr>Internal Frame Name Not Found!</c>\n\n";
            errorMsg += "Could not detect the internal frame name from the plist file.\n\n";
            errorMsg += "<cp>This probably means:</c>\n";
            errorMsg += "- Your icon's plist doesn't have any standard-named frames. (aka, `iconPart.png` instead of `iconPart_001.png`)\n";
            errorMsg += "- Your plist's structure is corrupted or invalid.\n\n";
            errorMsg += "**<cl>What do i do?</c>** - Try manually renaming the frames in your plist to match your icon's renamed name. Or, just manually change the offsets to whatever you found nice in the Workbench.";
            
            geode::MDPopup::create(
                "Remapping Failed",
                errorMsg,
                "OK", nullptr,
                [](bool) {}
            )->show();
            return;
        }
        
        addToLog("---", 1);
    }

    for (const auto& [frameName, offset] : m_modifiedOffsets) {
        std::string searchFrameName = frameName;
        
        if (remapNames && !internalFrameName.empty() && !iconShortName.empty()) {
            if (frameName.length() > iconShortName.length() && 
                frameName.substr(0, iconShortName.length()) == iconShortName) {
                
                std::string suffix = frameName.substr(iconShortName.length());
                
                searchFrameName = internalFrameName + suffix;
                
                addToLog(fmt::format("<cy>[REMAPPED]</c> `{}` <cl>-></c> `{}`", frameName, searchFrameName), 0);
            }
        }
        
        std::string frameKey = fmt::format("<key>{}</key>", searchFrameName);
        size_t framePos = plistContent.find(frameKey);

        if (framePos == std::string::npos) {
            if (!remapNames) {
                notFoundFrames.push_back(frameName);
                notFoundCount++;
            } else {
                failedUpdates.push_back(fmt::format("<cr>[ERROR]</c> frame **{}** (remapped to `{}`) not found in plist even after remapping.", frameName, searchFrameName));
            }
            continue;
        }

        size_t frameDictStart = plistContent.find("<dict>", framePos);
        if (frameDictStart == std::string::npos) {
            failedUpdates.push_back(fmt::format("<cr>[ERROR]</c> opening <dict> for frame **{}** not found.", frameName));
            continue;
        }

        size_t frameDictEnd = plistContent.find("</dict>", frameDictStart);
        if (frameDictEnd == std::string::npos) {
            failedUpdates.push_back(fmt::format("<cr>[ERROR]</c> closing </dict> for frame **{}** not found.", frameName));
            continue;
        }

        size_t searchStart = frameDictStart;
        size_t searchEnd = frameDictEnd;

        std::string frameSection = plistContent.substr(searchStart, searchEnd - searchStart);
        size_t offsetKeyPos = frameSection.find("<key>spriteOffset</key>");

        if (offsetKeyPos == std::string::npos) {
            failedUpdates.push_back(fmt::format("<cr>[ERROR]</c> spriteOffset for frame **{}** not found.", frameName));
            continue;
        }

        offsetKeyPos += searchStart;

        size_t stringStart = plistContent.find("<string>", offsetKeyPos);
        size_t stringEnd = plistContent.find("</string>", offsetKeyPos);

        if (stringStart == std::string::npos || stringEnd == std::string::npos ||
            stringStart > frameDictEnd || stringEnd > frameDictEnd) {
            failedUpdates.push_back(fmt::format("<cr>[ERROR]</c> spriteOffset string tags for frame **{}** not found.", frameName));
            continue;
        }

        std::string newOffsetStr = fmt::format("{{{},{}}}",
            static_cast<int>(std::round(offset.x)),
            static_cast<int>(std::round(offset.y)));

        plistContent.replace(
            stringStart + 8,
            stringEnd - (stringStart + 8),
            newOffsetStr
        );

        if (remapNames) {
            successfulUpdates.push_back(fmt::format("**{}** <cl>-></c> `{}`: `{}`", frameName, searchFrameName, newOffsetStr));
        } else {
            successfulUpdates.push_back(fmt::format("**{}**: `{}`", frameName, newOffsetStr));
        }
        updatedCount++;
    }

    if (!remapNames && notFoundCount > 0 && notFoundCount == m_modifiedOffsets.size()) {
        std::stringstream retryMsg;
        retryMsg << "# <cr>Frames Not Found.</c>\n\n";
        retryMsg << "---";
        retryMsg << "None of the frames you updated and were about to be changed were found in the plist file.\n\n";
        retryMsg << "## <cy>This could be caused by a</c> <co>Renamed/Duplicate Icon</c>\n\n";
        retryMsg << "If you've renamed this icon using More Icons' rename feature, the plist file still contains the old frame names, therefore, the mod isn't able to find the frames by searching for the icon's new name inside the plist file.\n\n";
        retryMsg << "**For Example:**\n";
        retryMsg << "- You renamed: `player_85` to `renamedtest`\n";
        retryMsg << "- Construct is looking for: `renamedtest_001.png` to apply your changes.\n";
        retryMsg << "- But, the plist file still has: `player_85_001.png`\n\n";
        retryMsg << "### <cp>What Now?</co>\n\n";
        retryMsg << "Click **Remap and Retry** to try and automatically find and use the internal frame names in your plist, instead of your icon's name.\n\n";
        retryMsg << "Construct will then:\n";
        retryMsg << "1. Try and find the first instance of a frame ending with `_001.png` in your plist.\n";
        retryMsg << "2. Use this to try and find the icon's internal name. (e.g., `player_85_001.png` = `player_85`)\n";
        retryMsg << "3. Replace your renamed name with the internal name.\n";
        retryMsg << "4. Search for the correct frames to apply your changes.\n\n";
        retryMsg << "<cy>**Note:**</c> If anything goes wrong, a backup of your icon's plist has been created at:\n";
        retryMsg << "`" << backupPath << "`\n\n";
        retryMsg << "So i'd recommend giving remapping a shot anyways.\n\n";
        retryMsg << "### <cr>Frames that weren't found:</cr>\n\n";
        for (const auto& frame : notFoundFrames) {
            retryMsg << "- `" << frame << "`\n";
        }

        std::string retryMsgStr = retryMsg.str();
        
        geode::MDPopup::create(
            "Couldn't find Frames",
            retryMsgStr,
            "Cancel", "Remap and Retry",
            [this](bool btn2) {
                if (btn2) {
                    processPlistSave(true);
                }
            }
        )->show();
        
        return;
    }

    if (!successfulUpdates.empty()) {
        addToLog(fmt::format("### <cg>Successfully Applied Updates: ({})</c>", updatedCount), 2);
        for (const auto& update : successfulUpdates) {
            addToLog(fmt::format("- <cg>{}</c>", update), 0);
        }
    }

    if (!failedUpdates.empty()) {
        addToLog(fmt::format("### <cr>Unapplied/Failed Updates ({})</c>", failedUpdates.size()), 2);
        for (const auto& failure : failedUpdates) {
            addToLog(fmt::format("- {}", failure), 0);
        }
    }

    if (updatedCount == 0) {
        addToLog("<cr>**Error:**</c> No offsets updated on plist file.", 2);
        addToLog("Please check the failed updates above for details, and send the log in your bug report if you think this is a mod issue.", 0);

        std::string finalLog = m_logStream.str();

        geode::MDPopup::create(
            "Plist Save Failed",
            finalLog,
            "OK", "Copy Log",
            [finalLog](bool btn2) {
                if (btn2) {
                    utils::clipboard::write(finalLog);
                    Notification::create("Log copied to clipboard!", NotificationIcon::Success)->show();
                }
            }
        )->show();
        return;
    }

    auto writeResult = utils::file::writeString(plistPath, plistContent);
    if (!writeResult) {
        addToLog("<cr>**Error:**</c> Failed to write to plist file.", 2);
        addToLog(fmt::format("**Error Details:**\n```\n{}\n```", writeResult.unwrapErr()), 0);

        std::string finalLog = m_logStream.str();

        geode::MDPopup::create(
            "Plist Save Failed",
            finalLog,
            "OK", "Copy Log",
            [finalLog](bool btn2) {
                if (btn2) {
                    utils::clipboard::write(finalLog);
                    Notification::create("Log copied to clipboard!", NotificationIcon::Success)->show();
                }
            }
        )->show();
        return;
    }

    addToLog("---", 2);
    addToLog("## <cg>Success!</c>", 2);
    addToLog(fmt::format("Plist file successfully updated with **{}** changes!", updatedCount), 2);
    addToLog(fmt::format("**Plist Path:**\n`{}`", plistPath), 1);
    addToLog(fmt::format("**Backup Path:**\n`{}`", backupPath), 1);
    addToLog("<cy>Remember to reload textures to see the applied changes!</c>", 2);
    
    if (remapNames) {
        addToLog("---", 2);
        addToLog("<cy>**Frame Name Remapping was used**</c>", 2);
        addToLog(fmt::format("Found icon internal name: `{}`", internalFrameName), 1);
        addToLog(fmt::format("Replaced `{}` with `{}` in all frame searches.", iconShortName, internalFrameName), 1);
        addToLog("If anything goes wrong or your icon looks bad after these changes, you can restore from the created backup file ", 1);
    }
    
    if (backupWasMade) {
        addToLog("---", 2);
        addToLog("<cg>Plist backup exists!</c>", 2);
        addToLog(fmt::format("**Backup is saved to:** `{}`", backupPath), 1);
    }

    std::string finalLog = m_logStream.str();

    geode::MDPopup::create(
        "Plist Saved Successfully",
        finalLog,
        "OK", "Copy Log",
        [finalLog](bool btn2) {
            if (btn2) {
                utils::clipboard::write(finalLog);
                Notification::create("Log copied to clipboard!", NotificationIcon::Success)->show();
            }
        }
    )->show();

    m_modifiedOffsets.clear();
}

void IconOffsetEditorPopup::addToLog(const std::string& logMsg, int newLines) {
    for (int i = 0; i < newLines; ++i) m_logStream << "\n";
    m_logStream << logMsg << "\n";
}

CCImage* IconOffsetEditorPopup::getIconImage() {
    if (!m_previewPlayer || !m_iconContainerNode) {
        log::error("Preview player or container node is null");
        return nullptr;
    }
    
    auto origIconPos = m_previewPlayer->getPosition();
    auto origContainerScale = m_iconContainerNode->getScale();
    auto origContainerPos = m_iconContainerNode->getPosition();
    
    auto bgCol = Mod::get()->getSettingValue<ccColor4B>("bg-color");
    
    auto canvasSize = m_iconContainerNode->getContentSize();
    
    int extraW = (m_currentIconType == IconType::Ship || m_currentIconType == IconType::Ufo) ? 6 : 0;
    int extraH = (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider) ? 8 : 0;
    
    int texWidth = static_cast<int>(canvasSize.width) + extraW;
    int texHeight = static_cast<int>(canvasSize.height) + extraH;
    
    auto renderTex = CCRenderTexture::create(texWidth, texHeight, kCCTexture2DPixelFormat_RGBA8888);
    if (!renderTex) {
        log::warn("couldn't create render texture.");
        return nullptr;
    }
    
    m_iconContainerNode->setScale(1.0f);
    m_iconContainerNode->setPosition({texWidth / 2.0f, texHeight / 2.0f});
    
    float newPosX = origIconPos.x;
    float newPosY = origIconPos.y;
    
    if (m_currentIconType == IconType::Robot || m_currentIconType == IconType::Spider) {
        newPosY = newPosY - 6.f;
    } else if (m_currentIconType == IconType::Ufo) {
        newPosY = newPosY - 4.f;
    }

    CCPoint origCubePos;
    if (m_cubePreview) {
        origCubePos = m_cubePreview->getPosition();
        float newCubePosX = origCubePos.x;
        float newCubePosY = origCubePos.y;

        if (m_currentIconType == IconType::Ufo) newCubePosY = newCubePosY - 4.f;
        m_cubePreview->setPosition({newCubePosX, newCubePosY});
    }
    
    m_previewPlayer->setPosition({newPosX, newPosY});
    
    renderTex->beginWithClear(bgCol.r / 255.f, bgCol.g / 255.f, bgCol.b / 255.f, bgCol.a / 255.f);
    m_iconContainerNode->visit();
    renderTex->end();
    
    m_iconContainerNode->setScale(origContainerScale);
    m_iconContainerNode->setPosition(origContainerPos);
    m_previewPlayer->setPosition(origIconPos);
    if (m_cubePreview) m_cubePreview->setPosition(origCubePos);
    
    CCImage* image = renderTex->newCCImage();
    return image;
}

void IconOffsetEditorPopup::onRenderIcon(CCObject* sender) {
    auto icInfo = MoreIcons::getIcon(m_currentIconType);
    if (!icInfo) {
        FLAlertLayer::create("Error", "Couldn't get icon info!", "aw :(")->show();
        return;
    }
    
    auto renderPath = Mod::get()->getSettingValue<std::filesystem::path>("renders-path");
    
    if (!std::filesystem::exists(renderPath)) std::filesystem::create_directories(renderPath);
    
    std::string renderFilename = fmt::format("{}-{}.png", icInfo->shortName, getRandomInt(1, 9999));
    
    auto finalPath = renderPath / renderFilename;
    
    log::info("rendering icon to: {}", finalPath.string());
    
    CCImage* render = getIconImage();
    
    if (!render) {
        FLAlertLayer::create(
            "Error",
            "Failed to create render image!",
            ":("
        )->show();
        log::error("getIconImage() returned nullptr");
        return;
    }
    
    bool saved = render->saveToFile(finalPath.string().c_str(), false);
    delete render;
    
    if (saved) {
        FLAlertLayer::create(
            "Rendered!",
            fmt::format("Icon rendered successfully!\nSaved to: <cy>{}</c>", finalPath.string()),
            ":D"
        )->show();
        log::info("Successfully rendered icon {} to {}", icInfo->shortName, finalPath.string());
    } else {
        FLAlertLayer::create(
            "Error",
            fmt::format("Failed to save render to:\n{}", finalPath.string()),
            ":("
        )->show();
        log::error("Failed to save image to file");
    }
}

void IconOffsetEditorPopup::onOpenRendersFolder(CCObject* sender) {
    geode::utils::file::openFolder(Mod::get()->getSettingValue<std::filesystem::path>("renders-path"));
}

void IconOffsetEditorPopup::onModSettings(CCObject* sender) {
    geode::openSettingsPopup(Mod::get());
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
                log::warn("{}SpritePart has frame but null texture! skipping to avoid crash, but please check ur icon...", indent);
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
                        log::warn("{}Cached frame has null texture: {} - skippin to avoid crash, but please check ur icon...", indent, frameName);
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
            log::warn("no sprites found using: {}", frameName);
        }
    }
}

void IconOffsetEditorPopup::onAddToOffsetX(CCObject* sender) {
    AddValuePopup::create([this](float value) {
        // Get current X value
        float currentX = 0.0f;
        try {
            std::string xStr = m_inputX->getString();
            if (!xStr.empty()) {
                currentX = std::stof(xStr);
            }
        } catch (const std::exception& e) {
            currentX = 0.0f;
        }
        
        // Add the value
        float newX = currentX + value;
        
        // Update input field
        m_inputX->setString(fmt::format("{:.1f}", newX));
        
        // Trigger the update (simulate clicking the Update button)
        onUpdateOffsets(nullptr);
        
        log::info("Added {} to X offset (was {}, now {})", value, currentX, newX);
    }, true)->show();
}

void IconOffsetEditorPopup::onAddToOffsetY(CCObject* sender) {
    AddValuePopup::create([this](float value) {
        // Get current Y value
        float currentY = 0.0f;
        try {
            std::string yStr = m_inputY->getString();
            if (!yStr.empty()) {
                currentY = std::stof(yStr);
            }
        } catch (const std::exception& e) {
            currentY = 0.0f;
        }
        
        // Add the value
        float newY = currentY + value;
        
        // Update input field
        m_inputY->setString(fmt::format("{:.1f}", newY));
        
        // Trigger the update (simulate clicking the Update button)
        onUpdateOffsets(nullptr);
        
        log::info("Added {} to Y offset (was {}, now {})", value, currentY, newY);
    }, false)->show();
}

class $modify(OffsetEditorGarageLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init()) return false;
        
        //auto editorSprite = CCSprite::createWithSpriteFrameName("GJ_editBtn_001.png");
		auto editorSprite = CircleButtonSprite::create(CCSprite::create("offsetIndicatorBtn.png"_spr), CircleBaseColor::Green, CircleBaseSize::SmallAlt);
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
