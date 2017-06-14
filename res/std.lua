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
