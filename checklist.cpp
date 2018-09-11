#include "checklist.h"

#include <iostream>
#include <QDebug>

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
    start = init.at(7);
    //int h = qstart.left(qstart.indexOf(':')) + (qstart.right(2) == "PM" ? 12 : 0);
    //int m = qstart.mid(qstart.indexOf(':'), qstart.indexOf(' ') - qstart.indexOf(':'));
    //start = QTime(h, m, 0);
    end = init.at(8);
    //h = qend.left(qend.indexOf(':')) + (qend.right(2) == "PM" ? 12 : 0);
    //m = qend.mid(qend.indexOf(':'), qend.indexOf(' ') - qend.indexOf(':'));
    //end = QTime(h, m, 0);
    protocol = init.at(9);
    distance = init.at(10).toDouble();
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

QDate Checklist::getDate()
{
    return date;
}

QString Checklist::getDateString()
{
    return date.toString("dd MMMM yyyy");
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
    coreData.append(date.toString("yyyy-MM-dd"));
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

QString Checklist::output()
{
    QString output = "";
    QMapIterator<QString, int> i(species);
    while (i.hasNext()) {
        i.next();
        output.append(QString::number(id) + ',' + i.key() + ',' + QString::number(i.value()) + ',');
        output.append(location + ',' + county + ',' + state + ',' + date.toString("MM/dd/yyyy") + ',');
        output.append(start + ',' + end + ',' + protocol + ',' + QString::number(distance) + '\n');
    }
    return output;
}
