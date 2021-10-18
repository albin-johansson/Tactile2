#include "directories.hpp"

#include <centurion.hpp>  // preferred_path

namespace Tactile {

auto GetPersistentFileDir() -> const std::filesystem::path&
{
  static const std::filesystem::path path{
      cen::preferred_path("albin-johansson", "tactile").copy()};
  return path;
};

}  // namespace Tactile
