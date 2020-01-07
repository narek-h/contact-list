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
    ui->treeView->setModel(model);
    ContactItemDelegate* delegate = new ContactItemDelegate(ui->treeView);
    ui->treeView->setItemDelegate(delegate);

    bool success = connect(ui->treeView,SIGNAL(clicked(const QModelIndex)),this,SLOT(itemClicked(QModelIndex)));
    Q_ASSERT(success);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::itemClicked (QModelIndex index )
{
    if (!index.parent().isValid()) {//Top item, no handling
        return;
    }

    ItemViewDialog* dialog = new ItemViewDialog(this);
    dialog->setData(index.data().toMap());
    dialog->show();
}
