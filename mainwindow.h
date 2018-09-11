#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "checklist.h"

#define V_MAJOR 1
#define V_MINOR 0

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pb_addSpecies_clicked();
    void on_pb_submitChecklist_clicked();
    void on_pb_cancelChecklist_clicked();
    void on_pb_resetFilter_clicked();
    void on_pb_applyFilter_clicked();

private:
    Ui::MainWindow *ui;
    QVector<QString> taxa;
    QStringList states;
    QStringList header;
    QMap<int, Checklist> database;
    QStringList dSpecies; //species which exist in the database
    QMap<QString, QStringList> dLocations; //locations which exist in the database (map of loc name to {county, state})
    bool compareSpecies(const QString& s1, const QString& s2); //compare 2 species taxonomically
    QList<QStringList> sortRecords(QList<QStringList> records);
    void sortDatabase();

    int currentID = -1;
    Checklist activeList;

    QString getActiveProtocol();
    void showEntireDatabase();
    void showEntireFirstLast(bool first);
    void printDatabase();
};

#endif // MAINWINDOW_H
