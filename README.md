<div align="center">

  <h1>🌐 Browser History & Bookmark Manager</h1>

  <p><b>A C++ Data Structures Project for Managing Web Browsing History & Dynamic Bookmarks</b></p>

  <p>
    <a href="https://github.com/Gej5yehe/data-structures-project/stargazers"><img src="https://img.shields.io/github/stars/Gej5yehe/data-structures-project?style=for-the-badge&color=007ACC" alt="Stars"></a>
    <a href="https://github.com/Gej5yehe/data-structures-project/issues"><img src="https://img.shields.io/github/issues/Gej5yehe/data-structures-project?style=for-the-badge&color=007ACC" alt="Issues"></a>
    <a href="https://github.com/Gej5yehe/data-structures-project/network/members"><img src="https://img.shields.io/github/forks/Gej5yehe/data-structures-project?style=for-the-badge&color=007ACC" alt="Forks"></a>
  </p>

  <p>
    <img src="https://img.shields.io/badge/Language-C++11-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++">
    <img src="https://img.shields.io/badge/Data%20Structures-Linked%20Lists-FF6F00?style=for-the-badge" alt="Data Structures">
    <img src="https://img.shields.io/badge/Algorithms-Merge%20Sort-4CAF50?style=for-the-badge" alt="Algorithms">
    <img src="https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-4A154B?style=for-the-badge" alt="Platform">
  </p>

  ---
</div>

## 📌 Overview

**Browser History & Bookmark Manager** is a lightweight, efficient C++ application engineered to simulate core browser mechanics including tab history navigation, related page indexing, and smart bookmark management. Built entirely using custom node-based pointers and low-level memory handling without relying on standard container libraries (`std::vector`, `std::list`).

---

## ✨ Key Features

<table>
  <tr>
    <td width="50%">
      <h3>📜 Browsing History</h3>
      <ul>
        <li><b>Navigation tracking:</b> Log page URLs, titles, and timestamps dynamically.</li>
        <li><b>URL Substring Search:</b> Case-insensitive fast text matching across history nodes.</li>
        <li><b>Automated Pruning:</b> Clean up old browsing records before a target cutoff date.</li>
        <li><b>Related Pages Index:</b> Sub-list attached to history nodes for quick related link discovery.</li>
      </ul>
    </td>
    <td width="50%">
      <h3>🔖 Smart Bookmarks</h3>
      <ul>
        <li><b>Dynamic Sorting:</b> Implements optimized custom <b>Merge Sort</b> for $O(N \log N)$ sorting by last visit recency.</li>
        <li><b>Least-Visited Pruning:</b> Automatically identifies and removes cold/unused bookmarks.</li>
        <li><b>Cascade Deletion:</b> Deleting a history entry automatically cleans up associated bookmarks.</li>
        <li><b>File Persistence:</b> Full serialisation and loading support via formatted text storage.</li>
      </ul>
    </td>
  </tr>
</table>

---

## 🛠️ Data Architecture

The engine uses three interlinked pointer-based data structures:

```
[pages_History] (Doubly Linked List)
       │
       ▼
 ┌───────────┐      next      ┌───────────┐
 │   Page    ├───────────────►│   Page    │
 │ (P_ID: 1) │◄───────────────┤ (P_ID: 2) │
 └─────┬─────┘      prev      └─────┬─────┘
       │                            │
       ├─► [related_Pages]          └─► [bookMarkNode]
       │   (Singly Linked List)         (Doubly Linked List)
       │   ├── R_ID: 101                 ├── Favorite: Yes
       │   └── R_ID: 102                 └── VisitCount: 5
```

| Structure | Type | Purpose |
| :--- | :--- | :--- |
| `pages_History` | **Doubly Linked List** | Bi-directional navigation across session browsing history |
| `bookMarkList` | **Doubly Linked List** | Quick insertion/deletion for saved user bookmarks |
| `related_Pages` | **Singly Linked List** | Sub-nodes storing contextual related page references per tab |

---

## 🚀 Quick Start

### 1️⃣ Prerequisites
Ensure you have a C++ compiler installed (`g++`, `clang++`, or MSVC `cl`).

### 2️⃣ Compilation

Using **GCC/G++**:
```bash
g++ -O2 -std=c++11 main.cpp -o browser_manager
```

Using **MSVC** (Developer Command Prompt):
```cmd
cl /O2 /EHsc main.cpp /Fe:browser_manager.exe
```

### 3️⃣ Execution

```bash
# Run executable
./browser_manager
```

---

## 💾 File Format Specification

The manager imports and exports session state using a structured plain-text format (`sample_data.txt`):

```text
-102, https://youtube.com, YouTube, 21/10/2025 11:47
*601, https://youtube.com/@NVIDIA, NVIDIA Channel
#yes, 5, 28/10/2025 21:33
```

- `-` : Represents a main **Page Entry** (`P_ID, URL, Title, Timestamp`)
- `*` : Represents a **Related Page Entry** linked to the preceding main page (`R_ID, URL, Title`)
- `#` : Represents **Bookmark Metadata** for the preceding page (`IsFavorite, VisitCount, LastVisited`)

---

## 💻 Sample Terminal Interface

<details>
<summary><b>Click to expand Console Menu options</b></summary>

```text
===== Browser History & Bookmark Manager =====
1. Load history & bookmarks from file
2. Open a new page
3. Cleanup history before a date
4. Find most related page
5. Delete a page by ID
6. Show most recent bookmarks
7. Search pages by URL
8. Remove least visited bookmark
9. Save everything to file
0. Exit
Enter your choice:
```
</details>

---

<div align="center">
  <sub>Designed & Developed for Data Structures & Algorithms</sub>
</div>
