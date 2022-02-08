#pragma once

#include "dialog.hpp"
#include "io/maps/parser/parse_error.hpp"
#include "tactile.hpp"

namespace tactile {

class MapImportErrorDialog final : public dialog_base
{
 public:
  MapImportErrorDialog();

  void Open(parsing::parse_error error);

 protected:
  void on_update(const Model& model, entt::dispatcher& dispatcher) override;

 private:
  maybe<parsing::parse_error> mError;
};

}  // namespace tactile
