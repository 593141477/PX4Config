#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mSettings = new QSettings("ThuSkyworks", "PX4Config", this);
    UISettings();
    loadSettingValues();

    on_mixer_channel_currentIndexChanged(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UISettings()
{
    ui->copter_type->addItem("Multicopter", QString("mc"));
    ui->copter_type->addItem("Fixed-Wing", QString("fw"));
    ui->copter_type->setCurrentIndex(0);
    ui->pwm_value_table->setSpan(0, 4, 4, 1);
    ui->pwm_value_table->setSpan(4, 4, 4, 1);
    for(int i=1; i<=8; i++)
        ui->mixer_channel->addItem(QString::number(i), i);
    ui->mixer_channel->setCurrentIndex(0);

    ui->board_rotation->addItem("No rotation", 0);
    ui->board_rotation->addItem("Yaw 45°", 1);
    ui->board_rotation->addItem("Yaw 90°", 2);
    ui->board_rotation->addItem("Yaw 135°", 3);
    ui->board_rotation->addItem("Yaw 180°", 4);
    ui->board_rotation->addItem("Yaw 225°", 5);
    ui->board_rotation->addItem("Yaw 270°", 6);
    ui->board_rotation->addItem("Yaw 315°", 7);
    ui->board_rotation->addItem("Roll 180°", 8);
    ui->board_rotation->addItem("Roll 180°, Yaw 45°", 9);
    ui->board_rotation->addItem("Roll 180°, Yaw 90°", 10);
    ui->board_rotation->addItem("Roll 180°, Yaw 135°", 11);
    ui->board_rotation->addItem("Pitch 180°", 12);
    ui->board_rotation->addItem("Roll 180°, Yaw 225°", 13);
    ui->board_rotation->addItem("Roll 180°, Yaw 270°", 14);
    ui->board_rotation->addItem("Roll 180°, Yaw 315°", 15);
    ui->board_rotation->addItem("Roll 90°", 16);
    ui->board_rotation->addItem("Roll 90°, Yaw 45°", 17);
    ui->board_rotation->addItem("Roll 90°, Yaw 90°", 18);
    ui->board_rotation->addItem("Roll 90°, Yaw 135°", 19);
    ui->board_rotation->addItem("Roll 270°", 20);
    ui->board_rotation->addItem("Roll 270°, Yaw 45°", 21);
    ui->board_rotation->addItem("Roll 270°, Yaw 90°", 22);
    ui->board_rotation->addItem("Roll 270°, Yaw 135°", 23);
    ui->board_rotation->addItem("Pitch 90°", 24);
    ui->board_rotation->addItem("Pitch 270°", 25);
    ui->board_rotation->setCurrentIndex(0);

    ui->safety_switch->addItem("Enabled", 0);
    ui->safety_switch->addItem("Disabled", 22027);
    ui->safety_switch->setCurrentIndex(0);
}

void MainWindow::on_close_button_clicked()
{
    close();
}

void MainWindow::on_save_button_clicked()
{
    QString message;
    QString PX4Dir;
    QString copterName, copterType;
    int copterId;
    bool validId;

    storeSettingValues();

    PX4Dir = ui->px4_directory->text();
    copterId = ui->copter_id->text().toInt(&validId);
    copterName = ui->copter_name->text();
    copterType = ui->copter_type->itemData(ui->copter_type->currentIndex()).toString();

    if(PX4Dir.isEmpty()){
        QMessageBox::warning(this, "Warning", "PX4 directory is empty");
        return;
    }
    if(!validId){
        QMessageBox::warning(this, "Warning", "Invalid Copter ID");
        return;
    }
    if(copterName.isEmpty()){
        QMessageBox::warning(this, "Warning", "Copter Name is empty");
        return;
    }

    message += "Configure saved.\n";

    try{
        config_file file(PX4Dir, copterId, copterName);
        file.createPWMScript(buildPWMScript());
        file.createStartScript(buildAutoStartScript(copterName, copterType));
        file.createMixerFile(buildMixerScript());
        file.updateRCdotAutoStart();
        file.updateRCdotMC_Apps();
        message += "Script file updated.";
    }catch(QString *s){
        message += s->toStdString().c_str();
        delete s;
    }


    QMessageBox::information(this, "Info", message);
}

QString MainWindow::buildPWMScript()
{
    QString content;
    QTextStream sm(&content);
    QAbstractItemModel* model = ui->pwm_value_table->model();
    int value;
    bool valid;

    value = model->data(model->index(0, 4)).toInt(&valid);
    if(valid)
        sm << "pwm rate -c 1234 rate PWM " << QString::number(value) << "\n";
    value = model->data(model->index(4, 4)).toInt(&valid);
    if(valid)
        sm << "pwm rate -c 5678 rate PWM " << QString::number(value) << "\n";

    for(int i=0; i<8; i++){
        value = model->data(model->index(i, 0)).toInt(&valid);
        if(valid)
            sm << "pwm min -c " << i+1 << " -p " << QString::number(value) << "\n";
        value = model->data(model->index(i, 1)).toInt(&valid);
        if(valid)
            sm << "pwm max -c " << i+1 << " -p " << QString::number(value) << "\n";
        value = model->data(model->index(i, 2)).toInt(&valid);
        if(valid)
            sm << "pwm disarmed -c " << i+1 << " -p " << QString::number(value) << "\n";
        value = model->data(model->index(i, 3)).toInt(&valid);
        if(valid)
            sm << "pwm failsafe -c " << i+1 << " -p " << QString::number(value) << "\n";
    }
    return content;
}

QString MainWindow::buildAutoStartScript(QString name, QString type)
{
    QString content = "#!nsh\n\
#\n\
# %1\n\
#\n\
#\n\
\n\
set VEHICLE_TYPE %2\n\
set MIXER %1\n\
\n\
set PWM_RATE 50\n\
set PWM_DISARMED 1500\n\
set PWM_MIN 1300\n\
set PWM_MAX 1700\n\
set PWM_OUT 12345678\n\
\n\
param set CBRK_IO_SAFETY %3\n\
param set SENS_BOARD_ROT %4\n\
\n";
    return content.arg(name, type, ui->safety_switch->currentData().toString(), ui->board_rotation->currentData().toString());
}

QString MainWindow::buildMixerScript()
{
    QString content;
    QTextStream sm(&content);
    int value;

    for (int ch = 0; ch < 8; ++ch)
    {
        QString line_buf;
        QTextStream sm_line(&line_buf);
        int line_cnt = 0;
        for(int i=0; i<4; i++){
            QString line = QString("S: 0");
            for(int j=0; j<4; j++){
                value = mMixerValues[ch][i][j];
                if(!mMixerValueValid[ch][i][j])
                    goto ignore_line;
                line += ' ';
                line += QString::number(value);
            }
            line += " -10000  10000\n";
            sm_line << line;
            line_cnt++;
            continue;
ignore_line:;
        }
        if(line_cnt > 0){
            sm << "M: " << line_cnt << "\n";
            sm << "O: 10000  10000  0 -10000  10000\n";
            sm << line_buf;
        }else{
            sm << "Z:\n";
        }
    }
    return content;
}

void MainWindow::storeSettingValues()
{
    QAbstractItemModel* model;

    mSettings->setValue("px4_directory",ui->px4_directory->text());

    mSettings->beginGroup("General");
    mSettings->setValue("copter_id", ui->copter_id->text());
    mSettings->setValue("copter_name", ui->copter_name->text());
    mSettings->setValue("copter_type", ui->copter_type->currentIndex());
    mSettings->endGroup();

    mSettings->beginGroup("PWM");
    model = ui->pwm_value_table->model();
    for(int i=0; i<model->rowCount(); i++)
        for(int j=0; j<model->columnCount(); j++){
            mSettings->setValue(QString::number(i)+','+QString::number(j), model->data(model->index(i,j)));
    }
    mSettings->endGroup();

    mSettings->beginGroup("Mixer");
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            for (int k = 0; k < 4; ++k)
            {
                mSettings->setValue(QString::number(i)+','+QString::number(j)+','+QString::number(k), 
                    mMixerValueValid[i][j][k] ? mMixerValues[i][j][k] : QVariant());
            }
        }
    }
    mSettings->endGroup();

    mSettings->beginGroup("Option");
    mSettings->setValue("safety_switch",ui->safety_switch->currentIndex());
    mSettings->setValue("board_rotation",ui->board_rotation->currentIndex());
    mSettings->endGroup();
}

void MainWindow::loadSettingValues()
{
    QAbstractItemModel* model;

    ui->px4_directory->setText(mSettings->value("px4_directory").toString());

    mSettings->beginGroup("General");
    ui->copter_id->setText(mSettings->value("copter_id").toString());
    ui->copter_name->setText(mSettings->value("copter_name").toString());
    ui->copter_type->setCurrentIndex(mSettings->value("copter_type").toInt());
    mSettings->endGroup();

    mSettings->beginGroup("PWM");
    model = ui->pwm_value_table->model();
    for(int i=0; i<model->rowCount(); i++)
        for(int j=0; j<model->columnCount(); j++){
            model->setData(model->index(i,j), mSettings->value(QString::number(i)+','+QString::number(j)));
    }
    mSettings->endGroup();

    mSettings->beginGroup("Mixer");
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            for (int k = 0; k < 4; ++k)
            {
                mMixerValues[i][j][k] =
                    mSettings->value(QString::number(i)+','+QString::number(j)+','+QString::number(k)).toInt(&mMixerValueValid[i][j][k]);
            }
        }
    }
    mSettings->endGroup();

    mSettings->beginGroup("Option");
    if(mSettings->value("safety_switch").isValid())
        ui->safety_switch->setCurrentIndex(mSettings->value("safety_switch").toInt());
    if(mSettings->value("board_rotation").isValid())
        ui->board_rotation->setCurrentIndex(mSettings->value("board_rotation").toInt());
    mSettings->endGroup();
}

void MainWindow::on_open_px4_directory_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    "Open Directory",
                                                    "",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                                                  );
    ui->px4_directory->setText(dir);
}

void MainWindow::on_mixer_channel_currentIndexChanged(int index)
{
    QAbstractItemModel* model;
    model = ui->mixer_value_table->model();
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++){
            model->setData(model->index(i,j), mMixerValueValid[index][i][j] ? QString::number(mMixerValues[index][i][j]) : QVariant());
        }

}

void MainWindow::on_mixer_value_table_cellChanged(int row, int column)
{
    QAbstractItemModel* model;
    model = ui->mixer_value_table->model();
    mMixerValues[ui->mixer_channel->currentIndex()][row][column] =
            model->data(model->index(row, column)).toInt(&mMixerValueValid[ui->mixer_channel->currentIndex()][row][column]);
}
