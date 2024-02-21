print("Testing Started")

local ret, err = pcall(function()
	require("lua_threaded")
	iFace = LuaThreaded.CreateInterface()
	iFace:InitGmod()
	iFace:RunString([[print(Vector(1, 1, 1), type(Vector(1, 1, 1)), isvector(Vector(1, 1, 1)))]])
	iFace:RunString([[print(Angle(1, 1, 1), type(Angle(1, 1, 1)), isangle(Vector(1, 1, 1)))]])
end)

if err then
	print("[ERROR] " .. err)
	file.Write("error.txt", err)
end

print("Testing Finished")