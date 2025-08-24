#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <fstream>
#include <ctime>
#include <climits>
#include <algorithm>
#include <vector>
using namespace std;

// Define structs
struct Node {
    string name, bank, expiryDate;
    long long cardNo;
    int pin, billingDate;
    Node* next, *prev;
};
Node *head = nullptr;
Node *tail = nullptr;

struct Array {
    int length = 0;
    Node** nodes;
    int index = 0;
    Array(Node* head) {
        for (Node* p = head; p; p = p->next) length++;
        nodes = new Node*[length];
        index = 0;
        for (Node* p = head; p; p = p->next) nodes[index++] = p;
    }
};

struct BST {
    Node* node;
    long long cardNo;
    BST* left, *right;
    BST(Node* n) : node(n), cardNo(n->cardNo), left(nullptr), right(nullptr) {}
};

struct AVL {
    Node* node;
    long long cardNo;
    AVL* left, *right;
    int height;
    AVL(Node* n) : node(n), cardNo(n->cardNo), left(nullptr), right(nullptr), height(1) {}
};

struct Heap {
    Node** heap;
    int length;
    Heap(Node* head) {
        length = 0;
        for (Node* p = head; p; p = p->next) length++;
        heap = new Node*[length];
        int i = 0;
        for (Node* p = head; p; p = p->next) heap[i++] = p;
        for (int i = length / 2 - 1; i >= 0; i--) heapify(i);
    }
    ~Heap() { delete[] heap; }
    void heapify(int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < length && heap[left]->cardNo > heap[largest]->cardNo) largest = left;
        if (right < length && heap[right]->cardNo > heap[largest]->cardNo) largest = right;
        if (largest != i) {
            swap(heap[i], heap[largest]);
            heapify(largest);
        }
    }
    Node* extractMax() {
        if (length == 0) return nullptr;
        Node* max = heap[0];
        heap[0] = heap[length - 1];
        length--;
        heapify(0);
        return max;
    }
};
//Queue
template <typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
        Node(T val) : data(val), next(nullptr) {}
    };
    Node *front, *rear;
    
public:
    Queue() : front(nullptr), rear(nullptr) {}
    ~Queue() {
        while (!isEmpty()) dequeue();
    }

    void enqueue(T val) {
        Node* newNode = new Node(val);
        if (isEmpty()) front = rear = newNode;
        else {
            rear->next = newNode;
            rear = newNode;
        }
    }

    T dequeue() {
        if (isEmpty()) throw runtime_error("Queue underflow");
        Node* temp = front;
        T val = front->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
        return val;
    }

    T peek() const {
        if (isEmpty()) throw runtime_error("Queue is empty");
        return front->data;
    }

    bool isEmpty() const {
        return front == nullptr;
    }
    Queue(const Queue& other) : front(nullptr), rear(nullptr) {
        Node* temp = other.front;
        while (temp) {
            enqueue(temp->data);
            temp = temp->next;
        }
    }
    Queue& operator=(const Queue& other) {
        if (this != &other) {
            while (!isEmpty()) dequeue();
            Node* temp = other.front;
            while (temp) {
                enqueue(temp->data);
                temp = temp->next;
            }
        }
        return *this;
    }
};
//Stack
template <typename T>
class Stack {
private:
    struct StackNode {
        T data;
        StackNode* next;
        StackNode(T val) : data(val), next(nullptr) {}
    };
    StackNode* top;

public:
    Stack() : top(nullptr) {}
    ~Stack() {
        while (!isEmpty()) pop();
    }

    void push(T val) {
        StackNode* newNode = new StackNode(val);
        newNode->next = top;
        top = newNode;
    }

    T pop() {
        if (isEmpty()) throw runtime_error("Stack underflow");
        StackNode* temp = top;
        T popped = top->data;
        top = top->next;
        delete temp;
        return popped;
    }

    T peek() const {
        if (isEmpty()) throw runtime_error("Stack is empty");
        return top->data;
    }

    bool isEmpty() const {
        return top == nullptr;
    }
};
// HashNode structure for chaining
struct HashNode {
    Node* data;         
    HashNode* next;     
    
    HashNode(Node* node) : data(node), next(nullptr) {}
};
//global varaibles
BST* bstRoot = nullptr;
AVL* avlRoot = nullptr;
Heap* heap = nullptr;
const int HASH_SIZE = 1000;
struct History {
    static Stack<Node*> stack;
};
Stack<Node*> History::stack;
// Hash tables
Node* openAddressingTable[HASH_SIZE] = {nullptr};  // Open addressing
HashNode* chainTable[HASH_SIZE] = {nullptr};       // Chaining
//Hashing logic
int hashFn(int key) {
    return (key % HASH_SIZE + HASH_SIZE) % HASH_SIZE;
}
void insertToOpenAddressing(Node* node) {
    int idx = hashFn(node->pin);
    int startIdx = idx;
    
    do {
        if (openAddressingTable[idx] == nullptr) {
            openAddressingTable[idx] = node;
            return;
        }
        idx = (idx + 1) % HASH_SIZE;
    } while (idx != startIdx);
    
    cout << "Open Addressing Table Full! Couldn't insert PIN: " 
         << node->pin << "\n";
}
Node* searchOpenAddressing(int pin) {
    int idx = hashFn(pin);
    int startIdx = idx;
    
    do {
        if (openAddressingTable[idx] && openAddressingTable[idx]->pin == pin) {
            return openAddressingTable[idx];
        }
        if (openAddressingTable[idx] == nullptr) break;  // Stop at empty slot
        idx = (idx + 1) % HASH_SIZE;
    } while (idx != startIdx);
    
    return nullptr;
}
void insertToChainTable(Node* node) {
    int idx = hashFn(node->pin);
    HashNode* newHashNode = new HashNode(node);
    
    newHashNode->next = chainTable[idx];
    chainTable[idx] = newHashNode;
}
Node* searchInChain(int pin) {
    int idx = hashFn(pin);
    HashNode* current = chainTable[idx];
    
    while (current != nullptr) {
        if (current->data->pin == pin) {
            return current->data;
        }
        current = current->next;
    }
    return nullptr;
}
void rebuildHashTables() {
    for (int i = 0; i < HASH_SIZE; i++) {
        openAddressingTable[i] = nullptr;
    }
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode* current = chainTable[i];
        while (current != nullptr) {
            HashNode* temp = current;
            current = current->next;
            delete temp;
        }
        chainTable[i] = nullptr;
    }
    Node* current = head;
    while (current != nullptr) {
        insertToOpenAddressing(current);
        insertToChainTable(current);
        current = current->next;
    }
}
void displayOpenAddressing() {
    cout << "\nOpen Addressing Hash Table Contents:\n";
    cout << "Index\tPIN\tName\n";
    cout << "----------------------------\n";
    
    int count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (openAddressingTable[i] != nullptr) {
            cout << i << "\t" 
                 << openAddressingTable[i]->pin << "\t"
                 << openAddressingTable[i]->name << "\n";
            count++;
        }
    }
    
    if (count == 0) {
        cout << "Hash table is empty\n";
    } else {
        cout << "----------------------------\n";
        cout << "Total entries: " << count << "\n";
    }
}
void displayChainTable() {
    cout << "\nChaining Hash Table Contents:\n";
    cout << "Index\tChain Contents\n";
    cout << "------------------------------------------------\n";
    
    int totalEntries = 0;
    int nonEmptyBuckets = 0;
    int maxChainLength = 0;
    
    for (int i = 0; i < HASH_SIZE; i++) {
        if (chainTable[i] != nullptr) {
            nonEmptyBuckets++;
            int chainLength = 0;
            
            cout << i << "\t";
            HashNode* current = chainTable[i];
            while (current != nullptr) {
                cout << current->data->pin << " (" 
                     << current->data->name << ") -> ";
                current = current->next;
                chainLength++;
            }
            cout << "NULL\n";
            
            totalEntries += chainLength;
            if (chainLength > maxChainLength) {
                maxChainLength = chainLength;
            }
        }
    }
    
    if (nonEmptyBuckets == 0) {
        cout << "Hash table is empty\n";
    } else {
        cout << "------------------------------------------------\n";
        cout << "Total entries: " << totalEntries << "\n";
        cout << "Non-empty buckets: " << nonEmptyBuckets << "\n";
        cout << "Load factor: " << (float)totalEntries/HASH_SIZE << "\n";
        cout << "Longest chain: " << maxChainLength << "\n";
    }
}
// Operations on BST
BST* insertBST(BST* root, Node* node) {
    if (!root) return new BST(node);
    if (node->cardNo < root->cardNo) root->left = insertBST(root->left, node);
    else if (node->cardNo > root->cardNo) root->right = insertBST(root->right, node);
    return root;
}

BST* buildBST() {
    BST* root = nullptr;
    for (Node* p = head; p; p = p->next) root = insertBST(root, p);
    return root;
}

Node* searchBST(BST* root, long long cardNo) {
    if (!root) return nullptr;
    if (cardNo == root->cardNo) return root->node;
    if (cardNo < root->cardNo) return searchBST(root->left, cardNo);
    return searchBST(root->right, cardNo);
}

// Operations on AVL
int height(AVL* node) { return node ? node->height : 0; }

int balanceFactor(AVL* node) { return node ? height(node->left) - height(node->right) : 0; }

AVL* rotateRight(AVL* y) {
    AVL* x = y->left;
    AVL* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    return x;
}

AVL* rotateLeft(AVL* x) {
    AVL* y = x->right;
    AVL* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    return y;
}

AVL* insertAVL(AVL* node, Node* n) {
    if (!node) return new AVL(n);
    if (n->cardNo < node->cardNo) node->left = insertAVL(node->left, n);
    else if (n->cardNo > node->cardNo) node->right = insertAVL(node->right, n);
    else return node;

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = balanceFactor(node);

    if (balance > 1 && n->cardNo < node->left->cardNo) return rotateRight(node);
    if (balance < -1 && n->cardNo > node->right->cardNo) return rotateLeft(node);
    if (balance > 1 && n->cardNo > node->left->cardNo) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && n->cardNo < node->right->cardNo) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

AVL* buildAVL() {
    AVL* root = nullptr;
    for (Node* p = head; p; p = p->next) root = insertAVL(root, p);
    return root;
}

Node* searchAVL(AVL* root, long long cardNo) {
    if (!root) return nullptr;
    if (cardNo == root->cardNo) return root->node;
    if (cardNo < root->cardNo) return searchAVL(root->left, cardNo);
    return searchAVL(root->right, cardNo);
}

// Graph Utilities
vector<Node*> getNodesByBank(const string& bank) {
    vector<Node*> nodes;
    for (Node* p = head; p; p = p->next)
        if (p->bank == bank) nodes.push_back(p);
    return nodes;
}

int expiryToMonths(const string& expiry) {
    int month = stoi(expiry.substr(0, 2));
    int year = stoi(expiry.substr(3, 2));
    return year * 12 + month;
}
vector<vector<int>> buildBankGraph(const vector<Node*>& nodes) {
    int n = nodes.size();
    vector<vector<int>> adj(n);
    if (n <= 1) return adj; 
    for (int i = 0; i < n; i++) {
        int next = (i + 1) % n;
        adj[i].push_back(next);
        adj[next].push_back(i);
        int prev = (i - 1 + n) % n;
        adj[i].push_back(prev);
        adj[prev].push_back(i);
    }
    return adj;
}
void printBFS(const vector<Node*>& nodes) {
    if (nodes.empty()) return;
    
    vector<vector<int>> adj = buildBankGraph(nodes);
    int n = nodes.size();
    vector<bool> visited(n, false);
    Queue<int> q;  // Using our custom Queue
    
    // Start from first node
    q.enqueue(0);
    visited[0] = true;
    
    cout << "BFS Traversal:\n";
    while (!q.isEmpty()) {
        int u = q.dequeue();
        cout << nodes[u]->name << " (" << nodes[u]->cardNo << ") -> ";
        
        for (int v : adj[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.enqueue(v);
            }
        }
    }
    cout << "END\n";
}
void printDFS(const vector<Node*>& nodes) {
    if (nodes.empty()) return;
    
    vector<vector<int>> adj = buildBankGraph(nodes);
    int n = nodes.size();
    vector<bool> visited(n, false);
    Stack<int> st;  // Using our custom Stack
    
    // Start from first node
    st.push(0);
    visited[0] = true;
    
    cout << "DFS Traversal:\n";
    while (!st.isEmpty()) {
        int u = st.pop();
        cout << nodes[u]->name << " (" << nodes[u]->cardNo << ") -> ";
        
        // Process neighbors in reverse order for DFS correctness
        for (int i = adj[u].size()-1; i >= 0; i--) {
            int v = adj[u][i];
            if (!visited[v]) {
                visited[v] = true;
                st.push(v);
            }
        }
    }
    cout << "END\n";
}

long long dijkstraPath(const vector<Node*>& nodes, long long srcCard, long long destCard) {
    int n = nodes.size();
    vector<vector<long long>> graph(n, vector<long long>(n, 0));
    int srcIdx = -1, destIdx = -1;

    for (int i = 0; i < n; i++) {
        if (nodes[i]->cardNo == srcCard) srcIdx = i;
        if (nodes[i]->cardNo == destCard) destIdx = i;
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            int diff = abs(expiryToMonths(nodes[i]->expiryDate) - expiryToMonths(nodes[j]->expiryDate));
            graph[i][j] = diff;
        }
    }

    if (srcIdx == -1 || destIdx == -1) return -1;

    vector<long long> dist(n, LLONG_MAX);
    vector<bool> visited(n, false);
    dist[srcIdx] = 0;

    for (int count = 0; count < n - 1; count++) {
        long long minDist = LLONG_MAX;
        int u = -1;
        for (int i = 0; i < n; i++) {
            if (!visited[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }
        if (u == -1) break;
        visited[u] = true;
        for (int v = 0; v < n; v++) {
            if (!visited[v] && graph[u][v] && dist[u] != LLONG_MAX && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
            }
        }
    }
    return dist[destIdx];
}

long long primMST(const vector<Node*>& nodes) {
    int n = nodes.size();
    if (n == 0) return 0;

    vector<vector<long long>> graph(n, vector<long long>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            graph[i][j] = abs(nodes[i]->billingDate - nodes[j]->billingDate);
        }
    }

    vector<long long> key(n, LLONG_MAX);
    vector<bool> inMST(n, false);
    key[0] = 0;
    long long totalWeight = 0;

    for (int count = 0; count < n; count++) {
        long long minKey = LLONG_MAX;
        int u = -1;
        for (int i = 0; i < n; i++) {
            if (!inMST[i] && key[i] < minKey) {
                minKey = key[i];
                u = i;
            }
        }
        if (u == -1) break;
        inMST[u] = true;
        totalWeight += key[u];
        for (int v = 0; v < n; v++) {
            if (!inMST[v] && graph[u][v] < key[v]) {
                key[v] = graph[u][v];
            }
        }
    }
    return totalWeight;
}
// DSU (Disjoint Set Union) for Kruskal's algorithm
class DSU {
    vector<int> parent, rank;
    
public:
    DSU(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; i++)
            parent[i] = i;
    }
    
    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    }
    
    void unite(int x, int y) {
        int rx = find(x), ry = find(y);
        if (rx == ry) return;
        
        if (rank[rx] < rank[ry]) {
            parent[rx] = ry;
        } else if (rank[rx] > rank[ry]) {
            parent[ry] = rx;
        } else {
            parent[ry] = rx;
            rank[rx]++;
        }
    }
};

// Kruskal's MST implementation
long long kruskalMST(const vector<Node*>& nodes) {
    int n = nodes.size();
    if (n <= 1) return 0;
    vector<tuple<int, int, int>> edges;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int weight = abs(nodes[i]->billingDate - nodes[j]->billingDate);
            edges.push_back({weight, i, j});
        }
    }
    sort(edges.begin(), edges.end());
    
    DSU dsu(n);
    long long totalWeight = 0;
    int edgesAdded = 0;
    
    for (auto& [w, u, v] : edges) {
        if (dsu.find(u) != dsu.find(v)) {
            dsu.unite(u, v);
            totalWeight += w;
            edgesAdded++;
            if (edgesAdded == n - 1) break;  
        }
    }
    
    return totalWeight;
}
//operations on data
void addNode(string name, string bank, string expiryDate, long long cardNo, int billingDate, int pin) {
    Node* node= new Node;
    node->name = name;
    node->bank = bank;
    node->expiryDate = expiryDate;
    node->cardNo = cardNo;
    node->billingDate = billingDate;
    node->pin = pin;
    node->prev = nullptr;
    node->next = nullptr;
    if (!head) {head = tail = node; return;}
    node->prev = tail;
    tail->next = node;
    tail = node;
    History::stack.push(node);
    rebuildHashTables();
}
void swapNodeData(Node* a, Node* b) {
    swap(a->name, b->name);
    swap(a->bank, b->bank);
    swap(a->expiryDate, b->expiryDate);
    swap(a->cardNo, b->cardNo);
    swap(a->pin, b->pin);
    swap(a->billingDate, b->billingDate);
}
int linearSearch(Node* head, int key) {
    int idx = 0;
    for(Node* p = head; p; p = p->next, ++idx)
        if(p->pin == key) return idx;
    return -1;
}
int binarySearch(Array& A, int key){
    int lo=0, hi=A.length-1;
    while(lo<=hi){
        int mid = lo + (hi-lo)/2;
        if(A.nodes[mid]->pin == key) return mid;
        if(A.nodes[mid]->pin < key) lo = mid+1;
        else hi = mid-1;
    }
    return -1;
}
void bubbleSortList(Node* head) {
    if(!head) return;
    bool swapped;
    do {
        swapped = false;
        for(Node* p = head; p->next; p = p->next) {
            if(p->pin > p->next->pin) {
                swapNodeData(p, p->next); // swap all data
                swapped = true;
            }
        }
    } while(swapped);
}
void selectionSortList(Node* head) {
    for(Node* i = head; i; i = i->next) {
        Node* minN = i;
        for(Node* j = i->next; j; j = j->next)
            if(j->pin < minN->pin) minN = j;
        swapNodeData(i, minN);  // Swap all data
    }
}
Node* insertionSortList(Node* head){
    Node* sorted = nullptr;
    while(head){
        Node* cur = head; head = head->next;
        cur->next = nullptr;
        if(!sorted || cur->pin < sorted->pin){
            cur->next = sorted; sorted = cur;
        } else {
            Node* p = sorted;
            while(p->next && p->next->pin < cur->pin)
                p = p->next;
            cur->next = p->next; p->next = cur;
        }
    }
    return sorted;
}
Node* mergeLists(Node* a, Node* b){
    Node dummy; Node* t = &dummy; dummy.next = nullptr;
    while(a && b){
        if(a->pin <= b->pin){
            t->next = a; a = a->next;
        } else {
            t->next = b; b = b->next;
        }
        t = t->next;
    }
    t->next = a ? a : b;
    return dummy.next;
}

Node* mergeSortList(Node* head){
    if(!head || !head->next) return head;
    // split
    Node *slow = head, *fast = head->next;
    while(fast && fast->next){
        slow = slow->next; fast = fast->next->next;
    }
    Node* mid = slow->next; slow->next = nullptr;
    return mergeLists(mergeSortList(head), mergeSortList(mid));
}
void print() {
    if (!head) return;
    Node *node = head;
    while (node) {
        cout<<"Card Holder Name: "<<node->name<<", Issuing Bank: "<<node->bank<<", Card Number: "<<node->cardNo<<", Card Expiry Date: "<<node->expiryDate<<", Card PIN: "<<node->pin;
        cout<<"\n";
        node=node->next;
    }
}
void print(string user) {
    if (!head) return;
    Node *node = head;
    while (node) {
        if (node->name == user) cout<<"Card Holder Name: "<<node->name<<", Issuing Bank: "<<node->bank<<", Card Number: "<<node->cardNo<<", Card Expiry Date: "<<node->expiryDate;
        node=node->next;
    }
}
void removeUser(long long cardNo, int pin) {
    if (!head) return;
    Node* node = head;
    while (node) {
        if (node->cardNo==cardNo && node->pin == pin) {
            if (head==tail) head = tail = nullptr;
            else if (node==head) {
                head=head->next;
                delete(head->prev);
                head->prev = nullptr;
                cout<<"Deleted\n";
                rebuildHashTables();
                return;
            }
            else if (node==tail) {
                tail = tail->prev;
                delete(tail->next);
                tail->next = nullptr;
                cout<<"Deleted\n";
                rebuildHashTables();
                return;
            }
            else {
                node->prev->next = node->next;
                node->next->prev = node->prev;
                delete node;
                cout<<"Deleted\n";
                rebuildHashTables();
                return;
            }
        }
        node = node->next;
    }
    cout<<"Not Found";
}
Node* getLastUser() {
    if (History::stack.isEmpty()) return nullptr;
    return History::stack.pop();
}
void removeLastUser() {
    if (History::stack.isEmpty()) return;
    Node* lastAdded = History::stack.peek();
    removeUser(lastAdded->cardNo, lastAdded->pin);
}
//Use queues to parse the csv
Queue<string> splitLine(string& line) {
    Queue<string> row;
    int currentIndex = 0;
    while (true) {
        int nextCommaIndex = line.find(',', currentIndex);
        if (nextCommaIndex == string::npos) {
            row.enqueue(line.substr(currentIndex));
            break;
        }
        string field = line.substr(currentIndex, nextCommaIndex - currentIndex);
        row.enqueue(field);
        currentIndex = nextCommaIndex + 1;
    }
    return row;
}

string getAttribute(Queue<string> row, int columnIndex) {
    // Process the queue (creates a copy)
    for (int i = 0; i < columnIndex && !row.isEmpty(); i++) {
        row.dequeue();
    }
    return row.isEmpty() ? "" : row.peek();
}

Queue<Queue<string>> parseFile(string fileLocation) {
    ifstream file(fileLocation);
    Queue<Queue<string>> rows;
    
    if (!file.is_open()) {
        cerr << "File cannot be opened: " << fileLocation << endl;
        return rows;
    }

    string line;
    getline(file, line);  // Skip header
    
    while (getline(file, line)) {
        rows.enqueue(splitLine(line));
    }
    
    return rows;
}

void parseQueueToLinkedList(Queue<Queue<string>> rows) {
    while (!rows.isEmpty()) {
        Queue<string> row = rows.dequeue();
        string bank = getAttribute(row, 2);
        long long cardNo = stoll(getAttribute(row, 3));
        string name = getAttribute(row, 4);
        string expiryDate = getAttribute(row, 7);
        int billingDate = stoi(getAttribute(row, 8));
        int pin = stoi(getAttribute(row, 9));
        addNode(name, bank, expiryDate, cardNo, billingDate, pin);
    }
}


//prompt user
int main() {
    string fileLocation = "bank.csv";
    Queue<Queue<string>> rows = parseFile(fileLocation);
    parseQueueToLinkedList(rows);
    bstRoot = buildBST();
    avlRoot = buildAVL();
    heap = new Heap(head);
    rebuildHashTables();
    int choice = 0;
    do {
        cout << "\n===== Bank Card Management System =====\n";
        cout << "1. Display all users\n";
        cout << "2. Display a specific user by name\n";
        cout << "3. Remove a user by Card Number and PIN\n";
        cout << "4. Display last added user\n";
        cout << "5. Remove last added user\n";
        cout << "6. Add User\n";
        cout << "7. BST Search by Card Number\n";
        cout << "8. AVL Search by Card Number\n";
        cout << "9. Heap Extract Max Card\n";
        cout << "10. Dijkstra Path (Expiry Similarity)\n";
        cout << "11. Prim's MST (Billing Date Similarity)\n";
        cout << "12. Kruskal's MST (Billing Date Similarity)\n";
        cout << "13. Open Addressing Search by PIN\n";
        cout << "14. Chaining Search by PIN\n";
        cout << "15. Display Open Addressing Table\n";
        cout << "16. Display Chaining Table\n";
        cout << "17. Bubble Sort by PIN\n";
        cout << "18. Selection Sort by PIN\n";
        cout << "19. Insertion Sort by PIN\n";
        cout << "20. Merge Sort by PIN\n";
        cout << "21. Display Sorted List\n";
        cout << "22. BFS to find all customers of a bank\n";
        cout << "23. DFS to find all customers of a bank\n";
        cout << "24. Linear search to find user by PIN\n";
        cout << "25. Binary search to find user by PIN\n";
        cout << "0. Exit\n";
        cout << "Select an option: ";
        cin >> choice;
        if (choice == 1) {
            print();
        } else if (choice == 2) {
            string username;
            cout << "Enter name of user to search: \n";
            cin>>username;
            print(username);
        } else if (choice == 3) {
            long long cardNo;
            int pin;
            cout << "Enter card number: ";
            cin >> cardNo;
            cout << "Enter PIN: ";
            cin >> pin;
            removeUser(cardNo, pin);
        } else if (choice == 4) {
            Node* node = getLastUser();
            if (node)
                cout << "Last added user: " << node->name << ", Card No: " << node->cardNo << ", Bank: " << node->bank << ", Expiry: " << node->expiryDate <<"\n";
            else
                cout << "No users found.\n";
        } else if (choice == 5) {
            removeLastUser();
            cout << "Last added user removed.\n";
        } else if (choice == 6) {
            cout<<"Enter user details:\n";
            cout<<"Enter name:\n";
            string name; 
            cin>>name;
            cout<<"Enter bank:\n";
            string bank;
            cin>>bank;
            cout<<"Enter card number:\n";
            long long cardNo; 
            cin>>cardNo;
            cout<<"Enter card expiry date:\n";
            string expiryDate; 
            cin>>expiryDate;
            cout<<"Enter card pin:\n";
            int pin; 
            cin>>pin;
            time_t now = time(nullptr);
            addNode(name, bank, expiryDate, cardNo, localtime(&now)->tm_mday, pin);
            cout<<"Added successfully";
        } else if (choice == 7) {
            long long cardNo;
            cout << "Enter card number: ";
            cin >> cardNo;
            Node* result = searchBST(bstRoot, cardNo);
            if (result) cout << "Found: " << result->name << endl;
            else cout << "Not found!\n";
        } else if (choice == 8) {
            long long cardNo;
            cout << "Enter card number: ";
            cin >> cardNo;
            Node* result = searchAVL(avlRoot, cardNo);
            if (result) cout << "Found: " << result->name << endl;
            else cout << "Not found!\n";
        } else if (choice == 9) {
            Node* maxNode = heap->extractMax();
            if (maxNode) cout << "Max Card: " << maxNode->cardNo << " (" << maxNode->name << ")\n";
            else cout << "Heap empty!\n";
        } else if (choice == 10) {
            string bank;
            long long src, dest;
            cout << "Enter bank name: ";
            cin >> bank;
            cout << "Enter source card number: ";
            cin >> src;
            cout << "Enter destination card number: ";
            cin >> dest;
            long long path = dijkstraPath(getNodesByBank(bank), src, dest);
            if (path == -1) cout << "Invalid cards or bank!\n";
            else cout << "Total expiry difference: " << path << " months\n";
        } else if (choice == 11) {
            string bank;
            cout << "Enter bank name: ";
            cin >> bank;
            long long total = primMST(getNodesByBank(bank));
            cout << "Minimum total date difference: " << total << " days" << endl;
        } else if (choice == 12) {
            string bank;
            cout << "Enter bank name: ";
            cin >> bank;
            long long total = kruskalMST(getNodesByBank(bank));
            cout << "Minimum total date difference: " << total << " days" << endl;
        } else if (choice == 13) {
            int pin;
            cout << "Enter PIN: ";
            cin >> pin;
            Node* result = searchOpenAddressing(pin);
            if (result) 
                cout << "Found: " << result->name << "\n";
            else 
                cout << "Not found!\n";
        } else if (choice == 14) {
            int pin;
            cout << "Enter PIN: ";
            cin >> pin;
            Node* result = searchInChain(pin);
            if (result) 
                cout << "Found: " << result->name << "\n";
            else 
                cout << "Not found!\n";
        } else if (choice == 15) {
            displayOpenAddressing();
        } else if (choice == 16) {
            displayChainTable();
        } else if (choice == 17) {
            bubbleSortList(head);
            cout << "List sorted using Bubble Sort\n";
        }
        else if (choice == 18) {
            selectionSortList(head);
            cout << "List sorted using Selection Sort\n";
        }
        else if (choice == 19) {
            head = insertionSortList(head);
            cout << "List sorted using Insertion Sort\n";
        }
        else if (choice == 20) {
            head = mergeSortList(head);
            cout << "List sorted using Merge Sort\n";
        }
        else if (choice == 21) {
            cout << "Current Sorted List:\n";
            Node* current = head;
            while (current) {
                cout << "Name: " << current->name 
                     << ", PIN: " << current->pin 
                     << ", Card: " << current->cardNo << "\n";
                current = current->next;
            }
        } else if(choice == 22) {
            string bank;
            cout << "Enter bank name: ";
            cin >> bank;
            vector<Node*> bankNodes = getNodesByBank(bank);
            printBFS(bankNodes);
            break;
        } else if (choice == 23) {
            string bank;
            cout << "Enter bank name: ";
            cin >> bank;
            vector<Node*> bankNodes = getNodesByBank(bank);
            printDFS(bankNodes);
            break;
        }   else if (choice == 24) {
                int pin;
                cout << "Enter PIN to search: ";
                cin >> pin;
                
                int index = linearSearch(head, pin);
                if (index != -1) {
                    Node* current = head;
                    for (int i = 0; i < index && current; i++) {
                        current = current->next;
                    }
                    if (current) {
                        cout << "Found at position " << index << ":\n";
                        cout << "Name: " << current->name << "\n";
                        cout << "Card: " << current->cardNo << "\n";
                        cout << "Bank: " << current->bank << "\n";
                    }
                } else {
                    cout << "PIN not found in the list.\n";
                }
            }
            else if (choice == 25) {
                int pin;
                cout << "Enter PIN to search: ";
                cin >> pin;
                head = mergeSortList(head);
                rebuildHashTables(); 
                Array sortedArray(head);
                int index = binarySearch(sortedArray, pin);
                if (index != -1) {
                    cout << "Found at position " << index << ":\n";
                    cout << "Name: " << sortedArray.nodes[index]->name << "\n";
                    cout << "Card: " << sortedArray.nodes[index]->cardNo << "\n";
                    cout << "Bank: " << sortedArray.nodes[index]->bank << "\n";
                } else {
                    cout << "PIN not found in the list.\n";
                }
            }else if (choice != 0) {
            cout << "Invalid choice! Please try again.\n";
        }
        if (choice >= 17 && choice <= 20) rebuildHashTables();
    } while (choice != 0);
    delete heap;
    return 0;
}