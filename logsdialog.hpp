#ifndef LOGSDIALOG_HPP
#define LOGSDIALOG_HPP

#include <QtWidgets>
#include <QtCore>
#include <QtGui>

#include "common.hpp"

namespace Ui {	class LogsDialog; }

class LogsDialog : public QDialog
{

		Q_OBJECT

	private:
		Ui::LogsDialog *ui;

	public:

		explicit LogsDialog(QWidget *parent = nullptr);
		virtual ~LogsDialog(void) override;

	public slots:

		void showLogs(const Common::LOGLIST& Logs);
		bool saveLogs(const QString& Path);

	private slots:

		void saveButtonClicked(void);

};

#endif // LOGSDIALOG_HPP
