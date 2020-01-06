#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "contactitemdelegate.h"
#include "contactlistmodel.h"
#include "itemviewdialog.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ContactListModel* model = new ContactListModel();
    ui->listView->setModel(model);
    ContactItemDelegate* delegate = new ContactItemDelegate(ui->listView);
    ui->listView->setItemDelegate(delegate);

    bool success = connect(ui->listView,SIGNAL(clicked(const QModelIndex)),this,SLOT(itemClicked(QModelIndex)));
    Q_ASSERT(success);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::itemClicked (QModelIndex index )
{
    QString data = index.data().toString();//index.data(Qt::UserRole).value<QString>();
    qDebug() <<" Data " << index.data().toMap().value("firstName").toString();
    ItemViewDialog* dialog = new ItemViewDialog(this);
    dialog->setData(index.data().toMap());
    dialog->show();
}
