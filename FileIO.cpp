#include "FileIO.hpp"
#include <QCryptographicHash>
#include <QDebug>
#include <QUuid>

FileIO::FileIO()
{
	m_userIdCounter = 0;
}

bool FileIO::write(const QString& source, const QString& data)
{
	if (source.isEmpty())
		return false;

	QFile file(source);
	if (!file.open(QFile::WriteOnly | QFile::Truncate))
		return false;

	QTextStream out(&file);
	out << data;
	file.close();

	return true;
}

bool FileIO::decodeAndWrite(const QString& source, const QString& data)
{
	if (source.isEmpty())
		return false;

	QFile file(source);
	if (!file.open(QFile::WriteOnly | QFile::Truncate))
		return false;

	QByteArray bytes;
	bytes.append(data);
	file.write(QByteArray::fromBase64(bytes));
	file.close();

	return true;
}

bool FileIO::setUserAvatar(int userLocalId, const QString &data)
{
	if(!m_tmpAvatarDir.isValid()) {
		return false;
	}

	QString avatar_path = this->getAvatarPath(userLocalId);

	return this->decodeAndWrite(avatar_path, data);
}

bool FileIO::hasAvatar(int userLocalId)
{
	return QFile::exists(this->getAvatarPath(userLocalId));
}

QString FileIO::getAvatarPath(int userLocalId)
{
	if(!m_tmpAvatarDir.isValid()) {
		return 0;
	}

	if(userLocalId == -1) {
		return this->m_tmpAvatarDir.path();
	}

	return m_tmpAvatarDir.path().append("/").append(userLocalId);
}

QUrl FileIO::getAvatarUrl(int userLocalId)
{
	return QUrl::fromLocalFile(this->getAvatarPath(userLocalId));
}

QString FileIO::getImagePath(const QString &name)
{
	if(!m_tmpImageDir.isValid()) {
		return 0;
	}

	return m_tmpImageDir.path().append("/").append(name);
}

QString FileIO::decodeImage(const QString &data)
{
	if(data.isEmpty()) {
		return "";
	}

	if(!m_tmpImageDir.isValid()) {
		return 0;
	}

	QString image_name(this->getMd5Hash(data));
	QString image_path(this->getImagePath(image_name));

	// Check to see if file exist already
	if(QFile::exists(image_path)) {
		return QUrl::fromLocalFile(image_path).toString();
	}

	// If file is not exist yet, create it
	if(this->decodeAndWrite(image_path, data)) {
		return QUrl::fromLocalFile(image_path).toString();
	}

	return 0;
}

int FileIO::getNewUserLocalId()
{
	return m_userIdCounter++;
}

QString FileIO::getMd5Hash(const QString &data)
{
	QCryptographicHash hash(QCryptographicHash::Md5);

	hash.addData(data.toUtf8());
	return hash.result().toHex();
}
