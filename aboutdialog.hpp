#ifndef ABOUTDIALOG_HPP
#define ABOUTDIALOG_HPP

#include <QtWidgets>

QT_BEGIN_NAMESPACE
namespace Ui {	class AboutDialog; }
QT_END_NAMESPACE

class AboutDialog : public QDialog
{

		Q_OBJECT

	private:

		Ui::AboutDialog* ui;

	public:

		explicit AboutDialog(QWidget* Parent = nullptr);
		virtual ~AboutDialog(void) override;

};

#endif // ABOUTDIALOG_HPP
