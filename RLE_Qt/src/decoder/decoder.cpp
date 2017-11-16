#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <iostream>
#include <decoder.h>

void decompressFile(const QString &inputFileName, const QString &outputFileName)
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
    qDebug() << "Size of compressed file: " + QString::number(compressedSize) + " bytes. \n";
    qDebug() << "Size of decompressed file: " + QString::number(decompressedSize) + " bytes. \n";
    qDebug() << "Compression ratio: " + QString::number(compressionRatio);

    inFile.close();
    outFile.close();
}

QString decode(const QString &content)
{
    QChar symbol;
    qint64 symCount = 1, count = 0;
    if(content.length() == 0) {
        return "";
    }
    QString decodedString = "", symCountStr = content[0];
    if (!symCountStr[0].isDigit()) {
        qFatal("Encoded string must start with number \n");
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
