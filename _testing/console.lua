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
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/files/delete",
	headers = header,
	success = function(res)
		local tbl = res ~= '' and json.decode(res) or {}
		if tbl.errors then
			print("debug.log failed! Reason: " .. tbl.errors[1].detail)
		end
	end,
	body = json.encode({
		root = "/",
		files = {
			"debug.log"
		}
	})
})

HTTP({
	method = "POST",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/files/delete",
	headers = header,
	success = function(res)
		local tbl = res ~= '' and json.decode(res) or {}
		if tbl.errors then
			print("console.log failed! Reason: " .. tbl.errors[1].detail)
		end
	end,
	body = json.encode({
		root = "/garrysmod/",
		files = {
			"console.log"
		}
	})
})

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

print("Waiting for Server to start")

local started = false
while not started do
	JSONHTTP({
		method = "GET",
		url = "https://" .. url .. "/api/client/servers/" .. id .. "/resources",
		headers = header,
		success = function(content)
			if content.attributes.current_state ~= "starting" then
				started = true
			end
		end,
	})
end

print("Server started")

HTTP({
	method = "POST",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/command",
	headers = header,
	success = function(res)
		local tbl = res ~= '' and json.decode(res) or {}
		if tbl.errors then
			print("Commands failed! Reason: " .. tbl.errors[1].detail)
		end
	end,
	body = json.encode({
		command = "lua_run require([[lua_threaded]]) iFace = LuaThreaded.CreateInterface() iFace:InitGmod()"
	})
})

local time = os.time() + 5
while time > os.time() do end

HTTP({
	method = "POST",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/command",
	headers = header,
	success = function(res)
		local tbl = res ~= '' and json.decode(res) or {}
		if tbl.errors then
			print("Commands failed! Reason: " .. tbl.errors[1].detail)
		end
	end,
	body = json.encode({
		command = "quit"
	})
})

print("Waiting for Server to stop")

local stopped = false
while not stopped do
	JSONHTTP({
		method = "GET",
		url = "https://" .. url .. "/api/client/servers/" .. id .. "/resources",
		headers = header,
		success = function(content)
			if content.attributes.current_state == "offline" then
				stopped = true
			end
		end,
	})
end

print("Server stopped")

HTTP({
	method = "GET",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/files/contents?file=%2Fdebug.log",
	headers = header,
	success = function(content)
		if content:sub(1, 10) ~= [[{"errors":]] then
			print(" ======================= debug.log =======================")
			print(content)
			print(" ======================= debug.log =======================")
			error("Server crashed!")
		end
	end,
})

HTTP({
	method = "GET",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/files/contents?file=%2Fgarrysmod%2Fconsole.log",
	headers = header,
	success = function(content)
		if content:sub(1, 10) ~= [[{"errors":]] then
			print(" ======================= console.log =======================")
			print(content)
			print(" ======================= console.log =======================")
		end
	end,
})