#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <QFileDialog>
#include <QLineEdit>
#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setDirectory(QLineEdit* edit) {
    QFileDialog dialog;
    QString str = dialog.getExistingDirectory();
    edit->setText(str);
}

bool MainWindow::checkDirExists() {
    const QString src = ui->editSrc->text();
    const QString dest = ui->editDest->text();
    const QDir srcDir(src);
    const QDir destDir(dest);
    if (src.size() == 0 || dest.size() == 0) return false;
    if (!srcDir.exists()) return false;
    if (!destDir.exists()) return false;
    return true;
}

int MainWindow::countFiles(const QDir& dir) const {
    int count = 0;
    foreach (QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        if (info.isFile()) count++;
        if (info.isDir()) count += countFiles(QDir(info.absoluteFilePath()));
    }
    return count;
}

void MainWindow::removeExcessFiles(const QDir& srcDir, const QDir& destDir) {
    foreach (QFileInfo info, destDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        if (info.isFile()) {
            // File case
            if (!srcDir.exists(info.fileName())) {
                // Remove file
                QFile::remove(info.absoluteFilePath());
                qInfo() << "Removed" << info.absoluteFilePath();
            }
            ui->progressBar->setValue(ui->progressBar->value() + 1);

        } else {
            // Directory case
            if (!srcDir.exists(info.fileName())) {
                // Remove directory
                QDir dir(info.absoluteFilePath());
                int count = countFiles(dir);
                dir.removeRecursively();
                qInfo() << "Removed recursively" << info.absoluteFilePath();
                ui->progressBar->setValue(ui->progressBar->value() + count);
            } else {
                // Recursively remove in sub-directory
                QDir srcChild(srcDir.absoluteFilePath(info.fileName()));
                QDir destChild(info.absoluteFilePath());
                removeExcessFiles(srcChild, destChild);
            }
        }
    }
}

void MainWindow::addMissingFiles(const QDir& srcDir, const QDir& destDir) {
    foreach (QFileInfo info, srcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        if (info.isFile()) {
            // File case
            if (!destDir.exists(info.fileName())) {
                // copy file
                QFile::copy(info.absoluteFilePath(), destDir.absoluteFilePath(info.fileName()));
                qInfo() << "Copied" << info.absoluteFilePath();
            }
            ui->progressBar->setValue(ui->progressBar->value() + 1);

        } else {
            // Directory case
            if (!destDir.exists(info.fileName())) {
                // Create empty directory
                destDir.mkdir(info.fileName());
            }
            // Recursively copy in sub-directory
            QDir srcChild(info.absoluteFilePath());
            QDir destChild(destDir.absoluteFilePath(info.fileName()));
            addMissingFiles(srcChild, destChild);
        }
    }}


void MainWindow::on_btnStart_clicked() {
    qInfo() << "Started copy";
    if (!checkDirExists()) {
        ui->labelProcess->setText("A directory path is incorrect or empty.");
        qWarning() << "Wrong path : copy aborted";
        return;
    }

    ui->labelProcess->setText("Replacing in progress...");

    // Init variables
    const QDir srcDir(ui->editSrc->text());
    const QDir destDir(ui->editDest->text());
    QString fileName;
    const int stepCount = countFiles(srcDir) + countFiles(destDir);
    ui->progressBar->setMaximum(stepCount);

    // Remove and add files
    removeExcessFiles(srcDir, destDir);
    addMissingFiles(srcDir, destDir);

    // End
    qInfo() << "Copy finished successfully";
    ui->labelProcess->setText("Replacing finished.");
}

void MainWindow::on_btnSrc_clicked() {
    setDirectory(ui->editSrc);
}

void MainWindow::on_btnDest_clicked() {
    setDirectory(ui->editDest);
}

