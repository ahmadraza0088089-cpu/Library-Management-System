# Library Management System (C++ — Single File)

Console-based Library Management System in **one `main.cpp` file**. All classes are defined in the file, and **all objects are created inside `main()`**.

## Classes & Objects

| Class | Object created in `main()` |
|-------|---------------------------|
| `Date` | `today` |
| `Library` | `library` |
| `Librarian` | `chiefLibrarian` |
| `Book` | `newBook`, `book1`–`book5` (when adding/searching) |
| `Member` | via `library.registerMember()` |
| `Transaction` | auto-created on book issue |

## OOP Concepts

- **Encapsulation** — private fields with getters/setters
- **Inheritance** — `Librarian` extends `Member`
- **Abstraction** — `borrowBook()`, `returnBook()`, `calculateFine()`
- **Polymorphism** — `getRole()` overridden in `Librarian`

## Build & Run

```powershell
cd library-management-system
g++ -std=c++14 -Wall -Wextra main.cpp -o lms.exe
.\lms.exe
```

Or with Make:

```bash
make
./lms
```

## Quick Start

1. Run the program
2. Press **5** to load sample data (creates Book, Member, Librarian objects)
3. Press **3 → 1** to issue a book
4. Press **4** to view reports

## File Structure

```
library-management-system/
├── main.cpp          ← Everything in one file
├── CMakeLists.txt
├── Makefile
└── README.md
```

Educational project — free to use and modify.
