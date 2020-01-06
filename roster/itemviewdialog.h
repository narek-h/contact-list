#ifndef ITEMVIEWDIALOG_H
#define ITEMVIEWDIALOG_H

#include <QDialog>

namespace Ui {
class ItemViewDialog;
}

class ItemViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ItemViewDialog(QWidget *parent = nullptr);
    ~ItemViewDialog();

    void setData(const QVariantMap&);


private:
    Ui::ItemViewDialog *ui;
};


#endif // ITEMVIEWDIALOG_H
