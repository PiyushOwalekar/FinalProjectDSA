# 1.1 Project Title

Cloud Object Storage Deduplication Engine

## 1.2 Problem Statement

CloudVault is a large-scale cloud storage service that must detect and eliminate duplicate files to save storage costs. Existing problems:
- Slow prefix-based folder navigation when searching by folder name prefixes.
- No reliable disk-state rollback when disk data is corrupted during writes.
# 1.1 Project Title

Cloud Object Storage Deduplication Engine

## 1.2 Problem Statement

CloudVault is a large-scale cloud storage service that must detect and eliminate duplicate files to save storage costs. Existing problems:
- Slow prefix-based folder navigation when searching by folder name prefixes.
- No reliable disk-state rollback when disk data is corrupted during writes.
- Files awaiting integrity checks pile up without ordering.
- Detecting duplicates by content comparison is expensive without cryptographic fingerprints.
- No map of how storage servers connect; cross-data-center transfers are slow and unpredictable.

This project implements a simplified deduplication engine in C++ that demonstrates fast prefix-based navigation, disk undo history, a verification queue, file fingerprinting for duplicate detection, space-based block sorting, a storage network map, minimal-cost path computation for file checks, and a simple load distributor.

## 1.3 Objectives

- Provide a folder directory supporting fast prefix-based search.
- Maintain a disk undo history to revert to previous block states.
- Implement a verification line (queue) for sequential file/block verification.
- Use cryptographic fingerprints (SHA-256) to detect exact duplicate files quickly.
- Implement a space sorter to prioritize blocks by space-saving potential.
- Model a storage network and compute minimal-cost paths between servers.
- Implement a load distributor that assigns deduplication tasks to the least busy server.
- Provide clear execution steps, sample inputs/outputs, and analysis of complexity.

## 1.4 System Overview / Architecture

This is a simplified, single-process C++ simulation of the major components:

- Folder Directory: A trie (prefix tree) or ordered map is used to support fast prefix-based folder/file lookup. For simplicity the implementation uses an ordered map (std::map) keyed by full path, which allows prefix-range queries.

- Disk Undo: A versioned log of block changes. Each change is recorded as a small commit containing block id, previous state, and new state. Undo re-applies previous states by rolling back commits.

- Verification Line: A FIFO queue (std::queue) that stores tasks (file paths or block ids) waiting for integrity or duplication checks.

- File Fingerprint: Files are hashed using SHA-256 to produce fingerprints. A hash table (std::unordered_map) maps fingerprints to canonical file IDs for instant duplicate detection.

- Space Sorter: A priority queue (std::priority_queue) orders blocks by space-savings potential (estimated bytes saved), so high-impact blocks are processed first.

- Storage Network: Modeled as a weighted undirected graph with servers as nodes and link costs as edge weights. A shortest-path algorithm (Dijkstra) finds minimal-cost paths for cross-server verification.

- Load Distributor: A min-heap (priority queue) keyed by server current-load; the next task is assigned to the least-busy server.

The components interact as follows: new files are inserted into the Folder Directory and pushed onto the Verification Line. The verifier computes file fingerprints and consults the fingerprint map to detect duplicates. Deduplication tasks are sorted by space-savings and distributed over the Storage Network using shortest paths and the Load Distributor.

## 1.5 Data Structures and Algorithms Used

- std::map (ordered map): for folder directory and prefix-range operations.
- std::unordered_map: for fingerprint -> file mapping (O(1) average lookups).
- std::queue: for the verification FIFO.
- std::priority_queue: for space sorter and for server load min-heap (with inverted comparison for min behavior).
- Vector/list structures (std::vector, std::list) for block histories.
- SHA-256: cryptographic hashing for file fingerprints.
- Dijkstra's algorithm: for minimal-cost path on the storage network graph.

Justification:
- Trie would be optimal for prefix match but std::map with range queries is simpler and sufficient for this demonstration.
- Hash tables provide quick duplicate lookups by fingerprint.
- Priority queues prioritize high-value work, improving overall storage savings.

## 1.6 Implementation Approach

The provided `Finalproject.cpp` implements a simulation with the following modules:

- Data models: FileEntry, Block, Server, NetworkGraph.
- Folder Directory APIs: addFile(path, size), findByPrefix(prefix).
- Disk Undo: commitBlockChange(blockId, oldState, newState), undoLastCommit().
- Verification Line: enqueueVerification(path), processVerification().
- Fingerprinting: computeSHA256(fileContents) and a fingerprint map.
- Space Sorter: push candidate blocks with estimated savings into a priority queue.
- Storage Network and Pathfinding: buildGraph(servers, edges) and shortestPath(src, dst) using Dijkstra.
- Load Distributor: assignNextTask(task) which pops the least-busy server and assigns the task.

Notes and assumptions:
- This is a simulation: file contents are represented as strings or generated byte arrays; actual disk IO is minimal.
- The SHA-256 implementation uses a small public-domain implementation included or the OS crypto APIs where available.
- The storage network is small and in-memory for demonstration.

## 1.7 Time and Space Complexity Analysis

- Folder Directory (std::map) insertion: O(log n) per file. Prefix search: O(log n + k) where k is number of matching entries.
- Fingerprint lookup (std::unordered_map): Average O(1) lookup and insert; worst-case O(n) with poor hashing.
- Verification queue operations: O(1) enqueue/dequeue.
- Space sorter (priority queue): O(log m) per insert/pop where m is number of candidate blocks.
- Dijkstra on graph with V nodes and E edges: O((V + E) log V) with a binary heap.
- Load distributor using a min-heap over S servers: O(log S) per assignment.

Space complexity:
- Folder directory: O(n) for n files.
- Fingerprint map: O(n) for storing one fingerprint per file.
- Disk undo log: O(c) for c commits retained.
- Graph: O(V + E).

## 1.8 Execution Steps

Build and run the provided C++ program. Example commands (macOS, zsh):

```bash
# compile
g++ -std=c++17 -O2 -o Finalproject Finalproject.cpp

# run
./Finalproject
```

If your system requires linking with OpenSSL for SHA-256, compile with:

```bash
g++ -std=c++17 -O2 -lssl -lcrypto -o Finalproject Finalproject.cpp
```

The program runs a small simulation, prints verification steps, found duplicates, and the server assignments for deduplication tasks.

## 1.9 Sample Inputs and Outputs

Sample input scenario is embedded in the program (see `Finalproject.cpp`). Example output (truncated):

- Files added:
  - /users/alice/docs/report.pdf (size: 1,024,000 bytes)
  - /users/bob/docs/report_copy.pdf (size: 1,024,000 bytes)
- Computed fingerprints: abc123... (SHA-256)
- Duplicate detected: /users/bob/docs/report_copy.pdf is duplicate of /users/alice/docs/report.pdf
- Verification queue processed: 5 tasks
- Space sorter prioritized blocks: Block42 (savings 512KB), Block17 (savings 256KB)
- Shortest path from ServerA to ServerC: ServerA -> ServerB -> ServerC (cost 7)
- Load assigned: TaskX -> Server2 (current load 3)

## 1.10 Results and Observations

- Prefix-based lookup using ordered maps provides acceptable performance for moderate directory sizes. For massive-scale systems, a trie or distributed index would be preferable.
- SHA-256 fingerprinting lets the system detect exact duplicates very quickly with minimal false positives.
- Disk undo via commit logs enables safe rollback for corruption, but keeping long histories increases storage overhead.
- Sorting by space-saving potential ensures high-impact deduplication runs first, improving immediate ROI.
- Modeling the storage network and computing minimal-cost paths helps reduce cross-server transfer costs during verification.
- Simple load distribution to the least-busy server balances work but may be improved with task locality and bandwidth-awareness.

## 1.11 Conclusion

This project demonstrates the core components of a deduplication engine suitable for cloud object storage: fast prefix navigation, undoable disk commits, verification queuing, cryptographic fingerprint-based duplicate detection, prioritized space-saving, network-aware minimal-cost path selection, and basic load distribution. The design choices trade simplicity for clarity; production systems (Google Drive, Dropbox) would add distributed indices, sharded fingerprint stores, robust consensus for commits, and network-aware schedulers.


---

How to extend:
- Replace std::map with a trie or a distributed key-value store for large-scale prefix lookup.
- Persist fingerprint maps to a distributed database with sharding and replication.
- Integrate with real disk storage and block-level deduplication.
- Improve load distribution by considering network bandwidth, latency, and data locality.

