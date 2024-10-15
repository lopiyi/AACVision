#pragma once

#include <QObject>


class DeleteFiles  : public QObject
{
	Q_OBJECT

public:
	DeleteFiles( QObject *parent=nullptr);
	~DeleteFiles();
public slots:
	void run(const QStringList& paths, QVector<double> days);
private:
	void deleteOldImages(const QString& dirPath, double day = 999);
};

