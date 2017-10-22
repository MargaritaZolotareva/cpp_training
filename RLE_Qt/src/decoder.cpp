#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <iostream>
#include <decoder.h>

void decompressFile(const QString& inputFileName, const QString& outputFileName)
{
    QString content;
    qint64 decompressedSize, compressedSize;
    double compressionRatio;
    QFile inFile(inputFileName), outFile(outputFileName);

    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << " Could not open the file for reading";
        return;
    }

    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << " Could not open the file for writing";
        return;
    }

    QTextStream in(&inFile);
    while (!in.atEnd()) {
        content += in.readLine();
    }
    compressedSize = inFile.size();
    QString decodedContent = decode(content);

    QTextStream out(&outFile);
    out << decodedContent;

    decompressedSize = decodedContent.length();
    compressionRatio = decompressedSize / (double)compressedSize;
    std::cout << "Size of compressed file: " + std::to_string(compressedSize) + " bytes. \n";
    std::cout << "Size of decompressed file: " + std::to_string(decompressedSize) + " bytes. \n";
    std::cout << "Compression ratio: " + std::to_string(compressionRatio);

    inFile.close();
    outFile.close();
}

QString decode(const QString& content)
{
    QChar symbol;
    qint64 symCount = 1, count = 0;
    QString decodedString = "", symCountStr = content[0];
    if (!symCountStr[0].isDigit()) {
        std::cerr << "Encoded string must start with number \n";
    }
    for (qint32 i = 1; i < content.length(); i++) {
        symbol = content[i];
        if (symbol.isDigit()) {
            symCountStr += symbol;
        }
        else if (symbol.isLetter()) {
            count = symCountStr.toInt();
            for (qint32 i = 0; i < count; i++) {
                decodedString += symbol;
                symCountStr = "";
            }
        }
    }

    return decodedString;
}
