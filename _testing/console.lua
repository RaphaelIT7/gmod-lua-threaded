websocket = require("websocket")
json = require("json")
require("http")

local args = {...}
local url = args[1]
local id = args[2]
local fullid = args[3]
local api = args[4]


JSONHTTP({
	method = "POST",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/power",
	headers = {
		Authorization = "Bearer " .. api
	},
	success = function(res)
		if res.errors then
			print("Restart failed! Reason: " .. res.errors[1].detail)
		end
	end,
	body = json.encode({
		signal = "restart"
	})
})

JSONHTTP({
	method = "POST",
	url = "https://" .. url .. "/api/client/servers/" .. id .. "/power",
	headers = {
		Authorization = "Bearer " .. api
	},
	success = function(res)
		if res.errors then
			print("Restart failed! Reason: " .. res.errors[1].detail)
		end
	end,
	body = json.encode({
		command = "lua_run timer.Simple(10, function() RunConsoleCommand([[quit]]) end) require([[lua_threaded]]) iFace = LuaThreaded.CreateInterface() iFace:InitGmod()"
	})
})

local uri = "wss://" .. url .. ":8080/api/servers/" .. id .. "/ws"

local client = websocket.client.sync()
local ok, err = client:connect(uri)
if not ok then
    print("Failed to connect:", err)
    return
end

local bytes_sent, err = client:send('{"event":"auth","args":["' .. api .. '"]}')
if not bytes_sent then
    print("Failed to send authentication message:", err)
    client:close()
    return
end

local run = true
local handles = {
	["console output"] = function(args)

	end,
	["status"] = function(args)
		run = false
	end,
}

while run do
    local message, opcode, err = client:receive()
    if not message then
        print("Failed to receive message:", err)
        client:close()
        break
    end
    
    local tbl = json.decode(message)
    if handles[tbl.event] then
    	handles[tbl.event](tbl.args)
    else
    	print(tbl.event, tbl.args)
    end
end