#pragma once

#include <entt/entt.hpp>

#include "editor/gui/dialogs/string_input_dialog.hpp"
#include "tactile_def.hpp"

namespace tactile {

class RenameLayerDialog final : public AStringInputDialog {
 public:
  RenameLayerDialog();

  ~RenameLayerDialog() override = default;

  void Show(LayerID id, std::string oldName);

 protected:
  void OnAccept(entt::dispatcher& dispatcher) override;

  [[nodiscard]] auto Validate(const Model& model, std::string_view input) const
      -> bool override;

 private:
  Maybe<LayerID> mTargetId;
  Maybe<std::string> mOldName;
};

}  // namespace tactile
