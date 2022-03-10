#include "../../../toolchain/ndk/arm/sysroot/usr/include/math.h"
#include <hook.h>
#include <mod.h>
#include <symbol.h>
#include <nativejs.h>
#include "recovered.hpp"


class CoolElytraModule : public Module {
	private:
	template<typename T> static int signum(T val) {
		return (T(0) < val) - (val < T(0));
	}
	static float lerp(float delta, float o, float n) {
		return o + (n - o) * delta;
	}
	public:
	static float previousRollAngle;
	static bool isRocketing;
	CoolElytraModule(): Module("coolelytra") {};
	virtual void initialize() {
		DLHandleManager::initializeHandle("libminecraftpe.so", "mcpe");
		HookManager::addCallback(
			SYMBOL("mcpe", "_ZN14CameraDirector6updateER15IClientInstanceff"),
			LAMBDA((CameraDirector* director, void* client, float f1, float f2), {
				onRenderLevel(director);
			}, ),
			HookManager::RETURN | HookManager::LISTENER
		);
		HookManager::addCallback(
			SYMBOL("mcpe", "_ZN20FireworksRocketActor10normalTickEv"),
			LAMBDA((FireworksRocketActor* actor), {
				if(actor->isAttachedToEntity()) {
					isRocketing = actor->life < actor->lifespan;
				}
			}, ),
			HookManager::CALL | HookManager::LISTENER
		);
    }
	static void onRenderLevel(CameraDirector* director) {
		LocalPlayer* player = GlobalContext::getLocalPlayer();
		if(player->getStatusFlag(ActorFlags::ELYTRA_FLYING) && !(player->isInWater() || player->isInLava()) && !isRocketing) {
			Vec3 facing = getVectorFromRotation(player->getRotation());
			Vec3 velocity = getPlayerInstantaneousVelocity();
			double horizontalFacingSquared = facing.x * facing.x + facing.z * facing.z;
			double horizontalSpeedSquared = velocity.x * velocity.x + velocity.z * velocity.z;
			float rollAngle = 0.0f;
			if(horizontalFacingSquared > 0.0 && horizontalSpeedSquared > 0.0) {
				double dot = (velocity.x * facing.x + velocity.z * facing.z) / sqrt(horizontalFacingSquared * horizontalSpeedSquared);
				if(dot >= 1.0) dot = 1.0;
				else if(dot <= -1.0) dot = -1.0;
				double direction = signum(velocity.x * facing.z - velocity.z * facing.x);
				rollAngle = (float) (atan(sqrt(horizontalSpeedSquared) * acos(dot) * wingPower) * direction * 57.29577951308);
			}
			rollAngle = (float) ((1.0 - rollSmoothing) * rollAngle + rollSmoothing * previousRollAngle);
			previousRollAngle = rollAngle;
			Camera* camera = director->getCamera();
			Vec3 angles = camera->getEulerAngles();
			camera->setOrientation(-angles.y, -angles.x, angles.z - (rollAngle * 0.017453292f));
		}
	}
	static Vec3 getPlayerInstantaneousVelocity() {
		LocalPlayer* player = GlobalContext::getLocalPlayer();
		Vec3 velocity = player->getStateVectorComponent().velocity;
		float tickDelta = GlobalContext::getMinecraft()->getTimer()->getAlpha();
		if(tickDelta < 0.01f) return velocity;
		double newvx = velocity.x;
		double newvy = velocity.y;
		double newvz = velocity.z;
		double gravity = 0.08;
		Vec2 rotation = player->getRotation();
		Vec3 facing = getVectorFromRotation(rotation);
		float pitchRadians = rotation.y * 0.017453292f;
		double horizontalFacingSquared = facing.x * facing.x + facing.z * facing.z;
		double horizontalFacing = sqrt(horizontalFacingSquared);
		double horizontalSpeed = sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
		newvy += gravity * (-1.0 + horizontalFacingSquared * 0.75);
		if(horizontalFacing > 0.0) {
			if(velocity.y < 0.0) {
				double lift = newvy * -0.1 * horizontalFacingSquared;
				newvx += facing.x * lift / horizontalFacing;
				newvy += lift;
				newvz += facing.z * lift / horizontalFacing;
			}
			if(pitchRadians < 0.0f) {
				double lift = horizontalSpeed * -(double)sinf(pitchRadians) * 0.04;
				newvx += -facing.x * lift / horizontalFacing;
				newvy += lift * 3.2;
				newvz += -facing.z * lift / horizontalFacing;
			}
			newvx += (facing.x / horizontalFacing * horizontalSpeed - velocity.x) * 0.1;
			newvz += (facing.z / horizontalFacing * horizontalSpeed - velocity.z) * 0.1;
		}
		newvx *= 0.9900000095367432;
		newvy *= 0.9800000190734863;
		newvz *= 0.9900000095367432;
		return Vec3 {
			lerp(tickDelta, velocity.x, (float) newvx),
			lerp(tickDelta, velocity.y, (float) newvy),
			lerp(tickDelta, velocity.z, (float) newvz)
		};
	}
	static Vec3 getVectorFromRotation(Vec2 const& rotation) {
		float f = cosf(-rotation.x * 0.017453292f - M_PI);
		float f1 = sinf(-rotation.x * 0.017453292f - M_PI);
		float f2 = -cosf(-rotation.y * 0.017453292f);
		float f3 = sinf(-rotation.y * 0.017453292f);
		return Vec3 { f1 * f2, f3, f * f2 };
	}
	static double wingPower;
	static double rollSmoothing;
};
float CoolElytraModule::previousRollAngle = 0.0f;
bool CoolElytraModule::isRocketing = false;
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