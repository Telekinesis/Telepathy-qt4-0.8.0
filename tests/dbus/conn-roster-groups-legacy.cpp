#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include <QtDBus/QtDBus>

#include <QtTest/QtTest>

#define TP_QT4_ENABLE_LOWLEVEL_API

#include <TelepathyQt4/ChannelFactory>
#include <TelepathyQt4/Connection>
#include <TelepathyQt4/ConnectionLowlevel>
#include <TelepathyQt4/Contact>
#include <TelepathyQt4/ContactFactory>
#include <TelepathyQt4/ContactManager>
#include <TelepathyQt4/PendingContacts>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/Debug>

#include <telepathy-glib/debug.h>

#include <tests/lib/glib/contactlist/conn.h>
#include <tests/lib/test.h>

using namespace Tp;

class TestConnRosterGroupsLegacy : public Test
{
    Q_OBJECT

public:
    TestConnRosterGroupsLegacy(QObject *parent = 0)
        : Test(parent), mConnService(0),
          mContactsAddedToGroup(0), mContactsRemovedFromGroup(0)
    { }

protected Q_SLOTS:
    void onGroupAdded(const QString &group);
    void onGroupRemoved(const QString &group);
    void onContactAddedToGroup(const QString &group);
    void onContactRemovedFromGroup(const QString &group);
    void expectConnInvalidated();
    void expectContact(Tp::PendingOperation*);
    void exitOnStateSuccess(Tp::ContactListState);

private Q_SLOTS:
    void initTestCase();
    void init();

    void testGroupsAfterStateChange();
    void testIntrospectAfterStateChange();
    void testRosterGroups();
    void testNotADeathTrap();

    void cleanup();
    void cleanupTestCase();

private:
    QString mConnName, mConnPath;
    ExampleContactListConnection *mConnService;
    ConnectionPtr mConn;
    ContactPtr mContact;

    QString mGroupAdded;
    QString mGroupRemoved;
    int mContactsAddedToGroup;
    int mContactsRemovedFromGroup;
    bool mConnInvalidated;
};

void TestConnRosterGroupsLegacy::onGroupAdded(const QString &group)
{
    mGroupAdded = group;
    mLoop->exit(0);
}

void TestConnRosterGroupsLegacy::onGroupRemoved(const QString &group)
{
    mGroupRemoved = group;
    mLoop->exit(0);
}


void TestConnRosterGroupsLegacy::onContactAddedToGroup(const QString &group)
{
    mContactsAddedToGroup++;
    mLoop->exit(0);
}

void TestConnRosterGroupsLegacy::onContactRemovedFromGroup(const QString &group)
{
    mContactsRemovedFromGroup++;
    mLoop->exit(0);
}

void TestConnRosterGroupsLegacy::expectConnInvalidated()
{
    mConnInvalidated = true;
    mLoop->exit(0);
}

void TestConnRosterGroupsLegacy::expectContact(Tp::PendingOperation *op)
{
    PendingContacts *contacts = qobject_cast<PendingContacts *>(op);
    QVERIFY(contacts != 0);

    QVERIFY(contacts->isValid());
    QCOMPARE(contacts->contacts().length(), 1);

    mContact = contacts->contacts()[0];

    mLoop->exit(0);
}

void TestConnRosterGroupsLegacy::exitOnStateSuccess(Tp::ContactListState state)
{
    qDebug() << "got contact list state" << state;

    if (state == ContactListStateSuccess) {
        mLoop->exit(0);
    }
}

void TestConnRosterGroupsLegacy::initTestCase()
{
    initTestCaseImpl();

    g_type_init();
    g_set_prgname("conn-roster-groups");
    tp_debug_set_flags("all");
    dbus_g_bus_get(DBUS_BUS_STARTER, 0);
}

void TestConnRosterGroupsLegacy::init()
{
    gchar *name;
    gchar *connPath;
    GError *error = 0;

    mConnService = EXAMPLE_CONTACT_LIST_CONNECTION(g_object_new(
            EXAMPLE_TYPE_CONTACT_LIST_CONNECTION,
            "account", "me@example.com",
            "simulation-delay", 0,
            "protocol", "example-contact-list",
            NULL));
    QVERIFY(mConnService != 0);
    QVERIFY(tp_base_connection_register(TP_BASE_CONNECTION(mConnService),
                "foo", &name, &connPath, &error));
    QVERIFY(error == 0);

    QVERIFY(name != 0);
    QVERIFY(connPath != 0);

    mConnName = QLatin1String(name);
    mConnPath = QLatin1String(connPath);

    g_free(name);
    g_free(connPath);
    initImpl();

    mConnInvalidated = false;
}

void TestConnRosterGroupsLegacy::testGroupsAfterStateChange()
{
    // Create a conn and make the roster groups related features ready
    mConn = Connection::create(mConnName, mConnPath,
            ChannelFactory::create(QDBusConnection::sessionBus()),
            ContactFactory::create());

    ContactManagerPtr contactManager = mConn->contactManager();

    Features features = Features() << Connection::FeatureRoster << Connection::FeatureRosterGroups;
    QVERIFY(connect(mConn->becomeReady(features),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(mConn->isReady(Connection::FeatureRoster), true);
    QCOMPARE(mConn->isReady(Connection::FeatureRosterGroups), true);

    // Now start connecting it, and wait for the ContactManager state to turn to Success
    QVERIFY(connect(contactManager.data(),
                    SIGNAL(stateChanged(Tp::ContactListState)),
                    SLOT(exitOnStateSuccess(Tp::ContactListState))));

    mConn->lowlevel()->requestConnect();

    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(static_cast<uint>(contactManager->state()),
             static_cast<uint>(ContactListStateSuccess));

    // The conn should be valid and have the roster groups features ready when it emits Success
    QVERIFY(mConn->isValid());
    QCOMPARE(mConn->isReady(Connection::FeatureRoster), true);
    QCOMPARE(mConn->isReady(Connection::FeatureRosterGroups), true);

    // We should have all the group data downloaded now, check for that
    QStringList expectedGroups;
    expectedGroups << QLatin1String("Cambridge") << QLatin1String("Francophones")
        << QLatin1String("Montreal");
    expectedGroups.sort();
    QStringList groups = contactManager->allKnownGroups();
    groups.sort();
    QCOMPARE(groups, expectedGroups);

    // Cambridge
    {
        QStringList expectedContacts;
        expectedContacts << QLatin1String("geraldine@example.com")
            << QLatin1String("helen@example.com")
            << QLatin1String("guillaume@example.com")
            << QLatin1String("sjoerd@example.com");
        expectedContacts.sort();
        QStringList contacts;
        Q_FOREACH (const ContactPtr &contact, contactManager->groupContacts(QLatin1String("Cambridge"))) {
            contacts << contact->id();
        }
        contacts.sort();
        QCOMPARE(contacts, expectedContacts);
    }

    // Francophones
    {
        QStringList expectedContacts;
        expectedContacts << QLatin1String("olivier@example.com")
            << QLatin1String("geraldine@example.com")
            << QLatin1String("guillaume@example.com");
        expectedContacts.sort();
        QStringList contacts;
        Q_FOREACH (const ContactPtr &contact, contactManager->groupContacts(QLatin1String("Francophones"))) {
            contacts << contact->id();
        }
        contacts.sort();
        QCOMPARE(contacts, expectedContacts);
    }

    // Montreal
    {
        QStringList expectedContacts;
        expectedContacts << QLatin1String("olivier@example.com");
        expectedContacts.sort();
        QStringList contacts;
        Q_FOREACH (const ContactPtr &contact, contactManager->groupContacts(QLatin1String("Montreal"))) {
            contacts << contact->id();
        }
        contacts.sort();
        QCOMPARE(contacts, expectedContacts);
    }
}

void TestConnRosterGroupsLegacy::testIntrospectAfterStateChange()
{
    // Create a conn and make the roster feature ready
    mConn = Connection::create(mConnName, mConnPath,
            ChannelFactory::create(QDBusConnection::sessionBus()),
            ContactFactory::create());

    ContactManagerPtr contactManager = mConn->contactManager();

    Features features = Features() << Connection::FeatureRoster;
    QVERIFY(connect(mConn->becomeReady(features),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(mConn->isReady(Connection::FeatureRoster), true);
    QCOMPARE(mConn->isReady(Connection::FeatureRosterGroups), false);

    // Now start connecting it, and wait for the ContactManager state to turn to Success
    QVERIFY(connect(contactManager.data(),
                    SIGNAL(stateChanged(Tp::ContactListState)),
                    SLOT(exitOnStateSuccess(Tp::ContactListState))));

    mConn->lowlevel()->requestConnect();

    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(static_cast<uint>(contactManager->state()),
             static_cast<uint>(ContactListStateSuccess));

    // The conn should be valid and have the roster feature ready when it emits Success, but not
    // RosterGroups because we didn't request it
    QVERIFY(mConn->isValid());
    QCOMPARE(mConn->isReady(Connection::FeatureRoster), true);
    QCOMPARE(mConn->isReady(Connection::FeatureRosterGroups), false);

    // We should have roster contacts now, but no groups
    QVERIFY(!contactManager->allKnownContacts().isEmpty());
    QVERIFY(contactManager->allKnownGroups().isEmpty());

    // Make RosterGroups ready too
    features = Features() << Connection::FeatureRosterGroups;
    QVERIFY(connect(mConn->becomeReady(features),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(mConn->isReady(Connection::FeatureRoster), true);
    QCOMPARE(mConn->isReady(Connection::FeatureRosterGroups), true);

    // We should still have the contacts, and the state should be success
    QVERIFY(!contactManager->allKnownContacts().isEmpty());
    QCOMPARE(static_cast<uint>(contactManager->state()),
             static_cast<uint>(ContactListStateSuccess));

    // We should have all the group data downloaded now, check for that
    QStringList expectedGroups;
    expectedGroups << QLatin1String("Cambridge") << QLatin1String("Francophones")
        << QLatin1String("Montreal");
    expectedGroups.sort();
    QStringList groups = contactManager->allKnownGroups();
    groups.sort();
    QCOMPARE(groups, expectedGroups);

    // Cambridge
    {
        QStringList expectedContacts;
        expectedContacts << QLatin1String("geraldine@example.com")
            << QLatin1String("helen@example.com")
            << QLatin1String("guillaume@example.com")
            << QLatin1String("sjoerd@example.com");
        expectedContacts.sort();
        QStringList contacts;
        Q_FOREACH (const ContactPtr &contact, contactManager->groupContacts(QLatin1String("Cambridge"))) {
            contacts << contact->id();
        }
        contacts.sort();
        QCOMPARE(contacts, expectedContacts);
    }
}

void TestConnRosterGroupsLegacy::testRosterGroups()
{
    mConn = Connection::create(mConnName, mConnPath,
            ChannelFactory::create(QDBusConnection::sessionBus()),
            ContactFactory::create());

    QVERIFY(connect(mConn->lowlevel()->requestConnect(),
                    SIGNAL(finished(Tp::PendingOperation*)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(mConn->isReady(), true);
    QCOMPARE(mConn->status(), ConnectionStatusConnected);

    Features features = Features() << Connection::FeatureRoster << Connection::FeatureRosterGroups;
    QVERIFY(connect(mConn->becomeReady(features),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(mConn->isReady(features), true);

    ContactManagerPtr contactManager = mConn->contactManager();

    QStringList expectedGroups;
    expectedGroups << QLatin1String("Cambridge") << QLatin1String("Francophones")
        << QLatin1String("Montreal");
    expectedGroups.sort();
    QStringList groups = contactManager->allKnownGroups();
    groups.sort();
    QCOMPARE(groups, expectedGroups);

    // Cambridge
    {
        QStringList expectedContacts;
        expectedContacts << QLatin1String("geraldine@example.com")
            << QLatin1String("helen@example.com")
            << QLatin1String("guillaume@example.com")
            << QLatin1String("sjoerd@example.com");
        expectedContacts.sort();
        QStringList contacts;
        Q_FOREACH (const ContactPtr &contact, contactManager->groupContacts(QLatin1String("Cambridge"))) {
            contacts << contact->id();
        }
        contacts.sort();
        QCOMPARE(contacts, expectedContacts);
    }

    // Francophones
    {
        QStringList expectedContacts;
        expectedContacts << QLatin1String("olivier@example.com")
            << QLatin1String("geraldine@example.com")
            << QLatin1String("guillaume@example.com");
        expectedContacts.sort();
        QStringList contacts;
        Q_FOREACH (const ContactPtr &contact, contactManager->groupContacts(QLatin1String("Francophones"))) {
            contacts << contact->id();
        }
        contacts.sort();
        QCOMPARE(contacts, expectedContacts);
    }

    // Montreal
    {
        QStringList expectedContacts;
        expectedContacts << QLatin1String("olivier@example.com");
        expectedContacts.sort();
        QStringList contacts;
        Q_FOREACH (const ContactPtr &contact, contactManager->groupContacts(QLatin1String("Montreal"))) {
            contacts << contact->id();
        }
        contacts.sort();
        QCOMPARE(contacts, expectedContacts);
    }

    QString group(QLatin1String("foo"));
    QVERIFY(contactManager->groupContacts(group).isEmpty());

    // add group foo
    QVERIFY(connect(contactManager.data(),
                    SIGNAL(groupAdded(const QString&)),
                    SLOT(onGroupAdded(const QString&))));
    QVERIFY(connect(contactManager->addGroup(group),
                    SIGNAL(finished(Tp::PendingOperation*)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
    while (mGroupAdded.isEmpty()) {
        QCOMPARE(mLoop->exec(), 0);
    }
    QCOMPARE(mGroupAdded, group);

    expectedGroups << group;
    expectedGroups.sort();
    groups = contactManager->allKnownGroups();
    groups.sort();
    QCOMPARE(groups, expectedGroups);

    // add Montreal contacts to group foo
    Contacts contacts = contactManager->groupContacts(QLatin1String("Montreal"));
    Q_FOREACH (const ContactPtr &contact, contacts) {
        QVERIFY(connect(contact.data(),
                        SIGNAL(addedToGroup(const QString&)),
                        SLOT(onContactAddedToGroup(const QString&))));
    }
    QVERIFY(connect(contactManager->addContactsToGroup(group, contacts.toList()),
                    SIGNAL(finished(Tp::PendingOperation*)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
    while (mContactsAddedToGroup != contacts.size()) {
        QCOMPARE(mLoop->exec(), 0);
    }
    Q_FOREACH (const ContactPtr &contact, contacts) {
        QVERIFY(contact->groups().contains(group));
    }

    // remove all contacts from group foo
    contacts = contactManager->groupContacts(group);
    Q_FOREACH (const ContactPtr &contact, contacts) {
        QVERIFY(connect(contact.data(),
                        SIGNAL(removedFromGroup(const QString&)),
                        SLOT(onContactRemovedFromGroup(const QString&))));
    }
    QVERIFY(connect(contactManager->removeContactsFromGroup(group, contacts.toList()),
                    SIGNAL(finished(Tp::PendingOperation*)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
    while (mContactsRemovedFromGroup != contacts.size()) {
        QCOMPARE(mLoop->exec(), 0);
    }
    Q_FOREACH (const ContactPtr &contact, contacts) {
        QVERIFY(!contact->groups().contains(group));
    }

    // add group foo
    QVERIFY(connect(contactManager.data(),
                    SIGNAL(groupRemoved(const QString&)),
                    SLOT(onGroupRemoved(const QString&))));
    QVERIFY(connect(contactManager->removeGroup(group),
                    SIGNAL(finished(Tp::PendingOperation*)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
    while (mGroupRemoved.isEmpty()) {
        QCOMPARE(mLoop->exec(), 0);
    }
    QCOMPARE(mGroupRemoved, group);

    expectedGroups.removeOne(group);
    expectedGroups.sort();
    groups = contactManager->allKnownGroups();
    groups.sort();
    QCOMPARE(groups, expectedGroups);
}

/**
 * Verify that ContactManager isn't a death-trap.
 *
 * Background: Connection::contactManager() used to unpredictably waver between NULL and the real
 * manager when the connection was in the process of being disconnected / otherwise invalidated,
 * which led to a great many segfaults, which was especially unfortunate considering the
 * ContactManager methods didn't do much any checks at all.
 */
void TestConnRosterGroupsLegacy::testNotADeathTrap()
{
    mConn = Connection::create(mConnName, mConnPath,
            ChannelFactory::create(QDBusConnection::sessionBus()),
            ContactFactory::create());
    QCOMPARE(mConn->isReady(), false);

    // Check that the contact manager doesn't crash, but returns an error (because the conn isn't
    // ready)
    QVERIFY(!mConn->contactManager().isNull());
    QVERIFY(connect(mConn->contactManager()->contactsForIdentifiers(QStringList()),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->lowlevel()->requestConnect(),
                    SIGNAL(finished(Tp::PendingOperation*)),
                    SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(mConn->isReady(), true);
    QCOMPARE(mConn->status(), ConnectionStatusConnected);

    // As the conn is now ready, the contact building functions shouldn't return an error now
    QVERIFY(!mConn->contactManager().isNull());

    QVERIFY(connect(mConn->contactManager()->contactsForIdentifiers(QStringList()),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->contactsForHandles(UIntList()),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->upgradeContacts(QList<ContactPtr>(),
                    Features()),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    // In fact, let's build a contact for future use
    QVERIFY(connect(mConn->contactManager()->contactsForIdentifiers(
                    QStringList() << QLatin1String("friendorfoe@example.com")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectContact(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
    QVERIFY(mContact->id() == QLatin1String("friendorfoe@example.com"));

    // Roster operations SHOULD still fail though, as FeatureRoster isn't ready 
    QVERIFY(connect(mConn->contactManager()->requestPresenceSubscription(
                    QList<ContactPtr>() << mContact,
                    QLatin1String("I just want to see you fail")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->removePresenceSubscription(
                    QList<ContactPtr>() << mContact,
                    QLatin1String("I just want to see you fail")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->authorizePresencePublication(
                    QList<ContactPtr>() << mContact,
                    QLatin1String("I just want to see you fail")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->removePresencePublication(
                    QList<ContactPtr>() << mContact,
                    QLatin1String("I just want to see you fail")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->blockContacts(QList<ContactPtr>() << mContact, true),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    // Now, make Roster ready
    Features features = Features() << Connection::FeatureRoster;
    QVERIFY(connect(mConn->becomeReady(features),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(mConn->isReady(features), true);

    // The roster functions should work now
    QVERIFY(connect(mConn->contactManager()->requestPresenceSubscription(
                    QList<ContactPtr>() << mContact,
                    QLatin1String("Please don't fail")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(mContact->subscriptionState() != Contact::PresenceStateNo);

    QVERIFY(connect(mConn->contactManager()->removePresenceSubscription(
                    QList<ContactPtr>() << mContact,
                    QLatin1String("Please don't fail")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QCOMPARE(mContact->subscriptionState(), Contact::PresenceStateNo);

    QVERIFY(connect(mConn->contactManager()->authorizePresencePublication(
                    QList<ContactPtr>() << mContact,
                    QLatin1String("Please don't fail")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->removePresencePublication(
                    QList<ContactPtr>() << mContact,
                    QLatin1String("Please don't fail")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    // The test CM doesn't support block, so it will never be successful

    // ... but still not the RosterGroup ones
    QVERIFY(connect(mConn->contactManager()->addGroup(QLatin1String("Those who failed")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->removeGroup(QLatin1String("Those who failed")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->addContactsToGroup(QLatin1String("Those who failed"),
                    QList<ContactPtr>() << mContact),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->removeContactsFromGroup(QLatin1String("Those who failed"),
                    QList<ContactPtr>() << mContact),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    // Make RosterGroups ready too
    features = Features() << Connection::FeatureRosterGroups;
    QVERIFY(connect(mConn->becomeReady(features),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(mConn->isReady(features), true);

    // Now that Core, Roster and RosterGroups are all ready, everything should work
    QVERIFY(!mConn->contactManager().isNull());

    QVERIFY(connect(mConn->contactManager()->contactsForIdentifiers(QStringList()),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->contactsForHandles(UIntList()),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->upgradeContacts(QList<ContactPtr>(),
                    Features()),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->requestPresenceSubscription(
                    QList<ContactPtr>() << mContact,
                    QLatin1String("Please don't fail")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(mContact->subscriptionState() != Contact::PresenceStateNo);

    QVERIFY(connect(mConn->contactManager()->removePresenceSubscription(
                    QList<ContactPtr>() << mContact,
                    QLatin1String("Please don't fail")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QCOMPARE(mContact->subscriptionState(), Contact::PresenceStateNo);

    QVERIFY(connect(mConn->contactManager()->authorizePresencePublication(
                    QList<ContactPtr>() << mContact,
                    QLatin1String("Please don't fail")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->removePresencePublication(
                    QList<ContactPtr>() << mContact,
                    QLatin1String("Please don't fail")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    // The test CM doesn't support block, so it will never be successful

    QVERIFY(connect(mConn->contactManager()->addGroup(QLatin1String("My successful entourage")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    qDebug() << "waiting for group to be added";

    // FIXME: Remove this once fd.o #29728 is fixed
    while (!mConn->contactManager()->allKnownGroups().contains(QLatin1String("My successful entourage"))) {
        mLoop->processEvents();
    }

    qDebug() << "group has been added";

    QVERIFY(connect(mConn->contactManager()->addContactsToGroup(QLatin1String("My successful entourage"),
                    QList<ContactPtr>() << mContact),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->removeContactsFromGroup(QLatin1String("My successful entourage"),
                    QList<ContactPtr>() << mContact),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->removeGroup(QLatin1String("My successful entourage")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectSuccessfulCall(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    // Now, invalidate the connection by disconnecting it
    QVERIFY(connect(mConn.data(),
                SIGNAL(invalidated(Tp::DBusProxy *,
                        const QString &, const QString &)),
                SLOT(expectConnInvalidated())));
    mConn->lowlevel()->requestDisconnect();

    // Check that contactManager doesn't go NULL in the process of the connection going invalidated
    do {
        QVERIFY(!mConn->contactManager().isNull());
        mLoop->processEvents();
    } while (!mConnInvalidated);

    QVERIFY(!mConn->isValid());
    QCOMPARE(mConn->status(), ConnectionStatusDisconnected);

    // Now that the conn is invalidated NOTHING should work anymore
    QVERIFY(connect(mConn->contactManager()->contactsForIdentifiers(QStringList()),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->contactsForHandles(UIntList()),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->upgradeContacts(QList<ContactPtr>(),
                    Features()),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->requestPresenceSubscription(QList<ContactPtr>(),
                    QLatin1String("You fail at life")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->removePresenceSubscription(QList<ContactPtr>(),
                    QLatin1String("You fail at life")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->authorizePresencePublication(QList<ContactPtr>(),
                    QLatin1String("You fail at life")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->removePresencePublication(QList<ContactPtr>(),
                    QLatin1String("You fail at life")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->blockContacts(QList<ContactPtr>() << mContact, true),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->addGroup(QLatin1String("Future failures")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->removeGroup(QLatin1String("Future failures")),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->addContactsToGroup(QLatin1String("Future failures"),
                    QList<ContactPtr>() << mContact),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(connect(mConn->contactManager()->removeContactsFromGroup(QLatin1String("Future failures"),
                    QList<ContactPtr>() << mContact),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(expectFailure(Tp::PendingOperation*))));
    QCOMPARE(mLoop->exec(), 0);
}

void TestConnRosterGroupsLegacy::cleanup()
{
    mContact.reset();

    if (mConn && mConn->requestedFeatures().contains(Connection::FeatureCore)) {
        QVERIFY(mConnService != NULL);

        if (TP_BASE_CONNECTION(mConnService)->status != TP_CONNECTION_STATUS_DISCONNECTED) {
            tp_base_connection_change_status(TP_BASE_CONNECTION(mConnService),
                    TP_CONNECTION_STATUS_DISCONNECTED,
                    TP_CONNECTION_STATUS_REASON_REQUESTED);
        }

        while (mConn->isValid()) {
            mLoop->processEvents();
        }

    }
    mConn.reset();

    if (mConnService != 0) {
        g_object_unref(mConnService);
        mConnService = 0;
    }

    cleanupImpl();
}

void TestConnRosterGroupsLegacy::cleanupTestCase()
{
    cleanupTestCaseImpl();
}

QTEST_MAIN(TestConnRosterGroupsLegacy)
#include "_gen/conn-roster-groups-legacy.cpp.moc.hpp"
