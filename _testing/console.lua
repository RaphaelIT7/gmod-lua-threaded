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

JSONHTTP({
	method = "POST",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/power",
	headers = header,
	success = function(res)
		if res.errors then
			print("Restart failed! Reason: " .. res.errors[1].detail)
		else
			print("Restarted successfully")
		end
	end,
	body = json.encode({
		signal = "restart"
	})
})

JSONHTTP({
	method = "POST",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/power",
	headers = header,
	success = function(res)
		if res.errors then
			print("Commands failed! Reason: " .. res.errors[1].detail)
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
		if res.errors then
			print("debug.log failed! Reason: " .. res.errors[1].detail)
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

print("Waiting for HTTP")

HTTP_WaitForAll()

print("Finished everything")
local time = os.time() + 20
while time > os.time() do end
print("Checking for any crash")

HTTP({
	method = "GET",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/files/contents?file=%2Fdebug.log",
	headers = header,
	success = function(content)
		print("Lol")
	end,
})

HTTP({
	method = "GET",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/files/contents?file=%2F",
	headers = header,
	success = function(content)
		
	end,
})

HTTP_WaitForAll()