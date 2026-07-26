# Browser History & Bookmark Manager

A C++ console application that simulates a browser's history and bookmark system using linked lists.

## Features

- Load/save browsing history and bookmarks from a text file
- Open new pages and track visit timestamps
- Search history by URL substring
- Cleanup old history entries before a given date
- Find the page with the most related pages
- Delete pages by ID (also removes associated bookmarks)
- Sort and display the N most recent bookmarks (merge sort)
- Remove the least visited bookmark

## Data Structures Used

- **Doubly linked list** for browsing history (`pages_History`)
- **Doubly linked list** for bookmarks (`bookMarkList`)
- **Singly linked list** for related pages attached to each page

## How to Compile & Run

```bash
g++ -o main main.cpp
./main
```

Or on Windows with MSVC:
```
cl main.cpp
main.exe
```

## File Format

The program reads/writes a specific text format:

```
-PageID, URL, Title, DD/MM/YYYY HH:MM
*RelatedID, URL, Title
#yes/no, visitCount, DD/MM/YYYY HH:MM
```

- Lines starting with `-` define a page entry
- Lines starting with `*` define related pages (linked to the page above)
- Lines starting with `#` define bookmark info (linked to the page above)

Check `sample_data.txt` for an example.
