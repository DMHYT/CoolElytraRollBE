const CoolElytraConfig = WRAP_NATIVE("CoolElytraConfig");
CoolElytraConfig.setWingPower(Math.max(__config__.getDouble("wingPower"), 0));
CoolElytraConfig.setRollSmoothing(Math.max(Math.min(__config__.getDouble("roolSmoothing"), 1), 0));