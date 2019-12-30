#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "contactlistmodel.h"
MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ContactListModel* model = new ContactListModel();
    ui->listView->setModel(model);

}

MainWindow::~MainWindow()
{
    delete ui;
}
