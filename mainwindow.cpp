#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCloseEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , modified(false)
{
    ui->setupUi(this);
    setCentralWidget(ui->textEdit);

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    connect(ui->textEdit, &QTextEdit::textChanged, this, &MainWindow::documentModified);

    connect(ui->actionCut, &QAction::triggered, ui->textEdit, &QTextEdit::cut);
    connect(ui->actionCopy, &QAction::triggered, ui->textEdit, &QTextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered, ui->textEdit, &QTextEdit::paste);
    connect(ui->actionSelect_All, &QAction::triggered, ui->textEdit, &QTextEdit::selectAll);

    updateEditActions();
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::updateEditActions);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newFile()
{
    if (!maybeSave()) return;

    ui->textEdit->clear();
    setCurrentFile(QString());
    modified = false;
}

void MainWindow::openFile()
{
    if (!maybeSave()) return;

    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Text files (*.txt);;All files (*)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Cannot open file");
        return;
    }

    QTextStream in(&file);
    ui->textEdit->setPlainText(in.readAll());
    file.close();

    setCurrentFile(fileName);
    modified = false;
    statusBar()->showMessage("Opened", 2000);
}

bool MainWindow::saveFile()
{
    if (currentFile.isEmpty())
        return saveFileAs();

    QFile file(currentFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Cannot save file");
        return false;
    }

    QTextStream out(&file);
    out << ui->textEdit->toPlainText();
    file.close();

    modified = false;
    statusBar()->showMessage("Saved", 2000);
    return true;
}

bool MainWindow::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save File As", "", "Text files (*.txt);;All files (*)");
    if (fileName.isEmpty()) return false;

    currentFile = fileName;
    return saveFile();
}

void MainWindow::documentModified()
{
    modified = true;
    updateEditActions();
}

bool MainWindow::maybeSave()
{
    if (!modified) return true;

    auto ret = QMessageBox::warning(this, "Warning", "Document has unsaved changes. Save now?",
                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (ret == QMessageBox::Yes)
        return saveFile();
    else if (ret == QMessageBox::Cancel)
        return false;

    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    currentFile = fileName;
    setWindowTitle(currentFile.isEmpty() ? "QuickPad" : currentFile);
}

void MainWindow::updateEditActions()
{
    bool hasSelection = ui->textEdit->textCursor().hasSelection();
    bool hasText = QApplication::clipboard()->mimeData()->hasText();

    ui->actionCut->setEnabled(hasSelection);
    ui->actionCopy->setEnabled(hasSelection);
    ui->actionPaste->setEnabled(hasText);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}
