#include "px4_config_file.h"
#include <QFile>
#include <QDebug>

const char *SEP_LINE = "#===========$$$===========#\n";

void config_file::createPWMScript(QString PWMContent)
{
    createFile(PWMContent, QString("ROMFS/px4fmu_common/init.d/pwm.%1_%2").arg(mCopterId,mCopterName));
}

void config_file::createStartScript(QString ScriptContent)
{
    createFile(ScriptContent, QString("ROMFS/px4fmu_common/init.d/%1_%2").arg(mCopterId,mCopterName));
}

void config_file::createMixerFile(QString MixerContent)
{
    createFile(MixerContent, QString("ROMFS/px4fmu_common/mixers/%1.main.mix").arg(mCopterName));
}

QString config_file::getPWMScriptContent()
{
    return readFile(QString("ROMFS/px4fmu_common/init.d/pwm.%1_%2").arg(mCopterId,mCopterName));
}

QString config_file::getStartScriptContent()
{
    return readFile(QString("ROMFS/px4fmu_common/init.d/%1_%2").arg(mCopterId,mCopterName));
}

QString config_file::getMixerFileContent()
{
    return readFile(QString("ROMFS/px4fmu_common/mixers/%1.main.mix").arg(mCopterName));
}

void config_file::createFile(QString content, QString pathname)
{
    qDebug() << "createFile:" << pathname;
    QFile file(mPX4FirmwareDir + '/' + pathname);
    if(!file.open(QIODevice::WriteOnly)){
        qDebug() << "Failed to open";
        throw new QString("Failed to write "+mPX4FirmwareDir + '/' + pathname);
        return;
    }
    file.write(content.toStdString().c_str(), content.size());
    file.close();
}

QString config_file::readFile(QString pathname)
{
    qDebug() << "readFile:" << pathname;
    QFile file(mPX4FirmwareDir+ '/' + pathname);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "Failed to open";
        throw new QString("Failed to read "+mPX4FirmwareDir + '/' + pathname);
        return "";
    }
    QString s(file.readAll());
    file.close();

    return s;
}

void config_file::updateCodeAfterSepLine(QString content, QString pathname)
{
    QString str = readFile(pathname);
    int index = str.indexOf(SEP_LINE);
    if(index != -1){
        str.truncate(index);
    }
    str.append(SEP_LINE);
    str.append("#Added by PX4Config, DO NOT EDIT!\n");
    str.append(content);
    createFile(str, pathname);
}

void config_file::updateRCdotAutoStart()
{
    const char* tpl =
"if param compare SYS_AUTOSTART %1\n\
then\n\
    sh /etc/init.d/%1_%2\n\
fi\n\
";

    updateCodeAfterSepLine(QString(tpl).arg(mCopterId, mCopterName),QString("ROMFS/px4fmu_common/init.d/rc.autostart"));
}

void config_file::updateRCdotMC_Apps(QString CopterType, bool remove)
{
    const char* tpl =
"if [ $MIXER == %2 ]\n\
    then\n\
    sh /etc/init.d/pwm.%1_%2\n\
fi\n\
";

    QString name = QString("ROMFS/px4fmu_common/init.d/rc.%1_apps").arg(CopterType);
    updateCodeAfterSepLine(
                remove ? "" : QString(tpl).arg(mCopterId, mCopterName),
                name
                );
}
