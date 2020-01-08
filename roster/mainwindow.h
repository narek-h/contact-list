#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

class QDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void itemClicked (QModelIndex index);

private:
    Ui::MainWindow *ui;
    QMap<int, QDialog*> openDialogs;
};

#endif // MAINWINDOW_H
