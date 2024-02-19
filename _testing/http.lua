--[[
	tbl = {
		failed = function(reason) end,
		success = function(body) end,
		method = "GET POST HEAD PUT DELETE PATCH OPTIONS",
		url = "www.google.com",
		body = "",
		type = "text/plain; charset=utf-8",
		timeout = 60,
		headers = {
			["APIKey"] = "SaliBonani" 
		}
	}
]]

function CreateDir(name)
	if os.name() == "Windows" then
		os.execute('mkdir "' .. string.Replace(name, "/", [[\]]) .. '"')
	else
		os.execute('mkdir -p "' .. name .. '"')
	end
end

local last_added_request = os.time()
local requests = {}
function HTTP_WaitForAll()
	local i = 0
	for key, tbl in pairs(requests) do
		i = i + 1

		if FileExists(tbl.httpfile) and not (ReadFile(tbl.httpfile) == "") then
		    local success = tbl.handle:close()
		    if success then
		        if tbl.success then
		            tbl.success(ReadFile(tbl.httpfile))
		        end
		    else
		        if tbl.failed then
		            tbl.failed("Request failed: " .. success)
		        end
		    end

			requests[key] = nil
		end
	end

	if (os.time() - last_added_request) > 60 then
		error("HTTP Took way too long. Assuming something broke!")
	end

	return i > 0
end

local i = 0
function HTTP(tbl)
	i = i + 1

    local method = tbl.method or "GET"
    local url = tbl.url or ""
    local body = tbl.body or ""
    local contentType = tbl.type or ""
    local timeout = tbl.timeout or 60
    local headers = ""
    if tbl.headers then
        for key, value in pairs(tbl.headers) do
            headers = headers .. " -H '" .. key .. ": " .. value .. "'"
        end
    end
    tbl.httpfile = "http/" .. i .. ".txt"

    local curlCommand = "curl -sb -X " .. method .. " " .. url .. (not (contentType == "") and (" -H 'Content-Type: " .. contentType .. "'") or "") .. headers .. (body == "" and "" or ("--data-raw '" .. body .. "'")) .. " --max-time " .. timeout .. " > " .. tbl.httpfile
   	local handle = io.popen(curlCommand)
   	tbl.handle = handle

   	table.insert(requests, tbl)
   	last_added_request = os.time()
end

function JSONHTTP(tbl)
	local func = tbl.success
	tbl.success = function(body)
		func(json.decode(body))
	end
	tbl.headers = tbl.headers or {}
	tbl.headers["Accept"] = "application/json"

	HTTP(tbl)
end