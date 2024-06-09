local code = [[local ret, err = pcall(function()
	print(Vector(1, 1, 1), type(Vector(1, 1, 1)), isvector(Vector(1, 1, 1)))
	print(Angle(1, 1, 1), type(Angle(1, 1, 1)), isangle(Vector(1, 1, 1)))

	print("=== Entity ===")

	print("Entity(39): ", Entity(39))

	PrintTable(ents)

	print("=== Get info_player_start ===")

	PrintTable(ents.FindByClass("info_player_start"))

	print("ENTITY META")
	
	local meta = FindMetaTable( "Entity" )
	PrintTable(meta)


	print("=== File Library ===")

	print("file.Exists (garrysmod.ver) ", file.Exists("garrysmod.ver", "MOD"))
	print("file.IsDir (data/) ", file.IsDir("data/", "MOD"))
	print("file.Size (garrysmod.ver) ", file.Size("garrysmod.ver", "MOD"))
	print("file.Time (garrysmod.ver) ", file.Time("garrysmod.ver", "MOD"))
	file.AsyncRead("garrysmod.ver", "MOD", function(fileName, gamePath, status, data)
		print("file.AsyncRead callback called length:" .. data:len() .. " status: " .. status .. " data:" .. data)
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

	print("=== Engine Library ===")
	print("engine.TickCount: ", engine.TickCount())
	print("engine.AbsoluteFrameTime: ", engine.AbsoluteFrameTime())
	print("engine.TickInterval: ", engine.TickInterval())
	print("engine.ActiveGamemode: ", engine.ActiveGamemode())

	print("=== engine.GetAddons ===")
	PrintTable(engine.GetAddons())
	print("=== engine.GetUserContent ===")
	PrintTable(engine.GetUserContent())
	print("=== engine.GetGames ===")
	PrintTable(engine.GetGames())
	print("=== engine.GetGamemodes ===")
	PrintTable(engine.GetGamemodes())

	hook = {
		Run = function(name, ...)
			local args = {...}
			print("hook.Run called!", name)
			PrintTable(args)
		end
	}

	gameevent.Listen("player_spawn")
	gameevent.Listen("player_activate")
	gameevent.Listen("player_connect")
	gameevent.Listen("player_connect_client")
	RunConsoleCommand("bot")

	print("GetConVar: ", GetConVar("hostname"))

	print("==== Shared Table ====")
	PrintTable(LuaThreaded.GetValue("example"))

	print("==== Debug Library ====")
	PrintTable(debug)

	print("==== RecipientFilter ====")
	local filter = RecipientFilter()
	print("1. RecipientFilter:__tostring", filter)
	filter:AddAllPlayers() -- The bot should be added.
	print("2. RecipientFilter:__tostring", filter)
	filter:RemoveAllPlayers()
	print("3. RecipientFilter:__tostring", filter)

	print("==== umsg Library ====")
	LuaThreaded.LockMain()
	umsg.Start("Test")
		umsg.String("Hello World")
	umsg.End()
	LuaThreaded.UnlockMain()

	error("Error handling test")
end)

if err then
	print("[ERROR] " .. err)
	--file.Write("error.txt", err)
end]]

print("Testing Started")

local ret, err = pcall(function()
	require("lua_threaded")
	LuaThreaded.ReadyThreads()
	LuaThreaded.SetValue("example", {
		["boolean"] = true,
		["number"] = 10,
		["string"] = "Hello World",
		["Angle"] = Angle(10, 9, 8),
		["Vector"] = Vector(1, 2, 3),
	})

	local startTime = SysTime()
	for k=1, 100000 do 
		LuaThreaded.SetValue(k, "Some Random String")
	end
	local endTime = SysTime()
	print("Added 100.000 keys in " .. endTime - startTime .. "s")

	local startTime = SysTime()
	for k=1, 100000 do 
		LuaThreaded.SetValue(k, "A new String")
	end
	local endTime = SysTime()
	print("Updating 100.000 keys in " .. endTime - startTime .. "s")

	local startTime = SysTime()
	for k=1, 100000 do 
		LuaThreaded.SetValue(k, nil)
	end
	local endTime = SysTime()
	print("Removed 100.000 keys in " .. endTime - startTime .. "s")

	if LuaThreaded.GetValue(1000) != nil then
		error("LuaThreaded.SetValue failed! Investigate!")
	end
	
	iFace = LuaThreaded.CreateInterface()
	iFace:InitGmod()
	iFace:RunFile("includes/init.lua")
	iFace:RunString(code)
	iFace:RunHook("ExampleHook", "ExampleArg", 1234, true, Vector(1, 2, 3), Angle(4, 5, 6))
end)

if err then
	print("[ERROR] " .. err)
	file.Write("error.txt", err)
end

print("Testing Finished")