local code = [[local ret, err = pcall(function()
	print(Vector(1, 1, 1), type(Vector(1, 1, 1)), isvector(Vector(1, 1, 1)))
	print(Angle(1, 1, 1), type(Angle(1, 1, 1)), isangle(Vector(1, 1, 1)))

	print("=== File Library ===")

	print("file.Exists (garrysmod.ver) ", file.Exists("garrysmod.ver", "MOD"))
	print("file.IsDir (data/) ", file.IsDir("data/", "MOD"))
	print("file.Size (garrysmod.ver) ", file.Size("garrysmod.ver", "MOD"))
	print("file.Time (garrysmod.ver) ", file.Time("garrysmod.ver", "MOD"))
	file.AsyncRead("garrysmod.ver", "MOD", function(fileName, gamePath, status, data)
		print("Callback called", data, status)
	end)


	print("=== File Class ===")
	local test = file.Open("test.txt", "wb", "DATA")
	print("File:__tostring ", test)
	test:Write("Test")
	test:WriteBool(true)
	test:WriteByte(100)
	test:WriteDouble(70)
	test:WriteFloat(1.5)
	test:WriteLong(2100000000)
	test:WriteShort(32000)
	test:WriteUInt64(4000000000000000)
	test:WriteUShort(64000)
	print("File:Size ", test:Size())
	print("File:Tell ", test:Tell())
	print("File:EndOfFile ", test:EndOfFile())
	test:Flush()
	test:Close()

	local test = file.Open("test.txt", "rb", "DATA")
	print("File:Seek ", test:Tell())
	print("File:Read ", test:Read(4))
	print("File:ReadBool ", test:ReadBool())
	print("File:ReadByte ", test:ReadByte())
	print("File:ReadDouble ", test:ReadDouble())
	print("File:ReadFloat ", test:ReadFloat())
	print("File:ReadLong ", test:ReadLong())
	print("File:ReadShort ", test:ReadShort())
	print("File:ReadUInt64 ", test:ReadUInt64())
	print("File:ReadUShort ", test:ReadUShort())
	test:Close()

	print("=== Timer Library ===")

	timer.Simple(1, function()
		print("timer.Simple works")
	end)

	timer.Create("Test", 0.5, 3, function()
		print("timer.Create works " .. timer.RepsLeft("Test"))
	end)

	print("timer.Exists ", timer.Exists("Test"))

	hook = {
		Run = function(name, ...)
			local args = {...}
			print("hook.Run called!", name)
			for k, v in pairs(args) do
				print(v)
			end
		end
	}

	error("Error handling test")
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
	--iFace:RunHook("ExampleHook", "ExampleArg", 1234, Vector(1, 2, 3))
end)

if err then
	print("[ERROR] " .. err)
	file.Write("error.txt", err)
end

print("Testing Finished")