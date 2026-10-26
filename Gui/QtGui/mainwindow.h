#pragma once

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QScrollArea>
#include <QByteArray>
#include <QDomDocument>
#include <QComboBox>
#include <QTableWidget>
#include <QStringList>

struct MessageItem {
    QString state;
    QString text;
    int firstLine = -1;
    int firstColumn = -1;
    int lastLine = -1;
    int lastColumn = -1;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void openFile();
    void saveSource();
    void saveSourceAs();
    void processCode();
    void saveOutput();
    void onMessageClicked(QListWidgetItem *item);
    void onTextChanged();

    // Profils d'environnement
    void onEnvironmentProfileChanged(int index);
    void addEnvironmentProfile();
    void deleteEnvironmentProfile();
    void addEnvironmentVariable();
    void removeEnvironmentVariable();

private:
    void setupUi();
    void applyDarkStyle();

    void processXmlResponse(const QString &xmlText,
                            const QByteArray &out,
                            bool is_xml,
                            int ret);

    void displayOutputSvg(const QByteArray &svgData);
    void displayOutputPng(const QByteArray &pngData);
    void clearOutput();
    void clearMessages();
    void addMessage(const MessageItem &msg);
    void addLocalMessage(const QString &state, const QString &text);
    void selectEditorPosition(int firstLine,
                              int firstColumn,
                              int lastLine,
                              int lastColumn);

    void showOutputPlaceholder(const QString &text);
    void updateFilename();

    // Gestion des profils d'environnement
    void loadEnvironmentProfiles();
    void saveEnvironmentProfiles();
    void applyEnvironmentProfile();

    void loadEnvironmentProfile();

    // Composants UI
    QPlainTextEdit *editor;
    QLabel *filenameLabel;
    QPushButton *openBtn;
    QPushButton *saveBtn;
    QPushButton *saveasBtn;
    QPushButton *sendBtn;

    QLabel *outputDisplay;
    QScrollArea *outputScrollArea;
    QPushButton *saveOutputBtn;

    QListWidget *messagesList;

    // Interface des variables d'environnement
    QComboBox *environmentProfileCombo;
    QTableWidget *environmentTable;
    QPushButton *addEnvironmentProfileBtn;
    QPushButton *deleteEnvironmentProfileBtn;
    QPushButton *addEnvironmentVariableBtn;
    QPushButton *removeEnvironmentVariableBtn;

    // État courant
    QString currentFilePath;
    QByteArray currentOutputData;
    QString currentOutputType; // "svg" ou "png"
    int width;
    int height;
    QString currentEnvironmentProfile;
    QStringList managedEnvironmentVariables;
};
