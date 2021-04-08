#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtWidgets>
#include <QtCore>
#include <QtGui>

#include "common.hpp"
#include "threadworker.hpp"
#include "logsdialog.hpp"
#include "settingsdialog.hpp"
#include "aboutdialog.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{

		Q_OBJECT

	private:

		QByteArray srcState;
		QByteArray dstState;

		Ui::MainWindow *ui;

		QProgressBar* Progress;
		QPushButton* Terminator;

		QThread Thread;
		ThreadWorker* Worker;

	public:

		explicit MainWindow(QWidget *parent = nullptr);
		virtual ~MainWindow(void) override;

	private slots:

		void startActionClicked(void);
		void settingsActionClicked(void);
		void aboutActionClicked(void);

		void openSourceClicked(void);
		void openDestClicked(void);

		void sourcePathChanged(const QString& Path);
		void destPathChanged(const QString& Path);

		void dataPathsChanged(void);

		void workerJobDone(const Common::LOGLIST& Log);

		void treeItemDbclicked(const QModelIndex& Index);

	signals:

		void onJobRequest(const QString&,
					   const QString&);

};

#endif // MAINWINDOW_HPP
