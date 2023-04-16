#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <QFileDialog>
#include <QLineEdit>
#include <filesystem>
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

void MainWindow::on_btnStart_clicked() {
    qInfo() << "Started copy";
    if (!checkDirExists()) {
        ui->labelProcess->setText("Le chemin des dossiers est incorrect ou incomplet.");
        qWarning() << "Wrong path : copy aborted";
        return;
    }

    ui->labelProcess->setText("Copie en cours...");

    // Init variables
    const QDir srcDir(ui->editSrc->text());
    const QDir destDir(ui->editDest->text());
    QString fileName;
    const int stepCount = srcDir.entryList(QDir::Files).count() + destDir.entryList(QDir::Files).count();
    int currentStep = 0;
    ui->progressBar->setMaximum(stepCount);

    // Remove excess files
    foreach (fileName, destDir.entryList(QDir::Files)) {
        if (!srcDir.exists(fileName)) {
            qInfo() << "Removed" << fileName;
            QFile::remove(destDir.absoluteFilePath(fileName));
        }
        ui->progressBar->setValue(++currentStep);
    }

    // Add missing files
    foreach (fileName, srcDir.entryList(QDir::Files)) {
        if (!destDir.exists(fileName)) {
            qInfo() << "Copied" << fileName;
            QFile::copy(srcDir.absoluteFilePath(fileName), destDir.absoluteFilePath(fileName));
        }
        ui->progressBar->setValue(++currentStep);
    }

    qInfo() << "Copy finished successfully";
    ui->labelProcess->setText("Copie terminé.");
}

void MainWindow::on_btnSrc_clicked() {
    setDirectory(ui->editSrc);
}

void MainWindow::on_btnDest_clicked() {
    setDirectory(ui->editDest);
}

