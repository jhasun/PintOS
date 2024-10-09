This is an iteration of the PintOS project, adopted from Stanford University.
This iteration of the project uses the Ubuntu 16.04 virtual machine (VMware workstation pro for Windows and UTM for MacOS).
The ISO image used is [here](https://releases.ubuntu.com/16.04/).

Due to the project being on an older version of Ubuntu, older libraries and dependencies are used. If you are downloading the
ISO image of Ubuntu 16.04 from the hyperlink above, most of the dependencies should already be installed. However, the versions
will be listed below.
<ul>
  <li>GCC: gcc 5.4.0</li>
  <li>Binutils: GNU ld 2.26.1</li>
  <li>Make: GNU make 4.1 (built for x86_64)</li>
  <li>Perl v5.22.1</li>
  <li>GDB: GNU gdb 7.11.1</li>
  <li>QEMU: qemu-system-i386 OR qemu-system-x86</li>
</ul>
=====================================================================

User Programs is the implementation of 13 system calls:
<ul>
  <li>SYS_HALT</li>
  <li>SYS_EXIT</li>
  <li>SYS_EXEC</li>
  <li>SYS_WAIT</li>
  <li>SYS_CREATE</li>
  <li>SYS_REMOVE</li>
  <li>SYS_OPEN</li>
  <li>SYS_FILESIZE</li>
  <li>SYS_READ</li>
  <li>SYS_WRITE</li>
  <li>SYS_SEEK</li>
  <li>SYS_TELL</li>
  <li>SYS_CLOSE</li>
</ul>

To properly implement these function calls, multiple files across the userprog and threads directories will have to be modified. Majority of the changes are located in the syscall.c file, where each system call is implemented, along with a system handler.
