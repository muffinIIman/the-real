#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// Struct definitions for MemoryBlock and Process
struct MemoryBlock {
    int id;
    int size;
    bool allocated;
};

struct Process {
    int id;
    int size;
};

class MemoryAllocator {
private:
    vector<MemoryBlock> memory;
    int nextBlockId;

public:
    MemoryAllocator(int size) : nextBlockId(1) {
        memory.push_back({0, size, false}); // Initialize the memory with a single block
    }

    // Best Fit Allocation
    void allocateBestFit(Process process) {
        auto bestFit = memory.end();
        int minFragmentation = INT_MAX;

        for (auto it = memory.begin(); it != memory.end(); ++it) {
            if (!it->allocated && it->size >= process.size && it->size - process.size < minFragmentation) {
                bestFit = it;
                minFragmentation = it->size - process.size;
            }
        }

        if (bestFit != memory.end()) {
            if (bestFit->size > process.size) {
                // Split the block
                memory.insert(bestFit + 1, {nextBlockId++, bestFit->size - process.size, false});
                bestFit->size = process.size;
            }
            bestFit->allocated = true;
            bestFit->id = process.id;
            cout << "Process " << process.id << " allocated to block " << bestFit->id << endl;
        } else {
            cout << "Process " << process.id << " cannot be allocated" << endl;
        }
    }

    void deallocate(int processId) {
        for (auto& block : memory) {
            if (block.id == processId && block.allocated) {
                block.allocated = false;
                cout << "Process " << processId << " deallocated from block " << block.id << endl;
                mergeFreeBlocks();
                return;
            }
        }
        cout << "Process " << processId << " not found" << endl;
    }

    void displayMemory() const {
        cout << "+-------+------+---------+" << endl;
        cout << "| Block | Size | Allocated |" << endl;
        cout << "+-------+------+---------+" << endl;
        for (const auto& block : memory) {
            cout << "| " << setw(5) << block.id << " | " << setw(4) << block.size << " | " << (block.allocated ? "Yes" : "No ") << "      |" << endl;
        }
        cout << "+-------+------+---------+" << endl;
    }

private:
    void mergeFreeBlocks() {
        for (auto it = memory.begin(); it != memory.end(); ++it) {
            if (!it->allocated) {
                auto next = it + 1;
                while (next != memory.end() && !next->allocated) {
                    it->size += next->size;
                    next = memory.erase(next);
                }
            }
        }
    };

    void printMemory() {
        for (const auto& block : memory) {
            cout << "Block " << block.id << " Size: " << block.size << " Allocated: " << (block.allocated ? "Yes" : "No") << endl;
        }
    }
};
// Function to check if a page is already in memory
bool isInMemory(const vector<int>& frames, int page) {
return find(frames.begin(), frames.end(), page) != frames.end();
}

// Function to simulate FIFO page replacement algorithm
int FIFO(const vector<int>& pages, int frameCount) {
vector<int> frames(frameCount, -1); // Initialize frames with -1
int faultCount = 0;
int currentIndex = 0;

vector<vector<int>> table; // Table to store the page table
table.push_back({-1, -1, -1, '0'}); // Header row

for (int page : pages) {
if (!isInMemory(frames, page)) {
frames[currentIndex] = page;
faultCount++;
currentIndex = (currentIndex + 1) % frameCount;

// Add row to the table
vector<int> row = {page};
for (int frame : frames) {
if (frame == -1)
row.push_back('-');
else
row.push_back(frame);
}
row.push_back('F');
table.push_back(row);
} else {
// Add row to the table indicating no page fault
vector<int> row = {page};
for (int frame : frames) {
if (frame == -1)
row.push_back('-');
else
row.push_back(frame);
}
row.push_back('-');
table.push_back(row);
}
}

// Print the table
for (const auto& row : table) {
for (int cell : row) {
if (cell == -1 || cell == 'F')
cout << setw(9) << (char)cell;
else
cout << setw(9) << cell;
cout << "|";
}
cout << endl;
for (size_t i = 0; i < row.size(); ++i) {
cout << "---------+";
}
cout << endl;
}

return faultCount;
}

// Function to simulate LRU page replacement algorithm
int LRU(const vector<int>& pages, int frameCount) {
vector<int> frames(frameCount, -1); // Initialize frames with -1
unordered_map<int, int> pageLastUsed; // Map to store the last used index of each page
int faultCount = 0;
int currentIndex = 0;

vector<vector<int>> table; // Table to store the page table
table.push_back({-1, -1, -1, '0'}); // Header row

for (int page : pages) {
if (!isInMemory(frames, page)) {
// Find the page in frames that was least recently used
int lruIndex = 0;
for (int i = 1; i < frameCount; ++i) {
if (pageLastUsed[frames[i]] < pageLastUsed[frames[lruIndex]]) {
lruIndex = i;
}
}

// Replace the least recently used page with the new page
frames[lruIndex] = page;
faultCount++;

// Update last used index of the new page
pageLastUsed[page] = currentIndex;

// Update last used index of replaced page
pageLastUsed[frames[lruIndex]] = currentIndex;

// Add row to the table
vector<int> row = {page};
for (int frame : frames) {
if (frame == -1)
row.push_back('-');
else
row.push_back(frame);
}
row.push_back('F');
table.push_back(row);
} else {
// Update last used index of the existing page
pageLastUsed[page] = currentIndex;

// Add row to the table indicating no page fault
vector<int> row = {page};
for (int frame : frames) {
if (frame == -1)
row.push_back('-');
else
row.push_back(frame);
}
row.push_back('-');
table.push_back(row);
}
currentIndex++;
}

// Print the table
for (const auto& row : table) {
for (int cell : row) {
if (cell == -1 || cell == 'F')
cout << setw(9) << (char)cell;
else
cout << setw(9) << cell;
cout << "|";
}
cout << endl;
for (size_t i = 0; i < row.size(); ++i) {
cout << "---------+";
}
cout << endl;
}

return faultCount;
}

// Function to simulate Optimal page replacement algorithm
int Optimal(const vector<int>& pages, int frameCount) {
vector<int> frames(frameCount, -1); // Initialize frames with -1
int faultCount = 0;

vector<vector<int>> table; // Table to store the page table
table.push_back({-1, -1, -1, '0'}); // Header row

for (size_t i = 0; i < pages.size(); ++i) {
int page = pages[i];
if (!isInMemory(frames, page)) {
faultCount++;

// If there's an empty frame, just put the page in it
auto emptyFrame = find(frames.begin(), frames.end(), -1);
if (emptyFrame != frames.end()) {
*emptyFrame = page;
} else {
// Find the page that will not be used for the longest time in the future
int farthestUsed = -1;
int farthestIndex = -1;
for (size_t j = 0; j < frames.size(); ++j) {
int nextPageIndex = i + 1;
while (nextPageIndex < pages.size() && pages[nextPageIndex] != frames[j]) {
++nextPageIndex;
}
if (nextPageIndex == pages.size()) {
farthestIndex = j;
break;
} else if (nextPageIndex > farthestUsed) {
farthestUsed = nextPageIndex;
farthestIndex = j;
}
}
frames[farthestIndex] = page;
}
}

// Add row to the table
vector<int> row = {page};
for (int frame : frames) {
if (frame == -1)
row.push_back('-');
else
row.push_back(frame);
}
if (isInMemory(frames, page))
row.push_back('-');
else
row.push_back('F');
table.push_back(row);
}

// Print the table
for (const auto& row : table) {
for (int cell : row) {
if (cell == -1 || cell == 'F')
cout << setw(9) << (char)cell;
else
cout << setw(9) << cell;
cout << "|";
}
cout << endl;
for (size_t i = 0; i < row.size(); ++i) {
cout << "---------+";
}
cout << endl;
}

return faultCount;
}

// Function to simulate LFU page replacement algorithm
int LFU(const vector<int>& pages, int frameCount) {
vector<int> frames(frameCount, -1); // Initialize frames with -1
unordered_map<int, int> pageFrequency; // Map to store the frequency of each page
int faultCount = 0;

vector<vector<int>> table; // Table to store the page table
table.push_back({-1, -1, -1, '0'}); // Header row

for (int page : pages) {
if (!isInMemory(frames, page)) {
faultCount++;

// If there's an empty frame, just put the page in it
auto emptyFrame = find(frames.begin(), frames.end(), -1);
if (emptyFrame != frames.end()) {
*emptyFrame = page;
} else {
// Find the page with the lowest frequency
int lfuIndex = 0;
for (int i = 1; i < frameCount; ++i) {
if (pageFrequency[frames[i]] < pageFrequency[frames[lfuIndex]]) {
lfuIndex = i;
}
}
frames[lfuIndex] = page;
}
}

// Increment the frequency of the current page
pageFrequency[page]++;

// Add row to the table
vector<int> row = {page};
for (int frame : frames) {
if (frame == -1)
row.push_back('-');
else
row.push_back(frame);
}
if (isInMemory(frames, page))
row.push_back('-');
else
row.push_back('F');
table.push_back(row);
}

// Print the table
for (const auto& row : table) {
for (int cell : row) {
if (cell == -1 || cell == 'F')
cout << setw(9) << (char)cell;
else
cout << setw(9) << cell;
cout << "|";
}
cout << endl;
for (size_t i = 0; i < row.size(); ++i) {
cout << "---------+";
}
cout << endl;
}

return faultCount;
}
void displayRemainingAlgorithmsMenu() {
cout << "  Remaining Algorithms Menu\n";
cout << "  -------------------------\n";
cout << "  1. First Fit\n"; // Added option for First Fit
cout << "  2. Best Fit\n"; // Added option for Best Fit
cout << "  3. Worst Fit\n"; // Added option for Worst Fit
cout << "  0. Return to Main Menu\n";
cout << "  Enter your choice: ";
}
void displayMainMenu() {
    cout << "=================Main Menu=================\n";
    cout << "    1. Page Replacement Algorithm Simulator\n";
    cout << "    2. fit algorithms\n";
     cout <<"    0. Exit\n";
    cout << "    Enter your choice: ";
}

void displayMenu() {
    cout << "Page Replacement Algorithm Menu\n";
    cout << "  1. FIFO\n";
    cout << "  2. LRU\n";
    cout << "  3. Optimal\n";
    cout << "  4. LFU\n"; // Adding LFU option to the menu
    cout << "  0. Back to Main Menu\n";
    cout << "  Enter your choice: ";
}
int main() {
    vector<int> pages ; // Reference string with repeated pages
    int frameCount ; // Number of frames
    int mainChoice, algorithmChoice;
    MemoryAllocator memory(100); // Initialize MemoryAllocator with memory size 100

    do {
        displayMainMenu();
        cin >> mainChoice;

        switch (mainChoice) {
            case 1:
                do {
                    displayMenu();
                    cin >> algorithmChoice;

                    switch (algorithmChoice) {
                        case 1:// Input pages manually
                            cout<<"Enter frame count:";
                          cin>>   frameCount ;
                    cout << "Enter the number of pages: ";
                    int numPages;
                    cin >> numPages;
                    pages.resize(numPages);

                    cout << "Enter the page numbers: ";
                    for (int i = 0; i < numPages; ++i) {
                        cin >> pages[i];
                    }

                            cout << "FIFO Page Replacement Algorithm\n";
                            cout << "---------------------------------\n";
                            // Call FIFO function
                                cout << "+-------+---------+---------+---------+----------+" << endl;
    cout << "| Page  | Frame 1 | Frame 2 | Frame 3 |  Fault   |" << endl;
    cout << "+-------+---------+---------+---------+----------+" << endl;
                            cout << FIFO(pages, frameCount) << " faults." << endl;
                            break;
                        case 2:
                            cout<<"Enter frame count:";
                          cin>>   frameCount ;
                             cout << "Enter the number of pages: ";

                    cin >> numPages;
                    pages.resize(numPages);

                    cout << "Enter the page numbers: ";
                    for (int i = 0; i < numPages; ++i) {
                        cin >> pages[i];
                    }


                            cout << "LRU Page Replacement Algorithm\n";
                            cout << "---------------------------------\n";
                            // Call LRU function
                                cout << "+-------+---------+---------+---------+----------+" << endl;
    cout << "| Page  | Frame 1 | Frame 2 | Frame 3 |  Fault   |" << endl;
    cout << "+-------+---------+---------+---------+----------+" << endl;
                            cout << LRU(pages, frameCount) << " faults." << endl;
                            break;
                        case 3:
                            cout<<"Enter frame count:";
                          cin>>   frameCount ;
                             cout << "Enter the number of pages: ";

                    cin >> numPages;
                    pages.resize(numPages);

                    cout << "Enter the page numbers: ";
                    for (int i = 0; i < numPages; ++i) {
                        cin >> pages[i];
                    }
                            cout << "Optimal Page Replacement Algorithm\n";
                            cout << "---------------------------------\n";
                            // Call Optimal function
                                cout << "+-------+---------+---------+---------+----------+" << endl;
    cout << "| Page  | Frame 1 | Frame 2 | Frame 3 |  Fault   |" << endl;
    cout << "+-------+---------+---------+---------+----------+" << endl;
                            cout << Optimal(pages, frameCount) << " faults." << endl;
                            break;
                        case 4:
                            cout<<"Enter frame count:";
                          cin>>   frameCount ; cout << "Enter the number of pages: ";

                    cin >> numPages;
                    pages.resize(numPages);

                    cout << "Enter the page numbers: ";
                    for (int i = 0; i < numPages; ++i) {
                        cin >> pages[i];
                    }
                            cout << "LFU Page Replacement Algorithm\n";
                            cout << "---------------------------------\n";
                            // Call LFU function
                                cout << "+-------+---------+---------+---------+----------+" << endl;
    cout << "| Page  | Frame 1 | Frame 2 | Frame 3 |  Fault   |" << endl;
    cout << "+-------+---------+---------+---------+----------+" << endl;
                            cout << LFU(pages, frameCount) << " faults." << endl;
                            break;
                        case 0:
                            cout << "Returning to Main Menu...\n";
                            break;
                        default:
                            cout << "Invalid choice. Please enter a valid option.\n";
                            break;
                    }
                } while (algorithmChoice != 0);
                break;
            case 2:
                do {
                    displayRemainingAlgorithmsMenu();
                    cin >> algorithmChoice;

                    switch (algorithmChoice) {
                        case 1: {
                            int processId, processSize;
                            cout << "Enter process ID: ";
                            cin >> processId;
                            cout << "Enter process size: ";
                            cin >> processSize;
                            memory.allocateBestFit({processId, processSize});
                            break;
                        }
                        case 2: {
                            int processId;
                            cout << "Enter process ID to deallocate: ";
                            cin >> processId;
                            memory.deallocate(processId);
                            break;
                        }
                        case 3:
                            memory.displayMemory();
                            break;
                        case 4:
                            cout << "Exiting...\n";
                            break;
                        default:
                            cout << "Invalid choice! Please try again.\n";
                            break;
                    }
                } while (algorithmChoice != 4);
                break;

            case 3:
                cout << "Exiting...\n";
                break;

            default:
                cout << "Invalid choice! Please try again.\n";
                break;
        }
    } while (algorithmChoice != 3);

    return 0;
}
