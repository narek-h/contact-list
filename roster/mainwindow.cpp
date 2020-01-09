#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "contactitemdelegate.h"
#include "contactlistmodel.h"
#include "itemviewdialog.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ContactListModel* model = new ContactListModel();
    model->mView = ui->treeView;
    ui->treeView->setModel(model);
    ContactItemDelegate* delegate = new ContactItemDelegate(ui->treeView);
    ui->treeView->setItemDelegate(delegate);

    bool success = connect(ui->treeView,SIGNAL(clicked(const QModelIndex)),this,SLOT(itemClicked(QModelIndex)));
    Q_ASSERT(success);

    success = connect(ui->lineEdit, SIGNAL(textChanged(const QString&)), model, SLOT(handleFilterTextChanged(const QString&)));
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
    if (openDialogs.contains(index.row())) { // already opened
        openDialogs.value(index.row())->activateWindow();
        return;
    }

    ItemViewDialog* dialog = new ItemViewDialog(this);
    connect(dialog, &QDialog::finished, this , [=](int) { openDialogs.remove(index.row()); });
    openDialogs.insert(index.row(), dialog);
    dialog->setData(index.data().toMap());
    dialog->show();
}
