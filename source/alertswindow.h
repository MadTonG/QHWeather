#ifndef ALERTSWINDOW_H
#define ALERTSWINDOW_H

#include <memory>
#include <vector>
#include <QDialog>
#include <QFont>
#include <QString>
#include <QStringBuilder>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QScrollBar>
#include "nwsendpoints.h"
#include "appoptions.h"

namespace Ui {
class AlertsWindow;
}

class AlertsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AlertsWindow(QWidget *parent = 0);
    ~AlertsWindow();
    void displayAlerts(std::vector<ActiveAlerts> &active_alerts);

private:
    Ui::AlertsWindow *ui;
    QFont alerts_window_font;
    QTextDocument *alerts_window_document;
    QTextCursor alerts_window_cursor;
    QTextCharFormat alerts_window_format;
};

#endif // ALERTSWINDOW_H
