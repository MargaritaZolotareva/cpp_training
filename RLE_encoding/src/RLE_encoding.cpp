#include <iostream>
#include <fstream>
using namespace std;

int getFileSize(fstream& streamName);
string encode(string content);

int main()
{
    string content, symbol;
    int uncompressedSize, compressedSize;
    double compressionRatio;
    fstream inFile("input.txt", ios::in), outFile("output.txt", ios::out);

    if (!inFile.is_open()) {
        cerr << "Unable to open file input.txt";
        exit(1);
    }
    if (!outFile.is_open()) {
        cerr << "Unable to open file output.txt";
        exit(1);
    }

    uncompressedSize = getFileSize(inFile);
    while (inFile >> symbol) {
        content += symbol;
    }
    string encodedContent = encode(content);
    outFile << encodedContent;
    outFile.flush();

    compressedSize = encodedContent.length();
    compressionRatio = uncompressedSize / (double)compressedSize;
    cout << "Size of uncompressed file: " + to_string(uncompressedSize) + " bytes. \n";
    cout << "Size of compressed file: " + to_string(compressedSize) + " bytes. \n";
    cout << "Compression ratio: " + to_string(compressionRatio);

    inFile.close();
    outFile.close();

    return 0;
}

int getFileSize(fstream& file)
{
    file.seekg(0, ios::end);
    int fileSize = file.tellg();
    file.seekg(0, ios::beg);

    return fileSize;
}

string encode(string content)
{
    char prevSymbol = content[0], nextSymbol;
    int symCount = 1;
    string encodedString = "";
    if (content.length() == 1) {
        return to_string(symCount) + content[0];
    }
    for (unsigned int i = 1; i < content.length(); i++) {
        nextSymbol = content[i];
        if (nextSymbol == prevSymbol) {
            symCount++;
            prevSymbol = nextSymbol;
        }
        else {
            encodedString += to_string(symCount) + prevSymbol;
            prevSymbol = nextSymbol;
            symCount = 1;
        }
        if (i == content.length() - 1) {
            encodedString += to_string(symCount) + prevSymbol;
        }
    }

    return encodedString;
}
