#include "checklist.h"

#include <iostream>

Checklist::Checklist()
{
    id = -1;
    //std::cout << "wtf" << std::endl;
}

Checklist::Checklist(QStringList init)
{
    id = init.at(0).toInt();
    addSpecies(init.at(1), init.at(2).toInt());
    location = init.at(3);
    county = init.at(4);
    state = init.at(5);
    QStringList dateElems = init.at(6).split("/");
    date = QDate(dateElems.at(2).toInt(), dateElems.at(0).toInt(), dateElems.at(1).toInt());
}

void Checklist::addSpecies(QString name, int count)
{
    if(name == "" || count == 0) return;
    species.insert(name, count);
}

int Checklist::getID()
{
    return id;
}

QMap<QString, int> Checklist::getSpecies()
{
    return species;
}

int Checklist::numSpecies()
{
    return species.size();
}

QStringList Checklist::coreData()
{
    QStringList coreData;
    coreData.append(location);
    coreData.append(county);
    coreData.append(state);
    coreData.append(date.toString("dd MMMM yyyy"));
    return coreData;
}

void Checklist::printInfo()
{
    std::cout << id << "\t" << location.toStdString() << "\t" << county.toStdString() << "\t" << state.toStdString() << "\t" << date.toString("dd MMMM yyyy").toStdString() << std::endl;
}

void Checklist::printSpecies()
{
    std::cout << id << "\t" << location.toStdString() << "\t" << county.toStdString() << "\t" << state.toStdString() << "\t" << date.toString("dd MMMM yyyy").toStdString() << std::endl;
    QMapIterator<QString, int> i(species);
    while (i.hasNext()) {
        i.next();
        std::cout << i.key().toStdString() << "\t" << i.value() << std::endl;
    }
}
