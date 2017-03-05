function updateDoor4()
	if findEntity("door4_act1"):hasActivated() and findEntity("door4_act2"):hasActivated() then
		findEntity("door4"):open()
	end
end

function updateDoor5()
	if findEntity("door5_act1"):hasActivated() and findEntity("door5_act2"):hasActivated() then
		findEntity("door5"):open()
	end
end

function updateDoor9()
	if findEntity("door9_act1"):hasActivated() and findEntity("door9_act2"):hasActivated() then
		findEntity("door9"):open()
	end
end

local alertedTanks = { };

function maybeAlertTank(name)
	if alertedTanks[name] then
		return
	end
	alertedTanks[name] = true
	
	local tank = findEntity(name)
	
	if tank then
		tank:detectPlayer()
	else
		print("maybeAlertTank(): " .. name .. " could not be alerted.")
	end
end