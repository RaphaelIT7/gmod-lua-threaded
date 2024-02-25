print("Testing Started")

local code = [[

local ret, err = pcall(function()
	print(Vector(1, 1, 1), type(Vector(1, 1, 1)), isvector(Vector(1, 1, 1)))
	print(Angle(1, 1, 1), type(Angle(1, 1, 1)), isangle(Vector(1, 1, 1)))

	print("File Exists (garrysmod.ver): " .. file.Exists("garrysmod.ver", "MOD"))
	local gm = file.Open("garrysmod.ver", "MOD")
	print("File:__tostring ", gm)
	print("File:Read ", gm:Read())
	gm:Close()
end)

if err then
	print("[ERROR] " .. err)
	--file.Write("error.txt", err)
end
]]

local ret, err = pcall(function()
	require("lua_threaded")
	iFace = LuaThreaded.CreateInterface()
	iFace:InitGmod()
	iFace:RunString(code)
end)

if err then
	print("[ERROR] " .. err)
	file.Write("error.txt", err)
end

print("Testing Finished")