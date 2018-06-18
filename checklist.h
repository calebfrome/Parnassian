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
        QTime start;
        QTime end;
        Protocol protocol;
        double distance;

    public:
        Checklist();
        Checklist(QStringList init);
        void addSpecies(QString name, int count);
        int getID();
        QMap<QString, int> getSpecies();
        int numSpecies();
        QStringList coreData();
        void printInfo();
        void printSpecies();
};

#endif // CHECKLIST_H
