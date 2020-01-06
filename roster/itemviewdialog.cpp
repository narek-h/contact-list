#include "itemviewdialog.h"
#include "ui_itemviewdialog.h"
#include <QDate>

ItemViewDialog::ItemViewDialog(QWidget *parent):
    QDialog(parent),
    ui(new Ui::ItemViewDialog)
{
    ui->setupUi(this);
    bool success = connect(ui->okButton, SIGNAL(clicked()), this, SLOT(close()));
    Q_ASSERT(success);
}


ItemViewDialog::~ItemViewDialog()
{
    delete ui;
}

void ItemViewDialog::setData(const QVariantMap& data)
{
    QString fullName = data.value("firstName").toString() + " " + data.value("lastName").toString();
    setWindowTitle(fullName);
    ui->username->setText(data.value("username").toString());
    ui->fullName->setText(fullName);
    ui->sex->setText(data.value("sex").toString());
    ui->country->setText(data.value("country").toString());

    //Parse birthday
    //TODO: clarify birthday format. For now just adding birthday2 field if exists, otherwise using birthday as is.
    QString birthday;
    QVariantMap birthday2 = data.value("birthday2").toMap();
    if (!birthday2.isEmpty()) {
        int day = birthday2.value("day").toInt();
        int month = birthday2.value("month").toInt();
        int year = birthday2.value("year").toInt();
        QDate date(year, month, day);
        if (!data.isEmpty()) {
            birthday = date.toString(Qt::ISODate);
        }
    }

    if (birthday.isEmpty()) {
        birthday = data.value("birthday").toString();
    }

    ui->birthdate->setText(birthday);
}
