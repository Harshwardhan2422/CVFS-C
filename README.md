Custom Virtual File System (CVFS) in C

📌 Description

This project is a Custom Virtual File System (CVFS) implemented using core C programming concepts.
It simulates how a file system works internally by managing files, inodes, and memory.

«⚠️ Note: The source file uses ".cpp" extension, but the implementation is purely in C style (no C++ features used).»

---

⚙️ Features

- Create file
- Open file
- Read file
- Write file
- Delete file (rm)
- Truncate file
- List all files (ls)
- Display file information (stat)
- Close file / Close all files

---

🧠 Concepts Used

- Inode structure
- File Table (UFDT)
- Superblock
- DILB (Disk Inode List Block)
- Memory management ("malloc", "free")
- File descriptors
- Command line interface
- String handling ("strcmp", "strcpy", etc.)

---

🧩 Internal Architecture

🔹 Superblock

- Stores total and free number of inodes
- Helps in efficient resource management
- Updated whenever files are created or deleted

🔹 DILB (Disk Inode List Block)

- Linked list of all inodes
- Each inode represents a file
- Stores metadata like:
  - File name
  - Size
  - Type
  - Link count
  - Reference count

🔹 UFDT (User File Descriptor Table)

- Maintains all active file descriptors
- Each entry points to a file table

🔹 File Table

- Stores read/write offsets
- Maintains file access mode
- Links file descriptor to inode

---

▶️ How to Run

g++ CVFS.cpp -o cvfs
./cvfs

---

💻 Sample Commands

create file1 3
open file1 3
write file1 hello
read file1 5
ls
stat file1
close file1
rm file1

---

🖥️ Sample Output

========================================
     Custom Virtual File System (CVFS)
========================================
Type 'help' to see available commands
========================================

CVFS: > create file1 3
File created successfully with FD : 0

CVFS: > write file1 hello
Bytes Written : 5

CVFS: > read file1 5
Data : hello

---

📁 Project Structure

CVFS.cpp
README.md

---

📍 Author

Harsh Borale

---

🚀 Future Improvements

- Add directory structure
- Implement hierarchical file system
- Add data persistence (save to disk)
- Improve permission handling
