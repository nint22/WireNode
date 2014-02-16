#ifndef CUSTOMMODEL_H
#define CUSTOMMODEL_H

#include <QAbstractTableModel>
#include <QTableWidgetItem>

#include "wiresim.h"

class CustomModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit CustomModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    void SetWireSim( WireSim* wireSim ) { m_wireSim = wireSim; }

    void update(); // Emmits dataChanged for everything

private:

    WireSim* m_wireSim;

signals:

public slots:

};

#endif // CUSTOMMODEL_H
