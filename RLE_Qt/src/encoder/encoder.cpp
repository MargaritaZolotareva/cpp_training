#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <iostream>
#include <encoder.h>

void compressFile(const QString &inputFileName, const QString &outputFileName)
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

    QTextStream inputStream(&inFile);
    while (!inputStream.atEnd()) {
        content += inputStream.readLine();
    }
    decompressedSize = inFile.size();
    QString encodedContent = encode(content);

    QTextStream outputStream(&outFile);
    outputStream << encodedContent;

    compressedSize = encodedContent.length();
    compressionRatio = decompressedSize / (double)compressedSize;
    qDebug() << "Size of decompressed file: " + QString::number(decompressedSize) + " bytes. \n";
    qDebug() << "Size of compressed file: " + QString::number(compressedSize) + " bytes. \n";
    qDebug() << "Compression ratio: " + QString::number(compressionRatio);

    inFile.close();
    outFile.close();
}

QString encode(const QString &content)
{
    QString encodedString = "";
    if(content.length() == 0) {
        return encodedString;
    }
    QChar prevSymbol = content[0], nextSymbol;
    qint64 symCount = 1;
    if (content.length() == 1) {
        return QString::number(symCount) + content[0];
    }
    for (qint32 i = 1; i < content.length(); i++) {
        nextSymbol = content[i];
        if (nextSymbol == prevSymbol) {
            symCount++;
            prevSymbol = nextSymbol;
        }
        else {
            encodedString += QString::number(symCount) + prevSymbol;
            prevSymbol = nextSymbol;
            symCount = 1;
        }
        if (i == content.length() - 1) {
            encodedString += QString::number(symCount) + prevSymbol;
        }
    }

    return encodedString;
}
