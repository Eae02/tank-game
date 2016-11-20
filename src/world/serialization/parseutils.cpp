#include "parseutils.h"
#include "entityparsers/pointlightparser.h"
#include "entityparsers/raylightparser.h"
#include "entityparsers/lightstripparser.h"
#include "entityparsers/enemytankparser.h"
#include "entityparsers/spiderbotparser.h"
#include "entityparsers/rocketturretparser.h"
#include "entityparsers/activatorparser.h"
#include "entityparsers/eventboxparser.h"
#include "entityparsers/zonehiderparser.h"
#include "entityparsers/propparser.h"
#include "entityparsers/doorparser.h"
#include "entityparsers/checkpointparser.h"
#include "entityparsers/deflectionfieldparser.h"
#include "entityparsers/nullentityparser.h"

#include "../entities/pickups/shieldpickupentity.h"

namespace TankGame
{
	static PointLightParser pointLightParser;
	static RayLightParser rayLightParser;
	static LightStripParser lightStripParser;
	
	static EnemyTankParser enemyTankParser;
	static SpiderBotParser spiderBotParser;
	static RocketTurretParser rocketTurretParser;
	static DeflectionFieldParser deflectionFieldParser;
	
	static ActivatorParser activatorParser;
	static EventBoxParser eventBoxParser;
	static ZoneHiderParser zoneHiderParser;
	static CheckpointParser checkpointParser;
	static PropParser propParser;
	static DoorParser doorParser;
	
	static NullEntityParser<ShieldPickupEntity> shieldPickupParser;
	
	const EntityParser* GetEntityParser(const std::string& className)
	{
		if (className == "RocketTurret")
			return &rocketTurretParser;
		if (className == "EnemyTank")
			return &enemyTankParser;
		if (className == "SpiderBot")
			return &spiderBotParser;
		if (className == "DeflectionField")
			return &deflectionFieldParser;
		
		if (className == "PointLight")
			return &pointLightParser;
		if (className == "RayLight")
			return &rayLightParser;
		if (className == "LightStrip")
			return &lightStripParser;
		
		if (className == "Activator")
			return &activatorParser;
		if (className == "EventBox")
			return &eventBoxParser;
		if (className == "ZoneHider")
			return &zoneHiderParser;
		if (className == "Checkpoint")
			return &checkpointParser;
		if (className == "Prop")
			return &propParser;
		if (className == "Door")
			return &doorParser;
		
		if (className == "ShieldPickup")
			return &shieldPickupParser;
		
		return nullptr;
	}
}
