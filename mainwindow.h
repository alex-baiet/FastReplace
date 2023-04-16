#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private:
        Ui::MainWindow *ui;
        /**
         * @brief Create a dialog to choose a directory and fill a LineEdit with the choosen directory.
         */
        void setDirectory(QLineEdit* edit);
        /**
         * @brief Return true if path in line edit are correctly set to directories paths.
         */
        bool checkDirExists();
        /**
         * @brief Replace directory content in dest with content in start.
         */
        void startReplace();

    private slots:
        void on_btnStart_clicked();
        void on_btnSrc_clicked();
        void on_btnDest_clicked();
};
#endif // MAINWINDOW_H
