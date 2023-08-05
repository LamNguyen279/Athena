# Athesar Project

Information:
	- Target: x86 simulation
	- Compiler: GCC
	- Debugger: GDB
	- Purpose: Creating Enviroment for develop Autosar modules

Enviroment Setup:
- Esclipse for Coding, Debugging, Compiling
- Cygwin for GCC, GDB

Source Tree:
	ProjectRoot
	|
	|__Athesar
		|__App: Contains Applications: Main.c, ....
		|
		|__Bsw: Contains BSW modules
		|
		|__Cfg: Contains Configuration files: *Cfg.c *Cfg.h
		|
		|__Glue: Glue layers for Simulate AUTOSR modules: virtual Os, Virtual TcpIp
		|
		|__Rte: Dummy RTE for Implement the RTE Callback function