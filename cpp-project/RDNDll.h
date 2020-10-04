/////////////////////////////////////////////////////////////////////
// File    : DllInterface.h
// Desc    :
// Created : November 17, 2000
// Author  : Drew
//
// (c) 2001 Relic Entertainment Inc.
//

// * exported interface for MOD dlls

#pragma once

/////////////////////////////////////////////////////////////////////
//

void RDNDllInterfaceInitialize();
void RDNDllInterfaceShutdown();

const char *RDNDLLVersion();