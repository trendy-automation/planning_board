#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void shiftReset();
    void toggleRow(int row);

    ~MainWindow();

public slots:
    void updatePlan(QList<int> plan);


private:
    Ui::MainWindow *ui;
    QTableWidget *tableWidget;

};

#endif // MAINWINDOW_H
