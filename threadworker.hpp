#ifndef THREADWORKER_HPP
#define THREADWORKER_HPP

#include <QtConcurrent>
#include <QtCore>
#include <QtGui>

#include "common.hpp"

class ThreadWorker : public QObject
{

		Q_OBJECT

	protected: struct PREFS
		{
			QString endl = "\n";
			QTextCodec* codec = QTextCodec::codecForName("UTF-8");

			int dpi = 300;

			QCryptographicHash::Algorithm checksum = QCryptographicHash::Sha1;
			QByteArray (QByteArray::*format)(void) const = QOverload<>::of(&QByteArray::toBase64);
			QDir::SortFlags sort = QDir::Name;
			QString text = QString();
		};

	private:

		mutable QMutex Mutex;

		volatile bool Kill = false;
		volatile bool Job = false;

		PREFS Prefs;

	public:

		explicit ThreadWorker(QObject *parent = nullptr);
		virtual ~ThreadWorker(void) override;

		bool isCanceled(void) const;
		bool isStartable(void) const;

		template<class Container>
		static void sort(Container& c);

	protected:

		void start(void);
		void stop(void);

	public slots:

		void setSettings(const Common::SETTINGS Settings);

		void startJob(const QString& src,
				    const QString& dst);

		void calcelJob(void);

	signals:

		void onJobDone(const Common::LOGLIST&);
		void onJobTitle(const QString&);
		void onJobStart(int, int);
		void onJobProgress(int);

};

template<class Container>
void ThreadWorker::sort(Container& c)
{
	QCollator collator;
	collator.setNumericMode(true);

	std::stable_sort(c.begin(), c.end(), collator);
}

#endif // THREADWORKER_HPP
