#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void openFile();
    bool saveFile();
    bool saveFileAs();
    void documentModified();
    void updateEditActions();

private:
    Ui::MainWindow *ui;
    QString currentFile;
    bool modified;

    bool maybeSave();
    void setCurrentFile(const QString &fileName);
};

#endif // MAINWINDOW_H
