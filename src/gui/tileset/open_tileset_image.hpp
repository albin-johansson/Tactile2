#pragma once

#include <qurl.h>
#include <qwidget.h>

#include <optional>

namespace tactile::gui {

/**
 * @brief Opens a file chooser dialog for selecting a tileset image and returns
 * the path of the selected file.
 *
 * @param parent the parent widget, defaults to null.
 *
 * @return the path of the image, if a file was selected; `std::nullopt`
 * otherwise.
 *
 * @since 0.1.0
 */
[[nodiscard]] auto open_tileset_image(QWidget* parent = nullptr)
    -> std::optional<QUrl>;

}  // namespace tactile::gui
