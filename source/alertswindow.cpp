#include "alertswindow.h"
#include "ui_alertswindow.h"

AlertsWindow::AlertsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AlertsWindow)
{
    extern Options options;
    ui->setupUi(this);
    alerts_window_font =
            QFont(options.alerts_plain_text_font_family,
            options.alerts_plain_text_font_size);
    ui->alertsTextEdit->setReadOnly(true);
    alerts_window_format.setFont(alerts_window_font);
    alerts_window_document = ui->alertsTextEdit->document();
    alerts_window_cursor = QTextCursor(alerts_window_document);
}

AlertsWindow::~AlertsWindow()
{
    delete ui;
}

void AlertsWindow::displayAlerts(std::vector<ActiveAlerts>
                                 &active_alerts)
{
    extern Options options;
    qint16 number_alerts = active_alerts.size();
    QString paragraph_start, paragraph_end, paragraph;
    alerts_window_document->clear();
    paragraph_start = "<p style=\"text-align: "
            % options.event_align % "; font-size: "
            % options.event_size % "px" % "; color: "
            % options.event_color % "\">";
    paragraph_end = "</p>";
    for(qint16 x=0; x<number_alerts; ++x) {
        paragraph = paragraph_start
                % active_alerts.at(x).event
                % paragraph_end;
        alerts_window_cursor.movePosition(QTextCursor::End);
        alerts_window_cursor.insertHtml(paragraph);
        alerts_window_cursor.insertBlock();
    }
    alerts_window_cursor.movePosition(QTextCursor::End);
    alerts_window_cursor.insertBlock();
    for(qint16 x=0; x<number_alerts; ++x) {
        paragraph = paragraph_start
                % active_alerts.at(x).headline
                % paragraph_end;
        alerts_window_cursor.insertHtml(paragraph);
        alerts_window_cursor.insertBlock();
        alerts_window_cursor.setBlockCharFormat(
                    alerts_window_format);
        alerts_window_cursor.insertText(
                    active_alerts.at(x).description);
        alerts_window_cursor.insertBlock();
        alerts_window_cursor.insertBlock();
    }
    if(number_alerts == 0) {
        paragraph = paragraph_start
                % "No alerts were found."
                % paragraph_end;
        alerts_window_cursor.insertHtml(paragraph);
        alerts_window_cursor.insertBlock();
        paragraph = paragraph_start
                % "Have a nice day!"
                % paragraph_end;
        alerts_window_cursor.insertHtml(paragraph);
        alerts_window_cursor.insertBlock();
    }
    // Set document scroll to the top
    QScrollBar *v_scroll_bar;
    v_scroll_bar = ui->alertsTextEdit->verticalScrollBar();
    v_scroll_bar->setSliderPosition(0);
}
