LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos/audio/include)

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := $(LOCAL_PATH)/hellocpp/main.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppDelegate.cpp \
                                       $(LOCAL_PATH)/../../../Classes/AwesomeDialogBox.cpp \
                                       $(LOCAL_PATH)/../../../Classes/Buddies.cpp \
                                       $(LOCAL_PATH)/../../../Classes/CursorLayer.cpp \
                                       $(LOCAL_PATH)/../../../Classes/Droppable.cpp \
                                       $(LOCAL_PATH)/../../../Classes/DualHudLayer.cpp \
                                       $(LOCAL_PATH)/../../../Classes/DualWorld.cpp \
                                       $(LOCAL_PATH)/../../../Classes/EndingCredit.cpp \
                                       $(LOCAL_PATH)/../../../Classes/Enemies.cpp \
                                       $(LOCAL_PATH)/../../../Classes/Episode.cpp \
                                       $(LOCAL_PATH)/../../../Classes/EpisodeLayer.cpp \
                                       $(LOCAL_PATH)/../../../Classes/FifteenSixLogo.cpp \
                                       $(LOCAL_PATH)/../../../Classes/Firework.cpp \
                                       $(LOCAL_PATH)/../../../Classes/GameManager.cpp \
                                       $(LOCAL_PATH)/../../../Classes/GameSharing.cpp \
                                       $(LOCAL_PATH)/../../../Classes/HelloWorldScene_Boss.cpp \
                                       $(LOCAL_PATH)/../../../Classes/HelloWorldScene.cpp \
                                       $(LOCAL_PATH)/../../../Classes/Hero.cpp \
                                       $(LOCAL_PATH)/../../../Classes/HudLayer.cpp \
                                       $(LOCAL_PATH)/../../../Classes/LanguageManager.cpp \
                                       $(LOCAL_PATH)/../../../Classes/Laser.cpp \
                                       $(LOCAL_PATH)/../../../Classes/LegendDaryButton.cpp \
                                       $(LOCAL_PATH)/../../../Classes/MobileTitle.cpp \
                                       $(LOCAL_PATH)/../../../Classes/Title.cpp \
                                       $(LOCAL_PATH)/../../../Classes/MovingPlatform.cpp \
                                       $(LOCAL_PATH)/../../../Classes/MyMessageBox.cpp \
                                       $(LOCAL_PATH)/../../../Classes/PanZoomLayer.cpp \
                                       $(LOCAL_PATH)/../../../Classes/ProfileLayer.cpp \
                                       $(LOCAL_PATH)/../../../Classes/SceneChanger.cpp \
                                       $(LOCAL_PATH)/../../../Classes/ServerManager.cpp \
                                       $(LOCAL_PATH)/../../../Classes/SettingLayer.cpp \
                                       $(LOCAL_PATH)/../../../Classes/SneakyJoystick.cpp \
                                       $(LOCAL_PATH)/../../../Classes/Shield.cpp \
                                       $(LOCAL_PATH)/../../../Classes/SneakyJoystickSkinnedBase.cpp \
                                       $(LOCAL_PATH)/../../../Classes/Trap.cpp \
                                       $(LOCAL_PATH)/../../../Classes/TriggerDoorInfo.cpp \
                                       $(LOCAL_PATH)/../../../Classes/VirtualPadButton.cpp \
                                       $(LOCAL_PATH)/../../../Classes/Weapon.cpp \
                                       $(LOCAL_PATH)/../../../Classes/ZOrderChangleSprite.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-add-path, $(LOCAL_PATH)/../../../cocos2d)
$(call import-module, cocos)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
