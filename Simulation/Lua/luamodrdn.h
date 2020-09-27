
#pragma once

class LuaConfig;

// defined in luaRDNplayer.cpp

class LuaRDNPlayerLib
{
public:
	static void Initialize(LuaConfig*);
	static void Shutdown(LuaConfig*);
};

// defined in luaRDNworld.cpp

class LuaRDNWorldLib
{
public:
	static void Initialize(LuaConfig*);
	static void Shutdown(LuaConfig*);
};

// defined in luaRDNdbg.cpp

class LuaRDNDebugLib
{
public:
	static void Initialize(LuaConfig*);
	static void Shutdown(LuaConfig*);
};

class LuaModControllerLib
{
public:
	static void Initialize(LuaConfig*);
	static void Shutdown(LuaConfig*);
};

class LuaCpuAILib
{
public:
	static void Initialize(LuaConfig*);
	static void Shutdown(LuaConfig*);
};
