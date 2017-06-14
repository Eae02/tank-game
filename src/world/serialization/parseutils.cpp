#include "parseutils.h"
#include "entityparsers/pointlightparser.h"
#include "entityparsers/raylightparser.h"
#include "entityparsers/lightstripparser.h"
#include "entityparsers/enemytankparser.h"
#include "entityparsers/spiderbotparser.h"
#include "entityparsers/rocketturretparser.h"
#include "entityparsers/ammoboxparser.h"
#include "entityparsers/energyballparser.h"
#include "entityparsers/activatorparser.h"
#include "entityparsers/eventboxparser.h"
#include "entityparsers/propparser.h"
#include "entityparsers/doorparser.h"
#include "entityparsers/checkpointparser.h"
#include "entityparsers/deflectionfieldparser.h"
#include "entityparsers/nullentityparser.h"
#include "entityparsers/conveyorbeltparser.h"
#include "entityparsers/flamethrowerparser.h"
#include "entityparsers/omniflamethrowerparser.h"

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
	static FlameThrowerParser flameThrowerParser;
	static OmniFlameThrowerParser omniFlameThrowerParser;
	
	static EnergyBallParser energyBallParser;
	static ActivatorParser activatorParser;
	static AmmoBoxParser ammoBoxParser;
	static EventBoxParser eventBoxParser;
	static CheckpointParser checkpointParser;
	static PropParser propParser;
	static DoorParser doorParser;
	static ConveyorBeltParser conveyorBeltParser;
	
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
		if (className == "FlameThrower")
			return &flameThrowerParser;
		if (className == "OmniFlameThrower")
			return &omniFlameThrowerParser;
		
		if (className == "EnergyBall")
			return &energyBallParser;
		if (className == "Activator")
			return &activatorParser;
		if (className == "AmmoBox")
			return &ammoBoxParser;
		if (className == "EventBox")
			return &eventBoxParser;
		if (className == "Checkpoint")
			return &checkpointParser;
		if (className == "Prop")
			return &propParser;
		if (className == "Door")
			return &doorParser;
		if (className == "ConveyorBelt")
			return &conveyorBeltParser;
		
		if (className == "ShieldPickup")
			return &shieldPickupParser;
		
		return nullptr;
	}
}
