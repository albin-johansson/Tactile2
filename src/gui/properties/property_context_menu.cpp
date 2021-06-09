#include "property_context_menu.hpp"

#include "icons.hpp"
#include "tactile_qstring.hpp"

namespace tactile {

PropertyContextMenu::PropertyContextMenu(QWidget* parent)
    : QMenu{parent}
    , mCopy{addAction(icons::copy(), tr("Copy"))}
    , mPaste{addAction(icons::paste(), tr("Paste"))}
    , mSep0{addSeparator()}
    , mRename{addAction(icons::rename(), tr("Rename"))}
    , mChangeType{addMenu(icons::object_layer(), tr("Change type"))}
    , mSep1{addSeparator()}
    , mAdd{addAction(icons::add(), tr("Add new property..."))}
    , mRemove{addAction(icons::remove(), tr("Remove"))}
{
  mCopy->setShortcut(QKeySequence::Copy);
  mPaste->setShortcut(QKeySequence::Paste);
  mRename->setShortcut(QKeySequence::fromString(TACTILE_QSTRING(u"CTRL+R")));
  mRemove->setShortcut(QKeySequence::Delete);

  mCopy->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  mPaste->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  mRename->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  mRemove->setShortcutContext(Qt::WidgetWithChildrenShortcut);

  mTypeString = mChangeType->addAction(tr("string"));
  mTypeInt = mChangeType->addAction(tr("int"));
  mTypeFloat = mChangeType->addAction(tr("float"));
  mTypeBool = mChangeType->addAction(tr("bool"));
  mTypeObject = mChangeType->addAction(tr("object"));
  mTypeColor = mChangeType->addAction(tr("color"));
  mTypeFile = mChangeType->addAction(tr("file"));

  connect(mCopy, &QAction::triggered, this, &PropertyContextMenu::S_Copy);
  connect(mPaste, &QAction::triggered, this, &PropertyContextMenu::S_Paste);
  connect(mRename, &QAction::triggered, this, &PropertyContextMenu::S_Rename);
  connect(mAdd, &QAction::triggered, this, &PropertyContextMenu::S_Add);
  connect(mRemove, &QAction::triggered, this, &PropertyContextMenu::S_Remove);

  connect(mTypeString, &QAction::triggered, [this] {
    emit S_ChangeType(core::property_type::string);
  });

  connect(mTypeInt, &QAction::triggered, [this] {
    emit S_ChangeType(core::property_type::integer);
  });

  connect(mTypeFloat, &QAction::triggered, [this] {
    emit S_ChangeType(core::property_type::floating);
  });

  connect(mTypeBool, &QAction::triggered, [this] {
    emit S_ChangeType(core::property_type::boolean);
  });

  connect(mTypeFile, &QAction::triggered, [this] {
    emit S_ChangeType(core::property_type::file);
  });

  connect(mTypeObject, &QAction::triggered, [this] {
    emit S_ChangeType(core::property_type::object);
  });

  connect(mTypeColor, &QAction::triggered, [this] {
    emit S_ChangeType(core::property_type::color);
  });

  AddActions(parent);
}

void PropertyContextMenu::AddActions(QWidget* widget)
{
  Q_ASSERT(widget);
  widget->addAction(mCopy);
  widget->addAction(mPaste);
  widget->addAction(mRename);
  widget->addAction(mRemove);
}

void PropertyContextMenu::DisableAll()
{
  mCopy->setEnabled(false);
  mPaste->setEnabled(false);
  mRename->setEnabled(false);
  mChangeType->setEnabled(false);
  mAdd->setEnabled(false);
  mRemove->setEnabled(false);
}

void PropertyContextMenu::SetCurrentType(core::property_type type)
{
  for (auto* elem : mChangeType->actions())
  {
    if (!elem->isSeparator())
    {
      elem->setEnabled(true);
    }
  }

  switch (type)
  {
    case core::property_type::string:
    {
      mTypeString->setEnabled(false);
      break;
    }
    case core::property_type::integer:
    {
      mTypeInt->setEnabled(false);
      break;
    }
    case core::property_type::floating:
    {
      mTypeFloat->setEnabled(false);
      break;
    }
    case core::property_type::boolean:
    {
      mTypeBool->setEnabled(false);
      break;
    }
    case core::property_type::file:
    {
      mTypeFile->setEnabled(false);
      break;
    }
    case core::property_type::color:
    {
      mTypeColor->setEnabled(false);
      break;
    }
    case core::property_type::object:
    {
      mTypeObject->setEnabled(false);
      break;
    }
  }
}

void PropertyContextMenu::SetAddEnabled(const bool enabled)
{
  mAdd->setEnabled(enabled);
}

void PropertyContextMenu::SetRemoveEnabled(const bool enabled)
{
  mRemove->setEnabled(enabled);
}

void PropertyContextMenu::SetRenameEnabled(const bool enabled)
{
  mRename->setEnabled(enabled);
}

void PropertyContextMenu::SetCopyEnabled(const bool enabled)
{
  mCopy->setEnabled(enabled);
}

void PropertyContextMenu::SetPasteEnabled(const bool enabled)
{
  mPaste->setEnabled(enabled);
}

void PropertyContextMenu::SetChangeTypeEnabled(const bool enabled)
{
  mChangeType->setEnabled(enabled);
}

}  // namespace tactile
