#pragma once

#include <QDialog>  // QDialog

#include "smart_pointers.hpp"

namespace Ui {
class about_dialog;
}

namespace tactile::gui {

/**
 * \class about_dialog
 *
 * \brief Represents the "About Tactile" dialog window that displays
 * information about the application.
 *
 * \see `QDialog`
 *
 * \since 0.1.0
 *
 * \headerfile about_dialog.hpp
 */
class about_dialog final : public QDialog
{
 public:
  explicit about_dialog(QWidget* parent = nullptr);

  ~about_dialog() noexcept override;

  static void spawn();

 private:
  unique<Ui::about_dialog> m_ui;
};

}  // namespace tactile::gui
