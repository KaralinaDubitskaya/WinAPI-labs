# Windows API
Operating Systems and System Programming (Part 2), BSUIR 2018.

## Laboratory work #1
__1. The event-based architecture of Windows applications                                                                              
2. The message processing mechanism                                                                                                     
3. The window redrawing mechanism__

The program allows you to move a shape (rectangle, ellipse or butterfly) using the keyboard and mouse sprite within the working area of the window.

Scrolling the mouse wheel moves the sprite vertically, scrolling with the shift key pressed down moves it horizontally.

Choose the shape by _Draw_ menu item.  
                                                                                                          
![Lab #1 screenshot](https://github.com/KaralinaDubitskaya/WinAPI-labs/blob/master/screenshots/lab1.png)

## Laboratory work #2
__1. Text output                                                                                                       
2. Working with fonts__

The program writes a scrollable text table into the window. 

Click *File → Open* to choose a text file to display. After that enter the number of columns in the table. Each line of the file is written into a cell of the table. 

The width of the columns is the same. The number of rows is calculated automatically. The height of the rows of the table is chosen in such a way to accommodate the text of each cell. When resizing the window, the table is recalculated and redrawn.
                                         
![Lab #2 screenshot](https://github.com/KaralinaDubitskaya/WinAPI-labs/blob/master/screenshots/lab2.JPG)

## Laboratory work #3
__1. Creation of dynamically loaded libraries (DLLs)                                                                    
2. Load-time dynamic linking                                                                                            
3. Run-time dynamic linking                                                                                              
4. DLL Injection__

*StringProcessing.dll* contains a function *ReplaeStringInVirtualMemory*, which replaces all occurrences of the given string with a new one located in the virtual memory of the specified process.
```
BOOL WINAPI ReplaceStringInVirtualMemory(const char *oldStr, const char *newStr, DWORD pid)
```
Project *StaticImport* performs a static import of the DLL and uses its functions.

Project *DynamicImport* implements run-time dynamic import of the DDL via *LoadLibrary* function from *Kernel32.dll*.

Project *DLLInjection* injects the DLL into a given process using a remote thread and calls the DLL's functions.

## Laboratory work #4
__Thread synchronization__

*TaskQueue* implements a queue of tasks in which several threads can insert elements atomically. *ThreadPool* is a handler for this queue, which extracts the task from it and distributes the specified number of threads.

The program uses it to sort lines of the specified text file. The input stream reads the file into memory, after that creates several tasks for sorting words of the text (by the number of sorting threads) and enqueue them. Thereafter, all sorted parts of the text are merged together and saved into the specified file.

The number of threads, input and output files are entered by the user.

## Contact 

Karalina Dubitskaya                                                                                                
dubitskaya.karalina@gmail.com

BSUIR 2018


