local code = [[local ret, err = pcall(function()
	print(Vector(1, 1, 1), type(Vector(1, 1, 1)), isvector(Vector(1, 1, 1)))
	print(Angle(1, 1, 1), type(Angle(1, 1, 1)), isangle(Vector(1, 1, 1)))

	print("file.Exists (garrysmod.ver) ", file.Exists("garrysmod.ver", "MOD"))
	print("file.IsDir (data/) ", file.IsDir("data/", "MOD"))
	print("file.Size (garrysmod.ver) ", file.Size("garrysmod.ver", "MOD"))
	print("file.Time (garrysmod.ver) ", file.Time("garrysmod.ver", "MOD"))
	local gm = file.Open("garrysmod.ver", "r", "MOD")
	print("File:__tostring ", gm)
	print(getmetatable(gm))
	for k, v in pairs(getmetatable(gm)) do
		print(k, v)
	end
	print("File:Read ", getmetatable(gm).Read(gm))
	gm:Close()
end)

if err then
	print("[ERROR] " .. err)
	--file.Write("error.txt", err)
end]]

print("Testing Started")

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