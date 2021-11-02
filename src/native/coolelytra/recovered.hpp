#ifndef COOLELYTRAROLL_RECOVERED_HPP
#define COOLELYTRAROLL_RECOVERED_HPP


class Vec3 {
    public:
    float x, y, z;
};

class Tessellator {
    public:
};

class ScreenContext {
    public:
    char filler1[16];
    Tessellator* tessellator;
};
class FrameRenderObject;

class Actor {
    public:
    bool isInWater() const;
    bool isInLava() const;
    Vec3* getRotation() const;
};


#endif //COOLELYTRAROLL_RECOVERED_HPP