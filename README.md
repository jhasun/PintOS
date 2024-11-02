# ECE 468 PintOS Project
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

## Table of Contents
* [Dev and Testing Guide](#dev-and-testing-guide)
* [Threads Overview](#threads-overview)
* [Modifications](#modifications)
* [Results](#results)

<hr>
## Dev and Testing Guide
To run the tests regarding the threads section of the PintOS project, first download the .zip file from this repository.
Once you have the .xip file, unzip the folder in your Ubuntu Virtual Machine (keep in mind that this version of the PintOS 
project uses the libraries listed above, so any major changes in testing environment may affect PintOS). Open terminal in 
Ubuntu, and use the command cd to go into the threads directory, where you use the make command. There should be a new 
directory called "build", cd into it and run make check. This will allow you to see what tests are passing.

## Threads Overview
<hr>

## Modifications
<hr>

## Results
