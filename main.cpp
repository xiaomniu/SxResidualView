#include "CMainWindow.h"

#include <QtWidgets/QApplication>

#include <QtGui/QFontDatabase>
#include <QtCore/QtDebug>
#include <QtCore/QFile>

#include <QtCore/QTranslator>
#include <QtCore/QTextCodec>

#include "Src_Core/CGlobal.h"

QString loadFontFamilyFromFiles(const QString &fontFileName)
{
    static QHash<QString, QString> tmd;
    if (tmd.contains(fontFileName)) {
        return tmd.value(fontFileName);
    }
    QString font = "";
    QFile fontFile(fontFileName);
    if(!fontFile.open(QIODevice::ReadOnly)) {
        qDebug()<<"Open font file error";
        return font;
    }

    int loadedFontID = QFontDatabase::addApplicationFontFromData(fontFile.readAll());
    QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(loadedFontID);
    if(!loadedFontFamilies.empty()) {
        font = loadedFontFamilies.at(0);
    }
    fontFile.close();

    if (!(font.isEmpty()))
        tmd.insert(fontFileName, font);
    return font;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    std::string sExeFullPath = QCoreApplication::applicationFilePath().toLocal8Bit().data();
    std::string sExeDir = sExeFullPath.substr(0, sExeFullPath.rfind('/'));//applicationDirPath

    GetGlobalPtr()->m_sExeDir = sExeDir;

    QIcon iconState((sExeDir+"/App1.png").c_str());
    a.setWindowIcon(iconState);

    QTextCodec *code = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForLocale(code);

    QTranslator translator;
    bool bTransOk = translator.load(QString((sExeDir+"/SuperXView01_CN.qm").c_str()));//res_rc
    bTransOk = a.installTranslator(&translator);

    QString sFontFamily = loadFontFamilyFromFiles((sExeDir+"/PingFang Medium.ttf").c_str());//res_rc
    QFont f(sFontFamily, 11);
    a.setFont(f);

    CGlobal* pGlobal = GetGlobalPtr();
    pGlobal->m_pQApplication = &a;
    pGlobal->m_pFont = &f;

    CMainWindow w;
    w.show();
    return a.exec();
}
