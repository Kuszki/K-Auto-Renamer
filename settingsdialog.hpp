#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QtWidgets>
#include <QtCore>
#include <QtGui>

#include "common.hpp"

namespace Ui {	class SettingsDialog; }

class SettingsDialog : public QDialog
{

		Q_OBJECT

	protected:

		static const QVector<QCryptographicHash::Algorithm> checksums;
		static const QVector<Common::FORMAT> formats;

	private:

		Ui::SettingsDialog *ui;

	public:

		explicit SettingsDialog(QWidget *parent = nullptr);
		virtual ~SettingsDialog(void) override;

		Common::SETTINGS settings(void) const;

		static Common::SETTINGS loadSettings(void);

	public slots:

		virtual void accept(void) override;

	private slots:

		void reset(void);
		void validate(void);

	signals:

		void onAccepted(const Common::SETTINGS&);

};

#endif // SETTINGSDIALOG_HPP
