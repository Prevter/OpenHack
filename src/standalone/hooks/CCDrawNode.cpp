#include "hooks.hpp"
#include "../../shared/hacks/hitboxes/hitboxes.hpp"

#include <dash/hook/cocos/CCDrawNode.hpp>

namespace openhack::hooks::CCDrawNode {

    bool drawPolygon(cocos2d::CCDrawNode *self, gd::cocos2d::CCPoint *verts, unsigned int count, const cocos2d::ccColor4F &fillColor,
                     float borderWidth, const cocos2d::ccColor4F &borderColor) {
        hacks::Hitboxes::modifyDraw((gui::Color &) fillColor, borderWidth, (gui::Color &) borderColor);
        return hook::CCDrawNode::drawPolygon(self, verts, count, fillColor, borderWidth, borderColor);
    }

    bool drawCircle(cocos2d::CCDrawNode *self, cocos2d::CCPoint const &position, float radius, cocos2d::_ccColor4F const &color,
                    float borderWidth, cocos2d::_ccColor4F const &borderColor, unsigned int segments) {
        hacks::Hitboxes::modifyDraw((gui::Color &) color, borderWidth, (gui::Color &) borderColor);
        return hook::CCDrawNode::drawCircle(self, position, radius, color, borderWidth, borderColor, segments);
    }

    void installHooks() {
        LOG_HOOK(CCDrawNode, drawPolygon);
        LOG_HOOK(CCDrawNode, drawCircle);
    }

}