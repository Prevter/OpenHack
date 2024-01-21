#pragma once
#include "../pch.h"

namespace robtop
{
    class CCSpritePlus : public cocos2d::CCSprite
    {
        uint8_t pad_1[0x20];

    public:
        cocos2d::CCArray *m_followers;
        CCSpritePlus *m_followingSprite;
        bool m_hasFollower;
        bool m_propagateScaleChanges;
        bool m_propagateFlipChanges;
    };

    class GameObject : public CCSpritePlus
    {
    public:
        inline uint32_t get_id()
        {
            return m_objectID;
        }
        inline cocos2d::CCPoint get_position()
        {
            return m_startPosition;
        }
        inline float get_x()
        {
            return m_startPosition.x;
        }
        inline float get_y()
        {
            return m_startPosition.y;
        }

        uint8_t pad_1[0xf];
        bool m_hasExtendedCollision;
        uint8_t pad_2[0x13];
        cocos2d::CCSprite *m_baseSprite;
        cocos2d::CCSprite *m_detailSprite;
        uint8_t pad_3[0x64];
        std::string m_particleString;
        uint8_t pad_4[0x1];
        bool m_particleUseObjectColor;
        uint8_t pad_5[0x32];
        int m_linkedGroup;
        uint8_t pad_6[0x23];
        int m_uniqueID;
        uint32_t m_objectType;
        uint8_t pad_7[0x14];
        double m_realXPosition;
        double m_realYPosition;
        cocos2d::CCPoint m_startPosition;
        uint8_t pad_8[0x1];
        bool m_hasNoAudioScale;
        uint8_t pad_9[0x2a];
        short m_enterChannel;
        short m_objectMaterial;
        uint8_t pad_10[0x4];
        bool m_hasNoGlow;
        int m_targetColor;
        int m_objectID;
        short m_customColorType;
        bool m_isDontEnter;
        bool m_isDontFade;
        bool m_hasNoEffects;
        bool m_hasNoParticles;
        uint8_t pad_11[0x16];
        int m_property53 = sizeof(std::string);
        uint8_t pad_12[0x18];
        void *m_baseColor;
        void *m_detailColor;
        int m_zLayer;
        int m_zOrder;
        uint8_t pad_13[0x10];
        bool m_shouldUpdateColorSprite;
        uint8_t pad_14[0x1];
        bool m_hasGroupParent;
        bool m_hasAreaParent;
        float m_scaleX;
        float m_scaleY;
        std::array<short, 10> *m_groups;
        short m_groupCount;
        bool m_hasGroupParentsString;
        uint8_t pad_15[0xf];
        short m_editorLayer;
        short m_editorLayer2;
        uint8_t pad_16[0x8];
        bool m_isNoTouch;
        uint8_t pad_17[0x2c];
        bool m_isHighDetail;
        uint8_t pad_18[0x11];
        bool m_isPassable;
        bool m_isHide;
        bool m_isNonStickX;
        bool m_isNonStickY;
        bool m_isIceBlock;
        bool m_isGripSlope;
        bool m_isScaleStick;
        bool m_isExtraSticky;
        bool m_isDontBoostY;
        bool m_isDontBoostX;
        uint8_t pad_19[0x11];
        int m_property155;
        int m_property156;
        uint8_t pad_20[0x26];
    };

    class EnhancedGameObject : public GameObject
    {
        bool m_hasCustomAnimation;
        bool m_hasCustomRotation;
        bool m_disableRotation;
        float m_rotationSpeed;
        bool m_animationRandomizedStart;
        float m_animationSpeed;
        bool m_animationShouldUseSpeed;
        bool m_animateOnTrigger;
        bool m_disableDelayedLoop;
        bool m_disableAnimShine;
        int m_singleFrame;
        bool m_animationOffset;
        bool m_animateOnlyWhenActive;
        bool m_isNoMultiActivate;
        bool m_isMultiActivate;
    };

    class EffectGameObject : public EnhancedGameObject
    {
        cocos2d::ccColor3B m_triggerTargetColor;
        float m_duration;
        float m_opacity;
        int m_targetGroupID;
        int m_centerGroupID;
        bool m_isTouchTriggered;
        bool m_isSpawnTriggered;
        bool m_hasCenterEffect;
        float m_shakeStrength;
        float m_shakeInterval;
        bool m_tintGround;
        bool m_usesPlayerColor1;
        bool m_usesPlayerColor2;
        bool m_usesBlending;
        float m_moveOffsetX;
        float m_moveOffsetY;
        int m_easingType;
        float m_easingRate;
        bool m_lockToPlayerX;
        bool m_lockToPlayerY;
        bool m_lockToCameraX;
        bool m_lockToCameraY;
        bool m_useMoveTarget;
        int m_moveTargetMode;
        float m_moveModX;
        float m_moveModY;
        bool m_property393;
        bool m_isDirectionFollowSnap360;
        int m_targetModCenterID;
        float m_directionModeDistance;
        bool m_isDynamicMode;
        bool m_isSilent;
        float m_rotationDegrees;
        int m_times360;
        bool m_lockObjectRotation;
        int m_rotationTargetID;
        float m_rotationOffset;
        int m_dynamicModeEasing;
        float m_followXMod;
        float m_followYMod;
        float m_followYSpeed;
        float m_followYDelay;
        int m_followYOffset;
        float m_followYMaxSpeed;
        float m_fadeInDuration;
        float m_holdDuration;
        float m_fadeOutDuration;
        int m_pulseMode;
        int m_pulseTargetType;
        cocos2d::ccHSVValue m_hsvValue;
        int m_copyColorID;
        bool m_copyOpacity;
        bool m_pulseMainOnly;
        bool m_pulseDetailOnly;
        bool m_pulseExclusive;
        bool m_property210;
        bool m_activateGroup;
        bool m_touchHoldMode;
        int m_touchToggleMode;
        int m_touchPlayerMode;
        bool m_isDualMode;
        int m_animationID;
        bool m_isMultiActivate;
        bool m_triggerOnExit;
        int m_itemID2;
        int m_property534;
        int m_itemID;
        bool m_targetPlayer1;
        bool m_targetPlayer2;
        bool m_followCPP;
        bool m_subtractCount;
        bool m_collectibleIsPickupItem;
        bool m_collectibleIsToggleTrigger;
        int m_collectibleParticleID;
        int m_collectiblePoints;
        bool m_hasNoAnimation;
        float m_gravityValue;
        bool m_isSinglePTouch;
        float m_zoomValue;
        bool m_cameraIsFreeMode;
        bool m_cameraEditCameraSettings;
        float m_cameraEasingValue;
        float m_cameraPaddingValue;
        bool m_cameraDisableGridSnap;
        bool m_property118;
        float m_timeWarpTimeMod;
        bool m_showGamemodeBorders;
        int m_ordValue;
        int m_channelValue;
        bool m_isReverse;
        int m_secretCoinID;
        bool m_ignoreGroupParent;
        bool m_ignoreLinkedObjects;
    };

    class StartPosObject : public EffectGameObject
    {
        int m_unknown;
    };
}