#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QTextStream>
#include <encoder.h>
#include <decoder.h>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption inputDirectoryOption(QStringList() << "input-file",
        QCoreApplication::translate("main", "Gets data from <filename>."),
        QCoreApplication::translate("main", "filename"));
    parser.addOption(inputDirectoryOption);

    QCommandLineOption outputDirectoryOption(QStringList() << "output-file",
        QCoreApplication::translate("main", "Puts data to <filename>."),
        QCoreApplication::translate("main", "filename"));
    parser.addOption(outputDirectoryOption);

    QCommandLineOption encodeOption(QStringList() << "encode",
        QCoreApplication::translate("main", "Encodes content of input file."));
    parser.addOption(encodeOption);

    QCommandLineOption decodeOption(QStringList() << "decode",
        QCoreApplication::translate("main", "Decodes content of input file."));
    parser.addOption(decodeOption);

    parser.process(app);

    if (parser.isSet(inputDirectoryOption) && parser.isSet(outputDirectoryOption)) {
        QString inputFile = parser.value(inputDirectoryOption);
        QString outputFile = parser.value(outputDirectoryOption);
        if (parser.isSet(encodeOption)) {
            compressFile(inputFile, outputFile);
        }
        else if (parser.isSet(decodeOption)) {
            decompressFile(inputFile, outputFile);
        }
        else {
            qFatal("Option --encode or --decode must be specified");
            app.exit(1);
        }
    }
    else {
        qFatal("Input and output files must be specified");
        app.exit(1);
    }

    app.exit(0);
}
