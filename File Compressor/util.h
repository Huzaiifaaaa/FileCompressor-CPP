//
// STARTER CODE: util.h
//
// TODO(Jamani Alford)
//

#include <iostream>
#include <fstream>
#include <map>
#include <queue>          // std::priority_queue
#include <vector>         // std::vector
#include <functional>     // std::greater
#include <string>
#include "bitstream.h"
#include "hashmap.h"
#include "mymap.h"
#pragma once



struct HuffmanNode {
    int character;
    int count;
    HuffmanNode* zero;
    HuffmanNode* one;
};

//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode* node) {
    if (node != nullptr) {
        freeTree(node->zero);
        freeTree(node->one);
        delete node;
    }
}


//
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
void buildFrequencyMap(string filename, bool isFile, hashmap &map) {
    if (isFile) {
        ifstream file(filename);
        char c;
        while (file.get(c)) {
            if (map.containsKey(c)) {
                map.put(c, map.get(c) + 1);
            } else {
                map.put(c, 1);
            }
        }
        map.put(PSEUDO_EOF, 1);
        return;
    } else {
        char c;
        for (int i = 0; i < filename.length(); ++i) {
            c = filename[i];
            if (map.containsKey(c)) {
                map.put(c, map.get(c) + 1);
            } else {
                map.put(c, 1);
            }
        }
        map.put(PSEUDO_EOF, 1);
        return;
    }
}
//
// *This function builds an encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmap &map) {
    struct compareFrequency {
        bool operator()(HuffmanNode* const&m, HuffmanNode* const&n){
            return m->count > n->count;
        }
    };
    vector<int> keys = map.keys();
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, compareFrequency> Q;

    for (int i = 0; i < keys.size(); ++i) {
        HuffmanNode *newNode = new HuffmanNode();
        newNode->character = keys[i];
        newNode->count = map.get(keys[i]);
        newNode->one = nullptr;
        newNode->zero = nullptr;
        Q.push(newNode);
    }

    while (Q.size() > 1) {
        HuffmanNode *node1 = Q.top();  // smaller
        Q.pop();
        HuffmanNode *node2 = Q.top();  // greater
        Q.pop();

        int sum = node1->count + node2->count;

        HuffmanNode *notCharNode = new HuffmanNode();
        notCharNode->character = NOT_A_CHAR;
        notCharNode->count = sum;
        notCharNode->zero = node1;
        notCharNode->one = node2;
        Q.push(notCharNode);
    }

    return Q.top();  // TO DO: update this return
}

//
// *This function builds the encoding map from an encoding tree.
//

void recursiveUtil(mymap<int, string> &encodingMap,
                   HuffmanNode *current,
                   HuffmanNode *prev,
                   string code) {
    if (current == nullptr) {
        if (!encodingMap.contains(prev->character)) {
            encodingMap.put(prev->character, code);
        }
        return;
    }
    if (code == "?") {
        recursiveUtil(encodingMap, current->zero, current, "0");
    } else {
        if (current->zero != nullptr) {
            recursiveUtil(encodingMap,current->zero, current, code+'0');
        } else {
            recursiveUtil(encodingMap, current->zero, current, code);
        }
    }

    if (code == "?") {
        recursiveUtil(encodingMap, current->one, current, "1");
    } else {
        if (current->one != nullptr) {
            recursiveUtil(encodingMap, current->one, current, code+'1');
        } else {
            recursiveUtil(encodingMap, current->one, current, code);
        }
    }
}

mymap <int, string> buildEncodingMap(HuffmanNode* tree) {
    mymap <int, string> encodingMap;
    recursiveUtil(encodingMap, tree, tree, "?");
    return encodingMap;  // TO DO: update this return
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input,
              mymap <int, string> &encodingMap,
              ofbitstream& output,
              int &size, bool makeFile) {
    string encoded;
    char c;
    while (input.get(c)) {
        string code = encodingMap.get(c);
        encoded += code;
        if (makeFile) {
            for (int i = 0; i < code.length(); ++i) {
                if (code[i] == '0') { output.writeBit(0); }
                if (code[i] == '1') { output.writeBit(1); }
            }
        }
    }
    string code = encodingMap.get(PSEUDO_EOF);
    encoded += code;
    if (makeFile) {
        for (int i = 0; i < code.length(); ++i) {
            if (code[i] == '0') { output.writeBit(0); }
            if (code[i] == '1') { output.writeBit(1); }
        }
    }
    size = encoded.length();
    return encoded;  // TO DO: update this return
}


//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
    HuffmanNode *current = encodingTree;
    string decodedData;
    int bit = input.readBit();
    while (bit != -1) {
        if (bit == 0) {
            current = current->zero;
        } else {
            current = current->one;
        }
        if (current->one == nullptr && current->zero == nullptr) {
            if (current->character == 256) {
                output << decodedData;
                break;
            } else {
                decodedData += current->character;
                current = encodingTree;
            }
        }
        bit = input.readBit();
    }
    return decodedData;
}

//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
    // building
    hashmap map;
    buildFrequencyMap(filename, true, map);
    HuffmanNode *root = buildEncodingTree(map);
    mymap<int, string> encodingMap = buildEncodingMap(root);

    // encoding
    ifstream input(filename);
    ofbitstream output(filename + ".huf");
    output << map;
    int size = 0;
    string encodedData = encode(input, encodingMap, output, size, true);
    input.close();
    output.close();

    freeTree(root);
    return encodedData;  // TO DO: update this return
}

//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename) {
    ifbitstream input(filename);
    filename = filename.substr(0, filename.length()-8);
    filename += "_unc.txt";
    ofstream output(filename);
    hashmap map;
    input >> map;
    HuffmanNode *root = buildEncodingTree(map);

    string decodedData = decode(input, root, output);
    input.close();
    output.close();

    freeTree(root);
    return decodedData;
}
