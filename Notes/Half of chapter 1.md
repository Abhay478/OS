# Operating Systems - I

## Introduction

### Components of a computer
- User - can be person, machine or other computer
- Application
- OS
- Hardware

## OS objectives
- Users: Ease of use, performance
  - Don't care about resource ut.
- Shared sys manages resource ut.
  - #### Operating System is a resource allocator and control program that makes efficient use of hardware and manages execution of user programs.
- Workstations have dedicated resources but often ut. shared res. from servers.
- Mobile devices are resource-poor, opt. for usability and efficiency.
- Embedded sys run without user intervention, no consideratioin for usability.
- No universally accepted definition.
- #### Kernel : One program running at all times.
- Everything else is either:
  - System program: Part of OS, but not kernel
  - Application program: Not associated with OS.
- #### Middleware : Set of frameworks that provide additional services to application developers, i.e. databases.

### System org
- 1+ CPU's, device controllers(disk, graphics adapter, USB), connect through common bus providing acces to shared memory.
- Concurrent execution of all devices and CPU's competeing for memory cycles.
- IO and CPU conc.

### controller
- Each controller controls specific device, has local buffer, and a os device driver. 
- Has its own registers
- Moves data from peripheral devices to local buffer storage
- synchronises memory access

### Interrupts
- Device controller informing driver that operation had been completed.
- Transfers control to the interrupt service routine(function, basically), through interrupt vectors which contains the addreses of all service routines(lot like a hash table).
- Must save address of interrupted instruction.
- trap/exception software-geneerated interrupt caused by error or user request.
- os is interrupt driven
- maskable - low priority.

#### IO Interrupt timeline
IO request -> transfer done, load into buffer -> interrupt -> handled -> ...

### Storage structure
- Registers
- Cache
- Main memory
  - Random access
  - volatile
  - DRAM
- Secondary storage
  - rigid metal/glass platters covered with magnetic recorrding stuff.
  - divided into tracks, then sectors.
  - disk controller determines logical interaction between device and system.
- Non-volatile memory (SSD)
### IO structure
- Direct memory access (DMA) : blockwise transfer by controller to memory without cpu intervention.

### System Architecture
- Single processor
- Multicore
- Multiprocessor
  - aka parallel/slightly coupled
  - Advantages
    - Increased throughput
    - economy of scale
    - Inc. reliability - graceful degradation/fault tolerance : If one processor fails, system still functions.
  - Types
    - Asymmetric 
    - Symmetric

#### Non-uniform memory access : 