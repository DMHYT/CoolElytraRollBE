#include "../../../../../../../DMH/glm/detail/qualifier.hpp"
#include "../../../../../../../DMH/glm/ext/quaternion_float.hpp"

#ifndef COOLELYTRAROLL_RECOVERED_HPP
#define COOLELYTRAROLL_RECOVERED_HPP


class BlockPos {
    public:
    float x, y, z;
    BlockPos(float, float, float);
};
class Vec2 {
    public:
    float x, y;
};
class Vec3 {
    public:
    float x, y, z;
    Vec3(BlockPos const&);
};

class Matrix {
    public:
    Matrix(glm::tquat<float, (glm::precision)0> const&);
    void mult(Matrix const&);
};

class Tessellator {
    public:
    Matrix* getTransformMatrix() const;
};

class ScreenContext {
    public:
    char filler1[16];
    Tessellator* tessellator;
};
class GameRenderer;
class LevelRenderer;
class FrameRenderObject;

class StateVectorComponent {
    public:
    Vec3 position;
    Vec3 lastPosition;
    Vec3 velocity;
};

class Actor {
    public:
    bool isInWater() const;
    bool isInLava() const;
    Vec2* getRotation() const;
    StateVectorComponent& getStateVectorComponent();
};
class Mob : public Actor {public:};
class Player : public Mob {public:};
class LocalPlayer : public Player {public:};


namespace GlobalContext {
    LocalPlayer* getLocalPlayer();
}


#endif //COOLELYTRAROLL_RECOVERED_HPP