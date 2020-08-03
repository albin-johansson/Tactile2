#pragma once

#include <memory>

class QImage;

namespace tactile::model {

struct [[deprecated]] tileset_info final {
  std::shared_ptr<QImage> image;
  int id{0};
  int tileWidth{1};
  int tileHeight{1};
};

}  // namespace tactile::model
