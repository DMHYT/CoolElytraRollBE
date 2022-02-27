#include <hook.h>
#include <mod.h>
#include <logger.h>
#include <symbol.h>
#include <jni.h>
#include <nativejs.h>
#include "recovered.hpp"
#include "../../../toolchain/ndk/arm/sysroot/usr/include/math.h"
#include "../../../../../../../DMH/glm/ext/quaternion_float.hpp"


class CoolElytraModule : public Module {
	private:
	template<typename T> static int signum(T val) {
		return (T(0) < val) - (val < T(0));
	}
	static float lerp(float delta, float o, float n) {
		return o + (n - o) * delta;
	}
	
	public:
	static float tickDelta;
	static float previousRollAngle;

	CoolElytraModule(): Module("coolelytra") {};

	virtual void initialize() {
		DLHandleManager::initializeHandle("libminecraftpe.so", "mcpe");
		HookManager::addCallback(SYMBOL("mcpe", "_ZN12GameRenderer18renderCurrentFrameEf"), LAMBDA((GameRenderer* renderer, float delta), { tickDelta = delta; }, ), HookManager::CALL | HookManager::LISTENER);
		HookManager::addCallback(SYMBOL("mcpe", "_ZN13LevelRenderer11renderLevelER13ScreenContextRK17FrameRenderObject"), LAMBDA((LevelRenderer* renderer, ScreenContext& ctx, FrameRenderObject& renderObj), {
			onRenderLevel(renderer, ctx, renderObj);
		}, ), HookManager::CALL | HookManager::LISTENER);
    }

	static void onRenderLevel(LevelRenderer* renderer, ScreenContext& ctx, FrameRenderObject& renderObj) {
		LocalPlayer* player = GlobalContext::getLocalPlayer();
		if(true /*TODO isFallFlying*/ && !(player->isInWater() || player->isInLava())) {
			Vec2* facing = player->getRotation();
			Vec3* velocity = getPlayerInstantaneousVelocity();
			double horizontalFacing = facing->x * facing->x + facing->y * facing->y;
			double horizontalSpeed = velocity->x * velocity->x + velocity->z * velocity->z;
			float rollAngle = 0.0f;
			if(horizontalFacing > 0.0 && horizontalSpeed > 0.0) {
				double dot = (velocity->x * facing->x + velocity->z * facing->y) / sqrt(horizontalFacing * horizontalSpeed);
				if(dot >= 1.0) dot = 1.0;
				else if(dot <= -1.0) dot = -1.0;
				double direction = signum(velocity->x * facing->y - velocity->z * facing->x);
				rollAngle = (float) (atan(sqrt(horizontalSpeed) * acos(dot) * wingPower) * direction * 57.29577951308);
			}
			rollAngle = (float) ((1.0 - rollSmoothing) * rollAngle + rollSmoothing * previousRollAngle);
			previousRollAngle = rollAngle;
			ctx.tessellator->getTransformMatrix()->mult(Matrix(glm::quat(rollAngle, 0, 0, 1)));
		}
	}
	static Vec3* getPlayerInstantaneousVelocity() {
		LocalPlayer* player = GlobalContext::getLocalPlayer();
		Vec3 velocity = player->getStateVectorComponent().velocity;
		if(tickDelta < 0.01f) {
			Vec3* pVelocity = &velocity;
			return pVelocity;
		}
		double newvx = velocity.x;
		double newvy = velocity.y;
		double newvz = velocity.z;
		double gravity = 0.08;
		Vec2* facing = player->getRotation();
		float pitchRadians = facing->x * 0.017453292f;
		double horizontalFacingSquared = facing->x * facing->x + facing->y * facing->y;
		double horizontalFacing = sqrt(horizontalFacingSquared);
		double horizontalSpeed = sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
		newvy += gravity * (-1.0 * horizontalFacingSquared * 0.75);
		if(horizontalFacing > 0.0) {
			if(velocity.y < 0.0) {
				double lift = newvy * -0.1 * horizontalFacingSquared;
				newvx += facing->x * lift / horizontalFacing;
				newvy += lift;
				newvz += facing->y * lift / horizontalFacing;
			}
			if(pitchRadians < 0.0f) {
				double lift = horizontalSpeed * -(double)sin(pitchRadians) * 0.04;
				newvx += -facing->x * lift / horizontalFacing;
				newvy += lift * 3.2;
				newvz += -facing->y * lift / horizontalFacing;
			}
			newvx += (facing->x / horizontalFacing * horizontalSpeed - velocity.x) * 0.1;
			newvz += (facing->y / horizontalFacing * horizontalSpeed - velocity.z) * 0.1;
		}
		Vec3 result(BlockPos(lerp(tickDelta, velocity.x, newvx), lerp(tickDelta, velocity.y, newvy), lerp(tickDelta, velocity.z, newvz)));
		Vec3* pResult = &result;
		return pResult;
	}
	static double wingPower;
	static double rollSmoothing;
};
float CoolElytraModule::tickDelta = 0.0f;
float CoolElytraModule::previousRollAngle = 0.0f;
double CoolElytraModule::wingPower = 1.25;
double CoolElytraModule::rollSmoothing = 0.85;


MAIN {
	Module* main_module = new CoolElytraModule();
}


JS_MODULE_VERSION(CoolElytraConfig, 1);
JS_EXPORT(CoolElytraConfig, setWingPower, "V(D)", (JNIEnv*, double power) {
	CoolElytraModule::wingPower = power;
	return 0;
});
JS_EXPORT(CoolElytraConfig, setRollSmoothing, "V(D)", (JNIEnv*, double smoothing) {
	CoolElytraModule::rollSmoothing = smoothing;
	return 0;
});