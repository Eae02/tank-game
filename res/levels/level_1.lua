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

local finalTanksRemaining = 3
for i = 1, 3 do
	findEntity("tankFinal_" .. i):onKilled(function ()
		finalTanksRemaining = finalTanksRemaining - 1
		if finalTanksRemaining == 0 then
			levelComplete("level_2")
		end
	end)
end
