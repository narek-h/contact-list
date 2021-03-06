#include "itemviewdialog.h"
#include "utils.h"
#include "ui_itemviewdialog.h"
#include <QDateTime>

ItemViewDialog::ItemViewDialog(QWidget *parent):
    QDialog(parent),
    ui(new Ui::ItemViewDialog)
{
    ui->setupUi(this);
    ui->verticalLayout->setAlignment(ui->okButton, Qt::AlignHCenter);
    bool success = connect(ui->okButton, SIGNAL(clicked()), this, SLOT(close()));
    Q_ASSERT(success);
}


ItemViewDialog::~ItemViewDialog()
{
    delete ui;
}

void ItemViewDialog::setData(const QVariantMap& data)
{
    QString firstName = data.value("firstName").toString();
    QString lastName = data.value("lastName").toString();
    QString fullName = firstName + " " + lastName;
    setWindowTitle(fullName);
    ui->username->setText(data.value("username").toString());
    ui->fullName->setText(fullName);
    ui->sex->setText(data.value("sex").toString());
    ui->country->setText(data.value("country").toString());

    //Draw avatar
    int size = 128;
    QImage image(size,size,QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter paint;
    paint.begin(&image);
    utils::paintAvatar(paint,
                       size,
                       QPoint(size/2,size/2),
                       utils::makeInitials(firstName,lastName),
                       utils::getColorForSex(data.value("sex").toString()),
                       Qt::blue);
    paint.end();
    ui->avatar->setPixmap(QPixmap::fromImage(image));

    //Parse birthday
    //TODO: clarify birthday format. For now just adding birthday2 field if exists, otherwise using birthday as is.
    QString birthday;
    QVariantMap birthday2 = data.value("birthday2").toMap();
    if (!birthday2.isEmpty()) {
        int day = birthday2.value("day").toInt();
        int month = birthday2.value("month").toInt();
        int year = birthday2.value("year").toInt();
        QDateTime date;
        date.setDate(QDate(year, month, day));
        if (date.isValid()) {
            birthday = date.date().toString(Qt::ISODate);
        }
    }

    if (birthday.isEmpty()) {
        QDateTime date = QDateTime::fromSecsSinceEpoch(data.value("birthday").toLongLong());
        //if (date.isEm)
        if (date.isValid()) {
            birthday = date.date().toString(Qt::ISODate);
        }
    }


    ui->birthdate->setText(birthday);
}
