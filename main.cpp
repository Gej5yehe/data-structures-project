#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
using namespace std;

// node for storing related pages as a singly linked list
struct related_Pages {
    int R_ID;
    string URL;
    string title;
    related_Pages* next;
};

// main page node - doubly linked list with a pointer to its related pages
struct Page {
    int P_ID;
    string URL;
    string title;
    time_t visitedAt;
    Page* prev, * next;
    related_Pages* rP_head;
};

// wrapper for the browsing history doubly linked list
struct pages_History {
    Page* head, * tail;
};

// bookmark node - also doubly linked, tracks visit stats
struct bookMarkNode {
    Page* page;
    bool favorite;
    int visitCount;
    time_t lastVisited;
    bookMarkNode* prev, * next;
};

// wrapper for the bookmark doubly linked list
struct bookMarkList {
    bookMarkNode* head, * tail;
};

// converts a date string like "21/10/2025 11:47" into a time_t value
time_t parseDateTime(const string& dateStr) {
    tm tmStruct = {};
    sscanf(dateStr.c_str(), "%d/%d/%d %d:%d",
        &tmStruct.tm_mday,
        &tmStruct.tm_mon,
        &tmStruct.tm_year,
        &tmStruct.tm_hour,
        &tmStruct.tm_min);

    tmStruct.tm_mon -= 1;      // months are 0-indexed in tm
    tmStruct.tm_year -= 1900;   // tm_year counts from 1900
    tmStruct.tm_sec = 0;

    return mktime(&tmStruct);
}

// quick helper to lowercase a string for case-insensitive comparisons
string toLowerCase(string s) {
    for (char& c : s) c = tolower(c);
    return s;
}

// formats a time_t back into our DD/MM/YYYY HH:MM format
string formatDateTime(time_t t) {
    char buf[40];
    tm* tmStruct = localtime(&t);
    strftime(buf, 40, "%d/%m/%Y %H:%M", tmStruct);
    return string(buf);
}

// reads the data file and populates history + bookmarks
// file format: '-' prefix for pages, '*' for related pages, '#' for bookmarks
bool parseFile(const string& fileName, pages_History& history, bookMarkList& bList) {
   
    ifstream file(fileName.c_str()); 
    string line;
    Page* currentPage = nullptr;

    if (!file.is_open()) {
        cout << "Error opening file." << endl;
        return false; 
    }

    while (getline(file, line)) {
        if (line.empty()) continue;

        char delimiter = line[0];
        string content = line.substr(1); 

        stringstream ss(content);
        string token;

        // page entry line
        if (delimiter == '-') {
            Page* newPage = new Page{};

            getline(ss, token, ',');
            newPage->P_ID = stoi(token);

            getline(ss, newPage->URL, ',');

            getline(ss, newPage->title, ',');
            
            // the rest of the line is the datetime
            string dateTimeStr;
            getline(ss, dateTimeStr);
            newPage->visitedAt = parseDateTime(dateTimeStr);
            
            // append to the end of the history list
            if (history.head == nullptr) {
                history.head = newPage;
                history.tail = newPage;
            } else {
                history.tail->next = newPage;
                newPage->prev = history.tail;
                history.tail = newPage;
            }
            
            currentPage = newPage;
        }

        // related page entry
        else if (delimiter == '*') {
            if (currentPage == nullptr) continue; 

            related_Pages* newRP = new related_Pages{};
            
            getline(ss, token, ',');
            newRP->R_ID = stoi(token);

            getline(ss, newRP->URL, ',');

            getline(ss, newRP->title);
            newRP->next = nullptr;
            
            // append to the current page's related list
            if (currentPage->rP_head == nullptr) {
                currentPage->rP_head = newRP;
            } else {
                related_Pages* rp_cur = currentPage->rP_head;
                while (rp_cur->next != nullptr) {
                    rp_cur = rp_cur->next;
                }
                rp_cur->next = newRP;
            }
        }

        // bookmark entry tied to the current page
        else if (delimiter == '#') {
            if (currentPage == nullptr) continue; 

            bookMarkNode* newBM = new bookMarkNode{};

            getline(ss, token, ',');
            newBM->favorite = (token == "yes");

            getline(ss, token, ',');
            newBM->visitCount = stoi(token);

            string dateTimeStr;
            getline(ss, dateTimeStr);
            newBM->lastVisited = parseDateTime(dateTimeStr);

            newBM->page = currentPage; 
            
            // append to bookmark list
            if (bList.head == nullptr) {
                bList.head = newBM;
                bList.tail = newBM;
            } else {
                bList.tail->next = newBM;
                newBM->prev = bList.tail;
                bList.tail = newBM;
            }
        }
    }
    
    file.close();
    return true; 
}

// adds a new page to the end of history with the current timestamp
void openPage(pages_History& history, Page pageData, related_Pages* relatedListHead) {
    Page* newPage = new Page;
 
    newPage->P_ID = pageData.P_ID;
    newPage->URL = pageData.URL;
    newPage->title = pageData.title;
    newPage->visitedAt = time(0);
 
    newPage->rP_head = relatedListHead;
 
    newPage->next = nullptr;
    newPage->prev = nullptr;
 
    if (history.head == nullptr) {
        history.head = newPage;
        history.tail = newPage;
    }   
    else {
        history.tail->next = newPage;
        newPage->prev = history.tail;
        history.tail = newPage;
    }
 
    cout << "Visited: " << newPage->title << endl;
}

// removes all pages visited before the cutoff date
void cleanupHistory(pages_History& history, time_t cutoffDate) {
    if (history.head == nullptr) {
        cout << "History is already empty." << endl;
        return;
    }

    int pagesDeleted = 0;
    Page* current = history.head;

    while (current != nullptr) {
        Page* nextNode = current->next;

        if (current->visitedAt < cutoffDate) {
            // handle head, tail, or middle node removal
            if (current == history.head) {
                history.head = current->next;
                if (history.head != nullptr) {
                    history.head->prev = nullptr;
                }
            }
            else if (current == history.tail) {
                history.tail = current->prev;
                if (history.tail != nullptr) {
                    history.tail->next = nullptr;
                }
            }
            else {
                current->prev->next = current->next;
                current->next->prev = current->prev;
            }

            if (history.head == nullptr) {
                history.tail = nullptr;
            }

            cout << "Deleted (Too Old): " << current->title << " (" << current->URL << ")" << endl;
            delete current;
            pagesDeleted++;
        }

        current = nextNode;
    }

    cout << "\nCleanup Complete. Total pages deleted: " << pagesDeleted << endl;
}

// finds the page that has the most related pages linked to it
Page* findMostRelatedPage(pages_History history) {
    Page* current = history.head;
    Page* mostRelatedPage = nullptr;
    int maxCount = -1;

    while (current != nullptr) {
        int currentCount = 0;
        related_Pages* relatedPtr = current->rP_head;

        // count how many related pages this page has
        while (relatedPtr != nullptr) {
            currentCount++;
            relatedPtr = relatedPtr->next;
        }

        if (currentCount > maxCount) {
            maxCount = currentCount;
            mostRelatedPage = current;
        }

        current = current->next;
    }

    return mostRelatedPage;
}

// deletes a page from history, cleans up its related pages,
// and removes any bookmarks pointing to it
void deletePage(pages_History& history, bookMarkList& bList, Page* page) {
    if (page == nullptr) return;

    // unlink the page from the doubly linked list
    if (page->prev)
        page->prev->next = page->next;
    else
        history.head = page->next;

    if (page->next)
        page->next->prev = page->prev;
    else
        history.tail = page->prev;

    // free the related pages linked list
    related_Pages* rp = page->rP_head;
    while (rp) {
        related_Pages* temp = rp;
        rp = rp->next;
        delete temp;
    }
    page->rP_head = nullptr;

    // remove any bookmarks that reference this page
    bookMarkNode* bm = bList.head;
    while (bm) {
        bookMarkNode* nextBM = bm->next;

        if (bm->page == page) {
            if (bm->prev)
                bm->prev->next = bm->next;
            else
                bList.head = bm->next;

            if (bm->next)
                bm->next->prev = bm->prev;
            else
                bList.tail = bm->prev;

            delete bm;
        }

        bm = nextBM;
    }

    delete page;
}

// merge sort for bookmarks - sorts by lastVisited descending (most recent first)
bookMarkNode* mergeSortOneBlock(bookMarkNode* head) {
    if (!head || !head->next) return head;

    // split the list in half using slow/fast pointer technique
    bookMarkNode* slow = head;
    bookMarkNode* fast = head->next;

    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }

    bookMarkNode* rightHead = slow->next;
    slow->next = nullptr;
    if (rightHead != nullptr) rightHead->prev = nullptr;

    bookMarkNode* left = mergeSortOneBlock(head);
    bookMarkNode* right = mergeSortOneBlock(rightHead);

    // merge using a dummy node to simplify the logic
    bookMarkNode dummy;
    dummy.next = nullptr;
    dummy.prev = nullptr;
    dummy.page = nullptr;
    dummy.visitCount = 0;
    dummy.lastVisited = 0;

    bookMarkNode* tail = &dummy;

    while (left != nullptr && right != nullptr) {
        if (left->lastVisited > right->lastVisited) {
            tail->next = left;
            left->prev = tail;
            left = left->next;
        }
        else {
            tail->next = right;
            right->prev = tail;
            right = right->next;
        }
        tail = tail->next;
    }

    // attach whatever's left
    if (left != nullptr) {
        tail->next = left;
        left->prev = tail;
    }
    if (right != nullptr) {
        tail->next = right;
        right->prev = tail;
    }

    bookMarkNode* sortedHead = dummy.next;
    if (sortedHead != nullptr) sortedHead->prev = nullptr;

    return sortedHead;
}

// sorts bookmarks by recency and prints the top N
void getRecentBookmarks(bookMarkList& bList, int N) {
    if (bList.head == nullptr) {
        cout << "No bookmarks to show." << endl;
        return;
    }

    // sort the list so most recent comes first
    bList.head = mergeSortOneBlock(bList.head);

    // update the tail pointer after sorting
    bookMarkNode* temp = bList.head;
    while (temp->next != nullptr) temp = temp->next;
    bList.tail = temp;

    cout << "--- Top " << N << " Most Recent Bookmarks ---" << endl;
    bookMarkNode* current = bList.head;
    int count = 0;

    while (current != nullptr && count < N) {
        char* dt = ctime(&current->lastVisited);
        string timeStr = (dt) ? dt : "Unknown Time";
        if (!timeStr.empty() && timeStr.back() == '\n') timeStr.pop_back();

        cout << count + 1 << ". " << current->page->title
            << " (" << timeStr << ")" << endl;

        current = current->next;
        count++;
    }
}

// searches history for pages whose URL contains the given substring
void searchPagesByURL(pages_History& history, const string& substring) {
    if (history.head == nullptr) {
        cout << "History is empty.\n";
        return;
    }

    string sub = toLowerCase(substring);
    bool found = false;

    Page* cur = history.head;

    cout << "\n=== Search Results for \"" << substring << "\" ===\n";

    while (cur) {
        string urlLower = toLowerCase(cur->URL);

        if (urlLower.find(sub) != string::npos) {
            found = true;

            char buf[40];
            tm* t = localtime(&cur->visitedAt);
            strftime(buf, 40, "%d/%m/%Y %H:%M", t);

            cout << "\nPage ID: " << cur->P_ID
                << "\nURL: " << cur->URL
                << "\nTitle: " << cur->title
                << "\nVisited At: " << buf
                << "\n-------------------------\n";
        }

        cur = cur->next;
    }

    if (!found) {
        cout << "No pages found containing \"" << substring << "\".\n";
    }
}

// walks through bookmarks and returns the one with the lowest visit count
bookMarkNode* findLeastVisited(bookMarkList& bList) {
    if (bList.head == nullptr) return nullptr;

    bookMarkNode* current = bList.head;
    bookMarkNode* minNode = bList.head;
    int minCount = bList.head->visitCount;

    while (current != nullptr) {
        if (current->visitCount < minCount) {
            minCount = current->visitCount;
            minNode = current;
        }
        current = current->next;
    }
    return minNode;
}

// finds and removes the bookmark with the fewest visits
void removeLeastVisited(bookMarkList& bList) {
    bookMarkNode* nodeToDelete = findLeastVisited(bList);

    if (nodeToDelete == nullptr) {
        cout << "The bookmark list is empty. Nothing to prune." << endl;
        return;
    }

    if (nodeToDelete == bList.head) {
        bList.head = nodeToDelete->next;
        if (bList.head != nullptr) bList.head->prev = nullptr;
    }
    else if (nodeToDelete == bList.tail) {
        bList.tail = nodeToDelete->prev;
        if (bList.tail != nullptr) bList.tail->next = nullptr;
    }
    else {
        nodeToDelete->prev->next = nodeToDelete->next;
        nodeToDelete->next->prev = nodeToDelete->prev;
    }

    if (bList.head == nullptr) bList.tail = nullptr;

    delete nodeToDelete;

    cout << "Pruning successful: Least visited bookmark has been removed." << endl;
}

// writes the current state back to a file in the same format we read it
void writeToFile(const string& filename, pages_History& history, bookMarkList& bList) {
    ofstream out(filename);
    if (!out.is_open()) {
        cout << "Error opening output file.\n";
        return;
    }

    Page* p = history.head;

    while (p) {
        out << "-" << p->P_ID << ", "
            << p->URL << ", "
            << p->title << ", "
            << formatDateTime(p->visitedAt) << "\n";

        // write related pages for this page
        related_Pages* rp = p->rP_head;
        while (rp) {
            out << "*" << rp->R_ID << ", "
                << rp->URL << ", "
                << rp->title << "\n";
            rp = rp->next;
        }

        // write bookmark info if this page is bookmarked
        bookMarkNode* bm = bList.head;
        while (bm) {
            if (bm->page == p) {
                out << "#"
                    << (bm->favorite ? "yes" : "no")
                    << ", " << bm->visitCount
                    << ", " << formatDateTime(bm->lastVisited)
                    << "\n";
            }
            bm = bm->next;
        }

        p = p->next;
    }

    out.close();
    cout << "File saved successfully to: " << filename << "\n";
}



int main() {
    pages_History history = { nullptr, nullptr };
    bookMarkList bList = { nullptr, nullptr };

    int choice;
    string filename;

    while (true) {
        cout << "\n===== Browser History & Bookmark Manager =====\n";
        cout << "1. Load history & bookmarks from file\n";
        cout << "2. Open a new page\n";
        cout << "3. Cleanup history before a date\n";
        cout << "4. Find most related page\n";
        cout << "5. Delete a page by ID\n";
        cout << "6. Show most recent bookmarks\n";
        cout << "7. Search pages by URL\n";
        cout << "8. Remove least visited bookmark\n";
        cout << "9. Save everything to file\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // flush newline

        if (choice == 0) break;

        switch (choice) {
        case 1: {
            cout << "Enter filename to load: ";
            getline(cin, filename);
            if (parseFile(filename, history, bList)) {
                cout << "File loaded successfully.\n";
            }
            break;
        }
        case 2: {
            Page p;
            cout << "Enter Page ID: "; cin >> p.P_ID; cin.ignore();
            cout << "Enter URL: "; getline(cin, p.URL);
            cout << "Enter Title: "; getline(cin, p.title);

            int nRelated;
            cout << "Number of related pages: "; cin >> nRelated; cin.ignore();
            related_Pages* rHead = nullptr;
            related_Pages* rTail = nullptr;
            for (int i = 0; i < nRelated; i++) {
                related_Pages* r = new related_Pages;
                cout << "Enter related page ID: "; cin >> r->R_ID; cin.ignore();
                cout << "Enter related URL: "; getline(cin, r->URL);
                cout << "Enter related Title: "; getline(cin, r->title);
                r->next = nullptr;
                if (!rHead) rHead = rTail = r;
                else { rTail->next = r; rTail = r; }
            }

            openPage(history, p, rHead);
            break;
        }

        case 3: {
            string dateStr;
            cout << "Enter cutoff date (DD/MM/YYYY HH:MM): ";
            getline(cin, dateStr);
            time_t cutoff = parseDateTime(dateStr);
            cleanupHistory(history, cutoff);
            break;
        }

        case 4: {
            Page* mostRelated = findMostRelatedPage(history);
            if (mostRelated) {
                cout << "Most related page: " << mostRelated->title << " (" << mostRelated->URL << ")\n";
            }
            else {
                cout << "No pages found.\n";
            }
            break;
        }

        case 5: {
            int id;
            cout << "Enter Page ID to delete: "; cin >> id; cin.ignore();
            Page* p = history.head;
            while (p && p->P_ID != id) p = p->next;
            if (p) deletePage(history, bList, p);
            else cout << "Page not found.\n";
            break;
        }

        case 6: {
            int N;
            cout << "Enter number of recent bookmarks to show: ";
            cin >> N; cin.ignore();
            getRecentBookmarks(bList, N);
            break;
        }

        case 7: {
            string sub;
            cout << "Enter URL substring to search: ";
            getline(cin, sub);
            searchPagesByURL(history, sub);
            break;
        }

        case 8: {
            removeLeastVisited(bList);
            break;
        }

        case 9: {
            cout << "Enter filename to save: ";
            getline(cin, filename);
            writeToFile(filename, history, bList);
            break;
        }

        default:
            cout << "Invalid choice. Try again.\n";
        }
    }

    cout << "Exiting program...\n";

    // free everything before we quit
    while (history.head) deletePage(history, bList, history.head);
    while (bList.head) removeLeastVisited(bList);

    return 0;
}
