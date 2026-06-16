#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <memory>
#include <map>
#include <stack>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

/*--------------------------------------------------
    FINGERPRINT ENGINE
---------------------------------------------------*/
class FingerprintEngine {
public:
    static uint64_t fnv1a64(const string& data) {
        const uint64_t OFFSET = 14695981039346656037ULL;
        const uint64_t PRIME = 1099511628211ULL;

        uint64_t hash = OFFSET;

        for(char c : data) {
            hash ^= static_cast<uint64_t>(c);
            hash *= PRIME;
        }

        return hash;
    }

    static string toHex(uint64_t value) {
        stringstream ss;
        ss << hex << setw(16) << setfill('0') << value;
        return ss.str();
    }
};

/*--------------------------------------------------
    1. TRIE DIRECTORY
---------------------------------------------------*/
class TrieNode {
public:
    map<char, unique_ptr<TrieNode>> children;
    bool isEnd = false;
    string fullPath;
};

class FolderDirectory {
private:
    unique_ptr<TrieNode> root;

    void dfs(TrieNode* node, vector<string>& result) {
        if(node->isEnd)
            result.push_back(node->fullPath);

        for(auto& child : node->children)
            dfs(child.second.get(), result);
    }

public:
    FolderDirectory() {
        root = make_unique<TrieNode>();
    }

    void insert(const string& path) {
        TrieNode* curr = root.get();

        for(char c : path) {
            if(!curr->children.count(c))
                curr->children[c] = make_unique<TrieNode>();

            curr = curr->children[c].get();
        }

        curr->isEnd = true;
        curr->fullPath = path;
    }

    vector<string> autocomplete(const string& prefix) {
        TrieNode* curr = root.get();

        for(char c : prefix) {
            if(!curr->children.count(c))
                return {};

            curr = curr->children[c].get();
        }

        vector<string> result;
        dfs(curr, result);
        return result;
    }
};

/*--------------------------------------------------
    2. DISK VERSIONING
---------------------------------------------------*/
class DiskVersionManager {
private:
    unordered_map<int, vector<string>> versions;

public:

    void writeBlock(int blockId, const string& data) {
        versions[blockId].push_back(data);
    }

    void rollback(int blockId) {

        if(versions[blockId].size() <= 1) {
            cout << "Cannot rollback block "
                 << blockId << endl;
            return;
        }

        versions[blockId].pop_back();

        cout << "Rollback successful for block "
             << blockId << endl;
    }

    string currentState(int blockId) {
        if(versions[blockId].empty())
            return "";

        return versions[blockId].back();
    }
};

/*--------------------------------------------------
    3. FAIR VERIFICATION QUEUE (FIFO)
---------------------------------------------------*/
class VerificationManager {

private:
    queue<string> q;

public:

    void addTask(const string& file) {
        q.push(file);
    }

    void processAll() {

        while(!q.empty()) {

            cout << "Verifying "
                 << q.front()
                 << endl;

            q.pop();
        }
    }
};

/*--------------------------------------------------
    4. DEDUPLICATION ENGINE
---------------------------------------------------*/
class Deduplicator {

private:

    unordered_map<uint64_t, vector<string>> fingerprints;

public:

    void addFile(const string& filename,
                 const string& content) {

        uint64_t fp =
            FingerprintEngine::fnv1a64(content);

        fingerprints[fp].push_back(filename);

        if(fingerprints[fp].size() > 1) {

            cout << "Duplicate Group Found:\n";

            for(auto& file : fingerprints[fp])
                cout << "   " << file << endl;
        }
    }
};

/*--------------------------------------------------
    5. SPACE SAVINGS PRIORITIZER
---------------------------------------------------*/
struct Block {

    int id;
    int savings;

    bool operator<(const Block& other) const {
        return savings < other.savings;
    }
};

class SpaceOptimizer {

private:
    priority_queue<Block> heap;

public:

    void addBlock(int id, int savings) {
        heap.push({id, savings});
    }

    void optimize() {

        while(!heap.empty()) {

            auto block = heap.top();
            heap.pop();

            cout << "Process Block "
                 << block.id
                 << " -> Save "
                 << block.savings
                 << " MB\n";
        }
    }
};

/*--------------------------------------------------
    6. STORAGE NETWORK + DIJKSTRA
---------------------------------------------------*/
class StorageNetwork {

private:

    int n;

    vector<vector<pair<int,int>>> graph;

public:

    StorageNetwork(int nodes)
        : n(nodes),
          graph(nodes) {}

    void addEdge(int u,
                 int v,
                 int cost) {

        graph[u].push_back({v,cost});
        graph[v].push_back({u,cost});
    }

    void shortestPath(int src,
                      int dest) {

        vector<long long> dist(n,1e18);
        vector<int> parent(n,-1);

        priority_queue<
            pair<long long,int>,
            vector<pair<long long,int>>,
            greater<>
        > pq;

        dist[src] = 0;
        pq.push({0,src});

        while(!pq.empty()) {

            pair<long long,int> current = pq.top();
            long long d = current.first;
            int u = current.second;
            pq.pop();

            if(d != dist[u])
                continue;

            for(auto edge : graph[u]) {
                int v = edge.first;
                int w = edge.second;

                if(dist[v] > dist[u] + w) {

                    dist[v] = dist[u] + w;
                    parent[v] = u;

                    pq.push({dist[v],v});
                }
            }
        }

        if(dist[dest] == 1e18) {
            cout << "No path exists" << endl;
            return;
        }

        cout << "Minimum Cost = "
             << dist[dest]
             << endl;

        vector<int> path;

        for(int cur=dest;
            cur!=-1;
            cur=parent[cur])

            path.push_back(cur);

        reverse(path.begin(), path.end());

        cout << "Path : ";

        for(int node : path)
            cout << node << " ";

        cout << endl;
    }
};

/*--------------------------------------------------
    7. LOAD BALANCER
---------------------------------------------------*/
struct Server {

    int id;
    int load;

    bool operator>(const Server& other) const {
        return load > other.load;
    }
};

class LoadBalancer {

private:

    priority_queue<
        Server,
        vector<Server>,
        greater<Server>
    > servers;

public:

    void addServer(int id, int load) {
        servers.push({id, load});
    }

    void assignTask() {

        if(servers.empty()) {
            cout << "No server available\n";
            return;
        }

        auto server = servers.top();
        servers.pop();

        cout << "Task assigned to Server "
             << server.id
             << endl;

        server.load++;

        servers.push(server);
    }
};

/*--------------------------------------------------
    MAIN DEMO
---------------------------------------------------*/
int main() {

    cout << "\n===== CLOUDVAULT ENGINE =====\n\n";

    FolderDirectory directory;

    directory.insert("root/docs/work");
    directory.insert("root/docs/personal");
    directory.insert("root/photos");

    cout << "Autocomplete Results:\n";

    for(auto& path :
        directory.autocomplete("root/do"))

        cout << path << endl;

    cout << endl;

    DiskVersionManager disk;

    disk.writeBlock(101,"Version1");
    disk.writeBlock(101,"Version2");
    disk.writeBlock(101,"Corrupted");

    disk.rollback(101);

    cout << disk.currentState(101)
         << endl << endl;

    VerificationManager verifier;

    verifier.addTask("critical.db");
    verifier.addTask("image.png");
    verifier.addTask("logs.zip");

    verifier.processAll();

    cout << endl;

    Deduplicator dedup;

    dedup.addFile("file1.txt","Hello");
    dedup.addFile("file2.txt","Hello");
    dedup.addFile("file3.txt","Different");

    cout << endl;

    SpaceOptimizer optimizer;

    optimizer.addBlock(1,50);
    optimizer.addBlock(2,300);
    optimizer.addBlock(3,120);

    optimizer.optimize();

    cout << endl;

    StorageNetwork network(5);

    network.addEdge(0,1,10);
    network.addEdge(1,2,5);
    network.addEdge(0,2,25);
    network.addEdge(2,3,4);

    network.shortestPath(0,3);

    cout << endl;

    LoadBalancer lb;

    lb.addServer(1,10);
    lb.addServer(2,5);
    lb.addServer(3,7);

    lb.assignTask();
    lb.assignTask();

    return 0;
}