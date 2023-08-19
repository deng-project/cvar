# CVar systems library

This library is meant to support so-called CVars in DENG. It also supports (un)serialization to JSON, Yaml and XML.

## Building

Install required pre-requisites.  

For Arch based distros:  
```
$ sudo pacman -S base-devel cmake
```

For Debian based distros:
```
$ sudo apt install build-essential cmake
```

Next build the project
```
$ git clone https://git.dengproject.org/deng/cvar
$ cd cvar
$ mkdir Build
$ cd Build
$ cmake ..
$ make -j9000
```

## Basic usage

Manipulating and accessing console variables can be done by using `CVarSystem` API.  
Setting variables is pretty straight forward:
```c++
CVar::CVarSystem& cvarSyst = CVar::CVarSystem::GetInstance();
cvarSyst.Set<CVar::String>("myVariable.myObject.myString", "Hello world!");
cvarSyst.Set<CVar::Bool>("myVariable.myObject.myBool", false);
cvarSyst.Set<CVar::Int>("myVariable.myObject.myInt", 420);
cvarSyst.Set<CVar::Float>("myVariable.myObject.myFloat", 6.9);
cvarSyst.Set<CVar::List>("myVariable.myObject.myList", {1, 2.01f, false, "Hello"});
```

For accessing variables, however, there are two ways it can be done. The first way is to use 
`CVarSystem::Get<T>()`. This method expects the variable to have specified data type T.  
From the previous example:  
```c++
CVar::String* pString = cvarSyst.Get<CVar::String>("myVariable.myObject.myString");
if (pString)
    std::cout << *pString << std::endl;
```
would output you `Hello world`. Note that the returned data type is a pointer to the variable, which
might be nullptr if the variable didn't exist or had a wrong data type.

