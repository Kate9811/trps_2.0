#ifndef SIMPLEDOC_PARSER_H
#define SIMPLEDOC_PARSER_H

#include <QObject>
#include <QString>
#include <QVector>

struct TextRun
{
    QString text;
    bool bold;
    bool italic;
    bool underline;
    bool isHeading;
    int headingLevel;
    bool isTable;
    int tableCols;
    QString fontName;
    int fontSize;
    QString alignment;
    double xPos;
};

class SimpleDocParser : public QObject
{
    Q_OBJECT

public:
    explicit SimpleDocParser(QObject *parent = nullptr);

    bool parse(const QString &filePath, QString &outText, QString &outFormatInfo);
    bool build(const QString &text, const QString &path);

private:
    // 🔴 ВОТ ЧЕГО У ТЕБЯ НЕ ХВАТАЛО
    bool parseTXT(const QString &filePath, QString &outText);
    bool parseDOCX(const QString &filePath, QString &outText);
    bool parsePDF(const QString &filePath, QString &outText);

    bool buildTXT(const QString &text, const QString &path);
    bool buildDOCX(const QString &text, const QString &path);
    bool buildPDF(const QString &text, const QString &path);

private:
    QVector<TextRun> m_runs;
    double m_pdfLeftMargin = 50.0;
};

#endif