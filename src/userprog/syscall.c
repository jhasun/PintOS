//////////////////////////////////////////////////////////////////////////////////////
///         University of Hawaii - College of Engineering
///                      Joshua Asuncion
///                     ECE 468 Fall 2024
///
///                     Pintos Progect B
/// 
//
///
///@brief:
///@see:  https://casys-kaist.github.io/pintos-kaist/project2/system_call.html   
//////////////////////////////////////////////////////////////////////////////////////
#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "devices/input.h"
#include "devices/shutdown.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/malloc.h"
//#include <sys/types.h>
#include "lib/kernel/list.h"

#define SYS_HALT     0  // System Call 0 HALT
#define SYS_EXIT     1  // System Call 1 EXIT
#define SYS_EXEC     2  // System Call 2 EXEC
#define SYS_WAIT     3  // System Call 3 WAIT
#define SYS_CREATE   4  // System Call 4 CREATE
#define SYS_REMOVE   5  // System Call 5 REMOVE
#define SYS_OPEN     6  // System Call 6 OPEN
#define SYS_FILESIZE 7  // System Call 7 FILESIZE
#define SYS_READ     8  // System Call 8 READ
#define SYS_WRITE    9  // System Call 9 WRITE
#define SYS_SEEK     10 // System Call 10 SEEK
#define SYS_TELL     11 // System Call 11 TELL
#define SYS_CLOSE    12 // System Call 12 CLOSE

static void syscall_handler (struct intr_frame *);
bool verify_ptr(const void *ptr);

/// @brief  void syscall_init(void):  Initializes the system call handler. This function 
///                                   should be called during the kernel initialization 
///                                   to set up system call handling.
///
/// @param void
void syscall_init (void) {
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


/// @brief bool verify_pointer(): Verifies if a given pointer is a valid user address
/// @param ptr 
/// @return 
bool verify_ptr(const void *ptr){
  
  // Check if the pointer is not within the user address space or NULL.
  if(ptr == NULL || !is_user_vaddr(ptr)){
    syscall_exit(-1); // Terminate the process if point is invalid
    return false;
  }

  // Check if the pointer is within the kernel address space. BIG NO, NO!
  if(is_kernel_vaddr(ptr)){
    syscall_exit(-1); // Terminate the process if point is invalid
    return false;
  }

  //struct thread* currentThread = thread_current(); // Get current thread

  // Check if address is a pointer to unmapped virtual memory. 
  // Use #ifdef for pagefir thread field
  #ifdef USERPROG
    struct thread* currentThread = thread_current();
    if (pagedir_get_page(currentThread->pagedir, ptr) == NULL) {
        syscall_exit(-1); // Terminate the process if pointer is unmapped
        return false;
    }
  #endif
    
  return true;
}


/// @brief int syscall_handler(): Handles system calls. 
///
///@details UNUSED is a macro or annotation used in C/C++ to indicate that a 
///         function parameter or variable is intentionally unused.
///         It prevents compiler warnings about unused parameters when a 
///         parameter is present in the function signature but isn't used in 
///         the body of the function.
///
/// @param UNUSED 
static void syscall_handler (struct intr_frame *f UNUSED){

  int syscall_number = *(int *)f->esp; // Retrieving the system call number from the stack

  // Switching based on the system call numbe
  switch (syscall_number) { 

  // CASE HALT: 
  case SYS_HALT: 
    halt();
    break;
    

    
  // CASE SYS_EXIT: 
  case SYS_EXIT: {

    // Validate the pointer to the argument (status)
    if (!verify_ptr(f->esp + 4)) {
      syscall_exit(-1);
      return;
    }

    int status = *(int *)(f->esp + 4);  // Retrieving the status code from the stack
    syscall_exit(status);                       // Exit the process with the given status code
    break;
  }


  // CASE SYS_EXEC
  case SYS_EXEC: {

  // Validate the pointer to the command line argument
   if (!verify_ptr(f->esp + 4)) {
      syscall_exit(-1);
      return;
    }

    const char *cmd_line = (const char *)(*(uint32_t *)(f->esp + 4));  // Retrieving the command line from the stack
    
    // Validate the pointer to the string (command line)
    if (!verify_ptr(cmd_line)) {
      syscall_exit(-1);
      return;
    }

    f->eax = exec(cmd_line);                                           // Execute the command and store the result in eax
    break;
  }


  // CASE SYS_WAIT
  case SYS_WAIT: {

  // Verify the pointer to the argument (PID)
  if (!verify_ptr(f->esp + 4)) {
    syscall_exit(-1);
    return;
  }
    int pid = *(int *)(f->esp + 4);   // Retrieving the process ID from the stack
    f->eax = wait(pid);               // Wait for the process to finish and store the result in eax
    break;
  }


  // CASE SYS_CREATE
  case SYS_CREATE: {
   
   // Validate the pointers to arguments (file name and initial size)
    if (!verify_ptr(f->esp + 4) || !verify_ptr((const char *)(*(uint32_t *)(f->esp + 4)))) {
      syscall_exit(-1);
      return;
    }

    const char *filename = (const char *)(*(uint32_t *)(f->esp +  4));   // Retrieving the filename from the stack
    uint32_t initial_size = *(uint32_t *)(f->esp + 8);                   // Retrieving the initial size from the stack
    f->eax = create(filename, initial_size);                             // Create a new file and store the result in eax
    break;
 }



   // CASE SYS_READ
  case SYS_READ: {

     // Validate the pointers to arguments (fd, buffer, and size)
    if (!verify_ptr(f->esp + 4) || !verify_ptr(f->esp + 8) || !verify_ptr(f->esp + 12)) {
      syscall_exit(-1);
      return;
    }

    int fd = *(int *)(f->esp + 4);                              // Retrieving the file descriptor from the stack
    uint8_t *buffer = (uint8_t *)(*(uint32_t *)(f->esp + 8));   // Retrieving the buffer from the stack
    uint32_t size = *(uint32_t *)(f->esp + 12);                 // Retrieving the size from the stack
    
    // Validate the buffer pointer (because user programs can pass any address)
    if (!verify_ptr(buffer)) {
      syscall_exit(-1);
      return;
    }

    f->eax = read(fd, buffer, size);                            // Read from the file and store the result in eax
    break;
  }



  // CASE SYS_WRITE
  case SYS_WRITE: {

    // Validate the pointers to arguments (fd, buffer, and size)
    if (!verify_ptr(f->esp + 4) || !verify_ptr(f->esp + 8) || !verify_ptr(f->esp + 12)) {
      syscall_exit(-1);
      return;
    }

    int fd = *(int *)(f->esp + 4);                               // Retrieving the file descriptor from the stack
    uint8_t *buffer = (uint8_t *)(*(uint32_t *)(f->esp + 8));    // Retrieving the buffer from the stack
    uint32_t size = *(uint32_t *)(f->esp + 12);                  // Retrieving the size from the stack
    
    // Validate the buffer pointer
    if (!verify_ptr(buffer)) {
      syscall_exit(-1);
      return;
    }
    
    f->eax = write(fd, buffer, size);                            // Write to the file and store the result in eax
    break;
  }



  // CASE SYS_REMOVE
  case SYS_REMOVE:{
    // Validate the pointer to arguments filename
    if (!verify_ptr(f->esp + 4) || !verify_ptr((const char *)(*(uint32_t *)(f->esp + 4)))) {
        syscall_exit(-1);
        return;
    }

    const char *filename = (const char *)(*(uint32_t *)(f->esp + 4));    // Retrieving the filename from the stack
    f->eax = remove(filename);                                           // Remove the file and store the result in eax
    break;

  }



  // CASE SYS_OPEN
  case SYS_OPEN:{

  // Validate pointer to the file name argument
    if (!verify_ptr(f->esp + 4) || !verify_ptr((const char *)(*(uint32_t *)(f->esp + 4)))) {
        syscall_exit(-1);
        return;
    }

    const char *filename = (const char *)(*(uint32_t *)(f->esp + 4));    // Retrieving the filename from the stack
    f->eax = open(filename);                                             // Open the file and store the result in eax
    break;
  }


  // CASE SYS_FILENAME
  case SYS_FILESIZE: {
    // Validate pointer to the file name argument
    if (!verify_ptr(f->esp + 4) || !verify_ptr((const char *)(*(uint32_t *)(f->esp + 4)))) {
        syscall_exit(-1);
        return;
    }

    int fd = *(int *)(f->esp + 4);    // Retrieving the file descriptor from the stack
    f->eax = filesize(fd);            // Get the file size and store the result in eax
    break;
  }


  // CASE SYS_SEEK
  case SYS_SEEK: {

    // Validate pointers to the file descriptor and position arguments
    if (!verify_ptr(f->esp + 4) || !verify_ptr(f->esp + 8)) {
        syscall_exit(-1);
        return;
    }

    int fd = *(int *)(f->esp + 4);                               // Retrieving the file descriptor from the stack
    uint32_t position = *(uint32_t *)(f->esp + 8);               // Retrieving the position from the stack
    seek(fd, position);                                          // Seek to the position in the file
    break;
  }
    


  // SYS_TELL  
  case SYS_TELL: {

    // Validate pointer to the file descriptor argument
    if (!verify_ptr(f->esp + 4)) {
        syscall_exit(-1);
        return;
    }

    int fd = *(int *)(f->esp + 4);                                // Retrieving the file descriptor from the stack
    f->eax = tell(fd);                                            // Get the current position in the file and store the result in eax
    break;
  }



  // SYS_CLOSE  
  case SYS_CLOSE: {

    // Validate the pointer to the file descriptor argument
    if (!verify_ptr(f->esp + 4)) {
        syscall_exit(-1);
        return;
    }

    int fd = *(int *)(f->esp + 4);                                // Retrieving the file descriptor from the stack
    close(fd);                                                    // Close the file                         
    break;
  }
 
  }

}


/// @brief void halt:  Shuts down Pintos by calling power_off().
///
/// @param  void
void halt (void){
  shutdown_power_off();
}


///@brief void exit():  Terminates the current user program, returning status to the kernel. 
///                     If the process's parent waits for it (see below), this is the status 
///                     that will be returned. Conventionally, a status of 0 indicates success 
///                     and nonzero values indicate errors.
///
///@details Modified struct thread in thread.h to include field elem 'exit_status'
///@param status
void syscall_exit (int status){

  struct thread *currentThread = thread_current();        // get current thread
  currentThread->exit_status = status;                    // store the status of the thread in exit_status

  if (currentThread->parent_waiting) {                    //  Check if parent is waiting for the thread
        sema_up(&currentThread->wait_sema);               // Signal parent that the child has exited
  }                         

  printf("%s: exit(%d)\n", currentThread->name, status);  // print the status of the thread
  thread_exit();                                          // exit the thread
}


/// @brief pid_t exec():  Change current process to the executable whose name is given in cmd_line, 
///                       passing any given arguments. This never returns if successful. Otherwise 
///                       the process terminates with exit state -1, if the program cannot load or 
///                       run for any reason. This function does not change the name of the thread 
///                       that called exec. Please note that file descriptors remain open across an 
///                       exec call.
///
///@details In Pintos, the lock_release(&filesys_lock) is used to release a lock on the file system.
///         The Pintos file system is not inherently thread-safe. To ensure safe access when multiple
///         threads attempt to read or write to files simultaneously, a global lock, such as 
///         filesys_lock, is used to prevent race conditions. `filesystem_lock` is a global variable
///         declared in syscall.h
///
/// @param cmd_line 
/// @return 
pid_t exec (const char *cmd_line){
    lock_acquire(&filesystem_lock);

  char* cmd_ptr;                                    // pointer to the command line
  char* file_name;                                  // pointer to the file name
  file_name = (char *) malloc(strlen(cmd_line)+1);  // allocate memory for the file name
  
  strlcpy(file_name, cmd_line, strlen(cmd_line)+1); // copy the file name to the allocated memory
  
  file_name = strtok_r(file_name, " ", &cmd_ptr);   // tokenize command line into file name and arguments
  if (file_name == NULL) {                          // Check if file name is NULL
    free(file_name);
    lock_release(&filesystem_lock);
    return -1;  
  }
  
  struct file* file = filesys_open(file_name);      // open the file
  if(file == NULL){                                 // Check if the file is not found
    free(file_name);                                // free the allocated memory
    lock_release(&filesystem_lock);                 // release the lock
    return -1;
  }
 
  file_close(file);                // Close the file after opening 

  free(file_name);                 // Free the allocated memory after the file is opened

  lock_release(&filesystem_lock);  // Release the lock before calling process_execute

  // Start a new process with the command line argument 'cmd_line'
  pid_t pid = process_execute(cmd_line);
  if (pid == TID_ERROR) {
      return -1;  
  }

  return pid;  // Return the PID of the new process
  
  }


/// @brief wait(): Causes the current process to wait for the process identified by pid to 
///                finish execution and return its exit status. If the pid is invalid or 
///                not a child of the calling process, it returns -1.
///
/// @param pid 
/// @todo Implement process_wait correctly
int wait (pid_t pid){
   return process_wait(pid);
}

/// @brief  bool create():  Creates a new file called `file` initially initial_size byte is size. 
///                         If successful, returns true, otherwise returns false.  
///
/// @param file 
/// @param initial_size 
/// @return true  if file created, false otherwise
bool create (const char *file, unsigned initial_size) {
  // Check file name is valid 
  if (file == NULL || *file == '\0') {
      return false;  
  }
    
  lock_acquire(&filesystem_lock);                    // Lock filesystem before interacting with the file system
  bool success = filesys_create(file, initial_size); // Create a new file using pintos file creation

  if (!success) {
      lock_release(&filesystem_lock);  
      return false;                 // Return false if the file creation failed
  }

  // Release the lock after successfully creating the file
  lock_release(&filesystem_lock);

  return true;  // Return true if the file was created successfully
      
}


/// @brief bool remove():  Removes a file called `file`
/// @param file 
/// @return true if file removed, false otherwise
bool remove (const char *file){

  // Check if file name is valid
  if(file == NULL || *file == '\0'){ // Check if file name is valid
    return false;
  }

  lock_acquire(&filesystem_lock);            // Lock filesystem before interacting with the file system
  bool success = filesys_remove(file);   // Remove the file from the filesystem using pintos file removal

  // Check if file removal was sucessessful
  if(!success){             
    lock_release(&filesystem_lock);  
    return false;                           // Return false if the file removal failed
  }

  // Release the lock after successfully removing the file
  lock_release(&filesystem_lock);  
  return success;
}


/// @brief int open():  Opens the file called file. Returns a nonnegative integer handle called a 
///                     "file descriptor" (fd), or -1 if the file could not be opened. File descriptors 
///                     numbered 0 and 1 are reserved for the console: fd 0 (STDIN_FILENO) is standard 
///                     input, fd 1 (STDOUT_FILENO) is standard output. The open system call will never 
///                     return either of these file descriptors, which are valid as system call arguments 
///                     only as explicitly described below. Each process has an independent set of file 
///                     descriptors. File descriptors are inherited by child processes. When a single file 
///                     is opened more than once, whether by a single process or different processes, each 
///                     open returns a new file descriptor. Different file descriptors for a single file 
///                     are closed independently in separate calls to close and they do not share a file 
///                     position. You should follow the linux scheme, which returns integer starting from
///                     zero, to do the extra.
///
/// @param file 
/// @return file_descriptor 
int open (const char *file){

  // Check if file name is valid
  if(file == NULL || *file == '\0'){ 
    return -1;
  }

  lock_acquire(&filesystem_lock);        // Lock filesystem before 
  
  struct file *f = filesys_open(file);   // Open the file using pintos file open
  
  // Check if file open was successful
  if(f == NULL){
    lock_release(&filesystem_lock);
    return -1;
  }

  struct thread *currentThread = thread_current();  // Get the current thread
  int file_descriptor = currentThread->next_fd;     // Get the next available file descriptor

  // Check if the file descriptor is valid
  if(file_descriptor >= 128){                        
    file_close(f);                                    
    lock_release(&filesystem_lock);             
    return -1;
  }

  currentThread->fd_table[file_descriptor] = f;     // Store the file in the file descriptor table
  currentThread->next_fd++;                         // Increment the next available file descriptor

  lock_release(&filesystem_lock);                   // Release the lock

  return file_descriptor;                           // Return the file descriptor

}


/// @brief int filesize(): Returns the size, in bytes, of the file open as fd.
///
/// @param fd 
/// @return 
int filesize (int fd){
  struct thread *currentThread = thread_current();   // Get current thread
  
  // Check if fd is valid
  if (fd < 2 || fd >= currentThread->next_fd || currentThread->fd_table[fd] == NULL) {  
      return -1;  
  }
  lock_acquire(&filesystem_lock);                   // Lock filesystem before 

  struct file *file = currentThread->fd_table[fd];  // Retrieve the file associated with fd
  
  lock_release(&filesystem_lock);                   // Release the lock

  return file_length(file);                         // Return the file's size in bytes

  }


/// @brief int read():  Read size bytes from the file open as fd into buffer. Returns the 
///                     number of bytes actually read (0 indicates end of file), or -1 if 
///                     the file could not be read (due to a condition other than end of 
///                     file). fd 0 reads from the keyboard using intput_getc.  
///
/// @param fd 
/// @param buffer 
/// @param size 
/// @return 
int read (int fd, void *buffer, unsigned size){
  struct thread *currentThread = thread_current();  // Get current thread

  // Check if fd is valid
  if (fd < 2 || fd >= currentThread->next_fd || currentThread->fd_table[fd] == NULL) {  
      return -1;  
  }
  
  lock_acquire(&filesystem_lock);                   // Acquire the lock
  
  // If fd == 0, read from keyboard
  if(fd == 0){
    for(unsigned i = 0; i < size; i++){
      ((char *)buffer)[i] = input_getc();            // Read a character from the keyboard
    }
      return size;
  }

  
  struct file *file = currentThread->fd_table[fd];  // Retrieve the file associated with fd
  int bytes_read = file_read(file, buffer, size);   // Read from the file and return the number of bytes read
  if (bytes_read < 0) {                             // EOF reached, read of 0 bytes
      return -1;  
  }

  lock_release(&filesystem_lock);                   // Release the lock

  return bytes_read;  // Return the number of bytes successfully read

}


/// @brief int write(): Writes size bytes from buffer to the open file fd. Returns the number of 
///                     bytes actually written, which may be less than size if some bytes could 
///                     not be written. Writing past end-of-file would normally extend the file, 
///                     but file growth is not implemented by the basic file system. The expected 
///                     behavior is to write as many bytes as possible up to end-of-file and return 
///                     the actual number written, or 0 if no bytes could be written at all. fd 1 
///                     writes to the console. Your code to write to the console should write all 
///                     of buffer in one call to putbuf(), at least as long as size is not bigger 
///                     than a few hundred bytes (It is reasonable to break up larger buffers). 
///                     Otherwise, lines of text output by different processes may end up 
///                     interleaved on the console, confusing both human readers and our grading 
///                     scripts.
///
/// @param fd 
/// @param buffer 
/// @param size 
/// @return 
int write (int fd, const void *buffer, unsigned size){

  struct thread  *currentThread = thread_current();  // Get current thread

  // Check if fd is valid
  if (fd < 0 || fd >= currentThread->next_fd || currentThread->fd_table[fd] == NULL) {  
      return -1;  
  }

  lock_acquire(&filesystem_lock);              // Acquire the lock to prevent concurrent access

  if(fd == 1 && size <= 300){                 //Check if fd ==1 && buffer is <= 300 btyes , write to console
      putbuf((char *)buffer, size);           // Write to the console           
      return size;
  }

  struct file *file = currentThread->fd_table[fd];      // Retrieve the file associated with fd
  int bytes_written = file_write (file, buffer, size);  // Write to the file, returns bytes_written
  
  if(bytes_written < 0){                               // Check if bytes were written successfully
    return -1;
  }

  lock_release(&filesystem_lock);                      // Release the lock

  return bytes_written;                                // Return the number of bytes written

}


/// @brief void seek(): Changes the next byte to be read or written in open file fd to position,
///                     expressed in bytes from the beginning of the file (Thus, a position of 0 
///                     is the file's start). A seek past the current end of a file is not an 
///                     error. A later read obtains 0 bytes, indicating end of file. A later 
///                     write extends the file, filling any unwritten gap with zeros. (However, 
///                     in Pintos files have a fixed length until project 4 is complete, so writes 
///                     past end of file will return an error.) These semantics are implemented in 
///                     the file system and do not require any special effort in system call 
///                     implementation.
///
/// @param fd 
/// @param position 
void seek (int fd, unsigned position){
  struct thread *currentThread = thread_current();  // Get current thread

  // Check if fd is valid
  if (fd < 0 || fd >= currentThread->next_fd || currentThread->fd_table[fd] == NULL) {  
      return;  
  }

  lock_acquire(&filesystem_lock);                        // Acquire the lock to prevent concurrent access

  struct file *file = currentThread->fd_table[fd];       // Retrieve the file associated with fd

  lock_release(&filesystem_lock);                        // Release the lock

  file_seek (file, position);                            // Seek to the specified position in the file

}


/// @brief unsigned tell(): Returns the position of the next byte to be read or written in open file fd, 
///                         expressed in bytes from the beginning of the file.
/// @param fd 
/// @return 
unsigned tell (int fd){
  struct thread *currentThread = thread_current();  // Get current thread

  // Check if fd is valid
  if (fd < 0 || fd >= currentThread->next_fd || currentThread->fd_table[fd] == NULL){
    return (unsigned) -1;
  }

  lock_acquire(&filesystem_lock);                   // Acquire the lock to prevent concurrent access

  struct  file *file = currentThread->fd_table[fd]; // Retrieve the file associated with fd
  
  off_t position = file_tell(file);                 // Get the current position in the file
  
  lock_release(&filesystem_lock);                   // Release the lock

  return position;                                  // Return the current position in the file (the next byte to be read or written to)

}


/// @brief void close(): Closes file descriptor fd. Exiting or terminating a process implicitly 
///                      closes all its open file descriptors, as if by calling this function 
///                      for each one.
/// @param fd 
void close (int fd){
  struct thread *currentThread = thread_current();  // Get current thread
  
  // Check if fd is valid
  if (fd < 0 || fd >= currentThread->next_fd || currentThread->fd_table[fd] == NULL){
    return;
  }

  lock_acquire(&filesystem_lock);                   // Acquire the lock to prevent concurrent access

  struct file *file =  currentThread->fd_table[fd]; // Retrieve the file associated with fd

  file_close (file);

  lock_release(&filesystem_lock);                    // Release the lock

  currentThread->fd_table[fd] = NULL;                // Clear the file descriptor table entry
}
