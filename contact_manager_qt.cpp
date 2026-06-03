// ============================================================
//  Contact Manager  -  Qt Frontend  (Single-File Version)
//  Build:  qmake -project && qmake && make   (add QT += widgets)
//  Or:     g++ contact_manager_qt.cpp -o cm \
//              $(pkg-config --cflags --libs Qt5Widgets)
//
//  All original OOP logic (Validator, Contact, ContactManager)
//  is kept intact. Only the UI layer is replaced by Qt.
// ============================================================

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QDialog>
#include <QComboBox>
#include <QMessageBox>
#include <QStackedWidget>
#include <QFrame>
#include <QScrollArea>
#include <QFont>
#include <QFontDatabase>
#include <QPalette>
#include <QColor>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QStatusBar>
#include <QSplitter>
#include <QListWidget>
#include <QTextEdit>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <QString>

using namespace std;

// ============================================================
//  BACKEND  (unchanged logic from original program)
// ============================================================

const string DATA_FILE = "contacts_data.txt";

class Validator
{
public:
    static bool isValidPhone(string phone)
    {
        if (phone.length() < 7)
            return false;

        for (size_t i = 0; i < phone.length(); i++)
        {
            if (i == 0 && phone[i] == '+')
                continue;
            if (!isdigit(phone[i]))
                return false;
        }
        return true;
    }
    static bool isValidEmail(string email)
    {
        int atCount = 0;
        bool afterAt = false, dotFound = false;
        for (char ch : email)
        {
            if (ch == ' ')
            {
                return false;
            }
            if (ch == '@')
            {
                atCount++;
                afterAt = true;
            }
            if (afterAt && ch == '.')
            {
                dotFound = true;
            }
        }
        return (atCount == 1 && dotFound);
    }
};

struct ContactData
{
    string name, personalNumber, businessNumber, email, contactType;
};

class ContactManager
{
public:
    static const int MAX = 200;
    vector<ContactData> contacts;

    ContactManager() { loadFromFile(); }

    bool nameExists(const string &name)
    {
        for (auto &c : contacts)
            if (c.name == name)
                return true;
        return false;
    }
    bool phoneExists(const string &phone)
    {
        for (auto &c : contacts)
            if (c.personalNumber == phone)
                return true;
        return false;
    }
    int findByName(const string &name)
    {
        for (int i = 0; i < (int)contacts.size(); i++)
            if (contacts[i].name == name)
                return i;
        return -1;
    }
    void sortContacts()
    {
        sort(contacts.begin(), contacts.end(),
             [](const ContactData &a, const ContactData &b)
             { return a.name < b.name; });
    }

    bool addContact(const ContactData &cd, string &error)
    {
        if ((int)contacts.size() >= MAX)
        {
            error = "List is full (max 200).";
            return false;
        }
        if (cd.name.empty())
        {
            error = "Name cannot be empty.";
            return false;
        }
        if (nameExists(cd.name))
        {
            error = "A contact with this name already exists.";
            return false;
        }
        if (!Validator::isValidPhone(cd.personalNumber))
        {
            error = "Invalid phone. Digits only, min 7 chars ('+' allowed at start).";
            return false;
        }
        if (!cd.email.empty() && !Validator::isValidEmail(cd.email))
        {
            error = "Invalid email format.";
            return false;
        }
        contacts.push_back(cd);
        saveToFile();
        return true;
    }

    bool editContact(int index, const ContactData &cd, string &error)
    {
        if (cd.name.empty())
        {
            error = "Name cannot be empty.";
            return false;
        }
        if (cd.name != contacts[index].name && nameExists(cd.name))
        {
            error = "A contact with this name already exists.";
            return false;
        }
        if (!Validator::isValidPhone(cd.personalNumber))
        {
            error = "Invalid phone number.";
            return false;
        }
        if (!cd.email.empty() && !Validator::isValidEmail(cd.email))
        {
            error = "Invalid email format.";
            return false;
        }
        contacts[index] = cd;
        saveToFile();
        return true;
    }

    void deleteContact(int index)
    {
        contacts.erase(contacts.begin() + index);
        saveToFile();
    }

    void saveToFile()
    {
        ofstream f(DATA_FILE);
        if (!f.is_open())
            return;
        f << contacts.size() << "\n";
        for (auto &c : contacts)
            f << c.name << "|" << c.personalNumber << "|"
              << c.businessNumber << "|" << c.email << "|" << c.contactType << "\n";
    }

    void loadFromFile()
    {
        ifstream f(DATA_FILE);
        if (!f.is_open())
            return;
        int n = 0;
        f >> n;
        f.ignore();
        for (int i = 0; i < n; i++)
        {
            string line;
            if (!getline(f, line))
                break;
            auto p1 = line.find('|');
            auto p2 = (p1 != string::npos) ? line.find('|', p1 + 1) : string::npos;
            auto p3 = (p2 != string::npos) ? line.find('|', p2 + 1) : string::npos;
            auto p4 = (p3 != string::npos) ? line.find('|', p3 + 1) : string::npos;
            if (p1 == string::npos || p2 == string::npos || p3 == string::npos || p4 == string::npos)
                continue;
            ContactData cd;
            cd.name = line.substr(0, p1);
            cd.personalNumber = line.substr(p1 + 1, p2 - p1 - 1);
            cd.businessNumber = line.substr(p2 + 1, p3 - p2 - 1);
            cd.email = line.substr(p3 + 1, p4 - p3 - 1);
            cd.contactType = line.substr(p4 + 1);
            contacts.push_back(cd);
        }
    }
};

// ============================================================
//  STYLES
// ============================================================
static const char *GLOBAL_STYLE = R"(
QMainWindow, QDialog {
    background-color: #000000;
}
QWidget {
    background-color: #070c1c;
    color: #e2e8f0;
    font-family: 'Segoe UI', 'SF Pro Display', sans-serif;
}
QLabel#titleLabel {
    font-size: 26px;
    font-weight: 700;
    color: #d6e7f7;
    letter-spacing: 1px;
}
QLabel#subtitleLabel {
    font-size: 12px;
    color: #596c87;
    letter-spacing: 2px;
    text-transform: uppercase;
}
QLabel#sectionLabel {
    font-size: 11px;
    font-weight: 600;
    color: #94a3b8;
    letter-spacing: 1.5px;
}
QLabel#statCount {
    font-size: 32px;
    font-weight: 700;
    color: #38bdf8;
}
QLabel#statLabel {
    font-size: 11px;
    color: #64748b;
    letter-spacing: 1px;
}
QPushButton#primaryBtn {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #0ea5e9,stop:1 #6366f1);
    color: #ffffff;
    border: none;
    border-radius: 10px;
    padding: 12px 28px;
    font-size: 13px;
    font-weight: 600;
    letter-spacing: 0.5px;
}
QPushButton#primaryBtn:hover {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #38bdf8,stop:1 #818cf8);
}
QPushButton#primaryBtn:pressed {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #0284c7,stop:1 #4f46e5);
}
QPushButton#dangerBtn {
    background: rgba(239,68,68,0.15);
    color: #f87171;
    border: 1px solid rgba(239,68,68,0.3);
    border-radius: 8px;
    padding: 8px 20px;
    font-size: 12px;
    font-weight: 600;
}
QPushButton#dangerBtn:hover {
    background: rgba(239,68,68,0.25);
    border-color: #f87171;
}
QPushButton#ghostBtn {
    background: rgba(148,163,184,0.08);
    color: #94a3b8;
    border: 1px solid rgba(148,163,184,0.15);
    border-radius: 8px;
    padding: 8px 20px;
    font-size: 12px;
    font-weight: 500;
}
QPushButton#ghostBtn:hover {
    background: rgba(148,163,184,0.15);
    color: #e2e8f0;
}
QPushButton#iconBtn {
    background: rgba(148,163,184,0.08);
    color: #94a3b8;
    border: 1px solid rgba(148,163,184,0.12);
    border-radius: 8px;
    padding: 8px 14px;
    font-size: 16px;
}
QPushButton#iconBtn:hover {
    background: rgba(56,189,248,0.12);
    color: #38bdf8;
    border-color: rgba(56,189,248,0.3);
}
QLineEdit {
    background: rgba(30,41,59,0.8);
    color: #e2e8f0;
    border: 1px solid rgba(148,163,184,0.15);
    border-radius: 10px;
    padding: 10px 16px;
    font-size: 13px;
    selection-background-color: #0ea5e9;
}
QLineEdit:focus {
    border: 1px solid #0ea5e9;
    background: rgba(14,165,233,0.05);
}
QLineEdit::placeholder { color: #475569; }
QComboBox {
    background: rgba(30,41,59,0.8);
    color: #e2e8f0;
    border: 1px solid rgba(148,163,184,0.15);
    border-radius: 10px;
    padding: 10px 16px;
    font-size: 13px;
    min-width: 140px;
}
QComboBox:focus { border-color: #0ea5e9; }
QComboBox::drop-down { border: none; width: 30px; }
QComboBox::down-arrow { image: none; width: 0; height: 0;
    border-left: 5px solid transparent; border-right: 5px solid transparent;
    border-top: 6px solid #94a3b8; margin-right: 10px; }
QComboBox QAbstractItemView {
    background: #1e293b; color: #e2e8f0;
    border: 1px solid rgba(148,163,184,0.2);
    border-radius: 8px; selection-background-color: #0ea5e9;
    outline: none; padding: 4px;
}
QTableWidget {
    background: transparent;
    border: none;
    gridline-color: rgba(148,163,184,0.08);
    outline: none;
    font-size: 13px;
}
QTableWidget::item {
    padding: 12px 16px;
    border-bottom: 1px solid rgba(148,163,184,0.07);
    color: #cbd5e1;
}
QTableWidget::item:selected {
    background: rgba(14,165,233,0.12);
    color: #e2e8f0;
}
QTableWidget::item:hover {
    background: rgba(148,163,184,0.06);
}
QHeaderView::section {
    background: rgba(15,17,23,0.9);
    color: #64748b;
    font-size: 11px;
    font-weight: 600;
    letter-spacing: 1.5px;
    padding: 12px 16px;
    border: none;
    border-bottom: 1px solid rgba(148,163,184,0.1);
    text-transform: uppercase;
}
QScrollBar:vertical {
    background: transparent; width: 6px; margin: 0;
}
QScrollBar::handle:vertical {
    background: rgba(148,163,184,0.2); border-radius: 3px; min-height: 30px;
}
QScrollBar::handle:vertical:hover { background: rgba(148,163,184,0.4); }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar:horizontal { background: transparent; height: 6px; }
QScrollBar::handle:horizontal { background: rgba(148,163,184,0.2); border-radius: 3px; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
QFrame#card {
    background: rgba(30,41,59,0.5);
    border: 1px solid rgba(148,163,184,0.1);
    border-radius: 16px;
}
QFrame#accentCard {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
        stop:0 rgba(14,165,233,0.12),stop:1 rgba(99,102,241,0.08));
    border: 1px solid rgba(14,165,233,0.2);
    border-radius: 16px;
}
QFrame#sidebar {
    background: rgba(15,23,42,0.9);
    border-right: 1px solid rgba(148,163,184,0.08);
}
QLabel#badgePersonal {
    background: rgba(34,197,94,0.15);
    color: #4ade80;
    border: 1px solid rgba(34,197,94,0.3);
    border-radius: 6px;
    padding: 2px 10px;
    font-size: 11px;
    font-weight: 600;
}
QLabel#badgeBusiness {
    background: rgba(99,102,241,0.15);
    color: #a5b4fc;
    border: 1px solid rgba(99,102,241,0.3);
    border-radius: 6px;
    padding: 2px 10px;
    font-size: 11px;
    font-weight: 600;
}
QStatusBar {
    background: rgba(15,23,42,0.95);
    color: #475569;
    font-size: 11px;
    border-top: 1px solid rgba(148,163,184,0.08);
}
QDialog {
    border: 1px solid rgba(148,163,184,0.12);
    border-radius: 16px;
}
)";

// ============================================================
//  ADD / EDIT DIALOG
// ============================================================
class ContactDialog : public QDialog
{
    Q_OBJECT
public:
    ContactData result;

    ContactDialog(QWidget *parent, const ContactData *existing = nullptr)
        : QDialog(parent)
    {
        setWindowTitle(existing ? "Edit Contact" : "New Contact");
        setFixedWidth(480);
        setStyleSheet(GLOBAL_STYLE);

        auto *root = new QVBoxLayout(this);
        root->setContentsMargins(32, 32, 32, 32);
        root->setSpacing(0);

        // Header
        auto *hdr = new QLabel(existing ? "✏  Edit Contact " : "  New Contact");
        hdr->setObjectName("titleLabel");
        hdr->setStyleSheet("font-size:20px;font-weight:700;color:#f8fafc;");
        root->addWidget(hdr);
        root->addSpacing(6);
        auto *sub = new QLabel(existing ? "Update contact information below"
                                        : "Fill in the details to add a contact");
        sub->setStyleSheet("color:#64748b;font-size:12px;");
        root->addWidget(sub);
        root->addSpacing(28);

        auto addField = [&](const QString &label) -> QLineEdit *
        {
            auto *lbl = new QLabel(label);
            lbl->setObjectName("sectionLabel");
            root->addWidget(lbl);
            root->addSpacing(6);
            auto *ed = new QLineEdit;
            ed->setFixedHeight(42);
            root->addWidget(ed);
            root->addSpacing(16);
            return ed;
        };

        nameEdit = addField("FULL NAME");
        pNumEdit = addField("PERSONAL PHONE");
        bNumEdit = addField("BUSINESS PHONE  (optional)");
        emailEdit = addField("EMAIL ADDRESS");

        // Type
        auto *typeLbl = new QLabel("CONTACT TYPE");
        typeLbl->setObjectName("sectionLabel");
        root->addWidget(typeLbl);
        root->addSpacing(6);
        typeCombo = new QComboBox;
        typeCombo->addItems({"Personal", "Business"});
        typeCombo->setFixedHeight(42);
        root->addWidget(typeCombo);
        root->addSpacing(28);

        // Buttons
        auto *btnRow = new QHBoxLayout;
        btnRow->setSpacing(12);
        auto *cancelBtn = new QPushButton("Cancel");
        cancelBtn->setObjectName("ghostBtn");
        cancelBtn->setFixedHeight(44);
        auto *saveBtn = new QPushButton(existing ? "Save Changes" : "Add Contact");
        saveBtn->setObjectName("primaryBtn");
        saveBtn->setFixedHeight(44);
        btnRow->addWidget(cancelBtn);
        btnRow->addWidget(saveBtn);
        root->addLayout(btnRow);

        if (existing)
        {
            nameEdit->setText(QString::fromStdString(existing->name));
            pNumEdit->setText(QString::fromStdString(existing->personalNumber));
            bNumEdit->setText(QString::fromStdString(existing->businessNumber == "N/A" ? "" : existing->businessNumber));
            emailEdit->setText(QString::fromStdString(existing->email));
            typeCombo->setCurrentText(QString::fromStdString(existing->contactType));
        }

        connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
        connect(saveBtn, &QPushButton::clicked, this, [this]
                { accept(); });
    }

    ContactData getData()
    {
        ContactData cd;
        cd.name = nameEdit->text().trimmed().toStdString();
        cd.personalNumber = pNumEdit->text().trimmed().toStdString();
        string b = bNumEdit->text().trimmed().toStdString();
        cd.businessNumber = b.empty() ? "N/A" : b;
        cd.email = emailEdit->text().trimmed().toStdString();
        cd.contactType = typeCombo->currentText().toStdString();
        return cd;
    }

private:
    QLineEdit *nameEdit, *pNumEdit, *bNumEdit, *emailEdit;
    QComboBox *typeCombo;
};

// ============================================================
//  MAIN WINDOW
// ============================================================
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow()
    {
        setWindowTitle("Contact Manager");
        setMinimumSize(1100, 700);
        resize(1200, 780);
        setStyleSheet(GLOBAL_STYLE);

        buildUI();
        refreshTable();
        updateStats();
    }

private:
    ContactManager mgr;
    QTableWidget *table;
    QLineEdit *searchBar;
    QLabel *totalLabel;
    QLabel *personalLabel;
    QLabel *businessLabel;
    QLabel *statusMsg;

    // ---- helpers ----
    void buildUI()
    {
        auto *central = new QWidget;
        setCentralWidget(central);
        auto *mainLayout = new QHBoxLayout(central);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        // ── Sidebar ──────────────────────────────────────────
        auto *sidebar = new QFrame;
        sidebar->setObjectName("sidebar");
        sidebar->setFixedWidth(260);
        auto *sideLayout = new QVBoxLayout(sidebar);
        sideLayout->setContentsMargins(24, 36, 24, 24);
        sideLayout->setSpacing(0);

        // Logo / Title
        auto *logoRow = new QHBoxLayout;
        auto *logoIcon = new QLabel("<>");
        logoIcon->setStyleSheet("font-size:28px;color:#38bdf8;");
        auto *logoText = new QVBoxLayout;
        auto *appName = new QLabel("Contacts");
        appName->setObjectName("titleLabel");
        appName->setStyleSheet("font-size:18px;font-weight:700;color:#f8fafc;");
        auto *appVer = new QLabel("VERSION 2.0");
        appVer->setObjectName("subtitleLabel");
        logoText->addWidget(appName);
        logoText->addWidget(appVer);
        logoRow->addWidget(logoIcon);
        logoRow->addSpacing(10);
        logoRow->addLayout(logoText);
        logoRow->addStretch();
        sideLayout->addLayout(logoRow);
        sideLayout->addSpacing(36);

        // Stats cards
        auto mkStatCard = [&](QLabel *&countLbl, const QString &label, const QString &color)
        {
            auto *card = new QFrame;
            card->setObjectName("card");
            auto *cl = new QVBoxLayout(card);
            cl->setContentsMargins(16, 14, 16, 14);
            cl->setSpacing(2);
            countLbl = new QLabel("0");
            countLbl->setObjectName("statCount");
            countLbl->setStyleSheet(QString("font-size:28px;font-weight:700;color:%1;").arg(color));
            auto *lbl2 = new QLabel(label);
            lbl2->setObjectName("statLabel");
            cl->addWidget(countLbl);
            cl->addWidget(lbl2);
            sideLayout->addWidget(card);
            sideLayout->addSpacing(10);
        };
        mkStatCard(totalLabel, "TOTAL CONTACTS", "#38bdf8");
        mkStatCard(personalLabel, "PERSONAL", "#4ade80");
        mkStatCard(businessLabel, "BUSINESS", "#a5b4fc");

        sideLayout->addStretch();

        // Add button in sidebar
        auto *addBtn = new QPushButton("  Add Contact");
        addBtn->setObjectName("primaryBtn");
        addBtn->setFixedHeight(48);
        sideLayout->addWidget(addBtn);
        sideLayout->addSpacing(8);

        // File info
        auto *fileInfo = new QLabel(QString("📁  %1").arg(QString::fromStdString(DATA_FILE)));
        fileInfo->setStyleSheet("color:#334155;font-size:11px;");
        fileInfo->setAlignment(Qt::AlignCenter);
        sideLayout->addWidget(fileInfo);

        mainLayout->addWidget(sidebar);

        // ── Content area ─────────────────────────────────────
        auto *content = new QWidget;
        auto *contentLayout = new QVBoxLayout(content);
        contentLayout->setContentsMargins(32, 32, 32, 24);
        contentLayout->setSpacing(0);

        // Top bar
        auto *topBar = new QHBoxLayout;
        topBar->setSpacing(12);

        searchBar = new QLineEdit;
        searchBar->setPlaceholderText("  🔍   Search by name, phone, or email…");
        searchBar->setFixedHeight(44);
        searchBar->setMinimumWidth(300);

        auto *filterCombo = new QComboBox;
        filterCombo->addItems({"All Types", "Personal", "Business"});
        filterCombo->setFixedHeight(44);
        filterCombo->setFixedWidth(140);

        auto *editBtn = new QPushButton("✏  Edit");
        editBtn->setObjectName("ghostBtn");
        editBtn->setFixedHeight(44);

        auto *delBtn = new QPushButton("🗑  Delete");
        delBtn->setObjectName("dangerBtn");
        delBtn->setFixedHeight(44);

        topBar->addWidget(searchBar, 1);
        topBar->addWidget(filterCombo);
        topBar->addStretch();
        topBar->addWidget(editBtn);
        topBar->addWidget(delBtn);
        contentLayout->addLayout(topBar);
        contentLayout->addSpacing(24);

        // Table card
        auto *tableCard = new QFrame;
        tableCard->setObjectName("card");
        auto *tableLayout = new QVBoxLayout(tableCard);
        tableLayout->setContentsMargins(0, 0, 0, 0);
        tableLayout->setSpacing(0);

        // Table header bar
        auto *tableHdr = new QWidget;
        tableHdr->setStyleSheet("background:transparent;");
        auto *tHdrLayout = new QHBoxLayout(tableHdr);
        tHdrLayout->setContentsMargins(20, 16, 20, 16);
        auto *tHdrTitle = new QLabel("ALL CONTACTS");
        tHdrTitle->setObjectName("sectionLabel");
        statusMsg = new QLabel("");
        statusMsg->setStyleSheet("color:#4ade80;font-size:12px;");
        tHdrLayout->addWidget(tHdrTitle);
        tHdrLayout->addStretch();
        tHdrLayout->addWidget(statusMsg);
        tableLayout->addWidget(tableHdr);

        // Separator
        auto *sep = new QFrame;
        sep->setFrameShape(QFrame::HLine);
        sep->setStyleSheet("background:rgba(148,163,184,0.1);max-height:1px;");
        tableLayout->addWidget(sep);

        // Table
        table = new QTableWidget;
        table->setColumnCount(6);
        table->setHorizontalHeaderLabels({"NAME", "TYPE", "PERSONAL", "BUSINESS", "EMAIL", "ACTIONS"});
        table->horizontalHeader()->setStretchLastSection(false);
        table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
        table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
        table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
        table->setColumnWidth(1, 110);
        table->setColumnWidth(5, 110);
        table->verticalHeader()->setVisible(false);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setShowGrid(false);
        table->setAlternatingRowColors(false);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setFocusPolicy(Qt::NoFocus);
        tableLayout->addWidget(table);

        contentLayout->addWidget(tableCard, 1);
        mainLayout->addWidget(content, 1);

        // ── Status bar ───────────────────────────────────────
        statusBar()->showMessage(QString("  Data file: %1").arg(QString::fromStdString(DATA_FILE)));

        // ── Connections ──────────────────────────────────────
        connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAdd);
        connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEdit);
        connect(delBtn, &QPushButton::clicked, this, &MainWindow::onDelete);
        connect(searchBar, &QLineEdit::textChanged, this, [this, filterCombo](const QString &t)
                { filterTable(t, filterCombo->currentText()); });
        connect(filterCombo, &QComboBox::currentTextChanged, this, [this, filterCombo](const QString &f)
                { filterTable(searchBar->text(), f); });
        connect(table, &QTableWidget::cellDoubleClicked, this, [this](int /*r*/, int /*c*/)
                { onEdit(); });
    }

    void refreshTable(const QString &search = "", const QString &filter = "All Types")
    {
        mgr.sortContacts();
        table->setRowCount(0);

        for (auto &c : mgr.contacts)
        {
            QString name = QString::fromStdString(c.name);
            QString phone = QString::fromStdString(c.personalNumber);
            QString bnum = QString::fromStdString(c.businessNumber);
            QString email = QString::fromStdString(c.email);
            QString type = QString::fromStdString(c.contactType);

            // Apply filter
            if (filter != "All Types" && type != filter.toStdString())
                continue;
            if (!search.isEmpty())
            {
                bool match = name.contains(search, Qt::CaseInsensitive) || phone.contains(search) || email.contains(search, Qt::CaseInsensitive);
                if (!match)
                    continue;
            }

            int row = table->rowCount();
            table->insertRow(row);
            table->setRowHeight(row, 52);

            auto mkItem = [&](const QString &text, Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft)
            {
                auto *item = new QTableWidgetItem(text);
                item->setTextAlignment(align);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                return item;
            };

            // Name (bold)
            auto *nameItem = mkItem(name);
            nameItem->setFont(QFont("Segoe UI", 13, QFont::DemiBold));
            nameItem->setForeground(QColor("#f1f5f9"));
            table->setItem(row, 0, nameItem);

            // Type badge via widget
            auto *typeWidget = new QWidget;
            auto *tLayout = new QHBoxLayout(typeWidget);
            tLayout->setContentsMargins(8, 0, 8, 0);
            tLayout->setAlignment(Qt::AlignCenter);
            auto *badge = new QLabel(type);
            badge->setObjectName(type == "Personal" ? "badgePersonal" : "badgeBusiness");
            tLayout->addWidget(badge);
            table->setCellWidget(row, 1, typeWidget);

            table->setItem(row, 2, mkItem(phone));
            table->setItem(row, 3, mkItem(bnum == "N/A" ? "—" : bnum));
            table->setItem(row, 4, mkItem(email.isEmpty() ? "—" : email));

            // Action buttons
            auto *actWidget = new QWidget;
            auto *actLayout = new QHBoxLayout(actWidget);
            actLayout->setContentsMargins(6, 4, 6, 4);
            actLayout->setSpacing(6);
            auto *editBtn2 = new QPushButton("✏");
            editBtn2->setObjectName("iconBtn");
            editBtn2->setFixedSize(32, 32);
            editBtn2->setToolTip("Edit");
            auto *delBtn2 = new QPushButton("🗑");
            delBtn2->setObjectName("iconBtn");
            delBtn2->setFixedSize(32, 32);
            delBtn2->setToolTip("Delete");
            delBtn2->setStyleSheet("QPushButton#iconBtn:hover{background:rgba(239,68,68,0.15);color:#f87171;border-color:rgba(239,68,68,0.3);}");
            actLayout->addWidget(editBtn2);
            actLayout->addWidget(delBtn2);
            table->setCellWidget(row, 5, actWidget);

            // Capture name for lambdas
            QString cName = name;
            connect(editBtn2, &QPushButton::clicked, this, [this, cName]
                    {
                int idx = mgr.findByName(cName.toStdString());
                if (idx < 0) return;
                openEditDialog(idx); });
            connect(delBtn2, &QPushButton::clicked, this, [this, cName]
                    {
                int idx = mgr.findByName(cName.toStdString());
                if (idx < 0) return;
                confirmDelete(idx, cName); });
        }

        updateStats();
    }

    void filterTable(const QString &search, const QString &filter)
    {
        refreshTable(search, filter);
    }

    void updateStats()
    {
        int total = (int)mgr.contacts.size();
        int personal = 0, business = 0;
        for (auto &c : mgr.contacts)
        {
            if (c.contactType == "Personal")
                personal++;
            else
                business++;
        }
        totalLabel->setText(QString::number(total));
        personalLabel->setText(QString::number(personal));
        businessLabel->setText(QString::number(business));
        statusBar()->showMessage(QString("  Data file: %1   •   %2 contact(s) loaded")
                                     .arg(QString::fromStdString(DATA_FILE))
                                     .arg(total));
    }

    void showStatus(const QString &msg, const QString &color = "#4ade80")
    {
        statusMsg->setStyleSheet(QString("color:%1;font-size:12px;").arg(color));
        statusMsg->setText(msg);
        QTimer::singleShot(3000, this, [this]
                           { statusMsg->clear(); });
    }

    void onAdd()
    {
        ContactDialog dlg(this);
        if (dlg.exec() != QDialog::Accepted)
            return;
        ContactData cd = dlg.getData();
        string err;
        if (!mgr.addContact(cd, err))
        {
            QMessageBox::warning(this, "Validation Error", QString::fromStdString(err));
            return;
        }
        refreshTable();
        showStatus("✓  Contact added successfully");
    }

    void onEdit()
    {
        int row = table->currentRow();
        if (row < 0)
        {
            QMessageBox::information(this, "No Selection", "Please select a contact to edit.");
            return;
        }
        auto *nameItem = table->item(row, 0);
        if (!nameItem)
            return;
        int idx = mgr.findByName(nameItem->text().toStdString());
        if (idx < 0)
            return;
        openEditDialog(idx);
    }

    void openEditDialog(int idx)
    {
        ContactDialog dlg(this, &mgr.contacts[idx]);
        if (dlg.exec() != QDialog::Accepted)
            return;
        ContactData cd = dlg.getData();
        string err;
        if (!mgr.editContact(idx, cd, err))
        {
            QMessageBox::warning(this, "Validation Error", QString::fromStdString(err));
            return;
        }
        refreshTable();
        showStatus("✓  Contact updated");
    }

    void onDelete()
    {
        int row = table->currentRow();
        if (row < 0)
        {
            QMessageBox::information(this, "No Selection", "Please select a contact to delete.");
            return;
        }
        auto *nameItem = table->item(row, 0);
        if (!nameItem)
            return;
        int idx = mgr.findByName(nameItem->text().toStdString());
        if (idx < 0)
            return;
        confirmDelete(idx, nameItem->text());
    }

    void confirmDelete(int idx, const QString &name)
    {
        auto btn = QMessageBox::question(this, "Delete Contact",
                                         QString("Are you sure you want to delete <b>%1</b>?<br>"
                                                 "<span style='color:#f87171;font-size:12px;'>"
                                                 "This action cannot be undone.</span>")
                                             .arg(name),
                                         QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (btn != QMessageBox::Yes)
            return;
        mgr.deleteContact(idx);
        refreshTable();
        showStatus("🗑  Contact deleted", "#f87171");
    }
};

// ============================================================
//  MAIN
// ============================================================
#include "contact_manager_qt.moc" // needed for Q_OBJECT in single-file build

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Contact Manager");
    app.setApplicationVersion("2.0");

    MainWindow w;
    w.show();
    return app.exec();
}
