declare interface CoolElytraConfig {
    setWingPower(power: number): void;
    setRollSmoothing(smoothing: number): void;
}
declare function WRAP_NATIVE(module: "CoolElytraConfig"): CoolElytraConfig;