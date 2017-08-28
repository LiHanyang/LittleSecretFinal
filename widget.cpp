#include "widget.h"
#include "ui_widget.h"

extern int id;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    rootItemModel(new QStandardItemModel(this)),
    accountItemModel(new QStandardItemModel(this)),
    rootCount(0),
    accountCount(0)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize(800,550);
    setUi();
    setWindow();
    setRootTable();
    setAccountTable();
    ui->accountTable->installEventFilter(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent * event)
{
    if(event->button() == Qt::LeftButton)
    {
        mouse_press = true;
    }
    move_point = event->globalPos() - this->pos();
}

void Widget::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(mouse_press)
    {
        QPoint move_pos = event->globalPos();
        move(move_pos - move_point);
    }
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->accountTable)
    {
        if (event->type() == QEvent::ToolTip) {
                 QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
                 int index =  ui->accountTable->rowAt(helpEvent->pos().y());
                 if (index != -1) {
                     QString _catalogue = accountItemModel->item(index)->text();
                     QString s = QString("select * from user where name=='%1' and catalogue_1=='%2' and catalogue_2=='%3' ").
                             arg(orcWidget.Code(userAccount)).arg(orcWidget.Code(catalogue_1)).arg(orcWidget.Code(_catalogue));
                     QSqlQuery query;
                     query.exec(s);
                     if(query.first())
                     {
                         QDateTime currentTime = QDateTime::currentDateTime();
                         QDateTime dateTime = QDateTime::fromString(orcWidget.Code(query.value(6).toString()), "yyyy-MM-dd hh:mm:ss");
                         if(dateTime.addSecs(15).secsTo(currentTime) > 0)
                         {
                             QToolTip::showText(helpEvent->globalPos(), tr("You have kept this password for 1 year. Please modify it."));
                         }
                     }
                 } else {
                     QToolTip::hideText();
                     event->ignore();
                 }
                 return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void Widget::getAccount(QString account)
{
    userAccount = account;
}

void Widget::getPassword(QString password)
{
    userPassword = password;
}

void Widget::firstShow()
{
    accountItemModel->clear();
    rootCount = 0;
    QString s = QString("select * from user");
    QSqlQuery query;
    query.exec(s);
    if(query.first())
    {
        do
        {
            if((userAccount == orcWidget.Code(query.value(1).toString())) && (NULL != orcWidget.Code(query.value(3).toString())))
            {
                rootItemModel->setItem(rootCount++, 0, new QStandardItem(orcWidget.Code(query.value(3).toString())));
                QColor nicePurple(200, 160, 255, 125);
                rootItemModel->item(rootCount - 1)->setBackground(nicePurple);
            }

            int i = 0;
            while(i + 1 < rootCount)
            {
                if(rootItemModel->item(i)->text() == rootItemModel->item(rootCount - 1)->text())
                {
                    rootItemModel->removeRow(rootCount - 1);
                    --rootCount;
                }
                ++i;
            }
        }
        while(query.next());
    }
}

void Widget::setUi()
{
    ui->setupUi(this);
    ui->accountEdit->setEnabled(false);
    ui->passwordEdit->setEnabled(false);
    ui->urlEdit->setEnabled(false);
    ui->saveButton->setEnabled(false);
    ui->rootDeleteButton->setEnabled(false);
    ui->accountDeleteButton->setEnabled(false);
    ui->searchCata_2->setEnabled(false);
    ui->accountAddButton->setEnabled(false);
}


void Widget::setWindow()
{
    setWindowTitle(" ");
}

void Widget::setRootTable()
{
    ui->rootTable->setModel(rootItemModel);
    ui->rootTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->rootTable->horizontalHeader()->setStretchLastSection(true);
    ui->rootTable->horizontalHeader()->setVisible(false);
    ui->rootTable->verticalHeader()->setVisible(false);
    ui->rootTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void Widget::setAccountTable()
{
    ui->accountTable->setModel(accountItemModel);
    ui->accountTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->accountTable->horizontalHeader()->setStretchLastSection(true);
    ui->accountTable->horizontalHeader()->setVisible(false);
    ui->accountTable->verticalHeader()->setVisible(false);
    ui->accountTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void Widget::on_exitButton_clicked()
{
    exit(0);
}

void Widget::on_rootAddButton_clicked()
{
    Msg msg;
    msg.exec();

    if ("" == msg.trans) return;

    //bool ok;
    QString cata = msg.trans;
    //QSqlQuery query;
    //QString s = QString("select * from user where name=='%1' ").arg(orcWidget.Code(userAccount));
    //query.exec(s);
    //query.first();
    //qDebug() << ok;
    isRepeated(rootItemModel, rootCount, catalogue_1, cata);

    if(true)
    {
        rootItemModel->setItem(rootCount++, 0, new QStandardItem(catalogue_1));
        QColor nicePurple(200, 160, 255, 125);
        rootItemModel->item(rootCount - 1)->setBackground(nicePurple);
    }
}

void Widget::isRepeated(QStandardItemModel *itemModel, int nCount, QString &catalogue, QString &cata)
{
    int count = 0;
    for(int i = 0; i < nCount; i++)
    {
        if(itemModel->item(i)->text() != cata)
        {
            continue;
        }
        ++count;
        break;
    }
    if(0 == count)
    {
        catalogue = cata;
    }
    else
    {
        int i = 0;
        while(true)
        {
            QString temp = QString("%1 (%2)").arg(cata).arg(i + 2);
            int tempCount = 0;
            for(int j = 0; j < nCount; j++)
            {
                if(itemModel->item(j)->text() != temp)
                {
                    continue;
                }
                ++tempCount;
                break;
            }
            if(0 == tempCount)
            {
                catalogue = temp;
                break;
            }
            ++i;
        }
    }
}

void Widget::on_rootDeleteButton_clicked()
{
    if(0 == rootCount) return;

    int row = ui->rootTable->currentIndex().row();
    if(row >= 0)
    {
        QString cata = rootItemModel->item(row)->text();
        QSqlQuery query;
        QString s = QString("delete from user where catalogue_1=='%1' ").arg(orcWidget.Code(cata));
        query.exec(s);

        rootItemModel->removeRow(row);
        --rootCount;

        accountItemModel->removeRows(0, accountCount);
        ui->accountEdit->clear();
        ui->passwordEdit->clear();
        ui->urlEdit->clear();
    }

    ui->rootDeleteButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
    ui->accountAddButton->setEnabled(false);
    ui->searchCata_2->setEnabled(false);
    catalogue_1 = "";
    catalogue_2 = "";
}

void Widget::on_accountAddButton_clicked()
{
    Msg msg;
    msg.exec();

    if ("" == msg.trans) return;

    int row = ui->rootTable->currentIndex().row();
    qDebug() << row;
    if(row >= 0)
    {
        //bool ok;
        qDebug() << msg.trans;
        QString cata = msg.trans;
        isRepeated(accountItemModel, accountCount, catalogue_2, cata);
        //qDebug() << ok;
        if(true)
        {
            accountItemModel->setItem(accountCount++, 0, new QStandardItem(catalogue_2));
            QColor nicePurple(200, 160, 255, 125);
            accountItemModel->item(accountCount - 1)->setBackground(nicePurple);
        }
    }
}


void Widget::on_accountDeleteButton_clicked()
{

    int row = ui->accountTable->currentIndex().row();
    if(row >= 0)
    {
        QString cata = accountItemModel->item(row)->text();
        QSqlQuery query;
        QString s = QString("delete from user where catalogue_2=='%1' ").arg(orcWidget.Code(cata));
        query.exec(s);

        accountItemModel->removeRow(row);
        --accountCount;

        ui->accountEdit->clear();
        ui->passwordEdit->clear();
        ui->urlEdit->clear();
    }

    if(0 == accountCount)
    {
        ui->accountEdit->setEnabled(false);
        ui->passwordEdit->setEnabled(false);
        ui->urlEdit->setEnabled(false);
    }

    ui->accountDeleteButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
    catalogue_2 = "";
}

void Widget::on_rootTable_clicked(const QModelIndex &index)
{
    ui->searchCata_2->clear();
    ui->rootDeleteButton->setEnabled(true);
    if(index.row() >= 0)
        ui->searchCata_2->setEnabled(true);

    accountItemModel->clear();
    accountCount = 0;
    ui->accountEdit->clear();
    ui->passwordEdit->clear();
    ui->urlEdit->clear();
    ui->accountEdit->setEnabled(false);
    ui->passwordEdit->setEnabled(false);
    ui->urlEdit->setEnabled(false);
    ui->saveButton->setEnabled(false);
    ui->accountDeleteButton->setEnabled(false);
    ui->accountAddButton->setEnabled(true);
    catalogue_1 = rootItemModel->item(index.row())->text();
    catalogue_2 = "";
    QString s = QString("select * from user where name=='%1' and catalogue_1=='%2' ").arg(orcWidget.Code(userAccount)).arg(orcWidget.Code(catalogue_1));
    QSqlQuery query;
    query.exec(s);
    while(query.next())
    {
        accountItemModel->setItem(accountCount++, 0, new QStandardItem(orcWidget.Code(query.value(4).toString())));
        checkTime(query, accountCount - 1);
    }
}

void Widget::checkTime(QSqlQuery &query, int row)
{
    QDateTime currentTime = QDateTime::currentDateTime();
    QDateTime dateTime = QDateTime::fromString(orcWidget.Code(query.value(6).toString()), "yyyy-MM-dd hh:mm:ss");
    int flag;
    if(dateTime.addSecs(5).secsTo(currentTime) < 0)
        flag = 0;
    else if(dateTime.addSecs(10).secsTo(currentTime) < 0)
        flag = 1;
    else if(dateTime.addSecs(15).secsTo(currentTime) < 0)
        flag = 2;
    else
        flag = 3;
    QColor niceGreen(175, 255, 175, 175);
    QColor niceYellow(240, 255, 165);
    QColor niceOrange(255, 200, 100);
    QColor niceRed(255, 0, 0, 125);
    switch(flag)
    {
    case 0:
        accountItemModel->item(row)->setBackground(niceGreen);
        break;
    case 1:
        accountItemModel->item(row)->setBackground(niceYellow);
        break;
    case 2:
        accountItemModel->item(row)->setBackground(niceOrange);
        break;
    case 3:
        accountItemModel->item(row)->setBackground(niceRed);
        break;
    }
}

void Widget::on_accountEdit_textChanged(const QString &arg1)
{
    ui->saveButton->setEnabled(true);
}

void Widget::on_passwordEdit_textChanged(const QString &arg1)
{
    ui->saveButton->setEnabled(true);
    QPixmap pixmap1(":/Resources/images/pw_weak.png");
    QPixmap pixmap2(":/Resources/images/pw_mid.png");
    QPixmap pixmap3(":/Resources/images/pw_strong.png");
    switch(checkPassword(arg1))
    {
    case 0:
        ui->securityLabel->setPixmap(pixmap1);
        break;
    case 1:
        ui->securityLabel->setPixmap(pixmap1);
        break;
    case 2:
        ui->securityLabel->setPixmap(pixmap2);
        break;
    case 3:
        ui->securityLabel->setPixmap(pixmap3);
        break;
    case 4:
        ui->securityLabel->setPixmap(pixmap3);
        break;
    }
}

void Widget::on_urlEdit_textChanged(const QString &arg1)
{
    ui->saveButton->setEnabled(true);
}

void Widget::on_accountTable_clicked(const QModelIndex &index)
{
    ui->accountEdit->clear();
    ui->passwordEdit->clear();
    ui->urlEdit->clear();

    if(0 != accountCount)
    {
        catalogue_2 = accountItemModel->item(index.row())->text();
        QString s = QString("select * from user where name=='%1' and catalogue_1=='%2' and catalogue_2=='%3' ").
                arg(orcWidget.Code(userAccount)).arg(orcWidget.Code(catalogue_1)).arg(orcWidget.Code(catalogue_2));
        QSqlQuery query;
        query.exec(s);
        if(query.first())
        {
            QStringList list = orcWidget.Code(query.value(5).toString()).split("\a");
            ui->accountEdit->setText(list[0]);
            ui->passwordEdit->setText(list[1]);
            ui->urlEdit->setText(list[2]);
        }
    }

    ui->accountEdit->setEnabled(true);
    ui->passwordEdit->setEnabled(true);
    ui->urlEdit->setEnabled(true);
    ui->accountDeleteButton->setEnabled(true);
    ui->saveButton->setEnabled(false);
}

int Widget::checkPassword(const QString& password)
{
    if(password.length() < 6) return 0;

    int upCount = 0;
    int lowCount = 0;
    int digitCount = 0;
    int otherCount = 0;
    for(auto ch : password)
    {
        if(ch.isUpper()) upCount = 1;
        else if(ch.isLower()) lowCount = 1;
        else if(ch.isDigit()) digitCount = 1;
        else if(!ch.isNull()) otherCount = 1;
    }
    return upCount + lowCount + digitCount + otherCount;
}

void Widget::on_saveButton_clicked()
{
    if(NULL != catalogue_1 && NULL != catalogue_2)
    {
        QString data = QString("%1\a%2\a%3").
                arg(ui->accountEdit->text()).arg(ui->passwordEdit->text()).arg(ui->urlEdit->text());

        QSqlQuery query;
        QString s = QString("select * from user where name=='%1' and catalogue_1=='%2' and catalogue_2=='%3' ").
                arg(orcWidget.Code(userAccount)).arg(orcWidget.Code(catalogue_1)).arg(orcWidget.Code(catalogue_2));
        query.exec(s);
        QDateTime currentTime = QDateTime::currentDateTime();
        QString temp = currentTime.toString("yyyy-MM-dd hh:mm:ss");
        if(query.first())
        {
            s = QString("update user set data=='%1' where name=='%2' and catalogue_1=='%3' and catalogue_2=='%4' ").
                    arg(orcWidget.Code(data)).arg(orcWidget.Code(userAccount)).arg(orcWidget.Code(catalogue_1)).arg(orcWidget.Code(catalogue_2));
            query.exec(s);
            s = QString("update user set time=='%1' where name=='%2' and catalogue_1=='%3' and catalogue_2=='%4' ").
                    arg(orcWidget.Code(temp)).arg(orcWidget.Code(userAccount)).arg(orcWidget.Code(catalogue_1)).arg(orcWidget.Code(catalogue_2));
            query.exec(s);
        }
        else
        {
            s = QString("insert into user values(%1, '%2', '%3', '%4', '%5', '%6', '%7') ").arg(++id).arg(orcWidget.Code(userAccount)).
                    arg(orcWidget.Code(userPassword)).arg(orcWidget.Code(catalogue_1)).arg(orcWidget.Code(catalogue_2)).arg(orcWidget.Code(data)).arg(orcWidget.Code(temp));
            query.exec(s);
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::FramelessWindowHint);
        msgBox.setStyleSheet("background-color:rgb(193,213,250)");
        msgBox.setText(tr("Please register a catalog!       "));
        msgBox.exec();
    }

    ui->saveButton->setEnabled(false);
}

void Widget::on_setRandomAccount_clicked()
{
    ui->accountEdit->setEnabled(true);
    const QString possibleCharacters("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    const int randomStringLength = 10;
    QString randomString;
    qsrand(QTime::currentTime().msec());
    for(int i=0; i<randomStringLength; i++)
    {
        int j = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(j);
        randomString.append(nextChar);
    }
    ui->accountEdit->setText(randomString);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(randomString);
    ui->saveButton->setEnabled(false);
}

void Widget::on_setRandomPassword_clicked()
{
    ui->passwordEdit->setEnabled(true);
    const QString possibleCharacters(".!@0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    const int randomStringLength = 10;
    QString randomString;
    qsrand(QTime::currentTime().msec());
    int upCount = 0;
    int lowCount = 0;
    int digitCount = 0;
    int punctCount = 0;
    while(upCount + lowCount + digitCount+ punctCount < 4)
    {
        upCount = 0;
        lowCount = 0;
        digitCount = 0;
        punctCount = 0;
        randomString.clear();
        for(int i=0; i<randomStringLength; i++)
        {
            int j = qrand() % possibleCharacters.length();
            QChar nextChar = possibleCharacters.at(j);
            randomString.append(nextChar);
            for(auto ch : randomString)
            {
                if(ch.isUpper()) upCount = 1;
                else if(ch.isLower()) lowCount = 1;
                else if(ch.isDigit()) digitCount = 1;
                else if(ch.isPunct()) punctCount = 1;
                qDebug()<<randomString;
            }
        }
    }
    ui->passwordEdit->setText(randomString);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(randomString);
    ui->saveButton->setEnabled(false);
}

void Widget::on_searchCata_1_textChanged(const QString &arg1)
{
    rootItemModel->clear();
    accountItemModel->clear();
    ui->searchCata_2->clear();
    ui->accountEdit->clear();
    ui->passwordEdit->clear();
    ui->urlEdit->clear();
    ui->saveButton->setEnabled(false);
    rootCount = 0;
    accountCount = 0;
    QString s = QString("select * from user where name=='%1' and catalogue_1 like '%%2%' ").
            arg(orcWidget.Code(userAccount)).arg(orcWidget.Code(arg1));
    QSqlQuery query;
    query.exec(s);
    if(query.first())
    {
        do
        {
            rootItemModel->setItem(rootCount++, 0, new QStandardItem(orcWidget.Code(query.value(3).toString())));
            QColor nicePurple(200, 160, 255, 125);
            rootItemModel->item(rootCount - 1)->setBackground(nicePurple);
            int i = 0;
            while(i + 1 < rootCount)
            {
                if(rootItemModel->item(i)->text() == rootItemModel->item(rootCount - 1)->text())
                {
                    rootItemModel->removeRow(rootCount - 1);
                    --rootCount;
                }
                ++i;
            }
        }
        while(query.next());
    }
    if(NULL == arg1)
    {
        firstShow();
    }
}

void Widget::on_searchCata_2_textChanged(const QString &arg1)
{
    accountItemModel->clear();
    ui->accountEdit->clear();
    ui->passwordEdit->clear();
    ui->urlEdit->clear();
    ui->saveButton->setEnabled(false);
    accountCount = 0;
    QString s;
    QSqlQuery query;
    s = QString("select * from user where name=='%1' and catalogue_1=='%2' and catalogue_2 like '%%3%' ").
            arg(orcWidget.Code(userAccount)).arg(orcWidget.Code(catalogue_1)).arg(orcWidget.Code(arg1));
    query.exec(s);
    while(query.next())
    {
        accountItemModel->setItem(accountCount++, 0, new QStandardItem(orcWidget.Code(query.value(4).toString())));
        checkTime(query, accountCount - 1);
    }
}

void Widget::on_min1Button_clicked()
{
    this->showMinimized();
}

void Widget::on_close1Button_clicked()
{
    exit(0);
}
