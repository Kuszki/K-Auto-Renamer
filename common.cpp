#include "common.hpp"

QMap<Common::LOG, QString> Common::LOGMSG(void)
{
	return
	{
		{ Common::LOG::DirCreateError, QCoreApplication::translate("Common", "Can not create directory") },
		{ Common::LOG::FileCreateError, QCoreApplication::translate("Common", "Can not create file") },
		{ Common::LOG::ImageOpenError, QCoreApplication::translate("Common", "Can not open image") },
		{ Common::LOG::MetaCreateError, QCoreApplication::translate("Common", "Can not create metafile") },
		{ Common::LOG::WrongDpiWarning, QCoreApplication::translate("Common", "Wrong DPI for file") },
		{ Common::LOG::WrongDocFile, QCoreApplication::translate("Common", "Wrong file format in directory") }
	};
}
