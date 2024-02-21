print("Testing Started")

local ret, err = pcall(function()
	require("lua_threaded")
	iFace = LuaThreaded.CreateInterface()
	iFace:InitGmod()
end)

if err then
	print("[ERROR] " .. err)
	file.Write("error.txt", err)
end

print("Testing Finished")