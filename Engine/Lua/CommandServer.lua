module(..., package.seeall)

if (outputDebugString ~= nil) then
    outputDebugString("Dbg Lua: Initializing CommandServer\n")
end

require "socket"
require "peuuid"
require "HierarchyBuilder"

g_tcp_client = nil;
g_tcp_server = nil;
g_keep_connection = 0

function createTCPServer( host, port, backlog )
    -- outputDebugString("Dbg Lua:  CommandServer.createTCPServer()\n")
    local err, res = 0, 0
    
    local numTries = 0
    res = nil
    while res == nil do
	
		g_tcp_server, err = socket.tcp(); --potentially recreate tcp object since unsuccessful bind will close the scoket
		if g_tcp_server==nil then 
			outputDebugString("Dbg Lua:  CommandServer.createTCPServer() : fatal error : g_tcp_server, err = socket.tcp();"..tostring(err).."\n")
			return nil, err;
		end
		
		g_tcp_server:setoption("reuseaddr", false);
		--g_tcp_server:setoption("exclusiveaddr", true);
		res = 1
		---[[
		res, err = g_tcp_server:bind(host, port);
        if res==nil then
			outputDebugString("Dbg Lua:  CommandServer.createTCPServer() : error : local res, err = g_tcp_server:bind(host, port); "..tostring(err).."\n")
		end
		
        
		if err == "address already in use" then
			outputDebugString("Dbg Lua:  CommandServer.createTCPServer() : bumping up port\n")
			port = port + 1
		else
            res, err = g_tcp_server:listen(backlog);
            if res==nil then
                outputDebugString("Dbg Lua:  CommandServer.createTCPServer() : error : res, err = g_tcp_server:listen(backlog); :"..tostring(err).."\n")
                
                return nil, err;
            end
            g_tcp_server:settimeout(0.001)
          
			---[[
            if err ==nil then
                --create a temp test connection to see if socket can actually accept
                temps = socket.tcp()
                temps:settimeout(0.25)
                g_tcp_server:settimeout(0.25)
            
            
                res, err = temps:connect("127.0.0.1", port)
                if err ~= nil then
                    outputDebugString("Error creating temp connection as expected: " .. tostring(err) .. "\n")
                end
                res, err = g_tcp_server:accept();
                if res ~= nil then
                    outputDebugString("Dbg Lua:  Temp test client connected. This confirms the socket is listening. Closing temp sockets..\n")
                    temps:close()
                    res:close()
                    g_tcp_server:settimeout(0.001)
            
                else
                    outputDebugString("Dbg Lua:  ERROR: Temp test client could not connect to the socket. This is bad (we shouldnt be able to successfuly bind socket but not be able to get connection). Lets try a different port..\n")
                    g_tcp_server:close()
                    temps:close()
                    port = port + 1
                end
            end
			--]]
			
        end
        
		numTries = numTries + 1
        if numTries >= 10 then
			outputDebugString("Dbg Lua:  CommandServer.createTCPServer() : Cancelling. Too many failures\n")
			return nil, err;
		end

		--]]
    end

    
    
    outputDebugString("Dbg Lua:  CommandServer.createTCPServer(): Created at "..tostring(host)..":"..tostring(port).." object:"..tostring(g_tcp_server).."\n")
	return port
end

function runTCPServer()
    
    outputDebugString("PE: LuaServer: runTCPServer() Entry..\n")
        
    local stop_server = 0;
    local num_cnx = 0;

    g_tcp_server:settimeout(0.001);
    --outputDebugString("Tiemout set\n")
            
    while( stop_server==0 ) do

        local err = "";
        -- Set a timeout of 10 ms for accept().
        --
        --outputDebugString("PE: LuaServer: Running Iteration.\n")
        --print("Server" .. tostring(g_tcp_server))
        
        --reads,writes, err = socket.select({ [1] = g_tcp_server }, {}, 0.001)
        --outputDebugString("socket.select(): ".." num reads: "..tostring(#reads).." num writes: "..tostring(#writes).."\n")
        if g_keep_connection == 0 then
			--outputDebugString("Executing g_tcp_server:accept();\n")
            g_tcp_client, err = g_tcp_server:accept();
			--outputDebugString("g_tcp_server:accept(): ".."client: "..tostring(g_tcp_client).." error: "..tostring(err).."\n")
        end
        
        if g_tcp_client~=nil and g_keep_connection == 0 then
            outputDebugString('Accepted new client \n')
            g_client_msg = ""
			line = nil
			while (line == nil) do
				line, err = g_tcp_client:receive('*l');
				outputDebugString("PE: PROGRESS: first line:" .. tostring(line) .. " err: " .. tostring(err).."\n")
				if line == nil then
					outputDebugString("PE: PROGRESS: nil first line received. trying again..\n")
				end
            end
			numLines = tonumber(line) 
            outputDebugString("receving "..numLines.." lines\n")
            while numLines > 0.1 do
                outputDebugString("PE: PORGRESS: LUA: CommandServer: receiving a line")
                
				line, err = g_tcp_client:receive('*l');
				if line ~= nil then
					if #line < 200 then
						outputDebugString("PE: PORGRESS: LUA: CommandServer: "..line.."\n")
					else
						outputDebugString("PE: PORGRESS: LUA: CommandServer: line is too long\n")
					end
					g_client_msg = g_client_msg.."\n"..line
			        numLines = numLines - 1
				else
					outputDebugString("Received nil line.. trying again..\n")
				end
                            end
            if #g_client_msg < 250 then
                outputDebugString('Recevied message: \n' .. g_client_msg .. '\nEnd of Message\n')
            else
                outputDebugString("Received message. Too long to print")
            end
            f = loadstring(g_client_msg);
            if (f ~= nil) then
                outputDebugString('Executing command from PyClient\n')
                
                f()
            else
                outputDebugString('Error while compiling command from PyClient\n')
            end

            if( g_client_msg~=nil ) then
                if(g_client_msg=="STOP") then
                    stop_server = 1;
                else
                    -- g_tcp_client:send( "Demoniak3D TCP Server Echo: " .. g_client_msg);
                end
            end

            if g_keep_connection == 0 then
                if g_tcp_client~=nil then
                    g_tcp_client:close();
                end
            end
        end
        -- Yield execution.
        coroutine.yield(1);
    end
end

function lines(str)
	local t = {}
	local function helper(line) table.insert(t, line) return "" end
	helper((str:gsub("(.-)\r?\n", helper)))
	return t
end

function sendToPyClient(res)
	outputDebugString("sendToPyClient() :Entry. g_tcp_client: "..tostring(g_tcp_client).."\n")
	
	l = lines(res)
	outputDebugString("Number fo lines: "..#l.."\n")
	
	g_tcp_client:send(#l..'\n')
	for il = 1, #l do
		bytesSent, err, lastByte = g_tcp_client:send(l[il]..'\n')
		if bytesSent ~= nil then
			outputDebugString(tostring(bytesSent).." bytes sent of "..#l[il].."\n")
		else
			outputDebugString("Only "..tostring(lastByte).." bytes sent of "..#l[il].."\n")
		end
	end
end

function sendToPyClientTcp(res, tcp)
	outputDebugString("sendToPyClient() :Entry..\n")
	
	l = lines(res)
	outputDebugString("Number fo lines: "..#l.."\n")
	
	tcp:send(#l..'\n')
	for il = 1, #l do
		bytesSent, err, lastByte = tcp:send(l[il]..'\n')
		if bytesSent ~= nil then
			outputDebugString(tostring(bytesSent).." bytes sent of "..#l[il].."\n")
		else
			outputDebugString("Only "..tostring(lastByte).." bytes sent of "..#l[il].."\n")
		end
	end
end


-- misc commands that could be put in separate files but are here for now

function memoryReport()
    outputDebugString("memoryReport() : Calling C function..\n")
    s = root.PE.Components.LuaEnvironment.l_MemoryReport()
    outputDebugString("memoryReport() : Got Reply.\n")
    CommandServer.sendToPyClient(s..'\n')
end

function loadSpeShader(t)
    outputDebugString("PE LUA_COMPONENT: loadSpeShader(): serverIP: "..t.serverIP.." port: "..t.serverPort.." filename: "..t.filename.." tecnique: "..t.techName.."\n")
    local tcp, err = socket.tcp();
    --g_tcp_client = tcp
    if tcp ~= nil then
        outputDebugString("PE LUA_COMPONENT: Created TCP socket\n")
        tcp:settimeout(0.1);
        local res,  err = tcp:connect(t.serverIP, tonumber(t.serverPort))
        if res == 1 then
            outputDebugString("PE LUA_COMPONENT: loadSpeShader() : Connected to PS3 SPE Shader Server\n")
        
            -- send data
            res = "{"
            res = res.."'filename':'"..t.filename.."',"
            res = res.."'techName':'"..t.techName.."',"
            res = res.."'text':'"..t.text.."',"
            res = res.."}\n"
            
            CommandServer.sendToPyClientTcp(res, tcp)
            
            tcp:close()
            outputDebugString("PE LUA_COMPONENT: loadSpeShader() : Closed connection to PS3 SPE Shader Server\n")
        else
            outputDebugString("PE LUA_COMPONENT: loadSpeShader() : Could NOT connect to PS3 SPE Shader Server\n")
        end
    else
        outputDebugString("PE LUA_COMPONENT: loadSpeShader() : Could NOT create TCP socket\n")
    end
    
    
end

