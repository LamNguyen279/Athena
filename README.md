Athesar Project
- Target: x86 simulation
- Compiler: GCC
- Debugger: GDB
- Purpose: Creating an environment for developing Autosar modules

Enviroment Setup
- Eclipse for Coding, Debugging, Compiling
- MinGw for GCC, GDB

Source Tree <br />
&emsp;ProjectRoot <br />
&emsp;| <br /> 
&emsp;|__Athesar <br />
&emsp;&emsp;&emsp;|<br />
&emsp;&emsp;&emsp;|__App: Contains Applications: Main.c ,... <br />
&emsp;&emsp;&emsp;| <br />
&emsp;&emsp;&emsp;|__Bsw: Contains BSW modules <br />
&emsp;&emsp;&emsp;| <br />
&emsp;&emsp;&emsp;|__Cfg: Contains Configuration files: *Cfg.c *Cfg.h <br />
&emsp;&emsp;&emsp;| <br />
&emsp;&emsp;&emsp;|__Glue: Glue layers for Simulate AUTOSR modules: virtual Os, Virtual TcpIp <br />
&emsp;&emsp;&emsp;| <br />
&emsp;&emsp;&emsp;|__Rte: Dummy RTE for Implementing the RTE Callback function <br />
