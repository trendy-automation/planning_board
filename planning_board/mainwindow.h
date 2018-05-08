#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "ComboBoxDelegate.h"
#include "SpinBoxDelegate.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QAbstractTableModel*model,QWidget *parent = 0);
    void setRowView(int row);
    void toggleView();

    ~MainWindow();

public slots:


private:
    Ui::MainWindow *ui;
    QTableView *tableView;

};

#endif // MAINWINDOW_H
