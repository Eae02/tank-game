function onPlayerKilled()
	spawnEnergyBalls07()
end

local energyBalls07 = nil

function spawnEnergyBalls07()
	if energyBalls07 then
		energyBalls07[1]:despawn()
		energyBalls07[2]:despawn()
	end
	
	energyBalls07 = { spawnEnergyBall(61.5, 48.5, 1, 0, 100), spawnEnergyBall(61.5, 47.5, 1, 0, 100) }
end

spawnEnergyBalls07()

function updateDoor7()
	if findEntity("door7_act1"):hasActivated() and findEntity("door7_act2"):hasActivated() then
		findEntity("door7"):open()
	end
end

function updateDoor8()
	if findEntity("door8_act1"):hasActivated() and findEntity("door8_act2"):hasActivated() then
		findEntity("door8"):open()
	end
end

r14 = {
	initialized = false,
	numFlameThrowers = 8, --Constant
	flameThrowerProgress = 0,
	activationState = { false, false, false, false },
	tanksAlive = 0,
	stage = 0
}

function r14.init()
	if r14.initialized then
		return
	end
	r14.initialized = true
	
	for i=0,2 do
		setTimer(function ()
			findEntity("r14_light" .. i):setEnabled(true)
		end, i * 0.9)
	end
	
	setTimer(function ()
		for i=3,6 do
			findEntity("r14_light" .. i):setEnabled(true)
		end
		r14.flameThrowerTimerCallback()
	end, 3)
end

function r14.stepFlameThrowers()
	local allDisabled = r14.tanksAlive > 0 or r14.stage == 3
	
	r14.flameThrowerProgress = (r14.flameThrowerProgress + 1) % r14.numFlameThrowers
	local mirrorFTProgress = (r14.flameThrowerProgress + (r14.numFlameThrowers / 2)) % r14.numFlameThrowers
	
	for i = 0, r14.numFlameThrowers - 1 do
		findEntity("r14_ft" .. i):setEnabled(not (i == r14.flameThrowerProgress or i == mirrorFTProgress or allDisabled))
	end
end

function r14.flameThrowerTimerCallback()
	r14.stepFlameThrowers()
	setTimer(r14.flameThrowerTimerCallback, 2)
end

function r14.setActivated(activator, activated)
	if activated and (r14.tanksAlive > 0 or r14.stage >= 3) then
		return
	end
	
	r14.activationState[activator] = activated
	findEntity("r14_actLight" .. activator):setEnabled(activated)
	
	if activated then
		r14.checkComplete()
	end
end

function r14.spawnTanks()
	local spawnX, spawnY = findEntity("r14_tankSpawn"):getPosition()
	
	local idlePath = { { 5, 31 }, { 10, 31 }, { 10, 26 }, { 5, 26 } }
	
	local spawnParams = { }
	
	for i = 1, 4 do
		local tank = spawnEnemyTank(spawnX + (i - 1) * 2, spawnY, spawnParams, idlePath)
		r14.tanksAlive = r14.tanksAlive + 1
		
		tank:onKilled(function ()
			r14.tanksAlive = r14.tanksAlive - 1
			if r14.tanksAlive == 0 then
				r14.stepFlameThrowers();
			end
		end)
	end
	
	r14.stepFlameThrowers();
end

function r14.checkComplete()
	local complete = true
	for i=0,3 do
		if not r14.activationState[i] then
			complete = false
			break
		end
	end
	
	if complete then
		explosion(7.5, 28.5)
		
		for i=0,3 do
			r14.setActivated(i, false)
		end
		
		r14.stage = r14.stage + 1
		
		setBossHP(1 - r14.stage / 3)
		
		if r14.stage == 1 then
			r14.spawnTanks()
		elseif r14.stage == 2 then
			r14.spawnTanks()
			findEntity("r14_turretDoor0"):open()
			findEntity("r14_turretDoor1"):open()
		elseif r14.stage == 3 then
			r14.stepFlameThrowers();
			setTimer(function ()
				setShowBossHP(false)
				levelComplete()
			end, 1)
		end
	end
end
