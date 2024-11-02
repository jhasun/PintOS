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
This section of the PintOS project is meant to educate students on synchronization and scheduling problems. A minimally
functional thread system is initially given, with the objective of the assignment being to extend the functionality of
the system to include three major improvements:
<ul>
  <li>Alarm Clock</li>
  <li>Priority Scheduling</li>
  <li>Multi Level Feedback Queue Scheduler</li>
</ul>

The alarm clock is already implemented in the system, however it needs to be reimplemented such that it avoids busy waiting,
that is, it spins in a loop checking the current time and calling thread_yield() until enough time has gone by.

Priority scheduling is the implementation of giving threads priority, where threads with higher priorities are allowed
access to the processor and lower priority threads yield. This also takes into account threads that are waiting for a lock,
 semaphore, or condition variable. Threads and locks should also be able to donate priority to other threads and locks, respectively.

The multi-level feedback queue scheduler follows in the footsteps of the priority scheduler and reduces the average 
response time for running jobs on the system. It should choose the thread to run based on priorities, however it does not 
do priority donation. Thread priority is now dynamically determined by the scheduler using "nice" values alongside
a "recent_cpu" value. A thread's "nice" value determines how nice a thread is to other threads, with higher values meaning 
that the thread's priority decreases. The "recent_cpu" value determines how much cpu time each process has recieved 
recently. Another value, "load_avg" should also be considered since it estimates the average number of threads ready to run
over the past minute.

Below are the key formulas needed:
```
priority = PRI_MAX - (recent_cpu / 4) - (nice * 2)
```
```
recent_cpu = (2 * load_avg) / (2 * load_avg + 1) * recent_cpu + nice
```
```
load_avg = (59/60) * load_avg + (1/60) * ready_threads
```
<hr>

## Modifications
**fixed_point.h:**

**timer.c and timer.h:**

**synch.c and synch.h:**

**thread.c and thread.h:**

<hr>

## Results
