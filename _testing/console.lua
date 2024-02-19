json = require("json")
require("http")

local args = {...}
local url = args[1]
local id = args[2]
local api = args[3]

local header = {
	["Accept"] = "application/json",
	["Content-Type"] = "application/json",
	["Authorization"] = "Bearer " .. api,
}

HTTP({
	method = "POST",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/power",
	headers = header,
	success = function(res)
		local tbl = res ~= '' and json.decode(res) or {}
		if tbl.errors then
			print("Restart failed! Reason: " .. tbl.errors[1].detail)
		else
			print("Restarted successfully")
		end
	end,
	body = json.encode({
		signal = "restart"
	})
})

HTTP({
	method = "POST",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/power",
	headers = header,
	success = function(res)
		local tbl = res ~= '' and json.decode(res) or {}
		if tbl.errors then
			print("Commands failed! Reason: " .. tbl.errors[1].detail)
		else
			print("Commands sent successfully")
		end
	end,
	body = json.encode({
		command = "lua_run timer.Simple(10, function() RunConsoleCommand([[quit]]) end) require([[lua_threaded]]) iFace = LuaThreaded.CreateInterface() iFace:InitGmod()"
	})
})

HTTP({
	method = "POST",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/files/delete",
	headers = header,
	success = function(res)
		local tbl = res ~= '' and json.decode(res) or {}
		if tbl.errors then
			print("debug.log failed! Reason: " .. tbl.errors[1].detail)
		else
			print("Deleted debug.log")
		end
	end,
	body = json.encode({
		root = "/",
		files = {
			"debug.log"
		}
	})
})

print("Waiting for Server to start")

local i = 0
local started = false
while i > 15 do
	JSONHTTP({
		method = "GET",
		url = "https://" .. url .. "/api/client/servers/" .. id .. "/resources",
		headers = header,
		success = function(content)
			print(content.attributes.current_state)
			i = i + 1
		end,
	})
end

print("Server started")

HTTP({
	method = "GET",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/files/contents?file=%2Fdebug.log",
	headers = header,
	success = function(content)
		print("Lol", content)
	end,
})

HTTP({
	method = "GET",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/files/contents?file=%2F",
	headers = header,
	success = function(content)
		
	end,
})