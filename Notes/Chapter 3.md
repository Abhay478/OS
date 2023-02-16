# Processes

## Components
- argc/argv, environment variables
- Program counter, process regs
- Stack : Local variables and return addresses.
- Heap : Dynamic variables
- Data : global variables
  - uninit
  - init
- Text section : Instructions

`

    % size -m /System/Applications/Preview.app/Contents/MacOS/Preview

    Segment __PAGEZERO: 4294967296 (zero fill) 

    Segment __TEXT: 2097152

        Section __text: 1642792
        Section __auth_stubs: 10288
        Section __objc_methlist: 76156
        Section __const: 5376
        Section __gcc_except_tab: 9096
        Section __objc_methname: 199138
        Section __objc_classname: 10197
        Section __objc_methtype: 35457
        Section __cstring: 72281
        Section __oslogstring: 530
        Section __dlopen_cstrs: 108
        Section __ustring: 232
        Section __unwind_info: 19352
        total 2081003
    Segment __DATA_CONST: 98304
        Section __auth_got: 5144
        Section __got: 3168
        Section __auth_ptr: 56
        Section __const: 12376
        Section __cfstring: 65600
        Section __objc_classlist: 2440
        Section __objc_catlist: 328
        Section __objc_protolist: 664
        Section __objc_imageinfo: 8
        total 89784
    Segment __DATA: 360448
        Section __objc_const: 194792
        Section __objc_selrefs: 47184
        Section __objc_protorefs: 112
        Section __objc_classrefs: 3280
        Section __objc_superrefs: 1912
        Section __objc_ivar: 5988
        Section __objc_data: 24400
        Section __data: 8808
        Section __objc_intobj: 624
        Section __objc_doubleobj: 48
        Section __objc_arraydata: 304
        Section __objc_arrayobj: 288
        Section __objc_dictobj: 40
        Section __bss: 1016 (zerofill)
        Section __common: 65544 (zerofill)
        total 354340
    Segment __LINKEDIT: 163840
    total 4297687040
`

## Process states
- New : Being created.
- Running : Instructions are being executed.
- Waiting: The process is waiting for some event to occur (such as an I/O completion or reception of a signal).
- Ready: The process is waiting to be assigned to a processor. Usually due to interrupt(timer interrupt, error, etc.).

## PCB
### Components
- Process state. The state may be new, ready, running, waiting, halted, and so on.
- Program counter : The counter indicates the address of the next instruction to be executed for this process.
- CPU registers : The registers vary in number and type, depending on the computer
  architecture. They include accumulators, index registers, stack pointers, and general-purpose registers, plus any condition-code information. Along with the program counter, this state information must be saved when an interrupt occurs, to allow the process to be continued correctly afterward when it is rescheduled to run.
- CPU-scheduling information : This information includes a process priority, pointers
  to scheduling queues, and any other scheduling parameters. (Chapter 5 describes process scheduling.)
- Memory-management information : This information may include such items as the value
  of the base and limit registers and the page tables, or the segment tables, depending on the memory system used by the operating system (Chapter 9).
- Accounting information : This information includes the amount of CPU and real time
  used, time limits, account numbers, job or process numbers, and so on.
- I/O status information : This information includes the list of I/O devices allocated
  to the process, a list of open files, and so on.

## Scheduling
- ready queue
- wait queue
  - child termination wait queue
  - interrupt wait queue  
### Context switch : When processor switches from one process to another.
 Overhead time arises due to context switching (saving into/reading from PCB).