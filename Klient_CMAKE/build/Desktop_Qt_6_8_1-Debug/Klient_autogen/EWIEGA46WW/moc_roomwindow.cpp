/****************************************************************************
** Meta object code from reading C++ file 'roomwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../roomwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'roomwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10RoomWindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN10RoomWindowE = QtMocHelpers::stringData(
    "RoomWindow",
    "onError",
    "",
    "QAbstractSocket::SocketError",
    "socketError",
    "onServerMessage",
    "onDisconnected",
    "closeEvent",
    "QCloseEvent*",
    "event",
    "onUpdatePlayerList",
    "playerList",
    "onStartGameClicked",
    "onPrepareGame",
    "onSendAnswersClicked",
    "onPlayAgainClicked",
    "onFirstPlayer",
    "onStartGame",
    "letter",
    "lockFields",
    "unlockFields",
    "openSummary",
    "onReturnToLobbyClicked"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN10RoomWindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  104,    2, 0x08,    1 /* Private */,
       5,    0,  107,    2, 0x08,    3 /* Private */,
       6,    0,  108,    2, 0x08,    4 /* Private */,
       7,    1,  109,    2, 0x08,    5 /* Private */,
      10,    1,  112,    2, 0x08,    7 /* Private */,
      12,    0,  115,    2, 0x08,    9 /* Private */,
      13,    0,  116,    2, 0x08,   10 /* Private */,
      14,    0,  117,    2, 0x08,   11 /* Private */,
      15,    0,  118,    2, 0x08,   12 /* Private */,
      16,    0,  119,    2, 0x08,   13 /* Private */,
      17,    1,  120,    2, 0x08,   14 /* Private */,
      19,    0,  123,    2, 0x08,   16 /* Private */,
      20,    0,  124,    2, 0x08,   17 /* Private */,
      21,    0,  125,    2, 0x08,   18 /* Private */,
      22,    0,  126,    2, 0x08,   19 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::QStringList,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   18,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject RoomWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ZN10RoomWindowE.offsetsAndSizes,
    qt_meta_data_ZN10RoomWindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN10RoomWindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RoomWindow, std::true_type>,
        // method 'onError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketError, std::false_type>,
        // method 'onServerMessage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'closeEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QCloseEvent *, std::false_type>,
        // method 'onUpdatePlayerList'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        // method 'onStartGameClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPrepareGame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSendAnswersClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPlayAgainClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFirstPlayer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStartGame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'lockFields'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'unlockFields'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openSummary'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReturnToLobbyClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void RoomWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RoomWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onError((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 1: _t->onServerMessage(); break;
        case 2: _t->onDisconnected(); break;
        case 3: _t->closeEvent((*reinterpret_cast< std::add_pointer_t<QCloseEvent*>>(_a[1]))); break;
        case 4: _t->onUpdatePlayerList((*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 5: _t->onStartGameClicked(); break;
        case 6: _t->onPrepareGame(); break;
        case 7: _t->onSendAnswersClicked(); break;
        case 8: _t->onPlayAgainClicked(); break;
        case 9: _t->onFirstPlayer(); break;
        case 10: _t->onStartGame((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->lockFields(); break;
        case 12: _t->unlockFields(); break;
        case 13: _t->openSummary(); break;
        case 14: _t->onReturnToLobbyClicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
}

const QMetaObject *RoomWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RoomWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN10RoomWindowE.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RoomWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}
QT_WARNING_POP
