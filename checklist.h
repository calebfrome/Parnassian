#ifndef CHECKLIST_H
#define CHECKLIST_H

#include "observation.h"
#include "QDate"
#include <QVector>
#include <QMap>

class Checklist
{
    enum Protocol {
        INCIDENTAL,
        STATIONARY,
        TRAVELING
    };

    private:
        int id;
        QDate date;
        QString location;
        QString county;
        QString state;
        QMap<QString, int> species;
        QString start;
        QString end;
        QString protocol;
        double distance;

    public:
        Checklist();
        Checklist(QStringList init);
        void addSpecies(QString name, int count);
        int getID();
        QDate getDate();
        QString getDateString();
        QMap<QString, int> getSpecies();
        int numSpecies();
        QStringList coreData();
        void printInfo();
        void printSpecies();
        QString output();
};

#endif // CHECKLIST_H
