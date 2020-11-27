#pragma once

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

template <typename T>
inline void SAFE_DELETE(T& p) { if (p != nullptr) { delete p; p = nullptr; } }

template <typename T>
inline void SAFE_DELETE_ARRAY(T& p) { if (p != nullptr) { delete[] p; p = nullptr; } }

#define GETTER(varType, parameter, memberVar) \
const varType Get##parameter() const \
{ \
    return memberVar; \
}

#define SETTER(varType, parameter, memberVar) \
void Set##parameter(varType parameter) \
{ \
    memberVar = parameter; \
}

#define GETTER_SETTER(varType, parameter, memberVar) \
const varType Get##parameter() const \
{ \
    return memberVar; \
} \
void Set##parameter(varType parameter) \
{ \
    memberVar = parameter; \
}
