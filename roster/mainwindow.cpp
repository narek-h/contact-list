#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "contactitemdelegate.h"
#include "contactlistmodel.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ContactListModel* model = new ContactListModel();
    ui->listView->setModel(model);
    ContactItemDelegate* delegate = new ContactItemDelegate(ui->listView);
    ui->listView->setItemDelegate(delegate);
}

MainWindow::~MainWindow()
{
    delete ui;
}
