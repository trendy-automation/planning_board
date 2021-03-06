#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QLabel>
#include <QProgressBar>
#include "ComboBoxDelegate.h"
#include "SpinBoxDelegate.h"
#include "StyledItemDelegate.h"
#include "GridDelegate.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QAbstractItemModel *model, QWidget *parent = 0);
    void setRowView(int row);
    void toggleView();

    ~MainWindow();

public slots:


private:
    Ui::MainWindow *ui;
    QTreeView *treeView;
    QLabel *statusLabelHour;
    QProgressBar *statusProgressBarHour;
    QLabel *statusLabelShift;
    QProgressBar *statusProgressBarShift;

};

#endif // MAINWINDOW_H
