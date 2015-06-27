#ifndef PX4_CONFIG_FILE_H__
#define PX4_CONFIG_FILE_H__

#include <QString>

class config_file
{
    QString mCopterName, mPX4FirmwareDir;
    QString mCopterId;
public:
    config_file(QString PX4FirmwareDir, int CopterId, QString CopterName):
        mPX4FirmwareDir(PX4FirmwareDir), mCopterName(CopterName), mCopterId(QString::number(CopterId)){}
//    ~config_file();

    void updateRCdotAutoStart();
    void updateRCdotMC_Apps();
    void createMixerFile(QString MixerContent);
    void createPWMScript(QString PWMContent);
    void createStartScript(QString ScriptContent);
    QString getMixerFileContent();
    QString getPWMScriptContent();
    QString getStartScriptContent();

private:
    void createFile(QString content, QString pathname);
    QString readFile(QString pathname);
    void updateCodeAfterSepLine(QString content, QString pathname);
};

#endif
