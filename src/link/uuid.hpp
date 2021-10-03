#pragma once

#pragma comment(lib, "rpcrt4.lib")
#include <windows.h>

#include <string>

namespace link
{
    std::string uuid_generate()
    {
        UUID uuid;
        UuidCreate(&uuid);
        char* cstr;
        UuidToStringA(&uuid, (RPC_CSTR*)&cstr);
        std::string str(cstr);
        RpcStringFreeA((RPC_CSTR*)&cstr);
        return str;
    }
}
