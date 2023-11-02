#ifndef GameManager_h
#define GameManager_h

#include "cocos2d.h"
#include "HudLayer.h"
#include "DualHudLayer.h"
#include "DualWorld.h"
#include "HelloWorldScene.h"
#include "LanguageManager.h"
#include "SimpleAudioEngine.h"
#include "Buddies.h"
#include "MobileTitle.h"
#include "../cocos2d/cocos/ui/UILayout.h"
#include "../cocos2d/cocos/ui/UIButton.h"
#include "../cocos2d/cocos/ui/UIText.h"
#include "../cocos2d/cocos/ui/UIImageView.h"
#include "../cocos2d/cocos/ui/UITextField.h"
#include "../cocos2d/cocos/ui/UIHelper.h"
#include "../cocos2d/cocos/ui/UILoadingBar.h"
#include "../cocos2d/cocos/ui/UIScrollView.h"

using namespace cocos2d;
using namespace cocos2d::ui;

#define TUTORIAL_DUNGEON 0
#define TUTORIAL_MENU 1
#define TUTORIAL_AFTER_BATTLE 2

#define MISSION_STATE_NOT_CLEAR 0
#define MISSION_STATE_CLEAR 1
#define MISSION_STATE_REWARDED 2

#define SEARCH_TICKET_REFILL_TIME 10800
#define LOCAL_TICKET_REFILL_TIME 1800
#define NETWORK_TICKET_REFILL_TIME 1800
#define DUNGEON_TICKET_REFILL_TIME 14400
#define BOSS_TICKET_REFILL_TIME 28800
#define FAMILY_LIONSTAR 0
#define FAMILY_STARK 1

#define POPUP_ZORDER 100
#define TOP_MOST_ZORDER 10000

#define NETWORK_HANDLE_STATE_NOT_READY 0
#define NETWORK_HANDLE_STATE_SHOULD_REQUEST 1
#define NETWORK_HANDLE_STATE_REQUESTED 2
#define NETWORK_HANDLE_STATE_ARRIVED 3
#define NETWORK_HANDLE_STATE_HANDLED 4
#define NETWORK_HANDLE_STATE_COMPLETE 5
#define NETWORK_HANDLE_STATE_ERROR 6

#define DAILY_MISSION_PICK_WEAPON 0
#define DAILY_MISSION_PICK_SHIELD 1
#define DAILY_MISSION_PICK_RUNE 2
#define DAILY_MISSION_PLAY_DUNGEON 3
#define DAILY_MISSION_MERGE_WEAPONS 4
#define DAILY_MISSION_DEFEAT_BAZOOKA 5
#define DAILY_MISSION_OPEN_CHEST 6
#define DAILY_MISSION_WIN_AVATAR 7
#define DAILY_MISSION_WIN_NETWORK 8
#define DAILY_MISSION_FIX_WEAPON 9
#define DAILY_MISSION_GET_MONTHLY 10

#define COLLECTION_NOT_FOUND 0
#define COLLECTION_FOUND 1
#define COLLECTION_REWARDED 2

#define FEELING_TALK 0
#define FEELING_EXCLAIM 1

#define IAP_NONE -1
#define IAP_STARTER_0 0
#define IAP_STARTER_1 1
#define IAP_STARTER_2 2
#define IAP_PACKAGE_0 3
#define IAP_PACKAGE_1 4
#define IAP_GEM_0 5
#define IAP_GEM_1 6
#define IAP_GEM_2 7
#define IAP_GEM_3 8
#define IAP_GEM_4 9
#define IAP_TICKET_0 10
#define IAP_STARTER_1_ORIGINAL 11
#define IAP_STARTER_2_ORIGINAL 12
#define IAP_PACKAGE_0_ORIGINAL 13
#define IAP_PACKAGE_1_ORIGINAL 14
#define IAP_GEM_0_ORIGINAL 15
#define IAP_TICKET_0_ORIGINAL 16
#define IAP_DUNGEON_ATM 17

#define SHIELD_BEGIN 600
#define SHIELD_WOOD 600
#define SHIELD_WOOD_ROUND 601
#define SHIELD_WOOD_CROSS 602
#define SHIELD_INDIAN 603
#define SHIELD_2STAR_START 604
#define SHIELD_WOOD_GUARD 604
#define SHIELD_STEEL 605
#define SHIELD_IRON 606
#define SHIELD_METAL 607
#define SHIELD_3STAR_START 608
#define SHIELD_RED 608
#define SHIELD_GOLD 609
#define SHIELD_GREEN 610
#define SHIELD_BLUE 611
#define SHIELD_4STAR_START 612
#define SHIELD_KNIGHT 612
#define SHIELD_PALADIN 613
#define SHIELD_LANCER 614
#define SHIELD_TANKER 615
#define SHIELD_5STAR_START 616
#define SHIELD_VEGAS 616
#define SHIELD_PHONIX 617
#define SHIELD_EYE 618
#define SHIELD_ATHENA 619
#define TOTAL_SHIELD_COUNT 620

#define RUNE_BUFF_TYPE_ATTACK 0
#define RUNE_BUFF_TYPE_DEF 1
#define RUNE_BUFF_TYPE_HP 2
#define RUNE_BUFF_TYPE_CRI 3

#define RUNE_BEGIN 800
#define RUNE_A_RED 800
#define RUNE_A_YELLOW 801
#define RUNE_A_GREEN 802
#define RUNE_A_BLUE 803
#define RUNE_D_RED 804
#define RUNE_D_YELLOW 805
#define RUNE_D_GREEN 806
#define RUNE_D_BLUE 807
#define RUNE_S_RED 808
#define RUNE_S_YELLOW 809
#define RUNE_S_GREEN 810
#define RUNE_S_BLUE 811
#define RUNE_Q_RED 812
#define RUNE_Q_YELLOW 813
#define RUNE_Q_GREEN 814
#define RUNE_Q_BLUE 815
#define RUNE_END 816

#define BAG_FILTER_NONE -1
#define BAG_FILTER_WEAPON 1
#define BAG_FILTER_SHIELD 2
#define BAG_FILTER_RUNE 3
#define BAG_FILTER_WEAPON_1STAR 4
#define BAG_FILTER_WEAPON_2STAR 5
#define BAG_FILTER_WEAPON_3STAR 6
#define BAG_FILTER_WEAPON_4STAR 7
#define BAG_FILTER_WEAPON_5STAR 8
#define BAG_FILTER_SHIELD_1STAR 9
#define BAG_FILTER_SHIELD_2STAR 10
#define BAG_FILTER_SHIELD_3STAR 11
#define BAG_FILTER_SHIELD_4STAR 12
#define BAG_FILTER_SHIELD_5STAR 13

#define ITEM_TYPE_WEAPON 1
#define ITEM_TYPE_SHIELD 2
#define ITEM_TYPE_RUNE 3


#define WEAPON_NOT_EXIST -1

#define WEAPON_BEGIN 0
#define WEAPON_PISTOL 0
#define WEAPON_MAGNUM 1
#define WEAPON_UZI 2
#define WEAPON_RIFLE 3
#define WEAPON_FIRE_BOTTLER 4
#define WEAPON_ANTI_BAT 5

#define WEAPON_2STAR_START 5
#define WEAPON_SNIPER_RIFLE 6
#define WEAPON_RIFLE_GRENADE 7
#define WEAPON_CROSS_BOW 8
#define WEAPON_STRIKER 9
#define WEAPON_MACHINE_GUN 10

#define WEAPON_3STAR_START 10
#define WEAPON_NIFE_THROWER 11
#define WEAPON_BALL_SHOOTER 12
#define WEAPON_FLAME_THROWER 13
#define WEAPON_ROCKET_LAUNCHER 14
#define WEAPON_BAZOOKA 15

#define WEAPON_4STAR_START 15
#define WEAPON_MATT_A4 16
#define WEAPON_LASER_GUN 17
#define WEAPON_ENERGY_GUN 18
#define WEAPON_GUIDED_ROCKET_LAUNCHER 19
#define WEAPON_SLUGGER 20

#define WEAPON_5STAR_START 20
#define WEAPON_DRAGON_RIFLE 21
#define WEAPON_SHARK_GUN 22
#define WEAPON_SLUGGER_NG 23
#define WEAPON_LIGHTNING_GUN 24
#define WEAPON_LASER_COIN 25

#define WEAPON_NO_MORE 26
#define TOTAL_WEAPON_COUNT 26

#define ITEM_RARENESS_NORMAL -1
#define ITEM_RARENESS_1STAR 0
#define ITEM_RARENESS_2STAR 1
#define ITEM_RARENESS_3STAR 2
#define ITEM_RARENESS_4STAR 3
#define ITEM_RARENESS_5STAR 4

#define WEAPON_POWER_MAX_LEVEL 10
#define WEAPON_CRITICAL_MAX_LEVEL 10

#define CHARACTER_DARY 0
#define CHARACTER_GABRIEL 1
#define CHARACTER_JOHN 2
#define CHARACTER_SUSAN 3
#define CHARACTER_PAUL 4
#define CHARACTER_BREE 5
#define CHARACTER_NINJA 6

#define CHARACTER_NO_MORE 7
#define CHARACTER_NONE 999

#define TOTAL_CHARACTER_COUNT 7


#define MAX_LEVEL 100

//#define ldPistolLevelKey "Key_Pistol_Key"
#define ldPistolUnlockedKey "Key_Pistol_Unlocked"
//#define ldMagnumLevelKey "Key_Magnum_Key"
#define ldMagnumUnlockedKey "Key_Magnum_Unlocked"
//#define ldUziLevelKey "Key_Uzi_Key"
#define ldUziUnlockedKey "Key_Uzi_Unlocked"
//#define ldSniperRifleLevelKey "Key_Sniper_Rifle_Key"
#define ldSniperRifleUnlockedKey "Key_Sniper_Rifle_Unlocked"
//#define ldSluggerLevelKey "Key_Slugger_Key"
#define ldSluggerUnlockedKey "Key_Slugger_Unlocked"
#define ldSluggerNGUnlockedKey "Key_SluggerNG_Unlocked"
//#define ldRocketLauncherLevelKey "Key_Rocket_Launcher_Key"
#define ldRocketLauncherUnlockedKey "Key_Rocket_Launcher_Unlocked"
//#define ldRifleGrenadeLevelKey "Key_Rifle_Grenade_Key"
#define ldRifleGrenadeUnlockedKey "Key_Rifle_Grenade_Unlocked"
//#define ldRifleLevelKey "Key_Rifle_Key"
#define ldRifleUnlockedKey "Key_Rifle_Unlocked"
//#define ldNifeThrowerLevelKey "Key_Nife_Thrower_Key"
#define ldNifeThrowerUnlockedKey "Key_Nife_Thrower_Unlocked"
//#define ldMattA4LevelKey "Key_Matt_A4_Key"
#define ldMattA4UnlockedKey "Key_Matt_A4_Unlocked"
//#define ldMachineGunLevelKey "Key_Machine_Gun_Key"
#define ldMachineGunUnlockedKey "Key_Machine_Gun_Unlocked"
//#define ldLaserGunLevelKey "Key_Laser_Gun_Key"
#define ldLaserGunUnlockedKey "Key_Laser_Gun_Unlocked"
//#define ldGuidedRocketLauncherLevelKey "Key_Guided_Rocket_Launcher_Key"
#define ldGuidedRocketLauncherUnlockedKey "Key_Guided_Rocket_Launcher_Unlocked"
//#define ldFireBottlerLevelKey "Key_Fire_Bottler_Key"
#define ldFireBottlerUnlockedKey "Key_Fire_Bottler_Unlocked"
//#define ldEnergyGunLevelKey "Key_Energy_Gun_Key"
#define ldEnergyGunUnlockedKey "Key_Energy_Gun_Unlocked"
//#define ldDragonRifleLevelKey "Key_Dragon_Rifle_Key"
#define ldDragonRifleUnlockedKey "Key_Dragon_Rifle_Unlocked"
//#define ldCrossBowLevelKey "Key_Cross_Bow_Key"
#define ldCrossBowUnlockedKey "Key_Cross_Bow_Unlocked"
//#define ldBazookaLevelKey "Key_Bazooka_Key"
#define ldBazookaUnlockedKey "Key_Bazooka_Unlocked"
//#define ldBallShooterLevelKey "Key_Ball_Shooter_Key"
#define ldBallShooterUnlockedKey "Key_Ball_Shooter_Unlocked"
//#define ldAntiBatLevelKey "Key_Anti_Bat_Key"
#define ldAntiBatUnlockedKey "Key_Anti_Bat_Unlocked"
//#define ldStrikerLevelKey "Key_Striker_Key"
#define ldStrikerUnlockedKey "Key_Striker_Unlocked"


#define ldCharacterDaryUnlockKey "Dary_Unlock"

#define KEY_COLLECT_MISSION_TYPE_FORMAT "KEY_COLLECT_MISSION_TYPE_FORMAT_%d"
#define KEY_COLLECT_MISSION_MAX_COUNT_FORMAT "KEY_COLLECT_MISSION_MAX_COUNT_FORMAT_%d"
#define KEY_COLLECT_MISSION_COLLECTED_COUNT_FORMAT "KEY_COLLECT_MISSION_COLLECTED_COUNT_FORMAT_%d"
#define KEY_DOUBLE_EXP_GET "DoubleExpGet"
#define KEY_REMOVE_ADS_GET "RemoveAdsGet"
#define KEY_INFINITY_LIFE_GET "InfiniteLivesGet"
#define KEY_LIFE "LifeKey"
#define KEY_LIFE_SPENT_TIME "LifeSpentTime"
#define KEY_ACHIEVE_FORMAT "KEY_ACHIEVE_FORMAT_%d"
#define KEY_ACHIEVE_LEVEL_FORMAT "KEY_ACHIEVE_LEVEL_FORMAT_%d"
#define KEY_BULLET_COUNT_AT_SLOT_FORMAT "BulletCountAtSlot%d"
#define KEY_LOADED_BULLET_COUNT_AT_SLOT_FORMAT "LoadedBulletCountAtSlot%d"
#define KEY_ISLAND_UNLOCKED "Island_Unlock"
#define KEY_DUNGEON_UNLOCKED "Dungeon_Unlock"
#define KEY_FIELD_UNLOCKED "Field_Unlock"
#define KEY_STARTER_PACK_PURCHASED "STARTER_PAC"
#define KEY_START_DAY "KEY_START_DAY"
#define KEY_STARTER_SUITCASE_LEVEL "KEY_STARTER_SUITCASE_LEVEL"
#define KEY_HEART_LEVEL "KEY_HEART_LEVEL"
#define KEY_STARTERS_ARMOR_LEVEL "KEY_STARTERS_ARMOR_LEVEL"
#define KEY_BOSS_CLEAR_FOR_DAY_FORMAT "KEY_BOSS_CLEAR_FOR_DAY_FORMAT%d"

#define KEY_IAP_USER "IAP_DONE"
#define KEY_IS_ALL_CLEAR "ISALLCLEAR"
#define KEY_IS_TUTORIAL_CLEAR "ISTUTORIALCLEAR"
#define KEY_ADS_COUNTER "AdsCounter"
#define KEY_IS_KNIGHT_CLEAR "IsKnightClear"
//#define KEY_WEAPON_COLLECTED_FORMAT "WeaponCollected%d"

#define KEY_CLEAR_COUNT "DungeonClearCount"
#define KEY_JEWEL_COLLECTED_FORMAT "JewelCollectedFormat%d"
#define KEY_DOES_IT_TALKED_FORMAT "DoesItTalked%s"

#define KEY_AUTO_TARGET_BOUGHT "KEY_AUTO_TARGET_BOUGHT"
#define KEY_AUTO_TARGET_SELECTED "AUTO_TARGET_SELECTED"
#define KEY_FIRST_LAUNCH "KEY_FIRST_LAUNCH"

#define KEY_TUTORIAL_GHOST_IDNEX "KEY_TUTORIAL_GHOST_IDNEX"
#define DATA_SECURE_OFFSET 1021
#define TUTORIAL_STAGE_START -100
#define TUTORIAL_STAGE_END -94


#define BUTTON_LEFT 0
#define BUTTON_RIGHT 1
#define BUTTON_LEFT_RIGHT 2
#define BUTTON_A 3
#define BUTTON_B 4

#define COMMAND_LEFT 0
#define COMMAND_RIGHT 1
#define COMMAND_JUMP 2
#define COMMAND_FIRE 3

#define STATE_STANDING 0
#define STATE_WALKING 1
#define STATE_JUMPING 2
#define STATE_FALLING 3

#define ENEMY_MOVE_STAND 0
#define ENEMY_MOVE_WANDER_ON_A_PLATFORM 1
#define ENEMY_MOVE_WANDER_CROSS_PLATFORM 2
#define ENEMY_MOVE_WANDER_CROSS_PLATFORM_CRASH_BY_WALL 3
#define ENEMY_MOVE_WANDER_ON_CEILING 4
#define ENEMY_MOVE_WANDER_ON_WALL 5
#define ENEMY_MOVE_HANG 6
#define ENEMY_MOVE_DROP 7
#define ENEMY_MOVE_DROP_CRASH 8
//#define ENEMY_MOVE_FLY_TO_HERO 9
#define ENEMY_MOVE_FLY_TO_HERO_HORIZONTALLY 10
#define ENEMY_MOVE_FLY_TO_HERO_FOLLOW 11
#define ENEMY_MOVE_FLY_TO_RIGHT 12
#define ENEMY_MOVE_FLY_TO_DOWN 13
#define ENEMY_MOVE_FLY_TO_LEFT 14
#define ENEMY_MOVE_FLY_TO_UP 15
#define ENEMY_MOVE_CUSTOM 16
#define ENEMY_MOVE_FLY_WANDER_STOP 17
#define ENEMY_MOVE_SWIM_TO_HERO_FOLLOW 18
#define ENEMY_MOVE_JUMP_STAND 19
#define ENEMY_MOVE_JUMP_WANDER 20
#define ENEMY_MOVE_JUMP_TO_TARGET 21
#define ENEMY_MOVE_TO_CLIFF 22
#define ENEMY_MOVE_CROSS_TO_TARGET 23


#define ENEMY_ACTION_NONE 0
#define ENEMY_ACTION_FIRE 1
#define ENEMY_ACTION_DEFENCE 2
#define ENEMY_ACTION_MOVE 3
#define ENEMY_ACTION_MOVE_TO_LEFT 4
#define ENEMY_ACTION_MOVE_TO_RIGHT 5


#define ENEMY_AFTER_FIND_HERO_STAND 0
#define ENEMY_AFTER_FIND_HERO_WANDER 1
#define ENEMY_AFTER_FIND_HERO_HANG 2
#define ENEMY_AFTER_FIND_HERO_FLY 3


#define UNIT_DARY 0
#define UNIT_DOOR 1
#define UNIT_COIN 2
#define UNIT_SHORT_THORN 3
#define UNIT_HOTDOG 4
#define UNIT_BAT 5
#define UNIT_SNAIL 6
#define UNIT_THORN_DROP 7
#define UNIT_FIRE 8
#define UNIT_BEE 9
#define UNIT_WOLF 10
#define UNIT_BUBBLE 11
#define UNIT_ELECTRIC_BALL 12
#define UNIT_STANDING_FISH 13
#define UNIT_RED_THORN_BOTTOM 14
#define UNIT_RED_THORN_TOP 15
#define UNIT_RED_THORN_LEFT 16
#define UNIT_RED_THORN_RIGHT 17
#define UNIT_GOLD_COIN 18
#define UNIT_STAR 19
#define UNIT_MEDI_KIT 20
#define UNIT_KEY 21
#define UNIT_SPRING_THEME0 22
#define UNIT_BROOM 23
#define UNIT_SIGN 24
#define UNIT_APPLEMON 25
#define UNIT_SQUIRREL 26
#define UNIT_FORKPORK_STAND 27
#define UNIT_BUDDY 28
#define UNIT_MISSILE_DROPPABLE 29
#define UNIT_MISSILE_CUSTOM 30
#define UNIT_MISSILE_STRAIGHT 31
#define UNIT_MISSILE_CHASING 32
#define UNIT_MACE_TRAP 33
#define UNIT_FROG 34
#define UNIT_RANDOM_BOX 35
#define UNIT_FORKPORK 36
#define UNIT_APPLEMON_WORM 37
#define UNIT_WOOD 38
#define UNIT_SKELETON_WIZARD 39
#define UNIT_FISH_MON 40
#define UNIT_GHOST 41
#define UNIT_BLUE_WORM 42
#define UNIT_FLYING_PENGUIN 43
#define UNIT_WALKING_PENGUIN 44
#define UNIT_BOARD_OWL 45
#define UNIT_TREE_MAN 46
#define UNIT_PUMPKIN 47
#define UNIT_PINK_RAKOON 48
#define UNIT_GREEB_WORM 49
#define UNIT_POT 50
#define UNIT_DRUM_BOMB 51
#define UNIT_THREE_EYES 52
#define UNIT_LEAP_WORM 53
#define UNIT_BLUE_POT_WATERBALL 54
#define UNIT_BEE_HIVE 55
#define UNIT_SPRING_THEME2 56

#define UNIT_DUNG_BEETLE 63
#define UNIT_DJ_MONKEY 64

#define UNIT_CART 65
#define UNIT_TOP 67
#define UNIT_BIRD 68
#define UNIT_BALLOON 69

#define UNIT_ODDEYE 70
#define UNIT_ODDEYERIDE 71
#define UNIT_HOTDOG_GUN 72
#define UNIT_OCTOPUS 73
#define UNIT_JELLYFISH 74

#define UNIT_FOOD 75
#define UNIT_COIN_PART 76
#define UNIT_LASER_TRAP 77
#define UNIT_FIRE_STICK 78

#define UNIT_PIERROT_DUMB 79
#define UNIT_SUITCASE 80
#define UNIT_PIERROT_NORMAL 81
#define UNIT_HOOD_GHOST 82
#define UNIT_THE_KNIGHT 83
#define UNIT_GATE 84
#define UNIT_NPC 85
#define UNIT_DUMMY 86
#define UNIT_PIERROT_DOG 87
#define UNIT_PIERROT_GHOST 88
#define UNIT_PIERROT_CROSS 89
#define UNIT_SELLER 90
#define UNIT_ONSALE 91
#define UNIT_SOLDOUT 92
#define UNIT_BAZOOKA_MINI 93
#define UNIT_BAZOOKA_BIG 94
#define UNIT_MACHINEGUN_MINI 95
#define UNIT_MACHINEGUN_BIG 96
#define UNIT_MISSILE_THROUGH 97
#define UNIT_PIERROT_TALL 98
#define UNIT_PIERROT_SCOT 99
#define UNIT_PIERROT_VERTIAL_EYE 100
#define UNIT_TRAP_KIND_OF 101
#define UNIT_JEWELRY_BOX 102
#define UNIT_DESTRUCTABLE 103
#define UNIT_KEYBOARD_KEY 104
#define UNIT_TUTORIAL_GHOST 105
#define UNIT_RUNNING_HUD 106

#define PET_NONE 0
#define PET_BOOGI 1
#define PET_BURI 2
#define PET_CHUS 3
#define PET_DORA_0 4
#define PET_DORA_1 5
#define PET_DORA_2 6
#define PET_DORA_3 7
#define PET_DORIS_0 8
#define PET_DORIS_1 9
#define PET_DORIS_2 10
#define PET_DORIS_3 11
#define PET_FRITH_0 12
#define PET_FRITH_1 13
#define PET_FRITH_2 14
#define PET_FRITH_3 15
#define PET_KIWI 16
#define PET_KUMURI 17
#define PET_LEXY 18
#define PET_LINDIS_0 19
#define PET_LINDIS_1 20
#define PET_LINDIS_2 21
#define PET_LINDIS_3 22
#define PET_NEPTUN 23
#define PET_OXTO 24
#define PET_PLUTO 25
#define PET_RIUS 26
#define PET_ROLO_0 27
#define PET_ROLO_1 28
#define PET_ROLO_2 29
#define PET_ROLO_3 30
#define PET_RURIK_0 31
#define PET_RURIK_1 32
#define PET_RURIK_2 33
#define PET_RURIK_3 34
#define PET_TERRY 35
#define PET_TORI 36
#define PET_TORORI 37
#define PET_UNO 38
#define PET_VETH 39
#define PET_WALOONG_0 40
#define PET_WALOONG_1 41
#define PET_WALOONG_2 42
#define PET_WALOONG_3 43
#define PET_ANGEL_DRAGON 44
#define PET_IRON_DRAGON 45
#define PET_WOOD_DRAGON 46
#define PET_STONE_DRAGON 47
#define PET_FALCON 48

#define PET_NO_MORE 49

#define PET_MOVE_FLY 0
#define PET_MOVE_WALK 1


#define TRAP_TYPE_RAIN 0
#define TRAP_TYPE_LASER 1

#define GM GameManager::getInstance()
#define SM ServerManager::getInstance()
#define MOBILE_TITLE GameManager::getInstance()->mobileTitleLayer
#define HUD GameManager::getInstance()->getHudLayer()
#define DUAL_HUD GameManager::getInstance()->getDualHudLayer()
#define WORLD GameManager::getInstance()->getWorld()
#define DUAL_WORLD GameManager::getInstance()->getDualWorld()
#define LM LanguageManager::getInstance()
#define LMText LanguageManager::getInstance()->getText
#define UD UserDefault::getInstance()
#define UDGetInt UserDefault::getInstance()->getIntegerForKey
#define UDSetInt UserDefault::getInstance()->setIntegerForKey
#define UDGetDouble UserDefault::getInstance()->getDoubleForKey
#define UDSetDouble UserDefault::getInstance()->setDoubleForKey
#define UDGetBool UserDefault::getInstance()->getBoolForKey
#define UDSetBool UserDefault::getInstance()->setBoolForKey
#define UDGetStr UserDefault::getInstance()->getStringForKey
#define UDSetStr UserDefault::getInstance()->setStringForKey
#define SPT_REMOVE_FUNC CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt))
#define SHAKE_ONCE CallFunc::create(CC_CALLBACK_0(HelloWorld::shakeScreenOnce, this))
#define strmake StringUtils::format
#define BTN_FROM_REF_AND_DISABLE Button* btn = (Button*)ref;btn->setEnabled(false);
#define BTN_FROM_REF_AND_DISABLE_FOR_A_SEC Button* btn = (Button*)ref;btn->setEnabled(false);btn->runAction(Sequence::create(DelayTime::create(1), CallFuncN::create(CC_CALLBACK_1(GameManager::enableButton, GM)), NULL));
#define BTN_FROM_REF Button* btn = (Button*)ref;


#define IAP_FLAG_INIT -1
#define IAP_FLAG_FAILED 2
#define IAP_FLAG_SUCCESS 3
#define IAP_FLAG_RESTORED 4

#define IAP_DETAIL_STARTER1 "mg.starter01"
#define IAP_DETAIL_STARTER2 "mg.starter02"
#define IAP_DETAIL_STARTER3 "mg.starter03"
#define IAP_DETAIL_PACKAGE1 "mg.package01"
#define IAP_DETAIL_PACKAGE2 "mg.package02"
#define IAP_DETAIL_GEM1 "mg.gem01"
#define IAP_DETAIL_GEM2 "mg.gem02"
#define IAP_DETAIL_GEM3 "mg.gem03"
#define IAP_DETAIL_GEM4 "mg.gem04"
#define IAP_DETAIL_GEM5 "mg.gem05"
#define IAP_DETAIL_MONTHLY_TICKET "mg.ticket01"


#define SOUND_COIN 2
#define SOUND_FIRE_SMALL 3
#define SOUND_LASER 4
#define SOUND_MACHINE_GUN_1 5
#define SOUND_MACHINE_GUN_2 6
#define SOUND_MEDICINE 7
#define SOUND_SPRING 8
#define SOUND_DDALKACK 9
#define SOUND_JUMP 10
#define SOUND_POWER_UP_45 11
#define SOUND_ROCKET_LAUNCH 12
#define SOUND_NOISE_37 13
#define SOUND_FA 16
#define SOUND_MISOL 17
#define SOUND_FOOT_STEP 21
#define SOUND_WATER_SPLASH 22
#define SOUND_SMALL_EXPLOSION 23
#define SOUND_GUN_FIRE_0 24
#define SOUND_GUN_FIRE_1 25
#define SOUND_GUN_FIRE_2 26
#define SOUND_RELOAD 27
#define SOUND_RELOAD_SHORT 28
#define SOUND_LASER_BUZZ 29
#define SOUND_LIGHTNING 30
#define SOUND_LASER_WOONG 31
#define SOUND_LASER_SHARK_GUN 32
#define SOUND_TOONG 33
#define SOUND_KOONG 34
#define SOUND_STONE_ROLL 35
#define SOUND_STICK_SOUND 36
#define SOUND_GLASS_BREAK 37
#define SOUND_WOOD_CRASH 38
#define SOUND_WOOD_DRAP 39
#define SOUND_GIGGLING 40
#define SOUND_THUNDER 41
#define SOUND_DUNGEON_NOISE 42

#define EFFECT_TWINKLE 0
#define EFFECT_EXPLODE_SMALL 1
#define EFFECT_EXPLODE_MIDDLE 2
#define EFFECT_EXPLODE_BIG 3
#define EFFECT_EXPLODE_HUGE 4
#define EFFECT_METAL_SLUG_EXPLOSION 11
#define EFFECT_SMOKE 5
#define EFFECT_GREEN_SMOKE 6
#define EFFECT_RED_SMOKE 7
#define EFFECT_BULLET_WALL 8
#define EFFECT_BULLET_WALL_SMALL 9
#define EFFECT_BOMB_WALL 10

#define BGM_NOVEMBER 0
#define BGM_BRIGHT_NO_EFFECT 1
#define BGM_GAMEOVER 2
#define BGM_WIN 3
#define BGM_REVIVE 4
#define BGM_BRIGHT 5
#define BGM_EXCITING 6

#define VEHICLE_NONE 0
#define VEHICLE_GOLIATH 1
#define VEHICLE_BROOM 2
#define VEHICLE_CART 3
#define VEHICLE_TOP 4
#define VEHICLE_BIRD 5
#define VEHICLE_BALLOON 6


#define ACTION_TAG_ANIMATION 0
#define ACTION_TAG_ROTATION 1

#define GUN_ORDINARY 0

#define BOSS_BAZOOKA 0
#define BOSS_MACHINE_GUN 1
#define BOSS_GIANT 2
#define BOSS_GIANT_HOOD 3

#define MARKET_APPSTORE_PAID 0
#define MARKET_PLAYSTORE_PAID 1
#define MARKET_APPSTORE_FREE 2
#define MARKET_PLAYSTORE_FREE 3
#define MARKET_NAVER_PAID 4
#define MARKET_TSTORE_PAID 5
#define MARKET_SAMSUNG_APPS 6
#define MARKET_MAC 7
#define MARKET_FUNBOX 8
#define MARKET_OUYA 9
#define MARKET_WINDOWS 10
#define MARKET_CHINA_NO_IAP 11

#define FONT_DEFAULT 0
#define FONT_BITDUST_ONE 1
#define FONT_BITDUST_TWO 2
#define FONT_ARIAL 3
#define FONT_VISITOR 4

#define GOLD 0
#define GEM 1

#define TAG_CHECK_MARK 1
#define TAG_FORE_LAYER 1000
#define TAG_UNIT_LAYER 1001
#define TAG_STAGE_LAYER 1002
#define TAG_DECO_LAYER 1003
#define TAG_BACK_LAYER 1004
#define TAG_THEME_LOCK 2001
#define TAG_THEME_LOCK_COVER 2002
#define TAG_THEME_STAR_BOARD 2003
#define TAG_THEME_STAR_COLLECTED_LABEL 2004
#define TAG_THEME_STAR_SLASH_LABEL 2005
#define TAG_THEME_STAR_TOTAL_LABEL 2006
#define TAG_NINJA_VAIL 2007

#define TAG_DISPOSABLE_MESSAGE 9900

#define MISSILE_EFFECT_NONE 0
#define MISSILE_EFFECT_SMOKE 1
#define MISSILE_EFFECT_RED_BALL 2
#define MISSILE_EFFECT_GREEN_SMOKE 3
#define MISSILE_EFFECT_RED_SMOKE 4
#define MISSILE_EFFECT_SHARK_GUN 5

#define INVENTORY_MAX_COUNT 100

#define FOOD_APPLE 0
#define FOOD_GREEN_APPLE 1
#define FOOD_PINE_APPLE 2
#define FOOD_PEAR 3
#define FOOD_BANANA 4
#define FOOD_STRAWBERRY 5
#define FOOD_CHERRY 6
#define FOOD_CARROT 7
#define FOOD_MUSHROOM 8
#define FOOD_CORN 9
#define FOOD_WATERMELON 10
#define FOOD_FISH 11
#define FOOD_BISCKET 12
#define FOOD_DONUT 13
#define FOOD_BREAD 14
#define FOOD_TOAST 15
#define FOOD_CHICKEN 16
#define FOOD_ICECREAM 17
#define FOOD_HAMBURGER 18
#define FOOD_POTATO_CHIPS 19
#define FOOD_CANDY 20
#define FOOD_HOTDOG 21
#define FOOD_ONIGIRI 22
#define FOOD_LOLLYPOP 23

#define COIN_OFFSET 7973
#define GEM_OFFSET 9997
#define GAME_EXP_UNIT 37

#define KEY_FOOD_FORMAT "FOOD_%d"
#define KEY_PET_FORMAT "PET_%d"
#define KEY_PET_LEVEL_FORMAT "PET_LEVEL_%d"
#define KEY_PREVIOUS_INVENTORY_COUNT "PreviousInventoryCount"
#define KEY_SKY_PET_INDEX "SkyPetIndex"
#define KEY_GROUND_PET_INDEX "GroundPetIndex"
#define KEY_COLLECTION_FORMAT "Pet_%d_Collected"
//#define KEY_ACHIEVEMENT_GOAL "Achievement_Goal_%d"
//#define KEY_ACHIEVEMENT_CURRENT "Achievement_Current_%d"
#define KEY_ACHIEVEMENT "KEY_ACHIEVEMENT"
#define KEY_TROPHY "KEY_TROPHY"

#define KEY_SOUND_VOLUMN "SoundVolumn"
#define KEY_MUSIC_VOLUMN "MusicVolumn"
#define KEY_SOUND_ONOFF "KEY_SOUND_ONOFF"
#define KEY_MUSIC_ONOFF "KEY_MUSIC_ONOFF"
#define KEY_BUTTON_SIZE "ButtonSize"
#define KEY_ALERT_ONOFF "KEY_ALERT_ONOFF"
//#define KEY_ACHIEVEMENT_REWARD_COLLECTED_FORMAT "Achievement_Reward_Collected_%d"
#define KEY_COLLECTION_REWARD_COLLECTED_FORMAT "Collection_Reward_Collected_%d"

#define KEY_STAGE_MASTER_FORMAT "StageMaster_%d"
#define KEY_STAR_COUNT_FORMAT "StarCount_%d_%d"
#define KEY_LAST_CLEAR_STAGE "LastClearStage"
#define KEY_JUST_CLEARED_PREVIOUS_STAR_COUNT "JustClearedPreviousStarCount"
#define KEY_IS_JUST_CLEARED "IsJustCleared"
#define KEY_JUST_CLEARED_STAGE "JustClearedStage"
#define KEY_STAGE_CLEAR_FORMAT "StageCleared_%d"
#define KEY_SELECTED_WEAPON "LastWeapon"
#define KEY_WEAPON_PURCHASED_FORMAT "WeaponPurchased_%d"
#define KEY_SELECTED_CHARACTER "LastCharacter"
#define KEY_CHARACTER_PURCHASED_FORMAT "CharacterPurchased_%d"
#define KEY_WEAPON_POWER_LEVEL "Weapon_Pwr_%d"
#define KEY_WEAPON_CRITICAL_LEVEL "Weapon_Cri_%d"
#define KEY_FREE_PET "FreePet"
#define KEY_BONUS_WEAPON_FORMAT "KEY_BONUS_WEAPON_FORMAT_%d"
#define KEY_GEM "KEY_GEM"
#define KEY_VIDEO_ADS_LEFT "KEY_VIDEO_ADS_LEFT"
#define KEY_VIDEO_NEXT_AVAILABLE_TIME "KEY_VIDEO_NEXT_AVAILABLE_TIME"
#define KEY_LEVEL "KEY_LEVEL"
#define KEY_PASSWORD "KEY_PASSWORD"
#define KEY_EXP "KEY_EXP"
#define KEY_VIDEO_STORE_RECEIVED_INDEX_FORMAT "KEY_VIDEO_STORE_RECEIVED_INDEX_FORMAT_%d"
#define KEY_VIDEO_STORE_WATCHED_TIME_FORMAT "KEY_VIDEO_STORE_WATCHED_TIME_FORMAT_%d"

#define KEY_TODAY_COUNT "KEY_TODAY_COUNT"
#define KEY_DAY_COUNT "KEY_DAY_COUNT"
#define KEY_DAY_CHECKED "KEY_DAY_CHECKED"
#define KEY_SAVED_ID "KEY_SAVED_ID"
#define KEY_RID "KEY_RID"
#define KEY_AVATAR "KEY_AVATAR"
#define AVATAR_COUNT 22
#define KEY_UNLOCKED_LOCAL_ENEMY "KEY_UNLOCKED_LOCAL_ENEMY"
#define KEY_NETWORK_BATTLE_RESULT_FORMAT "KEY_NETWORK_BATTLE_RESULT_FORMAT%d"
#define KEY_ACCEESS_REWARD_RECEIVED_AM79 "KEY_ACCEESS_REWARD_RECEIVED_AM79"
#define KEY_ACCEESS_REWARD_RECEIVED_PM1214 "KEY_ACCEESS_REWARD_RECEIVED_PM1214"
#define KEY_ACCEESS_REWARD_RECEIVED_PM1720 "KEY_ACCEESS_REWARD_RECEIVED_PM1720"
#define KEY_ACCEESS_REWARD_RECEIVED_PM2022 "KEY_ACCEESS_REWARD_RECEIVED_PM2022"
#define KEY_ACCEESS_REWARD_RECEIVED_PM2224 "KEY_ACCEESS_REWARD_RECEIVED_PM2224"
#define KEY_BAG_CONTENT "KEY_BAG_CONTENT"
#define key_GIFT_TAKE_DAY "key_GIFT_TAKE_DAY"
#define KEY_STARTER_PACK_PURCAHSED "KEY_STARTER_PACK_PURCAHSED"
#define KEY_SEARCH_STATE_FORMAT "KEY_SEARCH_STATE_FORMAT%d"
#define KEY_SEARCH_TICKET_COUNT "KEY_SEARCH_TICKET_COUNT"
#define KEY_SEARCH_TICKET_COUNT "KEY_SEARCH_TICKET_COUNT"
#define KEY_SEARCH_TICKET_REFILL_STARTED_TIME "KEY_SEARCH_REFILL_STARTED_TIME"
//#define KEY_LOCAL_TICKET_COUNT "KEY_LOCAL_TICKET_COUNT"
//#define KEY_LOCAL_TICKET_REFILL_STARTED_TIME "KEY_LOCAL_TICKET_REFILL_STARTED_TIME"
//#define KEY_NETWORK_TICKET_COUNT "KEY_NETWORK_TICKET_COUNT"
//#define KEY_NETWORK_TICKET_REFILL_STARTED_TIME "KEY_NETWORK_TICKET_REFILL_STARTED_TIME"
#define KEY_LAST_REFRESHED_DAY "KEY_LAST_REFRESHED_DAY"
#define KEY_DAY_COUNTER "KEY_DAY_COUNTER"
#define KEY_RUNE_EQUIPPED_FORMAT "KEY_RUNE_EQUIPPED_FORMAT_%d"
#define KEY_DAILY_REWARD_COUNTER "KEY_DAILY_REWARD_COUNTER"
#define KEY_COLLECTION_WEAPON_FOUND "KEY_COLLECTION_WEAPON_FOUND"
#define KEY_COLLECTION_SHIELD_FOUND "KEY_COLLECTION_SHIELD_FOUND"
#define KEY_COLLECTION_RUNE_FOUND "KEY_COLLECTION_RUNE_FOUND"
//#define KEY_DAILY_DUNGEON_TICKET "KEY_DAILY_DUNGEON_TICKET"
#define KEY_LAST_LAUNCH_DATE "KEY_LAST_LAUNCH_DATE"
#define KEY_MONTHLY_TICKET_END_DATE "KEY_MONTHLY_TICKET_END_DATE"
#define KEY_MONTHLY_GEM_END_DATE "KEY_MONTHLY_GEM_END_DATE"
#define KEY_MONTHLY_TICKET_RECEIVE_DATE "KEY_MONTHLY_TICKET_RECEIVE_DATE"
#define KEY_MONTHLY_GEM_RECEIVE_DATE "KEY_MONTHLY_GEM_RECEIVE_DATE"
#define KEY_MONTHLY_TICKET_IS_INSTANT_RECEIVED "KEY_MONTHLY_TICKET_IS_INSTANT_RECEIVED"
#define KEY_MONTHLY_GEM_IS_INSTANT_RECEIVED "KEY_MONTHLY_GEM_IS_INSTANT_RECEIVED"


#define KEY_ARMOR_ABILITY_LEVEL_FORMAT "KEY_ARMOR_ABILITY_LEVEL_FORMAT%d"
#define KEY_RESEARCH_ABILITY_LEVEL_FORMAT "KEY_RESEARCH_ABILITY_LEVEL_FORMAT%d"
#define KEY_AD_NOT_TODAY "AdNotToday"
#define KEY_SEARCH_ITEM_INDICES_FORMAT "KEY_SEARCH_ITEM_INDICES_FORMAT%d"

#define CURRENCY_TYPE_GEM_GRAY 0
#define CURRENCY_TYPE_GEM_GREEN 1
#define CURRENCY_TYPE_GEM_ORANGE 2
#define CURRENCY_TYPE_GEM_RED 3
#define CURRENCY_TYPE_GEM_PURPLE 4
//#define CURRENCY_TYPE_TICKET_LOCAL 5
//#define CURRENCY_TYPE_TICKET_NETWORK 6

#define KEY_CURRENCY_COUNT_FORMAT "KEY_CURRENCY_COUNT_FORMAT_%d"
#define KEY_SNAPSHOT_VER "snapShotVersion"
#define KEY_COIN_FARM "coinFarm"
#define KEY_COIN_COUNT "coinCount"
#define KEY_SOUL_COUNT "soulCount"


#define KEY_IAP_TOTAL "KEY_IAP_TOTAL"
#define KEY_STAGE "KEY_STAGE"
#define KEY_MISSION_STATES "KEY_MISSION_STATES"
#define KEY_POST_MESSAGES "KEY_POST_MESSAGES"
#define KEY_WEAPON_SLOT_FORMAT "KEY_WEAPON_SLOT_FORMAT_%d"
#define KEY_SHIELD_SLOT_FORMAT "KEY_SHIELD_SLOT_FORMAT_%d"

#define KEY_UPDATE_REWARD_PROVIDED "UpdateIssue"
#define KEY_SELECTED_LANGUAGE "SelectedLanguage"
#define KEY_NAME "Name"

#define KEY_TODAYS_SCORE "TodaysScore"
#define KEY_TODAYS_BOSS_CLEAR_COUNT "KEY_TODAYS_BOSS_CLEAR_COUNT"

//#define ACHIEVEMENT_COMPLETE_TUTORIAL 0
//#define ACHIEVEMENT_BEAT_BOSS 1
//#define ACHIEVEMENT_OPEN_GOLD_SUITCASE 2
//#define ACHIEVEMENT_PERFECT_SCORE 3
//#define ACHIEVEMENT_WHO_IS_THE_HOOD 4
//#define ACHIEVEMENT_WEAPON_MASTER 5
//#define ACHIEVEMENT_DUNGEON_MASTER 6
//#define ACHIEVEMENT_WHERE_IS_THE_PRINCESS 7
//#define ACHIEVEMENT_OPEN_JEWELRY_BOX 8
//#define ACHIEVEMENT_COLLECT_RELICS 9

#define STAGE_DUAL -8
#define STAGE_BOSS_DUNGEON -7
#define STAGE_DAILY_DUNGEON_3 -6
#define STAGE_DAILY_DUNGEON_2 -5
#define STAGE_DAILY_DUNGEON_1 -4
#define STAGE_DAILY_DUNGEON_0 -3
#define STAGE_ENTRANCE -2
#define STAGE_LOBBY -1
#define STAGE_NAMING -200
#define STAGE_NONE -9999

#define PAGE_TITLE 0
#define PAGE_THEME_SELECT 1
#define PAGE_STAGE_SELECT 2
#define PAGE_EGG 3
#define PAGE_INVENTORY 4
#define PAGE_COLLECTION 5
#define PAGE_AVHIEVE 6
#define PAGE_SHOP 7
#define PAGE_SETTING 8
#define PAGE_STAGE 9
#define PAGE_START_POPUP 10
#define PAGE_WEAPON_POPUP 11

#define CLOSE_TO_NEXT 0
#define CLOSE_TO_RETRY 1
#define CLOSE_TO_STAGES 2
#define CLOSE_TO_MAIN 3

#define DEFAULT_CRITICAL 5

#define VIDEO_GAME_OVER 1
#define VIDEO_WEAPON_RENT 2
#define VIDEO_GAME_CONTINUE 3
#define VIDEO_SHOP 4
#define VIDEO_VIDEOSTORE 5
//static const Color3B greenColor = {5,126,45};
//static const Color3B yesColor = {241,74,1};
//static const Color3B noColor = {46,124,179};

#define DIRECTION_UP 0
#define DIRECTION_RIGHT 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3

typedef struct PetInfo
{
    int maxLevel;
    int maxAttack;
    int petNumber;
    int levelRequired;
    const char* name;
    const char* description;
    int starCount;
}PET;
typedef struct BagItemInfo
{
    int value;
    int index;
}BagItem;
using namespace cocos2d;

class GameManager : public Node
{
private:
    int bulletType;
    
    //Constructor
    GameManager();
    
    //Instance of the singleton
    static GameManager* m_mySingleton;
    HudLayer* hudLayer = nullptr;
    DualHudLayer* dualHudLayer = nullptr;
    Layer* pauseLayer;
    Layer* optionLayer;
    Layer* gameOverLayer;
    Layer* achievementLayer;
    
    Scene* gameOverScene;
    HelloWorld* currentStageLayer = nullptr;
    DualWorld* currentDualLayer = nullptr;
    int sustainSoundTag=-1;
    
    int currentBGM;
    std::string userDefaultData;
    
public:
    MobileTitle* mobileTitleLayer = nullptr;
    HelloWorld* getWorld();
    DualWorld* getDualWorld();
    int collectedJewelIndex = -1;
//    bool isShieldPurcahsed();
//    void setShield(long time);
//    long getShieldPurchasedTime();
    void shakeIt(Node* node);
    bool mapUpdatedNeeded = false;
    bool isWeaponSet=false;
    int lifeUpdateNeeded;
    int life;
    Layer* settingLayer;
    const char* getFont(int font);
    void initGameManager();
    Layer* titleLayer;
    Layer* worldMap;
    bool isGuestPlay;
    bool isVideoReady();
    bool isUsingController;
    const char* convertMyUserDefaultToCharArray();
    void convertCharArrayToMyUserDefault(const char* data);

    int totalThemeCount;
    int totalStage;
    bool gameStarted;
    bool initComplete;
    int recommendedWeapon;
    Sprite* myPhoto;
    void setOutLine(Sprite* spt);
    void saveGameData();
    void setLoadedData(const char* key, const char* value);
    std::string getWeaponName(int index);
    std::string getShieldName(int index);
    std::string getItemName(int value);
    cocos2d::Point getGemCountPosition();
    bool isPaidGame;
    bool isInMiddleOfGame;
    bool firstPlayed;
    const char* version;
    int versionCode = 0;
    int receivedVersionCode = 0;
    int serverMaintenance = 0;
    cocos2d::Size originalSize;
    cocos2d::Size size;
    bool isStageSetOnce;
    int currentStageIndex = -1;
    
    bool leftPressed;
    bool rightPressed;
    bool jumpPressed;
    bool cPressed;
    bool firePressed;
    bool downPressed;
    bool upPressed;
    int market;
    
    int ranking=-1;
    int page;
    bool showVPad;
    bool developer = false;
    bool canJumpForever;
    bool paidUser;
    bool appAlreadyLaunched;
    int theme;
    const char* currentUserID;
    //Get instance of singleton
    static GameManager* getInstance();
    void showVideoDone();
    void showVideoFailed();
    void showVideo(int which);
    int videoIndex;
    void clickAdsDone();
    void clickAdsFailed();
    void exitGame();
    bool isVideoRewardEnergy;
    bool isVideoRewardAttack;
    bool isVideoRewardCoins;
    void setFontName(Label* lbl, const char* name, float fontSize);
    int getGemForCoin(int coinCount);
    //A function that returns zero "0"
    int ReturnZero(){return 0;}
    // another test function
    void runScene() { CCLOG("test");};
    void buttonDown(int buttonType);
    void buttonUp(int buttonType);
    void setCoin(int howMuch);
    void addCoin(int howMuch);
    int getCoin();
    int getSoul();
    void addSoul(int howMuch);
    void saveCoin();
    void addGem(int howMuch);
    void setGem(int howMuch);
    int getGem();
    void addCurrency(int howMuch, int currencyType);
    void setCurrency(int howMuch, int currencyType);
    int getCurrency(int currencyType, int defaultValue = 0);
    void setFontSize(Label* lbl, float size);
    int getGameLevel();
    void setGameLevel(int level);
    int getCurrentExp();
    int getCurrentExpMax();
    void showDisposableMessage(const char* msg, Node* parent);
    bool checkDataSecure();
    void addGameExp(int exp);
    void addGameLevel();
    int getRandomFood();
    int getFoodCount(int food);
    void addFoodCount(int food, int count);
    const char* getFoodSpriteName(int food);
    int getTotalStarCount();
    int getWeaponPrice(int index);
    
    void preLoadAllSoundEffect();
    void playBGM(int bgm);
    void stopBGM();
    int lastBGM;
    void resumeBGM();
    void playSoundEffect(int sound, float gain = 1.0f, float pan = 0);
    void stopSoundEffect(int sound);
    
    void makeLabelEllipsis(Label* lbl, float width);
    void setMusicVolumn(float vol);
    void setSoundVolumn(float vol);
    void setButtonSize(float siz);
    void setNotificationOn(bool onOff);
    int getBombCount();
    long getTodaysRandom();
    bool isFacebookReady;
    float getDistance(Node* target, Node* source);
    float getSoundVolumnByDistance(Node* target, Node* source);
    void setFacebookReady(bool ready);
    void setBombCount(int bomb);
    int getPotionCount();
    void setPotionCount(int bomb);
    float getMusicVolumn();
    float getSoundVolumn();
    float getButtonSize();
    bool getNotificationOn();
    double getAngle(cocos2d::Point pos1, cocos2d::Point pos2);
//    void SendAchievement(const char* text, int count, int goal);
    const char* getWeaponImageFileName(int weapon);
    const char* getShieldImageFileName(int item);
    void makeItSiluk(Node* node);
    void addMastery(int stageIndex);
    int getMastery(int stageIndex);
    int getWeaponPowerLevel(int weapon);
    void addWeaponPowerLevel(int weapon);
    int getWeaponCriticalLevel(int weapon);
    void addWeaponCriticalLevel(int weapon);
    float getWeaponPowerRuneResearchApplied(int value);
    float getWeaponCriticalPossibilityRuneResearchApplied(int value);
    
    void runAnimation(Sprite* spt, std::string name, bool repeat);
    void scrollTheLayer(ui::ScrollView* scrollLayer, bool isLeft, bool isHorizontal, int howMuch);
//    HelloWorld* getStageLayer();
    HudLayer* getHudLayer();
    DualHudLayer* getDualHudLayer();
    Scene* getTitleScene();
    Layer* getGameStartLayer();
    Scene* getGameStartScene();

    void loadFlagImage(ImageView* flag, int family);
    Layer* getTitleLayer();
    Layer* getShopLayer();
    Layer* getPauseLayer();
//    Layer* getGameOverLayer();
//    Scene* getGameOverScene();
    const char* getBulletName(int weaponType, int playerMissileDemage);
    Layer* getOptionLayer();
//    Layer* getAchievementLayer();
    void setCurrentStageLayer(HelloWorld* layer);
    void setCurrentDualLayer(DualWorld* layer);
    const char* getShortenedKoreanString(std::string str, int length);
    HelloWorld* getCurrentStageLayer();
    DualWorld* getCurrentDualLayer();
    
//    void setStageLayer(Layer* layer);
    void setHudLayer(HudLayer* layer);
    void setDualHudLayer(DualHudLayer* layer);
    void setStageScene(Scene* scene);
//    RepeatForever* getScaleUpDownAction(float scale);
    
    int getBulletType();
    bool DoesWeaponExist(int weaponType);
    int getNextToThisWeapon(int weaponType);
    int getNextWeaponToUnlock(int weaponType);
    
    int getWeaponPower(int weaponType);
    float getWeaponPower(int index, int rank);
    Missile* getBullet(int weaponType, int playerMissileDemage);
    bool getWeaponUnlocked(int weapon);
    bool getCharacterUnlocked(int costume);
    const char* getCharacterUnlockedKey(int costume);
    const char* getCharacterName(int costume);
    const char* getCharacterDesc(int costume);
    
    void pushLayer(Layer* parent, Layer* layer);
    void pushLayerWithoutDisable(Layer* parent, Layer* layer);
    void popLayer(Layer* layer);
    void popLayerWithoutAnimation(Layer* layer);
    void animateFadeIn(Node* layer, Node* parent);
    void animateFadeOut(Node* layer);
    void animationFadeInDone(Node* layer);
    void animationFadeOutDone(Node* layer);
    
//    void disableLayer(Layer* layer);
//    void enableLayer(Layer* layer);
    void showSpriteExplosion(Node* parent, const char* sptName, Vec2 pos, int sptCount, float radius, float dur, bool isInPlist = false);
    void showParticleExplosion(Node* prt, const char* sptName, cocos2d::Point pos, float scale);
    
    void scheduleLocalNotification(const char* title, const char* msg, int time);
    void scheduleLocalNotification();
    void unscheduleAllNotifications();
    long strToLong(const char* str);
    Sprite* getGrayScaleImage(const char* src);
    RenderTexture* createAdditiveBorder( Sprite* label, int size, Color3B color, GLubyte opacity );
    RenderTexture* createAdditive( Sprite* label, Color3B color, GLubyte opacity, int additiveCount);
    Sprite* getSpriteShapeSolidColorSprite(const char* src, Color3B color);
    bool isBoss(int theme, int stage);
    
    void reset();
    void backup();
    
    std::vector<PET*> petInfoArray;
    PET* createPetInfo(int petNumber, int maxLevel, int maxAttack, int levelRequired,int starCount, const char* name, const char* description);
    void setPetInfos();
    void arrangeInvetory();
    void addPet(int petNumber);
    int getInvetoryCount();
    void removePet(int index);
    bool setPetCollected(int index);
    bool getPetCollected(int index);
//    void initAchievement();
//    void setAchievementGoal(int achievementIndex, int goalCount);
//    bool setAchievement(int achievementIndex, int countToAdd);
//    const char* getAchievementId(int achievementIndex);
    void googleSignInOrOut(bool sign);
    bool isGoogleSigned;
//    int getAchievementGoalCount(int achievementIndex);
//    int getAchievementCurrentCount(int achievementIndex);
    
    int getPetLevel(int index);
    void setPetLevel(int index, int level);
    int getPetLevel(int petNumber, int exp);
    int getPetAttack(int petNumber, int level);
    int getPetAttack(int index);
//    void getPetAttack(int petNumber, int petExp, int& level, int& attack, int &expLeft);
    int getPetNumber(int index);
    const char* getPetSpriteName(int petNumber);
    const char* getPetName(int petNumber);
    
    void setCharacterPurchased(int character);
    bool isCharacterPurchased(int character);
    void setWeaponPurchased(int weapon);
    bool isWeaponPurchased(int weapon);
    Buddy* getSkyPet();
    Buddy* getGroundPet();
    
    void showInterstitialAds();
    int getSkyPetIndex();
    int getGroundPetIndex();
    Buddy* getPetFromInventory(int petIndex);
    Buddy* getPet(int petNumber);
    void setSkyPet(int index);
    void setGroundPet(int index);
    
    PET* getPetInfo(int petNumber);
//    int getIndexOf(std::vector<int> vec, int index);
    int getInventoryCount();
    Sprite* getCard(int index);
//    __String* getInAppItemPrice(const char* name);
//    static void setInAppItemPrice(const char* name, const char* price);
//    __Dictionary* priceArray;
    
    void nativeControllerButtonEvent(int controller, int button, bool isPressed, float value, bool isAnalog);
    
    void nativeControllerLAxisXEvent(int controller, float value);
    void nativeControllerLAxisYEvent(int controller, float value);
    
    Layout* waitingLayer;
    void showWaiting(Node* parent, cocos2d::Size size);
    void closeWaiting();
    
    ValueVector split(const std::string &str, const std::string &delim);
    
    bool isPlayerSet = false;
    int collectedKeyCount=0;
    void setWeaponCollected(int index);
    bool isGameClear = false;
    
    // menu selecting by keyboard
    Node* findUp(Node* currentNode);
    Node* findDown(Node* currentNode);
    Node* findLeft(Node* currentNode);
    Node* findRight(Node* currentNode);
    Node* findWithDirect(cocos2d::Point pos, int direction, Node* currentPanel);
    Node* getNodeAtThisPoint(cocos2d::Point pos, Node* currentNode);
    
    bool isOnlineEnabled = true;
    
    int totalKill=0;
    int totalDestroy=0;
    int totalCoin=0;
    int totalRoom=0;
    int totalBoss=0;
    float totalTime=60*60;
    int totalScore=0;
    
    bool jewelryCollected = false;
    bool isAutoTargetSelected = false;
    Label* lastDisposableLabel = nullptr;
    Sprite* lastDisposableLabelBack = nullptr;
    
    
    int currentPet = -1;
    std::string currentIAPID = "";
    int iapFlag = IAP_NONE;
    std::string iapDetail = "";
//    int iapStateFlag = IAP_STATE_NOT_INIT;
    
    void buyItem(int index);
    void alignToCenter(Node* node0, Node* node1, float gap, float centerX, float offsetX);
    void alignToCenter(Vector<Node*> array, float gap, float centerX);
    
    void setTimeLeft(Label* lbl, int seconds);
    void setTimeLeft(Text* lbl, int seconds);
    void setTimeLeft(TextBMFont* lbl, int seconds);
    std::string getTimeLeftInString(int seconds);
    std::string getTimeLeftInStringHMS(int seconds);
    int selectedDay = -1;
    
    
    int getMaxTotalBulletCount(int weapon);
    float getWeaponCoolTime(int weapon);
    
    int exp = 0;

    std::vector<BagItem*> bagContentArray;
//    std::vector<int> bagContentArray;
    void loadBagContent();
    void saveBagContent();
    int getItemCountInBag(int index);
    void addBagContent(int itemIndex, int level);
    void removeBagContent(int index);
    void removeBagContentWithoutUpdateOrder(int index);
    void setItemCollectionFound(int itemIndexx, int state);
    bool isCollectionHasNew();
    bool isCollectionHasNew(int itemType);
    BagItem* getBagItem(int index);
    int getBagContentItem(int index);
    int getBagContentLevel(int index);
    int getItemType(int value);
    void updateBagItemIndices();
    std::vector<BagItem*> getBagContents(int filter);
    Sprite* getItemSprite(int index);
    int getWeaponRareness(int index);
    int getShieldRareness(int index);
    float getShieldHp(int index);
    float getShieldHp(int index, int rank);
    float getShieldHPRuneApplied(int value);
    float getShieldDefRuneApplied(int value);
    float getShieldDef(int index);
    float getShieldDef(int index, int rank);
    int getItemRareness(int value);
    int getRuneBuff(int rank, int runeBuffType);
    int getRuneBuffForEffectee(int runeValue, int effectee);
    int getRuneBuffType(int runeValue);
    bool endsWith (std::string const &fullString, std::string const &ending);
    void showPopup(Node* popup);
    Color3B getRandomColor();
    void enableButton(Ref* ref);
    
    int getHeroHP();
    int getHeroDefence();
    int getHeroDefenceBuff();
    float getHeroDodge();
    int getHeroNextHP();
    int getHeroNextDefence();
    int getHeroNextDefenceBuff();
    float getHeroNextDodge();
    
    float getResearchCoolTime();
    float getResearchCoolTime(int level);
    int getResearchPower();
    int getResearchPower(int level);
    float getResearchCriticalPossibility();
    float getResearchCriticalPossibility(int level);
    int getResearchCriticalPower();
    int getResearchCriticalPower(int level);
    float getResearchNextCoolTime();
    int getResearchNextPower();
    float getResearchNextCriticalPossibility();
    int getResearchNextCriticalPower();
    

    void eraseSubStr(std::string & mainStr, const std::string & toErase);
    
    std::vector<int> appliedShieldArray;
    std::vector<float> appliedShieldHPArray;
    
    bool isLocalGamePlay;
    bool isBossFight = false; // test 
    int localPlayIndex;
    int networkPlayIndex;
    void gatherUp(Node* layer);
    void splitUp(Node* layer);
    
    std::string getLocalBattleEnemyEquipInString(int index);
    
    std::vector<int> getRandomNumbers(int minInclusive, int maxExclusive, int count);
    
    int dailyMissionComplete[12];
    void replaceAll(std::string& source, const std::string& from, const std::string& to);
    
    std::vector<int> playerShieldIndices;
    std::vector<int> playerShieldHps;
    int playerEnergyMax = 0;
    int playerEnergy = 0;
    std::vector<std::string> priceList;
    
    bool isTutorialOn = false;
    int tutorialType = TUTORIAL_DUNGEON;
    int tutorialIndex = 0;
    bool isBattleWon = false;

    int dualEnemyChar;
    std::string dualEnemyName;
    std::string dualEnemyEquip;
    std::string dualEnemyRune;
    std::string dualEnemyArmor;
    std::string dualEnemyResearch;
    int dualEnemyTrophy = 1000;
    
    void setCollectMission();
    
    int tipIndex = 0;
    int getDayDiff(int fromY, int fromM, int fromD, int toY, int toM, int toD);
    

    void addConsumedItem(std::string strSkuName);
    void onRestored(std::string strSkuID);
    std::vector<std::string> consumeSkuNameList;
    
    std::string iapList = "";
    
    Sprite* getAvatarIcon(int index);
    bool isMouseAvailable = false;
    
    double searchTicketRefillTimeLeft = 0;
    int searchTicketCount = 0;
    double dungeonTicketRefillTimeLeft = 0;
    int dungeonTicketCount = 0;
    double bossTicketRefillTimeLeft = 0;
    int bossTicketCount = 0;
    double localTicketRefillTimeLeft = 0;
    int localTicketCount = 0;
    double networkTicketRefillTimeLeft = 0;
    int networkTicketCount = 0;
    std::string strNetworkMatchDatas = "";
    std::string strNetworkMatchIDs = "";
    std::string strNetworkMatchNames = "";
    std::string strNetworkMatchTrophys = "";
    std::string strNetworkMatchAvatars = "";
    std::string strNetworkMatchEquips = "";
    std::string strNetworkMatchRunes = "";
    std::string strNetworkMatchArmors = "";
    std::string strNetworkMatchResearchs = "";
    bool shouldNetworkMatchRefresh = false;
    
    int lastWeaponSlot = 0;
    int videoShopIndex = 0;
    double videoShopWatchedTime = 0;
    int videoStore0Index = 0;
    double videoStore0WatchedTime = 0;
    int videoStore1Index = 0;
    double videoStore1WatchedTime = 0;
    int videoStore2Index = 0;
    double videoStore2WatchedTime = 0;
};

#endif
