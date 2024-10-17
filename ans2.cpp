#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <cmath>
#include <fstream>

using namespace std;

size_t hash_string(const string& str) {
    std::hash<string> hash_fn;
    return hash_fn(str);
}

size_t build_merkle_tree(const vector<string>& leaves) {
    vector<size_t> hashes;
    for (const auto& leaf : leaves) {
        hashes.push_back(hash_string(leaf));
    }
        while (hashes.size() > 1) {
        vector<size_t> new_hashes;
        for (size_t i = 0; i < hashes.size(); i += 2) {
            if (i + 1 < hashes.size()) {
                new_hashes.push_back(hash_string(to_string(hashes[i]) + to_string(hashes[i + 1])));
            } else {
                new_hashes.push_back(hashes[i]);
            }
        }
        hashes = new_hashes;
    }
    return hashes[0];
}

vector<size_t> get_verification_path(const vector<string>& leaves, size_t leaf_index) {
    vector<size_t> path;
    size_t index = leaf_index;
    vector<size_t> hashes;

    for (const auto& leaf : leaves) {
        hashes.push_back(hash_string(leaf));
    }

    while (hashes.size() > 1) {
        vector<size_t> new_hashes;
        for (size_t i = 0; i < hashes.size(); i += 2) {
            if (i + 1 < hashes.size()) {
                new_hashes.push_back(hash_string(to_string(hashes[i]) + to_string(hashes[i + 1])));
            } else {
                new_hashes.push_back(hashes[i]);
            }

            if (i == index || i + 1 == index) {
                size_t sibling_index = (i == index) ? i + 1 : i;
                if (sibling_index < hashes.size()) {
                    path.push_back(hashes[sibling_index]);
                }
                index /= 2;
            }
        }
        hashes = new_hashes;
    }

    return path;
}

bool verify_merkle_path(size_t root_hash, const string& leaf, size_t leaf_index, const vector<size_t>& path) {
    size_t computed_hash = hash_string(leaf);
    size_t index = leaf_index;

    cout << "Initial computed hash: " << computed_hash << endl;
    for (size_t sibling_hash : path) {
        // cout << "Index: " << index << ", Sibling hash: " << sibling_hash << endl;
        if (index % 2 == 0) {
            computed_hash = hash_string(to_string(computed_hash) + to_string(sibling_hash));
        } else {
            computed_hash = hash_string(to_string(sibling_hash) + to_string(computed_hash));
        }
        // cout << "New computed hash: " << computed_hash << endl;
        index /= 2;
    }

    cout << "Final computed hash: " << computed_hash << endl;
    cout << "Original root hash: " << root_hash << endl;

    return computed_hash == root_hash;
}

int main() {
    ifstream file("leaves.txt");
    vector<string> leaves;
    string leaf;

    while (getline(file, leaf, ',')) {
        leaves.push_back(leaf);
    }
    file.close();
    
    size_t root_hash = build_merkle_tree(leaves);
    cout << "Root hash: " << root_hash << endl;

    size_t k = 11;   
    leaves[10]=("hello");
    vector<size_t> verification_path = get_verification_path(leaves, k);
    
    // cout << "Verification path: ";
    // for (const auto& hash : verification_path) {
    //     cout << hash << " ";
    // }
    // cout << endl;

    bool is_valid = verify_merkle_path(root_hash, leaves[k], k, verification_path);
    cout << "Verification result: " << (is_valid ? "Valid" : "Invalid") << endl;

    return 0;
}
