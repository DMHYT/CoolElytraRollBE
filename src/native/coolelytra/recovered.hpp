#ifndef COOLELYTRAROLL_RECOVERED_HPP
#define COOLELYTRAROLL_RECOVERED_HPP


class BlockPos {
    public:
    float x, y, z;
    BlockPos(float, float, float);
};
class Vec2 {
    public:
    float y, x;
};
class Vec3 {
    public:
    union {
        struct {
            float x, y, z;
        };
        float values[3];
    };
};
class Camera {
    public:
    void setOrientation(float, float, float);
    Vec3 getEulerAngles() const;
};
class LevelRenderer;
class ScreenContext;
class FrameRenderObject;
class StateVectorComponent {
    public:
    Vec3 position;
    Vec3 lastPosition;
    Vec3 velocity;
};
enum ActorFlags { ELYTRA_FLYING = 32 };
class Actor {
    public:
    bool isInWater() const;
    bool isInLava() const;
    int getLifeSpan() const;
    Vec2 getRotation() const;
    StateVectorComponent& getStateVectorComponent() const;
    bool getStatusFlag(ActorFlags) const;
};
class FireworksRocketActor : public Actor {
    public:
    char filler[1256];
    int life;
    int lifespan;
    bool isAttachedToEntity() const;
};
class Mob : public Actor {public:};
class Player : public Mob {public:};
class LocalPlayer : public Player {public:};
class CameraDirector {
    public:
    Camera* getCamera() const;
};
class Timer {
    public:
    float getAlpha() const;
};
class Minecraft {
    public:
    Timer* getTimer();  
};
namespace GlobalContext {
    LocalPlayer* getLocalPlayer();
    Minecraft* getMinecraft();
}


#endif //COOLELYTRAROLL_RECOVERED_HPP