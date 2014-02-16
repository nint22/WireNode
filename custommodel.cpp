#include "custommodel.h"

CustomModel::CustomModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_wireSim( NULL )
{
}

int CustomModel::rowCount(const QModelIndex &parent) const
{
    return m_wireSim ? m_wireSim->GetHeight() : 0;
}

int CustomModel::columnCount(const QModelIndex &parent) const
{
    return m_wireSim ? m_wireSim->GetWidth() : 0;
}

QVariant CustomModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_wireSim == NULL )
        return QVariant();

    int x = index.column();
    int y = index.row();
    if( x >= 0 && x < m_wireSim->GetWidth() && y >= 0 && y < m_wireSim->GetHeight() )
    {
        return rand() % 2;//( m_wireSim->GetNode( x, y ).m_type );
    }
    else
    {
        return QVariant();
    }
}

void CustomModel::update()
{
}
