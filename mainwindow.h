#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "px4_config_file.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_close_button_clicked();

    void on_save_button_clicked();

    void on_open_px4_directory_clicked();

    void on_mixer_channel_currentIndexChanged(int index);

    void on_mixer_value_table_cellChanged(int row, int column);

private:
    Ui::MainWindow *ui;
    QSettings* mSettings;
    int mMixerValues[8][4][4];
    bool mMixerValueValid[8][4][4];

    void UISettings();
    QString buildPWMScript();
    QString buildAutoStartScript(QString name, QString type);
    QString buildMixerScript();

    void storeSettingValues();
    void loadSettingValues();
};

#endif // MAINWINDOW_H
