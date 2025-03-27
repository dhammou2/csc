Custom SC.EXE (CSC) - Windows Service Utility

This is a C++ implementation of the sc.exe Windows service management tool. It allows you to create, configure, and manage Windows services from the command line.


## Usage:

1. Download the master ZIP file and extract all contents.
2. Open a command prompt and navigate to the folder containing the executable (csc.exe).


## Available Commands and Options

1. Create a service:

```C++

csc.exe create ServiceName binpath= "C:\Path\To\Your\Service.exe
```

2. Start a service:

```C++
csc.exe start ServiceName
```
3. Stop a servce:

```C++
csc.exe stop ServiceName
```
4. Delete a service:

```C++
csc.exe delete ServiceName
```

5. Query a service:

```C++
csc.exe query ServiceName
```
6. Query the description of a service:

```C++
csc.exe qdescription ServiceName
```

7. Configure a service:

```C++
csc.exe config ServiceName OPTION= VALUE
``` 
    Available options and their values:

         * start= (auto, demand, disabled, boot, system)
         * error= (normal, severe, critical, ignore)
         * binPath=
         * password= 
         * description= 

8. Configure failure options:

```C++
csc.exe failure ServiceName option= VALUE
``` 
    Available values:

         * reset
         * reboot 
